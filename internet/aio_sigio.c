/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 23 Jun 2020 01:52:33 PM CST
 @ File Name	: aio_sigio.c
 @ Description	: 
 ************************************************************************/
#include "wrap/wrap.h"
#include <asm-generic/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define REUSE_ADDR

/***************************************************
* declaration 
***************************************************/
void set_aio(int fd);

int lfd;
int cfd_list[100];

void init_cfd_list()
{
	for(int idx = 0; idx < 100; ++idx) {
		cfd_list[idx] = -1;
	}
}

int get_unuse_cfd()
{
	for(int idx = 0; idx < 100; ++idx) {
		if (-1 == cfd_list[idx]) {
			return idx;
		}
	}

	return -1;
}



// 信号触发的模式就如epoll 的et模式一般, 就数据来的时候触发一遍， 不管有没有处理掉数据
// 每一个文件描述符都绑定在当前进程中, 因为不知道触发信号的到底是哪个文件描述符， 所以需要遍历当前有效的文件描述符， 并且需要注意都要设置为非阻塞的
// 需要注意的是， 如果描述符4的数据没有读完， 有新的sigio信号触发（可能是描述符4又写了， 或者其他描述符进行有新的数据来， 因为非阻塞地遍历， 所以描述符4会顺便把剩余的数据读出来）
void sig_io(int signo) 
{
	// lfd	
	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;
	bzero((void *)&clientAddr, sizeof(struct sockaddr_in));

	int ret = accept(lfd, (struct sockaddr *)&clientAddr, &addrlen);
	if (-1 == ret) {
		if (EAGAIN == errno || EWOULDBLOCK == errno) {
			printf("non new connection\n");
		} else {
			PrintError(stderr, 0, "call accept failed", EXIT_FAILURE);		
		}
	} else {
		PrintAddr(stdout, &clientAddr, "new client");
		printf("cfd is %d\n", ret);

		int cfd_idx = get_unuse_cfd();
		if (-1 == cfd_idx) {
			printf("too many client, close it\n");
			close(ret);
		} else {
			cfd_list[cfd_idx] = ret;
			set_aio(ret);
		}
	}

	// cfd_list
	char buf[101];
	for(int idx = 0; idx < 100; ++idx) {
		int cfd = cfd_list[idx]; 
		if (-1 == cfd) {
			continue;
		}

		int nread = read(cfd, buf, 100); 
		if (-1 == nread) {
			if (EAGAIN == errno || EWOULDBLOCK == errno) {
			
			} else {
				PrintError(stderr, 0, "call read failed", EXIT_FAILURE);		
			}
		} else if (0 == nread) {
			printf("close fd %d\n", cfd);
			cfd_list[idx] = -1;
			close(cfd);
		} else {
			printf("fd %d, nread %d\n", cfd, nread);
			buf[nread] = '\0';	
			printf("get string : %s\n", buf);	

			write(cfd, buf, nread);
		}
	}
	



}

void set_aio(int fd)
{
	signal(SIGIO, sig_io);
	int listenpid = fcntl(fd, F_GETOWN);
	printf("fd %d, before listen fd F_GETOWN is %d\n", fd, listenpid); 
	int ret = fcntl(fd, F_SETOWN, getpid()); 		
	if (-1 == ret) {
		perror("fcntl error");	
		exit(EXIT_FAILURE);
	}	
	listenpid = fcntl(fd, F_GETOWN);
	printf("fd %d, after listen fd F_GETOWN is %d\n", fd, listenpid);

	
	int flag = fcntl(fd, F_GETFL); 
	flag |= O_ASYNC;
	flag |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flag);
}

int main(int argc, char * argv[]) 
{
	init_cfd_list();

	int port = 8888;
	if (argc == 2) {
		port = atoi(argv[1]);
	}

	lfd = Socket(AF_INET, SOCK_STREAM, 0);

#ifdef REUSE_ADDR
	// 端口复用
	int opt = 1;
	if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
	}
#endif

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	Bind(lfd, (struct sockaddr *)&bindAddr, sizeof(bindAddr));  	
	Listen(lfd, SOMAXCONN);
	set_aio(lfd);
		
	int cnt = 1;
	while(1) {
		printf("%d\n", cnt++);
		sleep(2);
	}
	
	return 0;
}
