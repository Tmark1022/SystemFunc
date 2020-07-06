/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 06 Jul 2020 02:15:00 PM CST
 @ File Name	: select_wait_signal.c
 @ Description	: select echo 服务器， 不过等待
 ************************************************************************/
#include "wrap/wrap.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/select.h>
#include <errno.h>

#define REUSE_ADDR 

/***************************************************
* global variables
***************************************************/
unsigned int read_flag = 0;
int tcp_recv_buf = 0;
int tcp_send_buf = 0;
int port = 8888;
char * ip_addr = "0.0.0.0";

char buf[1024];

void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:s:r:")) != -1) {
		switch (opt) {
			case 'h':
				ip_addr = optarg; 
				break;
			case 'p':
				port = atoi(optarg);
				break;	
			case 's':
				tcp_send_buf = atoi(optarg);
				break;
			case 'r':
				tcp_recv_buf = atoi(optarg);
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

void sig_int(int signo) 
{	
	read_flag = ~read_flag;
	printf("read flag is %u\n", read_flag);
}

void set_socket_buf_value(int fd) 
{
	socklen_t recv_len, send_len; 
	recv_len = send_len = sizeof(int);
	if (tcp_recv_buf > 0) {
		if (-1 == setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &tcp_recv_buf, recv_len)) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}
	
	if (tcp_send_buf > 0) {
		if (-1 == setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &tcp_send_buf, send_len)) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}	
}

void print_socket_buf_value(int fd) 
{
	int recv_value, send_value;
	socklen_t recv_len, send_len; 
	recv_len = send_len = sizeof(int);
	if (-1 == getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &recv_value, &recv_len)) {
		PrintError(stderr, 0, "call getsockopt failed", EXIT_FAILURE);		
	}
	if (-1 == getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &send_value, &send_len)) {
		PrintError(stderr, 0, "call getsockopt failed", EXIT_FAILURE);		
	}
	
	printf("fd (%d), send buf is %d, recv buf is %d\n", fd, send_value, recv_value);
}

int main(int argc, char * argv[]) 
{
	signal(SIGINT, sig_int);

	HandleOpt(argc, argv);
	int fd = Socket(AF_INET, SOCK_STREAM, 0);

#ifdef REUSE_ADDR
	// 端口复用
	int opt = 1;
	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
	}
#endif
	
	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(port);
	struct in_addr tmpAddr; 
	int ret  = inet_pton(AF_INET, ip_addr, &tmpAddr);
	if (-1 == ret) {
		PrintError(stderr, 0, "inet_pton error", EXIT_FAILURE);				
	} else if (0 == ret) {
		PrintError(stderr, 0, "inet_pton error, invalid ip", EXIT_FAILURE);				
	}	
	svrAddr.sin_addr.s_addr = tmpAddr.s_addr;
	Bind(fd, (struct sockaddr *)&svrAddr, sizeof(svrAddr));  	
	Listen(fd, SOMAXCONN);

	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));

	// 将listen fd 加入监听	
	int maxFd = fd;
	fd_set allRSet, runRSet; 
	FD_ZERO(&allRSet);
	FD_SET(fd, &allRSet);

	while (1) {
		runRSet = allRSet;
		ret = select(maxFd + 1,  &runRSet, NULL, NULL, NULL);	

		if (0 == ret) {
			// 超时， 如果设置timeout
			continue; 
		} else if (-1 == ret) {
			if ( EINTR == errno) {
				// 信号中断了慢速系统调用
				continue; 
			} else {
				PrintError(stderr, 0, "call select failed", EXIT_FAILURE);		
			}
		}

		for(int i = 0; i < FD_SETSIZE; ++i) {

			if (FD_ISSET(i, &runRSet)) {
				--ret;	

				if (i == fd) {
					// 处理新的链接
					int cfd = Accept(fd, (struct  sockaddr * )&clientAddr, &addrlen);

					// 超过select 支持的最大连接数 
					if ( cfd >= FD_SETSIZE) {
						PrintError(stderr, EMFILE, "exceed FD_SETSIZE", 0);		
						close(cfd);	
						continue;
					}

					if (cfd > maxFd) {
						maxFd = cfd;
					}
					FD_SET(cfd, &allRSet);	
					set_socket_buf_value(cfd); 
					print_socket_buf_value(cfd);
				} else {

					if (!read_flag) {
						//printf("read flag is 0\n");
						continue;
					} else {
						//printf("do it\n");
					}

					// 读数据
					int cnt = Read(i, buf, 1024);
					if (cnt > 0) {
						printf("handle %d data\n", cnt);
						Write(i, buf, cnt);
					} else if (0 == cnt) {
						// 关闭连接		
						FD_CLR(i, &allRSet);
						close(i);
					}
				}	
			}


			if (ret <= 0) {
				break;
			}
		}
	}
	
	close(fd);

	return 0;
}

