/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sat 02 May 2020 11:58:26 AM CST
 @ File Name	: detach.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_CNT 5

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
	printf("(%d)th thread, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", (int)arg, getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
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
	pthread_t arr[THREAD_CNT];
	int ret;
	for(int i = 0; i < THREAD_CNT; ++i) {
		// 循环创建线程
		ret = pthread_create(arr + i, NULL, ThreadHandler, (void *)i);			
		if (ret) {
			perror_thread(ret, "", 1);
		}
	}

	printf("main, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
			
	user_data_t * pres;
	char buf[100];
	for(int i = 0; i < THREAD_CNT; ++i) {
		if (2 == i) {	
			pthread_detach(arr[i]);
		}

		ret = pthread_join(arr[i], (void **)(&pres));
		if (ret) {
			sprintf(buf, "join (%d, %lu) error", i, arr[i]);
			perror_thread(ret, buf, 0);
		}
		printf("get join data, (%d, %s)\n", pres->id, pres->name);
		fflush(NULL);
	}

	return 0;
}
