/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sat 02 May 2020 12:14:27 PM CST
 @ File Name	: cancel.c
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

void * ThreadHandlerCancel(void * arg) 
{
	printf("ThreadHandlerCancel, (%d)th thread, pid : %d, threadid : %lu, pgrpid : %d, sid : %d, tgpid : %d\n", (int)arg, getpid(), pthread_self(), getpgid(0), getsid(0), tcgetpgrp(0));	
	fflush(NULL);

	sleep(3 + (int)arg);
	while (1);


	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t arr[THREAD_CNT];
	int ret;
	for(int i = 0; i < THREAD_CNT; ++i) {
		// 循环创建子进程
		if (1 != i) {
			ret = pthread_create(arr + i, NULL, ThreadHandler, (void *)i);			
		} else {
			ret = pthread_create(arr + i, NULL, ThreadHandlerCancel, (void *)i);			
		}

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
		if (1 == i) {
			// kill it 
			pthread_cancel(arr[i]);
		}

		ret = pthread_join(arr[i], (void **)(&pres));
		if (ret) {
			sprintf(buf, "join (%d, %lu) error", i, arr[i]);
			perror_thread(ret, buf, 0);
			continue;
		}
		if (NULL == pres) {
			printf("get join data, %d, null\n", i);
		} else if (PTHREAD_CANCELED == pres) {
			printf("get join data, %d, PTHREAD_CANCELED\n", i);
		} else {
			printf("get join data, (%d, %s)\n", pres->id, pres->name);
		}
		fflush(NULL);
	}

	return 0;
}

