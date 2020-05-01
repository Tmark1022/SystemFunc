/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 01 May 2020 09:22:44 PM CST
 @ File Name	: exit.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

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

	// call pthread_exit
	pthread_exit(NULL);

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

	// call pthread_exit
	pthread_exit(NULL);

	printf("main, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
	// wait for child thread
	sleep(1);

	return 0;
}
