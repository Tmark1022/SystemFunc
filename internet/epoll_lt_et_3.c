/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 20 May 2020 02:20:22 PM CST
 @ File Name	: epoll_lt_et_3.c
 @ Description	: 
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

#include "wrap/wrap.h"

#define REUSE_ADDR 
#define EVENTS_MAX 20		// 一次epoll_wait最多处理EVENTS_MAX个事件

// #define USE_LOOP_READ

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

void EventAdd(int fd, uint32_t evbit)
{
	int op = EPOLL_CTL_ADD;
	event.events = evbit;
	event.data.fd = fd;

	int ret = epoll_ctl(epfd, op, fd, &event);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
}

void EventDel(int fd)
{
	int op = EPOLL_CTL_DEL;
	int ret = epoll_ctl(epfd, op, fd, NULL);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
}

void EventMod(int fd, uint32_t evbit)
{
	int op = EPOLL_CTL_MOD;
	event.events = evbit;
	event.data.fd = fd;

	int ret = epoll_ctl(epfd, op, fd, &event);
	if (-1 == ret) {
		char tmp[256];
		sprintf(tmp, "%s failed", __func__);
		PrintError(stderr, 0, tmp, EXIT_FAILURE);	
	}
}

int DoAccept(int fd)
{
	struct sockaddr_in clientAddr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));
	int cfd = Accept(lfd, (struct  sockaddr * )&clientAddr, &addrlen);

	SetNonBlocking(cfd);
	
	// ET 触发
	printf("使用边缘触发\n");
	EventAdd(cfd, EPOLLIN | EPOLLET);	

	return cfd;
}

void DoRead(int fd) 
{
	char buf[11];
	// 每次就读10个字符
	int cnt = Read(fd, buf, 10);
	if (0 == cnt) {
		// close
		EventDel(fd);	
		close(fd);
	} else {
		// 读到数据
		buf[cnt] = '\0';
		printf("=========读到%d字符, %s\n", cnt, buf);

		// TODO, ==================================================
		// 写完后MOD一下， 那么就算是ET模式， 下次wait还是触发事件
		EventMod(fd, EPOLLIN | EPOLLET);
	}
}


void DoReadLoop(int fd) 
{
	char buf[SHRT_MAX];
	int totalCnt = 0;
	
	while(1) {
		// 与DoRead一样， 每次读10， 模仿一次读不完的情况
		ssize_t cnt = read(fd, buf + totalCnt, 10);
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
			EventDel(fd);	
			close(fd);
			return ;
		} else {
			totalCnt += cnt;
		}
	}	

	// 读到数据
	buf[totalCnt] = '\0';
	printf("***********读到%d字符, %s\n", totalCnt, buf);
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
			if (events[i].data.fd == lfd) {
				// new connection
				DoAccept(events[i].data.fd);
			} else {
				// read ready
				if (events[i].events & EPOLLIN) {
#ifdef USE_LOOP_READ
					DoReadLoop(events[i].data.fd);
#else
					DoRead(events[i].data.fd);
#endif
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
	
	EventAdd(lfd, EPOLLIN);	
	EventLoop();

	close(lfd);
	close(epfd);

	return 0;
}


