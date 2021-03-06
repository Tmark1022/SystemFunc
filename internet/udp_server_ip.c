/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 25 May 2020 07:28:37 PM CST
 @ File Name	: udp.c
 @ Description	: 简单udp通信， 参数指定IP, 测试限定local address不同的效果和端口复用
 ************************************************************************/
#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "wrap/wrap.h"
/***************************************************
* global variable, marco 
***************************************************/
char * ip_addr = "127.0.0.1";
int reuse_addr = 0;

/***************************************************
* other 
***************************************************/
void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:vR")) != -1) {
		switch (opt) {
			case 'h':
				ip_addr = optarg; 
				break;
			case 'v':
               		    fprintf(stdout, "Usage: %s [-h ip][-p port]\n", argv[0]);
               		    exit(EXIT_SUCCESS);
			case 'R':
				reuse_addr = 1;
				break;	
               		default: 
               		    fprintf(stderr, "Usage: %s [-h ip][-p port]\n", argv[0]);
               		    exit(EXIT_FAILURE);
               	}
	}
	
	/*
	if (optind >= argc) {
		fprintf(stderr, "need more argument\n");
		exit(EXIT_FAILURE);
	}
	*/
}


int main(int argc, char *argv[]) {

	HandleOpt(argc, argv);
	
	int lfd = Socket(AF_INET, SOCK_DGRAM, 0);


	if (reuse_addr) {
		// 端口复用 (对于udp， SO_REUSEADDR 能重复绑定完全相同的ip和port对, 参考unp 7.5.11)
		int opt = 1;
		if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}

	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(8888);
	struct in_addr tmpAddr; 
	int ret  = inet_pton(AF_INET, ip_addr, &tmpAddr);
	if (-1 == ret) {
		PrintError(stderr, 0, "inet_pton error", EXIT_FAILURE);				
	} else if (0 == ret) {
		PrintError(stderr, 0, "inet_pton error, invalid ip", EXIT_FAILURE);				
	}	
	svrAddr.sin_addr.s_addr = tmpAddr.s_addr;

	Bind(lfd, (struct sockaddr *)&svrAddr, sizeof(svrAddr)); 

	struct sockaddr_in cliAddr;
	socklen_t len = sizeof(cliAddr);

	// 需要注意udp会截断数据包， 并丢弃剩余的部分。
	char buf[50];
	//char buf[1024];

	while (1) {
		int cnt = recvfrom(lfd, buf, sizeof(buf), 0, (struct sockaddr *)&cliAddr, &len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call recvfrom failed", EXIT_FAILURE);		
		}
		PrintAddr(stdout,  &cliAddr, "new client");
		printf("i am %s, recv %d bytes data\n", ip_addr, cnt);

		cnt = sendto(lfd, buf, cnt, 0, (struct sockaddr * )&cliAddr, len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
		}
		printf("i am %s, send %d bytes data\n", ip_addr, cnt);
	}

	close(lfd);	

	return 0;
}
