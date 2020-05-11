/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 10:44:05 AM CST
 @ File Name	: call_system_server.c
 @ Description	: 接受客户端的输入并调用system
 ************************************************************************/
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>


char buf[1024];


void PrintAddr(struct sockaddr_in * addr, const char * str)
{
	char arr[1024];	
	inet_ntop(AF_INET, &(addr->sin_addr), arr, sizeof(arr)); 
	printf("%s, ip %s, port %d\n", str, arr, ntohs(addr->sin_port));	
}

int main(int argc, char *argv[]) {
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("call socket failed");
		exit(EXIT_FAILURE);
	}
	
	int ret;	
	struct sockaddr_in serverSockAddr;
	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_port = htons(8888);
	serverSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	ret = bind(sockfd, (const struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr_in));
	if (-1 == ret) {
		perror("call bind failed");
		exit(EXIT_FAILURE);
	}

	ret = listen(sockfd, SOMAXCONN);
	if (-1 == ret) {
		perror("call listen failed");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in clientSockAddr;
	socklen_t len;
	int clientfd;
	clientfd = accept(sockfd, (struct sockaddr *)&clientSockAddr, &len);
	if (-1 == clientfd) {
		perror("call accept failed");
		exit(EXIT_FAILURE);
	}
	
	char clentAddrStr[1024];
	const char * aaaa;
	if (NULL == (aaaa = inet_ntop(AF_INET, (void * )&clientSockAddr.sin_addr.s_addr, clentAddrStr, sizeof(clentAddrStr)))) {
		perror("inet_ntop error");
		exit(EXIT_FAILURE);
	}

	PrintAddr(&clientSockAddr, "new client");

	// 重定向标准输出到连接到的client	
	if ( -1 == dup2(clientfd, STDOUT_FILENO)) {
		perror("dup2 error");
		exit(EXIT_FAILURE);
	}

	while( (ret = read(clientfd, buf, 1024 - 1))) {
		if (-1 == ret) {
			abort();
		}
		
		// 尾部出入\0
		buf[ret] = '\0';
		int status = system(buf);		
		fprintf(stderr, "call %s, status is %d\n", buf, status);
	}
	
	close(sockfd);
	close(clientfd);
	
	return 0;
}
