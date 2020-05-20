/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 19 May 2020 11:06:42 AM CST
 @ File Name	: epoll_lt_et_1.c
 @ Description	: LT与ET对比， 管道 , 使用非阻塞IO
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
#include <fcntl.h>

#include "wrap/wrap.h"

#define EVENTS_MAX 20		// 一次epoll_wait最多处理EVENTS_MAX个事件

#define ET_TRIGGER_METHOD
#define USE_LOOP_READ

int epfd;			// epoll instance fd
int pipefd[2];			// 管道的读写端


struct epoll_event events[EVENTS_MAX];
struct epoll_event event;

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

void DoRead(int fd) 
{
	char buf[6];
	// 每次就读5个字符
	int cnt = Read(fd, buf, 5);
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
		// EventMod(fd, EPOLLIN | EPOLLET);
	}
}


void DoReadLoop(int fd) 
{
	char buf[SHRT_MAX];
	int totalCnt = 0;
	
	while(1) {
		// 与DoRead一样， 每次读5， 模仿一次读不完的情况
		ssize_t cnt = read(fd, buf + totalCnt, 5);
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
			if (events[i].data.fd == pipefd[0]) {
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
	if (-1 == pipe(pipefd)) {
		PrintError(stderr, 0, "pipe failed", EXIT_FAILURE);	
	}
	
	int pid = fork();
	if (-1 == pid) {
		PrintError(stderr, 0, "fork failed", EXIT_FAILURE);	
	} else if ( 0 == pid) {
		// child
		close(pipefd[0]);
		char buf[10];
		int no = 'A';
		while (1) {
			printf("child begin write\n");
			memset(buf, no, 4); 			
			memset(buf + 4, '\n', 1); 			
			++no;
			memset(buf + 5, no, 4); 			
			memset(buf + 9, '\n', 1); 			
			++no;

			Write(pipefd[1],buf, sizeof(buf)); 
			// 每3秒写一遍数据到服务器		
			sleep(3);
		}

		close(pipefd[1]);	
	} else {
		// parent
		close(pipefd[1]);
		InitEpoll(1024);

		// 设置非阻塞IO
		SetNonBlocking(pipefd[0]);

#ifdef ET_TRIGGER_METHOD
		printf("使用ET触发\n");
		EventAdd(pipefd[0], EPOLLIN | EPOLLET);	
#else
		printf("使用LT触发\n");
		EventAdd(pipefd[0], EPOLLIN);	
#endif
		EventLoop();

		close(pipefd[0]);
		close(epfd);
	}

	return 0;
}


