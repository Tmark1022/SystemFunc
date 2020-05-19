/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 19 May 2020 11:06:17 AM CST
 @ File Name	: epoll_normal.c
 @ Description	: epoll 实现高并发Echo， lfd（LT非阻塞）, cfd（LT非阻塞）
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/epoll.h>

#include "wrap/wrap.h"

#define REUSE_ADDR 

#define EVENTS_MAX 20		// 一次epoll_wait最多处理EVENTS_MAX个事件

typedef struct {
	int fd;			// 文件描述符
	int bufLen;		// 缓存有效数据长度
	char buf[1024];		// 读写缓存
} Node;

int epfd;			// epoll instance fd
int lfd;			// listen sock fd

struct epoll_event events[EVENTS_MAX];
struct epoll_event event;

void InitSock(int port)
{
	lfd = Socket(AF_INET, SOCK_STREAM, 0);

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
}

void InitEpoll(int size)
{
	epfd = epoll_create(size);	
	if (-1 == epfd) {
		PrintError(stderr, 0, "InitEpoll failed", EXIT_FAILURE);	
	}
}

Node * CreateNode(int fd) 
{
	Node * node = malloc(sizeof(Node));
	if (NULL == node) {
		PrintError(stderr, 0, "malloc error", EXIT_FAILURE);	
	}
	node->fd = fd;
	node->bufLen = 0;
	memset(node->buf, 0, sizeof(node->buf));

	return node;
}

void EventAdd(Node * node, uint32_t evbit)
{
	int op = EPOLL_CTL_ADD;
	event.events = evbit;
	event.data.ptr = (void *)node;

	int fd = node->fd;	
	int ret = epoll_ctl(epfd, op, fd, &event);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
}

void EventDel(Node * node)
{
	int op = EPOLL_CTL_DEL;
	int fd = node->fd;
	int ret = epoll_ctl(epfd, op, fd, NULL);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
	free(node);
	node = NULL;
}

void EventMod(Node * node, uint32_t evbit)
{
	int op = EPOLL_CTL_MOD;
	event.events = evbit;
	event.data.ptr = (void *)node;

	int fd = node->fd;	
	int ret = epoll_ctl(epfd, op, fd, &event);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
}

int DoAccept(Node * node)
{
	struct sockaddr_in clientAddr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));
	int cfd = Accept(lfd, (struct  sockaddr * )&clientAddr, &addrlen);

	SetNonBlocking(cfd);
	
	// 加入epoll监听
	EventAdd(CreateNode(lfd), EPOLLIN);	

	return cfd;
}

// TODO
int DoRead(Node * node) 
{

	return 0;
}

// TODO
int DoWrite(Node * node)
{

	return 0;
}


void EventLoop()
{
	while(1) {
		int nready = epoll_wait(epfd, events, EVENTS_MAX, -1);

		if (-1 == nready) {
			if (EINTR == errno) {
				PrintError(stderr, 0, "contine epoll_wait", 0);		
				continue;
			} else {
				PrintError(stderr, 0, "call epoll_wait error", EXIT_FAILURE);		
			}
		}

		for (int i = 0; i < nready; ++i) {
			Node * node = (Node *)events[i].data.ptr;
			if (node->fd == lfd) {
				// new connection
				DoAccept(node);
			} else {
				// read ready
				if (events[i].events & EPOLLIN) {
					DoRead(node);
				}

				// write ready
				if (events[i].events & EPOLLOUT) {
					DoWrite(node);	
				}
			}
		}
	}
}

int main(int argc, char * argv[]) 
{
	int port = 8888;
	if (argc == 2) {
		port = atoi(argv[1]);
	}

	InitSock(port);
	InitEpoll(1024);
	
	EventAdd(CreateNode(lfd), EPOLLIN);	
	EventLoop();

	close(lfd);
	close(epfd);

	return 0;
}


