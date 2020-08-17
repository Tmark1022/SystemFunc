/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 17 Aug 2020 02:23:01 PM CST
 @ File Name	: client_normal_udp_select.c
 @ Description	: udp socket connect 时 捕捉异步错误（icmp port unreachable）
 ************************************************************************/
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <netinet/in.h>
#include <signal.h>
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
#include <sys/signal.h>

/***************************************************
* global variable, marco 
***************************************************/
#define MAX_BUF_SIZE 1024
#define max(a, b) (a) >= (b) ? (a) : (b)

int port = 8888;
char * ip_addr = "127.0.0.1";

int sockfd = -1;
int status = 0;
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
	while ((opt = getopt(argc, argv, "h:p:")) != -1) {
		switch (opt) {
			case 'h':
				ip_addr = optarg; 
				break;
			case 'p':
				port = atoi(optarg);
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

void do_connect(int cfd)
{
	struct sockaddr_in svrAddr = construct_sockaddr_in(ip_addr, port);
	if (-1 == connect(cfd, (const struct sockaddr *)&svrAddr, sizeof(struct sockaddr_in))) {
			perror("connect  error");	
			exit(1);	
	}

	status = 1;
	printf("udp socket connect successfully\n");
}

void disconnect(int cfd)
{	
	struct sockaddr_in svrAddr;
	bzero(&svrAddr, sizeof(struct sockaddr_in));
	svrAddr.sin_family = AF_UNSPEC;
	if (-1 == connect(cfd, (const struct sockaddr *)&svrAddr, sizeof(struct sockaddr_in))) {
			perror("connect  error");	
			exit(1);	
	}
	status = 0;
	printf("udp socket disconnect successfully\n");
}

void sig_int(int signo)
{
	if (status) {
		disconnect(sockfd);
	} else {
		do_connect(sockfd);
	}
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
	
		// udp socket
		if (FD_ISSET(cfd, &readfds)) {

			struct sockaddr_in cliAddr;
			bzero(&cliAddr, sizeof(struct sockaddr_in));
			socklen_t len = sizeof(cliAddr);
			int cnt = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&cliAddr, &len);
			if (-1 == cnt) {
				PrintError(stderr, 0, "call recvfrom failed", EXIT_FAILURE);		
			}

			write(STDOUT_FILENO, buf, cnt);
		}

		// stdin 
		if (FD_ISSET(stdin_fileno, &readfds)) {
			int cnt = read(stdin_fileno, buf, MAX_BUF_SIZE);
			if (cnt == -1) {
				if (EINTR != errno) {
					PrintError(stderr, 0, "read failed", EXIT_FAILURE);		
				} else {
					continue;
				}
			} else if (cnt == 0) {
				// eof
				return ;
			}				
			
				
			// send data to peer
			printf("status %d\n", status);
			if (status) {
				// connect 
				int ret = write(cfd, buf, cnt);
				if (-1 == ret) {
					PrintError(stderr, 0, "call write failed..", EXIT_FAILURE);		
				}
			} else {
				// disconnect
				struct sockaddr_in svrAddr = construct_sockaddr_in(ip_addr, port);
				socklen_t len = sizeof(svrAddr);
				int ret = sendto(cfd, buf, cnt, 0, (struct sockaddr * )&svrAddr, len);
				if (-1 == ret) {
					PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
				}
			}	
		}

	}
}

int main(int argc, char *argv[]) {
	HandleOpt(argc, argv);
	status = 0;
	signal(SIGINT, sig_int);

	// 客户端socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sockfd) {
		PrintError(stderr, 0, "socket failed", EXIT_FAILURE);		
	}
			
	str_cli(stdin, sockfd);
	close(sockfd);
	return 0;
}
