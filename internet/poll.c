/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 18 May 2020 07:36:20 PM CST
 @ File Name	: poll.c
 @ Description	: poll 实现高并发echo服务器
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <errno.h>

#include "wrap/wrap.h"

#define OPEN_MAX 1024

char buf[1024];

int main(int argc, char *argv[]) {

	int defaultPort = 8888; 
	if (2 == argc) {
		defaultPort = atoi(argv[1]);
	}
	
	int fd = Socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;	
	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		PrintError(stderr, 0, "get sockopt error", EXIT_FAILURE);
	}

	struct sockaddr_in serverAddr, tmpAddr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(defaultPort);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

	Listen(fd, SOMAXCONN);
	
	struct pollfd fds[OPEN_MAX];
	for (int i = 0; i < OPEN_MAX; ++i) {
		fds[i].fd = -1;
	}

	nfds_t nfds = 0;
	fds[0].fd = fd;	
	fds[0].events = POLLIN;
	++nfds;	

	
	while(1) {
		// 设置阻塞形式， 返回值不会是0
		int nready = poll(fds, nfds, -1);

		if (-1 == nready) {
			if (EINTR == errno) {
				continue;
			} else {
				PrintError(stderr, 0, "call poll error", EXIT_FAILURE);	
			}
		}
		
		// 先处理 listen
		if (fds[0].revents & POLLIN) {
			--nready;
			int cfd = Accept(fds[0].fd, (struct sockaddr *)&tmpAddr, &addrlen);
			int i;	
			for (i = 1; i < OPEN_MAX; ++i) {
				if (fds[i].fd < 0) {
					break;
				}	
			}

			if (i >= OPEN_MAX) {
				PrintError(stderr, EMFILE, "too many open file", 0);	
				close(cfd);
			}
			
			fds[i].fd = cfd;
			fds[i].events = POLLIN;

			if ( i + 1 > nfds ) {
				nfds = i + 1;
			}	
		}

		if (nready <= 0) {
			continue;
		}
		
		for (int i = 1; i < nfds; ++i) {
			if (fds[i].revents & POLLIN) {
				int cnt = Read(fds[i].fd, buf, 1024);
				if (0 == cnt) {
					close(fds[i].fd);
					fds[i].fd = -1;	
					continue;
				}
				Write(fds[i].fd, buf, cnt);
			}
		}
	}

	return 0;
}
