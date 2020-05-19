/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 17 May 2020 05:28:33 PM CST
 @ File Name	: select.c
 @ Description	: select 实现高并发echo
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

char buf[1024];


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

	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));

	// 将listen fd 加入监听	
	int maxFd = fd;
	fd_set allRSet, runRSet; 
	FD_ZERO(&allRSet);
	FD_SET(fd, &allRSet);

	int ret;
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
				} else {
					// 读数据
					int cnt = Read(i, buf, 1024);
					if (cnt > 0) {
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

