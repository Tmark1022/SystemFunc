/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 15 May 2020 03:44:27 PM CST
 @ File Name	: restart_slow_system_call.c
 @ Description	: 恢复慢速系统调用
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define RESTART_FLAG

char buf[1024];

void sig_alarm(int signo)
{
	printf("get alarm signal\n");
}

int main(int argc, char *argv[]) {

	struct sigaction new;
	sigemptyset(&new.sa_mask);
	new.sa_handler = sig_alarm;		
#ifdef RESTART_FLAG
	new.sa_flags = SA_RESTART; 
#else 
	new.sa_flags = 0;
#endif
	if (-1 == sigaction(SIGALRM, &new, NULL)) {
		perror("sigaction error");
		exit(EXIT_FAILURE);
	}

	bzero((void *)buf, sizeof(buf));

	alarm(3);
	
	// 阻塞读， 等待信号处理打断
	if (-1 == read(STDIN_FILENO, buf, sizeof(buf) - 1)) {
		perror("read error");
		exit(EXIT_FAILURE);
	}
	
	printf("get string : %s\n", buf);

	return 0;
}
