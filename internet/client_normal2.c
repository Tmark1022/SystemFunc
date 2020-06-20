/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 10 May 2020 10:37:44 PM CST
 @ File Name	: client_normal.c
 @ Description	: 简单的客户端， 有输入
 ************************************************************************/
#include "wrap/wrap.h"
#include <sys/socket.h>
#include <signal.h>

char inbuf[1024];
char outbuf[10240];
int sockfd;

void sig_int(int signo)
{
	// TODO
	// shundown 关闭写端后尝试写write时， 居然直接把进程给kill掉了， 连返回都没有， 也不产生core dump 文件， 测试环境ubuntu 18.04, 内核 linux 4.15.0-101-generic 
	printf("get sig_int, do shutdown\n");	
	if (-1 == shutdown(sockfd, SHUT_RDWR)) {
		perror("shutdown failed");	
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("call socket failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverSockAddr;
	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serverSockAddr.sin_addr);
	Connect(sockfd, (const struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr_in));

	struct sockaddr_in addrtmp;
	socklen_t lentmp = sizeof(struct sockaddr_in);
	if (-1 == getsockname(sockfd, (struct sockaddr *)(&addrtmp), &lentmp)) {
		perror("getsockname error");
		exit(EXIT_FAILURE);	
	}
	PrintAddr(stdout, &addrtmp, "my bind sock addr");


	lentmp = sizeof(struct sockaddr_in);
	if (-1 == getpeername(sockfd, (struct sockaddr *)(&addrtmp), &lentmp)) {
		perror("getsockname error");
		exit(EXIT_FAILURE);	
	}
	PrintAddr(stdout, &addrtmp, "peer sock addr");
		
	signal(SIGINT, sig_int);	

	int ret;
	while(NULL != fgets(inbuf, sizeof(inbuf), stdin)) {
		WriteCount(sockfd, inbuf, strlen(inbuf));
	
		ret = read(sockfd, outbuf, sizeof(outbuf) - 1);
		if (-1 == ret) {
			perror("read error");
			exit(EXIT_FAILURE);
		}	
		printf("ret is %d\n", ret);

		// 尾部出入\0
		outbuf[ret] = '\0';	
		printf("%s\n", outbuf);
			
	}
	
	close(sockfd);
	
	return 0;
}
