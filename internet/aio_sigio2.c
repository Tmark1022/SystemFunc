/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 23 Jun 2020 03:33:02 PM CST
 @ File Name	: aio_sigio2.c
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
char * identifier = "none";

// 这两个值会在parent中设置, child_do_flag 设置为1让子进程来处理accept以及后续的客户端通讯
int child_do_flag = 0;
int child_pid;

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
	printf("------------------i am %s, my pid is %d, call sig_io\n", identifier, getpid());	

	if (child_do_flag) {
		printf("i am %s, my pid is %d, child_do_flag is true, call kill, kill_pid is %d\n", identifier, getpid(), child_pid);
		kill(child_pid, SIGIO);	
		return ;
	}

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
	signal(SIGIO, sig_io);
	set_aio(lfd);
	
	int pid = fork();
	if (-1 == pid) {
		// error
		PrintError(stderr, 0, "call fork failed", EXIT_FAILURE);		
	} else if (0 == pid) {
		// child
		identifier = "child";	
	} else {
		// parent
		identifier = "parent";
		child_pid = pid;

		// 让子进程来call accept， 那么后续的fd都设置为子进程的pid了
		child_do_flag = 1;
	}

	// printf info
	int listenpid = fcntl(lfd, F_GETOWN);
	printf("i am %s, my pid is %d, fd %d, before listen fd F_GETOWN is %d\n", identifier, getpid(), lfd, listenpid); 

	while(1) {
		pause();
	}
	
	return 0;
}
