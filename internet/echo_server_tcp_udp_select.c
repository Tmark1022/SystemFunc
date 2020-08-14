/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 14 Aug 2020 02:44:44 PM CST
 @ File Name	: echo_server_tcp_udp_select.c
 @ Description	: 使用select组合 tcp 和 udp echo 服务器
 ************************************************************************/
#include "wrap/wrap.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/select.h>
#include <errno.h>

/***************************************************
* global variables or marco
***************************************************/
#define max(a,b) (a) >= (b) ? (a) : (b)
#define MAX_TCP_CLIENT	1024
#define MAX_LINE	1024
fd_set allRSet; 
int max_fd = -1;
int client_list[MAX_TCP_CLIENT];
int max_idx = -1;

/***************************************************
* getopt variables 
***************************************************/
int tcp_recv_buf = 0;
int tcp_send_buf = 0;

int port = 8888;
char * ip_addr = "0.0.0.0";

int udp_flag = 0;
int all_open_flag = 0;

int reuse_addr = 0;


/***************************************************
* other 
***************************************************/
void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:s:r:Rua")) != -1) {
		switch (opt) {
			case 'h':
				ip_addr = optarg; 
				break;
			case 'p':
				port = atoi(optarg);
				break;	
			case 'R':
				reuse_addr = 1;	
				break;
			case 's':
				tcp_send_buf = atoi(optarg);
				break;
			case 'r':
				tcp_recv_buf = atoi(optarg);
				break;
			case 'u':
				udp_flag = 1;	
				break;
			case 'a':
				all_open_flag = 1;	
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

int init_tcp_socket()
{
	int fd = Socket(AF_INET, SOCK_STREAM, 0);

	// 端口复用
	if (reuse_addr) {
		int opt = 1;
		if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}
	
	// 发送，接收缓冲区, tcp建立好的连接继承于listenfd
	set_socket_buf_value(fd); 
	print_socket_buf_value(fd);

	struct sockaddr_in svrAddr;
	bzero(&svrAddr, sizeof(struct sockaddr_in));
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
	
	printf("init tcp socket successfully.\n");

	return fd;
}

int init_udp_socket()
{
	int fd = Socket(AF_INET, SOCK_DGRAM, 0);

	// 端口复用
	if (reuse_addr) {
		int opt = 1;
		if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}
	
	// 发送，接收缓冲区, tcp建立好的连接继承于listenfd
	set_socket_buf_value(fd); 
	print_socket_buf_value(fd);

	struct sockaddr_in svrAddr;
	bzero(&svrAddr, sizeof(struct sockaddr_in));
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
	
	printf("init udp socket successfully.\n");

	return fd;
}

void add_to_fd_set(int fd) 
{
	FD_SET(fd, &allRSet);	
	if (fd > max_fd) {
		max_fd = fd;
	}
}

void remove_from_fd_set(int fd) 
{
	FD_CLR(fd, &allRSet);	
}

void handler_tcp_new_connect(int listen_fd)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));

	int cfd = Accept(listen_fd, (struct  sockaddr * )&clientAddr, &addrlen);
	int idx = 0;
	for (; idx < MAX_TCP_CLIENT; ++idx) {
		if (client_list[idx] < 0) {
			break;
		}
	}	

	if ( idx >= MAX_TCP_CLIENT) {
		PrintError(stderr, EMFILE, "exceed FD_SETSIZE", 0);		
		close(cfd);	
		return ;
	}
	client_list[idx] = cfd;
	if (idx > max_idx) {
		max_idx = idx;
	}

	add_to_fd_set(cfd); 
}	

void close_tcp_connect(int cfd) 
{
	remove_from_fd_set(cfd);
	for(int idx = 0; idx < MAX_TCP_CLIENT; ++idx) {
		if (cfd == client_list[idx]) {
			client_list[idx] = -1;
			break;
		}
	}
	close(cfd);
	printf("close tcp fd %d\n", cfd);
}

void handler_tcp_read(int cfd)
{
	char buf[MAX_LINE];
	int cnt = read(cfd, buf, MAX_LINE);
	if (-1 == cnt) {
		if (EINTR == errno) {
			return ;
		} else if (ECONNRESET == errno) {
			// RST
			PrintError(stderr, 0, "read failed", 0);		
			close_tcp_connect(cfd);	
		} else {
			PrintError(stderr, 0, "read failed", EXIT_FAILURE);		
		}
	} else if (0 == cnt) {
		// eof
		close_tcp_connect(cfd);	
	} else {
		printf("get %d  bytes data in tcp (fd %d)\n", cnt, cfd);
		Write(cfd, buf, cnt);
	}
}

void handler_udp_read(int fd)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));

	char buf[MAX_LINE];
	int cnt = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &addrlen);
	if (-1 == cnt) {
		PrintError(stderr, 0, "call recvfrom failed", EXIT_FAILURE);		
	}

	char arr[INET_ADDRSTRLEN];	
	inet_ntop(AF_INET, &(clientAddr.sin_addr), arr, sizeof(arr)); 
	printf("udp, recv %d bytes data from %s:%d\n", cnt, arr, ntohs(clientAddr.sin_port));

	if (-1 == sendto(fd, buf, cnt, 0, (struct sockaddr * )&clientAddr, addrlen)) {
		PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
	}
}	

int main(int argc, char * argv[]) 
{
	// signal(SIGINT, sig_int);
	HandleOpt(argc, argv);

	int tcp_socket = 0;
	int udp_socket = 0;
	if (all_open_flag) {
		tcp_socket = init_tcp_socket();
		udp_socket = init_udp_socket();
	} else if (udp_flag) {
		udp_socket = init_udp_socket();
	} else {
		tcp_socket = init_tcp_socket();
	}

	// init client_list 
	for (int idx = 0; idx < MAX_TCP_CLIENT; ++idx) {
		client_list[idx] = -1;
	}
	
	// init all_set 
	FD_ZERO(&allRSet);
	if (tcp_socket) {
		add_to_fd_set(tcp_socket);
	}
	if (udp_socket) {
		add_to_fd_set(udp_socket);
	}



	fd_set runRSet; 
	while (1) {
		runRSet = allRSet;
		int nreadys = select(max_fd + 1,  &runRSet, NULL, NULL, NULL);	
		if (-1 == nreadys) {
			if (EINTR == errno) {
				continue;
			} else {
				PrintError(stderr, 0, "call select failed", EXIT_FAILURE);		
			}
		}	
		
		// tcp 监听套接字
		if (tcp_socket && FD_ISSET(tcp_socket, &runRSet)) {
			--nreadys;	
			handler_tcp_new_connect(tcp_socket);
		}

		if (nreadys <= 0) {
			continue;
		}
		
		// tcp 建立的连接
		if (tcp_socket) {
			for (int idx = 0; idx <= max_idx; ++idx) {
				int cfd = client_list[idx];
				if (cfd < 0) {
					continue;
				}

				if (FD_ISSET(cfd, &runRSet)) {
					--nreadys;	
					handler_tcp_read(cfd);	
				}
			}

		}
	
		if (nreadys <= 0) {
			continue;
		}

		// udp
		if (udp_socket && FD_ISSET(udp_socket, &runRSet)) {
			--nreadys;	
			handler_udp_read(udp_socket);
		}

	}
	
	return 0;
}

