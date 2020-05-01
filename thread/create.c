/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 01 May 2020 06:03:00 PM CST
 @ File Name	: create.c
 @ Description	: 简单的线程创建demo 
 @ Note		: 需要注意的是，如果主线程执行到main的return，那么相当于执行exit一般，
		  整个程序（无论是否还有线程在执行）都结束了, 注意区分主线程执行pthread_exit（等待所有线程都结束才真正地exit）
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void perror_thread(int num)
{
	fprintf(stderr, "%s", strerror(num)); 
	exit(EXIT_FAILURE);
}

void * ThreadHandler(void * arg) 
{
	printf("thread, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
	fflush(NULL);
	sleep(60);

	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t ti;
	int ret;
	ret = pthread_create(&ti, NULL, ThreadHandler, NULL);
	if (0 != ret) {
		perror_thread(ret);
	}

	printf("main, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
	// wait for child thread
	sleep(60);

	return 0;
}
