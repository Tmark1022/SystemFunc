/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 10 May 2020 06:18:21 PM CST
 @ File Name	: echo_server.c
 @ Description	: 简单的echo服务器
 ************************************************************************/
#include "wrap/wrap.h"
#include <sys/socket.h>

char buf[1024];

void DoReadWrite(int cfd) 
{
	int cnt;
	while ((cnt = ReadLine(cfd, buf, 1023)) > 0) {
		buf[cnt] = '\0'; 
		printf("read cnt is %d\n, get string :%s\n", cnt, buf);	
		WriteCount(cfd, buf, cnt);
	}
}

int main(int argc, char * argv[]) 
{
	int port = 8888;
	if (argc == 2) {
		port = atoi(argv[1]);
	}

	int fd = Socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	Bind(fd, (struct sockaddr *)&bindAddr, sizeof(bindAddr));  	

	Listen(fd, SOMAXCONN);

	socklen_t addrlen;
	struct sockaddr_in clientAddr;
	int cfd = Accept(fd, (struct  sockaddr * )&clientAddr, &addrlen);
	
	DoReadWrite(cfd);

	close(cfd);	
	close(fd);

	return 0;
}
