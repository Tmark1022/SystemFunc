/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 29 May 2020 04:29:36 PM CST
 @ File Name	: aio_echo_server.c
 @ Description	: 异步IO实现高并发echo
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <aio.h>
#include <signal.h>

#include "wrap/wrap.h"

/*
       aio_read(3)     Enqueue a read request.  This is the asynchronous analog of read(2).

       aio_write(3)    Enqueue a write request.  This is the asynchronous analog of write(2).

       aio_fsync(3)    Enqueue a sync request for the I/O operations on a file descriptor.  This is the asynchronous analog of fsync(2) and fdatasync(2).

       aio_error(3)    Obtain the error status of an enqueued I/O request.

       aio_return(3)   Obtain the return status of a completed I/O request.

       aio_suspend(3)  Suspend the caller until one or more of a specified set of I/O requests completes.

       aio_cancel(3)   Attempt to cancel outstanding I/O requests on a specified file descriptor.

       lio_listio(3)   Enqueue multiple I/O requests using a single function call.*
 */

volatile int gQuitFlag = 0;
int actCnt = 0;			// 正在活动的请求数

typedef struct {
	int idx;		// 数组下标
	int status;		// 是否正在使用0, 1
	struct aiocb cb;				
}Node;

#define BUF_SIZE 1024
#define AIOCB_CNT 5
Node gNodeList[AIOCB_CNT];

void sig_int(int signo)
{
	gQuitFlag = 1; 
	printf("try to quit\n");
}

void sig_alrm(int signo)
{
	printf("get signo alarm\n");
}

// 实现一个支持等待X秒的简易版accept, 使用进程alarm， 程序有用到alarm的相关程序 或 已经有监听SIGALRM 的请别使用
int AcceptAlarm(int socket, struct sockaddr *address, socklen_t * address_len, unsigned int sec)
{
	signal(SIGALRM, sig_alrm);
	alarm(sec);

	int ret = accept(socket, address, address_len);
	if (-1 == ret) {
		if (errno != EINTR)
			PrintError(stderr, 0, "call accept failed", EXIT_FAILURE);		
	}

	// cancel panding alarm and reset handle func
	alarm(0);
	signal(SIGALRM, SIG_DFL);

#if defined(AF_FAMILY) && (AF_FAMILY) == (AF_INET)
	if (-1 != ret && NULL != address) {
		// 这里只考虑ipv4	
		PrintAddr(stdout, (struct sockaddr_in *)address, "new client");
	}
#endif

	return ret;
}

void InitNodeList()
{
	for (int idx = 0; idx < AIOCB_CNT; ++idx) {
		gNodeList[idx].idx = idx;		
		gNodeList[idx].status = 0;		
		memset(&(gNodeList[idx].cb), 0, sizeof(struct aiocb));
		gNodeList[idx].cb.aio_buf = malloc(BUF_SIZE);
		if (NULL == gNodeList[idx].cb.aio_buf) {
			PrintError(stderr, 0, "malloc failed", EXIT_FAILURE);	
		}
	}
}

void FreeNodeList()
{
	for (int idx = 0; idx < AIOCB_CNT; ++idx) {
		gNodeList[idx].status = 0;		
		free((void *)gNodeList[idx].cb.aio_buf);
		gNodeList[idx].cb.aio_buf = NULL;
		if (NULL == gNodeList[idx].cb.aio_buf) {
			PrintError(stderr, 0, "malloc failed", EXIT_FAILURE);	
		}
		memset(&(gNodeList[idx].cb), 0, sizeof(struct aiocb));
	}
}

int GetUnusedNode()
{
	for (int idx = 0; idx < AIOCB_CNT; ++idx) {
		if (gNodeList[idx].status == 0) {
			return idx;	
		}
	}

	return -1; 
}


void  read_cb(union sigval arg)
{
	int idx = arg.sival_int;
	struct aiocb * aiocbp = &(gNodeList[idx].cb);
	printf("idx %d, fd %d, read successfully", idx, aiocbp->aio_fildes);

}

void  write_cb(union sigval arg)
{
	int idx = arg.sival_int;
	struct aiocb * aiocbp = &(gNodeList[idx].cb);
	printf("idx %d, fd %d, write successfully", idx, aiocbp->aio_fildes);

}



int main(int argc, char *argv[]) {
	
	int lfd = Socket(AF_INET, SOCK_STREAM, 0); 	

	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(8888);
	svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind(lfd, (struct sockaddr *)&svrAddr, sizeof(svrAddr));	

	Listen(lfd, SOMAXCONN);
	
	struct sockaddr_in cliAddr;
	socklen_t len = sizeof(cliAddr);
	int sec = 3;

	signal(SIGINT, sig_int);
	signal(SIGQUIT, sig_int);
	InitNodeList();

	while (1) {
		if (gQuitFlag && 0 == actCnt) {
			// 设置了退出并且没有正在运行的请求
			break;
		}	

		if (!gQuitFlag) {
			// wait 3s 
			memset(&cliAddr, 0, sizeof(cliAddr));
			int cfd =  AcceptAlarm(lfd, (struct sockaddr *)&cliAddr, &len, sec);
			if (-1 != cfd) {
				// new connect
				int idx = GetUnusedNode();
				if (-1 == idx) {
					printf("too many connect");
					close(cfd);
				} else {
					// begin aio_read
					gNodeList[idx].status = 1;

					struct aiocb * aiocbp = &(gNodeList[idx].cb);		
					aiocbp->aio_fildes = cfd;
					aiocbp->aio_offset = 0;
					aiocbp->aio_nbytes = BUF_SIZE;
					aiocbp->aio_reqprio = 0;
					aiocbp->aio_sigevent.sigev_notify = SIGEV_THREAD; 
					aiocbp->aio_sigevent.sigev_value.sival_int = idx; 
					aiocbp->aio_sigevent.sigev_notify_function = read_cb;

					if (-1 == aio_read(aiocbp)) {
						PrintError(stderr, 0, "call aio_read failed", EXIT_FAILURE);		
					}
				}

			}
		}

		// check aio_error
		
	}


	FreeNodeList();
	close(lfd);

	return 0;
}
