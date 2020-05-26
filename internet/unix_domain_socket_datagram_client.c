/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 26 May 2020 11:18:46 AM CST
 @ File Name	: unix_domain_socket_datagram_client.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#include "wrap/wrap.h"

#define SERVER_SOCKET_FILE "/tmp/9Lq7BNBnBycd6nxy_dgram_server.socket"

char cliSockPath[108];
int cfd;

void sig_int(int signo) 
{
	close(cfd);	
	// 通信完后才删掉， 不然通信过程中找不到
	unlink(cliSockPath);
	printf("exit success\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

	srand(time(NULL));
	sprintf(cliSockPath, "/tmp/9Lq7BNBnBycd6nxy_client_%d.socket", rand());

	cfd = Socket(AF_UNIX, SOCK_DGRAM, 0);

	signal(SIGINT, sig_int);
	signal(SIGQUIT, sig_int);

	struct sockaddr_un svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sun_family = AF_UNIX; 
	strcpy(svrAddr.sun_path, SERVER_SOCKET_FILE);

	struct sockaddr_un cliAddr;
	memset(&cliAddr, 0, sizeof(cliAddr));
	cliAddr.sun_family = AF_UNIX; 
	strcpy(cliAddr.sun_path, cliSockPath);

	unlink(cliSockPath);
	Bind(cfd, (struct sockaddr *)&cliAddr, sizeof(cliAddr)); 
	char buf[1024];

	struct sockaddr_un tmpAddr;
	memset(&tmpAddr, 0, sizeof(tmpAddr));
	socklen_t len = sizeof(tmpAddr);
	
	while (1) {
		fgets(buf, 1024, stdin);

		ssize_t cnt = sendto(cfd, buf, strlen(buf), 0, (struct sockaddr *)&svrAddr, sizeof(svrAddr));
		if (-1 == cnt) {
			char tmp[100];
			sprintf(tmp, "sendto error, errno %d", errno);
			PrintError(stderr, 0, tmp, EXIT_FAILURE);	
		}		

		len = sizeof(tmpAddr);
		cnt = recvfrom(cfd, buf, 1023, 0, (struct sockaddr *)&tmpAddr, &len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "recvfrom error", EXIT_FAILURE);	
		}	
		buf[cnt] = '\0'; 
		printf("recv data, sock file : %s, %d\n%s", tmpAddr.sun_path, len, buf);
	}
	
	close(cfd);	

	// 通信完后才删掉， 不然通信过程中找不到
	unlink(cliSockPath);

	return 0;
}
