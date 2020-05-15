/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 15 May 2020 05:44:53 PM CST
 @ File Name	: echo_server_multithread.c
 @ Description	: 多线程回显服务器
 ************************************************************************/
#include "wrap/wrap.h"
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define REUSE_ADDR

struct threadNode {
	int cfd;
	struct in_addr addr;
	uint16_t port;
};

void * DoReadWrite(void * arg) 
{
	if (NULL == arg) {
		PrintError(stderr, EINVAL, "call DoReadWrite failed", EXIT_FAILURE);		
	}


	struct threadNode * tmp = (struct threadNode *)arg;	
	char arr[INET_ADDRSTRLEN];	
	inet_ntop(AF_INET, &(tmp->addr), arr, sizeof(arr)); 
	fprintf(stdout, "DoReadWrite, tid(%ld), %s:%d\n", pthread_self(), arr, ntohs(tmp->port));	

	int cfd = tmp->cfd; 
	int cnt;
	char buf[1024];
	while ((cnt = ReadLine(cfd, buf, 1023)) > 0) {
		buf[cnt] = '\0'; 
		printf("tid(%ld), read cnt is %d\n, get string :%s\n", pthread_self(), cnt, buf);	
		WriteCount(cfd, buf, cnt);
	}
	
	free(arg);
	
	// 关闭描述符， 不然只是单端(客户端)关闭
	close(cfd);
	return NULL;
}

int main(int argc, char * argv[]) 
{
	int port = 8888;
	if (argc == 2) {
		port = atoi(argv[1]);
	}

	int fd = Socket(AF_INET, SOCK_STREAM, 0);

#ifdef REUSE_ADDR
	// 端口复用
	int opt = 1;
	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
	}
#endif

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	Bind(fd, (struct sockaddr *)&bindAddr, sizeof(bindAddr));  	

	Listen(fd, SOMAXCONN);

	socklen_t addrlen;
	struct sockaddr_in clientAddr;
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));


	int cfd;
	pthread_t tid;
	while (1) {			
		cfd = Accept(fd, (struct  sockaddr * )&clientAddr, &addrlen);

		struct threadNode * tmp = malloc(sizeof(struct threadNode));	
		tmp->cfd = cfd;
		tmp->addr.s_addr = clientAddr.sin_addr.s_addr;
		tmp->port = clientAddr.sin_port; 

		int ret = pthread_create(&tid, NULL, DoReadWrite, (void *)tmp);	
		if (0 != ret) {
			PrintError(stderr, ret, "call pthread_create failed", EXIT_FAILURE);		
		}
		ret = pthread_detach(tid);
		if (0 != ret) {
			PrintError(stderr, ret, "call pthread_detach failed", EXIT_FAILURE);		
		}
		
	}
	
	close(fd);

	return 0;
}
