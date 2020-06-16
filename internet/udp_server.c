/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 25 May 2020 07:28:37 PM CST
 @ File Name	: udp.c
 @ Description	: 简单udp通行
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "wrap/wrap.h"


int main(int argc, char *argv[]) {
	
	int lfd = Socket(AF_INET, SOCK_DGRAM, 0);

	// 端口复用
	int opt = 1;
	if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
	}

	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(8888);
	svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
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
		printf("recv %d bytes data\n", cnt);

		cnt = sendto(lfd, buf, cnt, 0, (struct sockaddr * )&cliAddr, len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
		}
		printf("send %d bytes data\n", cnt);
	}

	close(lfd);	

	return 0;
}
