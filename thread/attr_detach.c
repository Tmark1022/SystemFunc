/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 04 May 2020 10:53:52 AM CST
 @ File Name	: attr_detach.c
 @ Description	: 通过线程属性来设置线程detach
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MARCO_SET_DETACH

typedef struct{
	int id;
	char name[256];
}user_data_t;

void perror_thread(int num, const char * str, int exitFlag)
{
	fprintf(stderr, "%s:%s\n", str, strerror(num)); 
	if (exitFlag) 
		exit(EXIT_FAILURE);
}

void * ThreadHandler(void * arg) 
{
	printf("ThreadHandler, (%d)th thread, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", (int)arg, getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
	fflush(NULL);

	sleep(3 + (int)arg);

	// call pthread_exit
	user_data_t * pointer = malloc(sizeof(user_data_t));
	pointer->id = (int)arg;
	sprintf(pointer->name, "%dth thread", (int)arg);
	pthread_exit((void *)pointer);

	return (void *)pointer;
}


int main(int argc, char *argv[]) {
	int ret;
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	if (0 != ret) {
		perror_thread(ret, "attr init error", 1);
	}

#ifdef MARCO_SET_DETACH	
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (0 != ret) {
		perror_thread(ret, "attr set detach error", 1);
	}
#endif
	
	pthread_t tpid; 
	ret = pthread_create(&tpid, &attr, ThreadHandler, (void *)1); 
	if (0 != ret) {
		perror_thread(ret, "thread create error", 1);
	}

	ret = pthread_attr_destroy(&attr);
	if (0 != ret) {
		perror_thread(ret, "attr destory error", 1);
	}
	

	void * res = NULL;			
	ret = pthread_join(tpid, (void **)(&res)); 
	if (0 != ret) {
		perror_thread(ret, "thread join error", 0);
	}
	user_data_t * aa = (user_data_t * )res;
	if (NULL != res && -1 != (int)res) {
		printf("join a thread, (%d,%s)\n",aa->id, aa->name);			
		fflush(NULL);
	}

	return 0;
}
