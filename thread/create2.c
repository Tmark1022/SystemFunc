/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 01 May 2020 06:03:00 PM CST
 @ File Name	: create2.c
 @ Description	: 简单的线程创建demo2, 在demo1的基础上进行简单的线程间通信
 @ Note		: 需要注意的是，如果主线程执行到main的return，那么相当于执行exit一般，
		  整个程序（无论是否还有线程在执行）都结束了, 注意区分主线程执行pthread_exit（等待所有线程都结束才真正地exit）
		  创建的新线程共享主线程的内存空间（用户空间，text、data、bss、堆等, 栈） 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/*
 * 1、全局变量
 * 2、函数传参
 */

typedef struct{
	int id;
	char name[256];
}user_data_t;

// 全局变量
user_data_t gData;

void perror_thread(int num)
{
	fprintf(stderr, "%s", strerror(num)); 
	exit(EXIT_FAILURE);
}

void * ThreadHandler(void * arg) 
{
	printf("thread, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
	user_data_t * pointer = (user_data_t *)arg;	

	printf("gData(%d, %s), arg(%d, %s)\n", gData.id, gData.name, pointer->id, pointer->name);
	fflush(NULL);

	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t ti;
	int ret;
	gData.id = 1;
	strcpy(gData.name, "hello world");	
	user_data_t * arg = malloc(sizeof(user_data_t));
	arg->id = 2;
	sprintf(arg->name, "terry %s", "mark");
	ret = pthread_create(&ti, NULL, ThreadHandler, (void *)arg);
	if (0 != ret) {
		perror_thread(ret);
	}

	printf("main, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
	// wait for child thread
	sleep(1);

	return 0;
}
