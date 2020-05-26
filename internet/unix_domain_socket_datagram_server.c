/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 26 May 2020 11:06:05 AM CST
 @ File Name	: unix_domain_socket_datagram_server.c
 @ Description	: 本地套接字通信 数据包（udp）
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "wrap/wrap.h"

#define SERVER_SOCKET_FILE "/tmp/9Lq7BNBnBycd6nxy_dgram_server.socket"

int main(int argc, char *argv[]) {

	int lfd = Socket(AF_UNIX, SOCK_DGRAM, 0);

	struct sockaddr_un svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sun_family = AF_UNIX; 
	strcpy(svrAddr.sun_path, SERVER_SOCKET_FILE);

	unlink(SERVER_SOCKET_FILE);
	Bind(lfd, (struct sockaddr *)&svrAddr, sizeof(svrAddr)); 

	struct sockaddr_un cliAddr;
	socklen_t len = sizeof(cliAddr);
	char buf[1024];

	while (1) {
		memset(&cliAddr, 0, sizeof(cliAddr));
		
		// TODO
		// 本地套接字这里需要重置长度， 不然上一个连接可能返回一个小的值(sun_path短一些)，然后下一个使用就会有问题	
		len = sizeof(cliAddr);
		ssize_t cnt = recvfrom(lfd, buf, sizeof(buf), 0, (struct sockaddr *)&cliAddr, &len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "recvfrom error", EXIT_FAILURE);	
		}	
		printf("new data, sock file : %s, %d\n", cliAddr.sun_path, len);

		cnt = sendto(lfd, buf, cnt, 0, (struct sockaddr *)&cliAddr, len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "sendto error", EXIT_FAILURE);	
		}		
	}

	close(lfd);	

	// 通信完后才删掉， 不然通信过程中找不到
	unlink(SERVER_SOCKET_FILE);

	return 0;
}
