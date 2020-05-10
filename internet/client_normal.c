/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 10 May 2020 10:37:44 PM CST
 @ File Name	: client_normal.c
 @ Description	: 简单的客户端， 有输入
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

char inbuf[1024];
char outbuf[10240];

int main(int argc, char *argv[]) {
	
	int ret;	
	uint16_t defaultPort = 10220;
	if (argc == 2) {
		defaultPort = atoi(argv[1]);
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("call socket failed");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in mySockAddr;
	mySockAddr.sin_family = AF_INET;
	mySockAddr.sin_port = htons(defaultPort);
	inet_pton(AF_INET, "127.0.0.1", &mySockAddr.sin_addr);
	ret = bind(sockfd, (const struct sockaddr *)&mySockAddr, sizeof(struct sockaddr_in));
	if (-1 == ret) {
		perror("call bind failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverSockAddr;
	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serverSockAddr.sin_addr);
	ret = connect(sockfd, (const struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr_in));
	if (-1 == ret) {
		perror("call connect failed");
		exit(EXIT_FAILURE);
	}
		
	while(NULL != fgets(inbuf, sizeof(inbuf), stdin)) {
		// 去掉fgets插入的\n
		inbuf[strlen(inbuf) - 1] = '\0';	
		write(sockfd, inbuf, strlen(inbuf));
		ret = read(sockfd, outbuf, sizeof(outbuf) - 1);
		if (-1 == ret) {
			perror("read error");
			exit(EXIT_FAILURE);
		}
		
		// 尾部出入\0
		outbuf[ret] = '\0';	
		printf("%s\n", outbuf);
			
	}
	
	close(sockfd);
	
	return 0;
}
