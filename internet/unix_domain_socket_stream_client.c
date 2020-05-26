/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 26 May 2020 10:44:58 AM CST
 @ File Name	: unix_domain_socket_stream_client.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>

#include "wrap/wrap.h"

#define SERVER_SOCKET_FILE "/tmp/9Lq7BNBnBycd6nxy_server.socket"
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

	cfd = Socket(AF_UNIX, SOCK_STREAM, 0);

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

	Connect(cfd, (const struct sockaddr *)&svrAddr, sizeof(svrAddr));

	char buf[1024];
	
	while (1) {
		fgets(buf, 1024, stdin);
		Write(cfd, buf, strlen(buf));

		int cnt = Read(cfd, buf, 1023);
		buf[cnt] = '\0';
		printf("%s", buf);
	}
		
	close(cfd);	

	// 通信完后才删掉， 不然通信过程中找不到
	unlink(cliSockPath);

	return 0;
}
