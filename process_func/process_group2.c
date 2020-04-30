/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 30 Apr 2020 10:38:14 AM CST
 @ File Name	: process_group2.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	int pid = fork();
	if (-1 == pid) {
		perror("");	
		exit(EXIT_FAILURE);
	} else if (0 == pid) {
		// child, 子进程屏蔽SIGINT信号
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		sigprocmask(SIG_BLOCK, &set, NULL);
	} else {
		// parent does nothing.	
	}

	while(1) {
		printf("ppid : %d, pid : %d, pgrpid : %d, sid : %d, tpgid : %d\n", getppid(), getpid(), getpgid(0), getsid(0), tcgetpgrp(STDIN_FILENO));
		sleep(1);
	}
	
	return 0;
}

