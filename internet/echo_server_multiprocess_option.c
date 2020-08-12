/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 12 Aug 2020 04:42:47 PM CST
 @ File Name	: echo_server_multiprocess_option.c
 @ Description	: 
 ************************************************************************/
#include "wrap/wrap.h"
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/***************************************************
* global variable, marco 
***************************************************/
int port = 8888;
char * ip_addr = "127.0.0.1";
int reuse_addr = 0;

/***************************************************
* other 
***************************************************/
void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:vR")) != -1) {
		switch (opt) {
			case 'h':
				ip_addr = optarg; 
				break;
			case 'p':
				port = atoi(optarg);
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
	HandleOpt(argc, argv); 
	int fd = Socket(AF_INET, SOCK_STREAM, 0);

	// 端口复用
	if (reuse_addr) {
		printf("reuse address \n");
		int opt = 1;
		if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}


	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);

	struct in_addr tmpAddr; 
	int ret  = inet_pton(AF_INET, ip_addr, &tmpAddr);
	if (-1 == ret) {
		PrintError(stderr, 0, "inet_pton error", EXIT_FAILURE);				
	} else if (0 == ret) {
		PrintError(stderr, 0, "inet_pton error, invalid ip", EXIT_FAILURE);				
	}	
	bindAddr.sin_addr.s_addr = tmpAddr.s_addr;	

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
