/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 10 Aug 2020 01:38:36 PM CST
 @ File Name	: client_normal_select.c
 @ Description	: 
 ************************************************************************/
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
	while ((opt = getopt(argc, argv, "h:p:s:b:vR")) != -1) {
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
			case 'R':
				reuse_addr = 1;
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
	
	if (bind_ip_addr != NULL && port != 0) {
		struct sockaddr_in bindAddr = construct_sockaddr_in(bind_ip_addr, bind_port);
		bind(sockfd, (struct sockaddr *)&bindAddr, sizeof(struct sockaddr_in));
	}	

	connect(sockfd, (const struct sockaddr *)&svrAddr, sizeof(struct sockaddr_in));

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



int main(int argc, char *argv[]) {
	HandleOpt(argc, argv);
	int cfd = do_connect();	
	str_cli(stdin, cfd);
	close(cfd);
	return 0;
}
