/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 11:30:39 AM CST
 @ File Name	: test_struct_server.c
 @ Description	: 
 ************************************************************************/
#pragma pack(4)

#include "wrap/wrap.h"
#include "test_struct.h"

void DoReadWrite(int cfd) 
{
	int cnt;
	Proto pto;
	/*
	while ((cnt = ReadCount(cfd, buf, 1024)) > 0) {
		printf("read cnt is %d\n, get string :%s\n", cnt, buf);	
		WriteCount(cfd, buf, cnt);
	}
	*/
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


