/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 19 May 2020 11:07:15 AM CST
 @ File Name	: epoll_reactor.c
 @ Description	: epoll反应器
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/epoll.h>
#include <limits.h>
#include <time.h>

#include "wrap/wrap.h"

#define REUSE_ADDR 
#define EVENTS_MAX			20	// 一次epoll_wait最多处理EVENTS_MAX个事件
#define EVENT_NODE_MAX			1024	// 事件NODE的
#define MAX_WRITE_BYTE			40	// 每次最大写多少字节
#define EVNET_NODE_LIVE_TIMESTAMP	60	// 事件节点保活时间戳


#define MAX_SEGMENT_LENGTH SHRT_MAX	// 最大报文长度(这里指代应用程序能够发送的最大的数据长度), 循环读时会把数据读到该长度的一片存储空间中， 保证一定能够装得下所有数据

typedef struct {
	int fd;				// 文件描述符
	char buf[MAX_SEGMENT_LENGTH];	// 读写缓存
	int bufLen;			// 缓存有效数据长度(或者写时剩余的未写字节数)
	int wIndex;			// 写开始位置
	uint32_t event;			// 事件记录	

	void * (*pfunc)(void *);	// 回调函数指针
	void * regArgs;			// 注册参数	

	uint8_t status;			// 是否正在使用中
	time_t	lastAccess;		// 上次访问事件戳	
} EventNode;

int epfd;				// epoll instance fd
struct epoll_event events[EVENTS_MAX];
EventNode NodeList[EVENT_NODE_MAX + 1];		// 一个专门来装lfd的 

/***************************************************
* declaration 
***************************************************/
void * DoReadLoop(void * arg);
void * DoWrite(void * node);

/***************************************************
* EventNode struct Handle 
***************************************************/
int FindUnuseNode()
{
	int i = 0;
	for (; i < EVENT_NODE_MAX; ++i) {
		if (!NodeList[i].status) break;	
	}	

	return i;
}

void ResetLastAccess(EventNode * node)
{
	node->lastAccess = time(NULL);
}

int SetEventNode(EventNode * node, int fd, uint32_t event, void * (*pfunc)(void *), void * regArgs)
{
	node->fd = fd;	
	node->event = event;
	node->pfunc = pfunc;
	node->regArgs = regArgs;

	ResetLastAccess(node);

	return 1;
}

/***************************************************
* epoll event reactor handler 
***************************************************/
void EventAdd(EventNode * node)
{
	int op = EPOLL_CTL_ADD;

	struct epoll_event evn;
	evn.events = node->event;
	evn.data.ptr = (void *)node;
	node->status = 1;
	node->bufLen = 0;
	node->wIndex = 0;

	int fd = node->fd;	
	int ret = epoll_ctl(epfd, op, fd, &evn);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
}

void EventDel(EventNode * node)
{
	int op = EPOLL_CTL_DEL;
	node->status = 0;
	node->bufLen = 0;
	node->wIndex = 0;

	int fd = node->fd;
	int ret = epoll_ctl(epfd, op, fd, NULL);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
}

void EventMod(EventNode * node)
{
	int op = EPOLL_CTL_MOD;

	struct epoll_event evn;
	evn.events = node->event;
	evn.data.ptr = (void *)node;

	node->status = 1;

	int fd = node->fd;	
	int ret = epoll_ctl(epfd, op, fd, &evn);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
}

void EventLoop()
{
	while(1) {
		// 关闭超时连接
		time_t now = time(NULL);
		for (int i = 0; i < EVENT_NODE_MAX; ++i) {
			if (!NodeList[i].status) {
				continue;
			}
			
			if (now - EVNET_NODE_LIVE_TIMESTAMP > NodeList[i].lastAccess) {
				// 超时， 强制关闭连接
				if (NodeList[i].regArgs) {
					struct sockaddr_in * tmp = (struct sockaddr_in *)NodeList[i].regArgs;
					PrintAddr(stdout, tmp, "关闭超时连接");
					free(NodeList[i].regArgs);
					NodeList[i].regArgs = NULL;
				} else {
					printf("关闭超时连接%d\n", NodeList[i].fd);
				}
				EventDel(NodeList + i);
				close(NodeList[i].fd);
			}	
		}

		int nready = epoll_wait(epfd, events, EVENTS_MAX, 1000);

		if (-1 == nready) {
			if (EINTR == errno) {
				PrintError(stderr, 0, "contine epoll_wait", 0);		
				continue;
			} else {
				PrintError(stderr, 0, "call epoll_wait error", EXIT_FAILURE);		
			}
		} else if (0 == nready) {
			continue;
		}

		for (int i = 0; i < nready; ++i) {
			EventNode * node = (EventNode *)events[i].data.ptr;	
			node->pfunc(node);
		}
	}
}

/***************************************************
* other 
***************************************************/
int InitSock(int port)
{
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);

#ifdef REUSE_ADDR
	// 端口复用
	int opt = 1;
	if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		

	}
#endif

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	Bind(lfd, (struct sockaddr *)&bindAddr, sizeof(bindAddr));  	

	Listen(lfd, SOMAXCONN);

	// 设置nonblocking
	SetNonBlocking(lfd);

	return lfd;
}

void InitEpoll(int size)
{
	epfd = epoll_create(size);	
	if (-1 == epfd) {
		PrintError(stderr, 0, "InitEpoll failed", EXIT_FAILURE);	
	}
}


/***************************************************
* callback func  
***************************************************/
void * DoAccept(void * arg)
{
	EventNode * node = (EventNode *)arg;
	ResetLastAccess(node);

	// 看是否有空位
	int index = FindUnuseNode();
	if (index >= EVENT_NODE_MAX) {
		// lfd 为 LT 模式， 下次epoll_wait继续触发
		PrintError(stderr, EMFILE, "too mant connected sockets", 0);	
		return NULL;
	}	

	int lfd = node->fd; 

	struct sockaddr_in * clientAddr = malloc(sizeof(struct sockaddr_in));
	socklen_t addrlen = sizeof(struct sockaddr_in);
	bzero(clientAddr, sizeof(struct sockaddr_in));

	int cfd = Accept(lfd, (struct  sockaddr * )clientAddr, &addrlen);
	SetNonBlocking(cfd);

	SetEventNode(NodeList + index, cfd, EPOLLIN | EPOLLET, DoReadLoop, clientAddr);	
	EventAdd(NodeList + index);	

	return NULL;
}

void * DoReadLoop(void * arg) 
{
	EventNode * node = (EventNode *)arg;
	ResetLastAccess(node);
	int fd = node->fd;

	int totalCnt = 0;
	
	while(1) {
		// 每次读100， 模仿一次读不完的情况
		ssize_t cnt = read(fd, node->buf + totalCnt, 100);
		if (-1 == cnt) {
			if (EAGAIN == errno) {
				// 读到结尾了
				break;
			} else if (EINTR == errno) {
				// 被信号中断， 重来
				continue;
			} else {
				// error
				PrintError(stderr, 0, "DoReadLoop, call read failed", EXIT_FAILURE);		
			}
		} else if (0 == cnt) {
			// close	
			if (node->regArgs) {
				free(node->regArgs);
				node->regArgs = NULL;
			}
			EventDel(node);
			close(fd);			
			return NULL;
		} else {
			totalCnt += cnt;
		}
	}	

	// 读到数据
	char tmpStr[100];
	sprintf(tmpStr, "***********读到%d字符, 切换为写", totalCnt);
	PrintAddr(stdout, (struct sockaddr_in *)node->regArgs, tmpStr);

	node->bufLen = totalCnt;
	node->wIndex = 0;

	SetEventNode(node, fd, EPOLLOUT, DoWrite, node->regArgs);	
	EventMod(node);
	return NULL;
}


void * DoWrite(void * arg)
{
	EventNode * node = (EventNode *)arg;
	ResetLastAccess(node);
	int fd = node->fd;

	int writeCnt = node->bufLen;

#ifdef MAX_WRITE_BYTE
	// 测试下写多遍,模拟写缓冲不够， 要写多次， 通过事件通知继续写
	if (writeCnt > MAX_WRITE_BYTE) {
		writeCnt = MAX_WRITE_BYTE;
	}
#endif

	int cnt = Write(fd, node->buf + node->wIndex, writeCnt);
	node->bufLen -= cnt;
	node->wIndex += cnt;

	char tmpStr[100];
	sprintf(tmpStr, "写了%d字符, 还剩余%d没有写", cnt, node->bufLen);
	PrintAddr(stdout, (struct sockaddr_in *)node->regArgs, tmpStr);

	if (node->bufLen <= 0) {
		// 都写光了, 改回读
		sprintf(tmpStr, "已经写光，切回读");
		PrintAddr(stdout, (struct sockaddr_in *)node->regArgs, tmpStr);
		SetEventNode(node, fd, EPOLLIN | EPOLLET, DoReadLoop, node->regArgs);	
		EventMod(node);
	} 

	return NULL;
}


int main(int argc, char * argv[]) 
{
	int port = 8888;
	if (argc == 2) {
		port = atoi(argv[1]);
	}

	int lfd = InitSock(port);
	InitEpoll(1024);

	memset(NodeList, 0, sizeof(EventNode));

	// lfd 使用LT 模式
	SetEventNode(NodeList + EVENT_NODE_MAX, lfd, EPOLLIN, DoAccept, NULL); 		
	EventAdd(NodeList + EVENT_NODE_MAX);	

	EventLoop();
	
	EventDel(NodeList + EVENT_NODE_MAX);

	close(lfd);
	close(epfd);

	return 0;
}


