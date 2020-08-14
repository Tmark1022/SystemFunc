/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 10 Aug 2020 01:38:36 PM CST
 @ File Name	: client_normal_select.c
 @ Description	: 
 ************************************************************************/
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <math.h>
#include <fcntl.h>
#include <netinet/tcp.h>

/***************************************************
* global variable, marco 
***************************************************/
#define MAX_BUF_SIZE 1024
#define max(a, b) (a) >= (b) ? (a) : (b)

int port = 8888;
char * ip_addr = "127.0.0.1";

int bind_port = 0;
char * bind_ip_addr = NULL;

int reuse_addr = 0;

int source_socket = 0; 
unsigned int tcp_send_buf = 0, tcp_recv_buf= 0;

/***************************************************
* other 
***************************************************/
void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode)
{

#if (-1 == my_errno)
	fprintf(stream, "%s\n", headStr);	
#else 
	if (!my_errno) {
		my_errno = errno;
	}	
	fprintf(stream, "%s : %s\n", headStr, strerror(my_errno));	
#endif
	if (exitCode) {
		exit(exitCode);	
	}
}

void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:s:b:vriR:S:")) != -1) {
		switch (opt) {
			case 'h':
				ip_addr = optarg; 
				break;
			case 'p':
				port = atoi(optarg);
				break;	
			case 's':
				bind_ip_addr = optarg; 
				break;
			case 'b':
				bind_port = atoi(optarg);
				break;	
			case 'v':
               		    fprintf(stdout, "Usage: %s [-h ip][-p port][-s src_addr][-b src_port]\n", argv[0]);
               		    exit(EXIT_SUCCESS);
			case 'r':
				reuse_addr = 1;
				break;	
			case 'i':
				source_socket = 1;
				break;
			case 'S':
				tcp_send_buf = atoi(optarg);	
				break;
			case 'R':
				tcp_recv_buf = atoi(optarg);	
				break;
               		default: 
               		    fprintf(stderr, "Usage: %s [-h ip][-p port][-s src_addr][-b src_port]\n", argv[0]);
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

void print_socket_tcp_mss_value(int fd) 
{
	int mss;
	socklen_t len;
	if (-1 == getsockopt(fd, IPPROTO_TCP, TCP_MAXSEG, &mss, &len)) {
		PrintError(stderr, 0, "call getsockopt failed", EXIT_FAILURE);		
	}	
	printf("fd (%d), mss is %d\n", fd, mss);
}


struct sockaddr_in  construct_sockaddr_in(const char * addr, int p)
{
	struct sockaddr_in svrAddr;
	bzero(&svrAddr, sizeof(struct sockaddr_in));
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(p);

	struct in_addr tmpAddr; 
	int ret  = inet_pton(AF_INET, addr, &tmpAddr);
	if (-1 == ret) {
		PrintError(stderr, 0, "inet_pton error", EXIT_FAILURE);				
	} else if (0 == ret) {
		PrintError(stderr, 0, "inet_pton error, invalid ip", EXIT_FAILURE);				
	}	
	svrAddr.sin_addr.s_addr = tmpAddr.s_addr;

	return svrAddr;
}

int  do_connect()
{
	// 构造服务器的地址struct
	struct sockaddr_in svrAddr = construct_sockaddr_in(ip_addr, port);

	// 客户端socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		PrintError(stderr, 0, "socket failed", EXIT_FAILURE);		
	}
		
	if (reuse_addr) {
		// 端口复用
		int opt = 1;
		if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}
	
	// 连接前
	set_socket_buf_value(sockfd);
	print_socket_buf_value(sockfd);
	print_socket_tcp_mss_value(sockfd);

	
	if (bind_ip_addr != NULL && port != 0) {
		struct sockaddr_in bindAddr = construct_sockaddr_in(bind_ip_addr, bind_port);
		if (-1 == bind(sockfd, (struct sockaddr *)&bindAddr, sizeof(struct sockaddr_in))) {
			perror("bind error");	
			exit(1);
		}
	}	

	if (-1 == connect(sockfd, (const struct sockaddr *)&svrAddr, sizeof(struct sockaddr_in))) {
			perror("connect  error");	
			exit(1);	
	}
	
	// 连接后
	print_socket_buf_value(sockfd);
	print_socket_tcp_mss_value(sockfd);

	return sockfd;
}

void str_cli(FILE *fp, int cfd)
{
	char buf[MAX_BUF_SIZE];
	fd_set all_read_set;
	FD_ZERO(&all_read_set);		
	int stdin_fileno = fileno(fp);
	FD_SET(stdin_fileno, &all_read_set);
	FD_SET(cfd, &all_read_set);
	
	
	while (1) {
		int nfds = max(stdin_fileno, cfd) + 1; 
		fd_set readfds  = all_read_set;
		int ret = select(nfds, &readfds, NULL, NULL, NULL);
		if (-1 == ret) {
			if (EINTR == errno) {
				continue;
			} else {
				PrintError(stderr, 0, "select failed", EXIT_FAILURE);		
			}
		}
	
		if (FD_ISSET(cfd, &readfds)) {
			int cnt = read(cfd, buf, MAX_BUF_SIZE);
			if (cnt == -1) {
				if (EINTR != errno) {
					PrintError(stderr, 0, "read failed", EXIT_FAILURE);		
				} else {
					continue;
				}
			} else if (cnt == 0) {
				printf("get FIN\n");
				return ;	
			}				

			write(STDOUT_FILENO, buf, cnt);
		}


		if (FD_ISSET(stdin_fileno, &readfds)) {
			int cnt = read(stdin_fileno, buf, MAX_BUF_SIZE);
			if (cnt == -1) {
				if (EINTR != errno) {
					PrintError(stderr, 0, "read failed", EXIT_FAILURE);		
				} else {
					continue;
				}
			} else if (cnt == 0) {
				// eof, shutdown	
				FD_CLR(stdin_fileno, &all_read_set);

				printf("do, shutdown\n");
				shutdown(cfd, SHUT_WR);		// send FIN, 当服务器也调用close时， 客户端就会接收到服务器发出的在四次挥手过程中的第二个FIN， 完成四次挥手过程, 对应就是客户端read 返回0
				continue;	
			}				

			write(cfd, buf, cnt);
		}

	}
}

void source_socket_handler(int cfd) 
{
	char source_buf[1024];
	for(int i = 0; i < 1024; ++i) {
		source_buf[i] = '1';
	}
	int cnt = 1000;	
	int interval = 100000;

	// 设置非阻塞
	int flag = fcntl(cfd, F_GETFL);
	flag |=  O_NONBLOCK;
	fcntl(cfd, F_SETFL, flag);	

	while (1) {		
		int ret = write(cfd, source_buf, cnt);
		if (-1 == ret) {
			if (EAGAIN == errno || EWOULDBLOCK == errno) {
				printf("write block\n");	
			} else {
				perror("write error");
				exit(1);
			}
		}

		printf("write cnt : %d, suc : %d\n", cnt, ret);
		usleep(interval);	
	}	
}

int main(int argc, char *argv[]) {
	HandleOpt(argc, argv);
	int cfd = do_connect();	
	
	if (source_socket)  {
		source_socket_handler(cfd);
	} else {
		str_cli(stdin, cfd);
	}

	close(cfd);
	return 0;
}
