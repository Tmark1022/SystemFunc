/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 25 May 2020 09:39:10 PM CST
 @ File Name	: udp_client.c
 @ Description	: udp通信客户端
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "wrap/wrap.h"


int main(int argc, char *argv[]) {
	
	int cfd = Socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &svrAddr.sin_addr);

	struct sockaddr_in tmpAddr;
	socklen_t len = sizeof(tmpAddr);
	char buf[1024];

	while (1) {
		fgets(buf, 1024, stdin);

		int cnt = sendto(cfd, buf, strlen(buf), 0, (struct sockaddr * )&svrAddr, sizeof(svrAddr));
		if (-1 == cnt) {
			PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
		}

		cnt = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&tmpAddr, &len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call recvfrom failed", EXIT_FAILURE);		
		}
		PrintAddr(stdout,  &tmpAddr, "recv data from");
		buf[cnt] = '\0';		
		printf("%s", buf);
	}

	close(cfd);	

	return 0;
}
