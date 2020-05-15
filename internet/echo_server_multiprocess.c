/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 15 May 2020 04:18:41 PM CST
 @ File Name	: echo_server_multiprocess.c
 @ Description	: 多进程回显服务器
 ************************************************************************/
#include "wrap/wrap.h"
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define REUSE_ADDR

char buf[1024];


void sig_chld(int signo)
{
	int childPid;
	while((childPid = waitpid(-1, NULL, WNOHANG)) > 0) {
		printf("wait child process %d\n", childPid);
	}
}

void DoReadWrite(int cfd) 
{
	int cnt;
	while ((cnt = ReadLine(cfd, buf, 1023)) > 0) {
		buf[cnt] = '\0'; 
		printf("pid(%d), read cnt is %d\n, get string :%s\n", getpid(), cnt, buf);	
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

#ifdef REUSE_ADDR
	// 端口复用
	int opt = 1;
	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
	}
#endif

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	Bind(fd, (struct sockaddr *)&bindAddr, sizeof(bindAddr));  	

	Listen(fd, SOMAXCONN);

	socklen_t addrlen;
	struct sockaddr_in clientAddr;
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));

	// 信号回收子进程
	signal(SIGCHLD, sig_chld);

	int cfd;
	int pid;	
	while (1) {	
		cfd = Accept(fd, (struct  sockaddr * )&clientAddr, &addrlen);

		pid = fork();
		if (-1 == pid) {
			// error
			PrintError(stderr, 0, "fork error", EXIT_FAILURE);			
		} else if (0 == pid) {
			// child
			close(fd);		// 关闭监听套接字		
			DoReadWrite(cfd);	
			exit(EXIT_SUCCESS);
		} else {
			// parent;
			close(cfd);
		}	
	}
	
	close(cfd);	
	close(fd);

	return 0;
}
