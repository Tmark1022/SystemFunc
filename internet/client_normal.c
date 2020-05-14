/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 10 May 2020 10:37:44 PM CST
 @ File Name	: client_normal.c
 @ Description	: 简单的客户端， 有输入
 ************************************************************************/
#include "wrap/wrap.h"

char inbuf[1024];
char outbuf[10240];

int main(int argc, char *argv[]) {

	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("call socket failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverSockAddr;
	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serverSockAddr.sin_addr);
	Connect(sockfd, (const struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr_in));
		
	int ret;
	while(NULL != fgets(inbuf, sizeof(inbuf), stdin)) {
		WriteCount(sockfd, inbuf, strlen(inbuf));
	
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
