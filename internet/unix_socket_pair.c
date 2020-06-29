/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 29 Jun 2020 03:29:20 PM CST
 @ File Name	: unix_socket_pair.c
 @ Description	: socketpair创建匿名套接字对（unix domain socket）
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <ctype.h>


int main(int argc, char *argv[]) {
	
	int fds[2];
	if (-1 == socketpair(AF_UNIX, SOCK_DGRAM, 0, fds)) {
		perror("socketpair error");	
		exit(EXIT_FAILURE);
	}
	
	// fork 父子进程
	int pid = fork();
	if (-1 == pid) {
		perror("fork error");	
		exit(EXIT_FAILURE);
	} else if (0 == pid) {
		// child
		int fd = fds[1];  	
		close(fds[0]);
		
		char buf[1024];

		while (1) {
			fgets(buf, 1024, stdin);
			write(fd, buf, strlen(buf));	
			int cnt = read(fd, buf, 1024);
			write(STDOUT_FILENO, buf, cnt);
		}
	} else {
		// parent
		int fd = fds[0];  	
		close(fds[1]);
		
		// echo
		char buf[1024];
		while (1) {
			int cnt = read(fd, buf, 1024);	
			for(int idx = 0; idx < cnt; ++idx) {
				buf[idx] = toupper(buf[idx]);
			}
			write(fd, buf, cnt);
		}
	}
	
	return 0;
}
