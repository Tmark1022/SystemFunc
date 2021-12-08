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

#include "wrap/wrap.h"

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

/***************************************************
* decalration  
***************************************************/
void  write_cb(union sigval arg);

void sig_int(int signo)
{
	gQuitFlag = 1; 
	printf("try to quit\n");

	for (int idx = 0; idx < AIOCB_CNT; ++idx) {	
		if (gNodeList[idx].status == 0) {
			continue;
		}		

		struct aiocb * aiocbp = &(gNodeList[idx].cb);		
		int ret = aio_cancel(aiocbp->aio_fildes, aiocbp);
		if (-1 == ret) {
			PrintError(stderr, 0, "call aio_cancel failed", EXIT_FAILURE);		
		} else {
			if (AIO_CANCELED == ret) 
				printf("aio_cancel status is AIO_CANCELED\n");
			if (AIO_NOTCANCELED == ret) 
				printf("aio_cancel status is AIO_NOTCANCELED\n");
			if (AIO_ALLDONE == ret) 
				printf("aio_cancel status is AIO_ALLDONE\n");
		}
	}	
}

void sig_alrm(int signo)
{
	printf("get signo alarm\n");
}

// 实现一个支持等待X秒的简易版accept, 使用进程alarm， 程序有用到alarm的相关程序 或 已经有监听SIGALRM 的请别使用
int AcceptAlarm(int socket, struct sockaddr *address, socklen_t * address_len, unsigned int sec)
{

	// linux signal 默认居然是自动恢复慢速系统调用， fuck
	// signal(SIGALRM, sig_alrm);
	struct sigaction sa;
	sa.sa_handler = sig_alrm;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_INTERRUPT;
	sigaction(SIGALRM, &sa, NULL);


	alarm(sec);

	int ret = accept(socket, address, address_len);
	if (-1 == ret) {
		if (errno != EINTR)
			PrintError(stderr, 0, "call accept failed", EXIT_FAILURE);		
	}

	// cancel panding alarm and reset handle func
	alarm(0);
	sa.sa_handler = SIG_DFL;
	sigaction(SIGALRM, &sa, NULL);

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
	int ret = aio_return(aiocbp);
	if (-1 == ret) {
		PrintError(stderr, 0, "call aio_return failed", 0);		
	}
	printf("idx %d, fd %d, read successfully, return status is %d\n", idx, aiocbp->aio_fildes, ret);

	if (0 == ret) {
		gNodeList[idx].status = 0;	
		--actCnt;
		close(aiocbp->aio_fildes);
		printf("close fd %d\n", aiocbp->aio_fildes);	
	} else {
		char *bufp = (char *)aiocbp->aio_buf;
		bufp[ret] = '\0';
		printf("read data : %s\n", bufp);	
		
		// 异步写
		aiocbp->aio_nbytes = ret;
		aiocbp->aio_sigevent.sigev_notify = SIGEV_THREAD; 
		aiocbp->aio_sigevent.sigev_value.sival_int = idx; 
		aiocbp->aio_sigevent.sigev_notify_function = write_cb;

		if (-1 == aio_write(aiocbp)) {
			PrintError(stderr, 0, "call aio_write failed", EXIT_FAILURE);		
		}
	}

}

void  write_cb(union sigval arg)
{
	int idx = arg.sival_int;
	struct aiocb * aiocbp = &(gNodeList[idx].cb);
	int ret = aio_return(aiocbp);
	if (-1 == ret) {
		PrintError(stderr, 0, "call aio_return failed", 0);		
	}
	printf("idx %d, fd %d, write successfully, return status is %d\n", idx, aiocbp->aio_fildes, ret);
	
	if (gQuitFlag) {
		// 已经关闭， 不再进行读，aio_cancel并不能cancel掉异步 request 
		gNodeList[idx].status = 0;
		--actCnt;
		close(aiocbp->aio_fildes);
		return ;
	}	

	// 继续异步读
	aiocbp->aio_nbytes = BUF_SIZE;
	aiocbp->aio_sigevent.sigev_notify = SIGEV_THREAD; 
	aiocbp->aio_sigevent.sigev_value.sival_int = idx; 
	aiocbp->aio_sigevent.sigev_notify_function = read_cb;
	
	if (-1 == aio_read(aiocbp)) {
		PrintError(stderr, 0, "call aio_read failed", EXIT_FAILURE);		
	}
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
	int sec = 5;

	signal(SIGINT, sig_int);
	//signal(SIGQUIT, sig_int);
	InitNodeList();

	while (1) {
		if (gQuitFlag && 0 == actCnt) {
			// 设置了退出并且没有正在运行的请求
			printf("exit\n");
			break;
		}	

		if (!gQuitFlag) {
			// wait 5s 
			memset(&cliAddr, 0, sizeof(cliAddr));
			int cfd =  AcceptAlarm(lfd, (struct sockaddr *)&cliAddr, &len, sec);
			//printf("accept return status : %d\n", cfd);
			if (-1 != cfd) {
				// new connect
				int idx = GetUnusedNode();
				if (-1 == idx) {
					printf("too many connect");
					close(cfd);
				} else {
					// begin aio_read
					gNodeList[idx].status = 1;
					++actCnt;

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
		// printf("begin check aio_error, now actCnt is %d\n", actCnt);
		for (int idx = 0; idx < AIOCB_CNT; ++idx) {	
			if (gNodeList[idx].status == 0) {
				continue;
			}		

			struct aiocb * aiocbp = &(gNodeList[idx].cb);		
			int ret = aio_error(aiocbp);
			if (ret == 0 || ret == EINPROGRESS) {
				continue;
			} else if (ECANCELED == ret) {
				printf("idx %d, fd %d, cancel\n", idx, aiocbp->aio_fildes);			
				gNodeList[idx].status = 0;	
				--actCnt;
				close(aiocbp->aio_fildes);
			} else {	
				PrintError(stderr, 0, "call aio_error failed", EXIT_FAILURE);		
			}
		}	
		//printf("end check aio_error, now actCnt is %d\n", actCnt);
	}

	FreeNodeList();
	close(lfd);

	return 0;
}
