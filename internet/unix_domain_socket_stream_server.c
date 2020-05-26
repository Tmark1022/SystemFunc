/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 26 May 2020 09:51:18 AM CST
 @ File Name	: unix_domain_socket_stream_server.c
 @ Description	: 本地套接字字节流服务器
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#include "wrap/wrap.h"

#undef AF_FAMILY
#define SERVER_SOCKET_FILE "/tmp/9Lq7BNBnBycd6nxy_server.socket"

void * ThreadHandler(void * arg)
{
	int cfd = (int)arg;	
	char buf[1024];
	
	while (1) {
		int cnt = Read(cfd, buf, 1024);
		if (0 == cnt) {
			close(cfd);
			printf("%lu, done\n", pthread_self());
			break;
		}

		Write(cfd, "hello,", strlen("hello,"));
		Write(cfd, buf, cnt);
	}

	return NULL ;
}


int main(int argc, char *argv[]) {

	int lfd = Socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sun_family = AF_UNIX; 
	strcpy(svrAddr.sun_path, SERVER_SOCKET_FILE);

	unlink(SERVER_SOCKET_FILE);
	Bind(lfd, (struct sockaddr *)&svrAddr, sizeof(svrAddr)); 

	Listen(lfd, SOMAXCONN);

	struct sockaddr_un cliAddr;
	socklen_t len = sizeof(cliAddr);
	pthread_t tid;

	while (1) {
		memset(&cliAddr, 0, sizeof(cliAddr));
		int cfd = Accept(lfd, (struct sockaddr * )&cliAddr, &len);
		printf("new connect , sock file : %s, %d\n", cliAddr.sun_path, len);
		
		int ret = pthread_create(&tid, NULL, ThreadHandler, (void *)cfd);	
		if (0 != ret) {
			PrintError(stderr, ret, "call pthread_create error", EXIT_FAILURE); 
		}
		ret = pthread_detach(tid);
		if (0 != ret) {
			PrintError(stderr, ret, "call pthread_create error", EXIT_FAILURE); 
		}
	}

	close(lfd);	

	// 通信完后才删掉， 不然通信过程中找不到
	unlink(SERVER_SOCKET_FILE);

	return 0;
}
