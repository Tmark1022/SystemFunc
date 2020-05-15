/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 10:44:05 AM CST
 @ File Name	: call_system_server.c
 @ Description	: 接受客户端的输入并调用system
 ************************************************************************/
#include "wrap/wrap.h"
#include <sys/socket.h>

char buf[1024];

void DoReadWrite(int cfd) 
{	
	// 重定向标准输出到连接到的client	
	if ( -1 == dup2(cfd, STDOUT_FILENO)) {
		perror("dup2 error");
		exit(EXIT_FAILURE);
	}

	int cnt, status;
	while ((cnt = ReadLine(cfd, buf, 1023)) > 0) {
		// 去掉尾部\n,并改成'\0'
		buf[cnt - 1] = '\0';
		status = system(buf);
		fprintf(stderr, "read cnt is %d\nget string :%s\nreturn status is %d\n", cnt, buf, status);	
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

