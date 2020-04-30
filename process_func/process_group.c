/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 30 Apr 2020 09:53:55 AM CST
 @ File Name	: process_group.c
 @ Description	: apue说控制终端键入的信号会发送给前台进程的所有进程，但是结果怎么设置捕捉后只被一个进程调用？？？【环境：ubuntu 18.04， 内核: Linux 4.15.0-96-generic】 , 调用方法， shell 执行 ./process_group| ./process_group | ./process_group 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

void handler(int signum) 
{
	
	// 不知为什么只被一个进程捕捉
	printf("ppid : %d, pid : %d, pgrpid : %d, sid : %d, tpgid : %d\n", getppid(), getpid(), getpgid(0), getsid(0), tcgetpgrp(STDIN_FILENO));
}	

int main(int argc, char *argv[]) {
	struct sigaction new;	
	new.sa_flags = 0;
	sigemptyset(&new.sa_mask);	
	new.sa_handler = handler;
	sigaction(SIGQUIT, &new, NULL);

	while(1);
		
	return 0;
}

