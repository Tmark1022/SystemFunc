/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 10 May 2020 06:18:21 PM CST
 @ File Name	: echo_server.c
 @ Description	: 简单的echo服务器
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

char buf[1024];

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
	if (NULL == (aaaa = inet_ntop(AF_INET, &clientSockAddr.sin_addr, clentAddrStr, sizeof(clentAddrStr)))) {
		perror("inet_ntop error");
		exit(EXIT_FAILURE);
	}

	printf("new client, ip %s, port %d\n", clentAddrStr, ntohs(clientSockAddr.sin_port));	

	// echo 
	while( (ret = read(clientfd, buf, 1024 - 1))) {
		if (-1 == ret) {
			abort();
		}
		
		// 尾部出入\0
		buf[ret] = '\0';
		printf("get string : %s\n", buf);

		write(clientfd, buf, ret);
	}
	
	close(sockfd);
	close(clientfd);
	
	return 0;
}
