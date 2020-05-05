/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 05 May 2020 02:10:38 PM CST
 @ File Name	: mutex.c
 @ Description	: mutex实现线程同步简单demo , 程序希望无论父进程还是子进程， 都能同时打印完hello world 或 HELLO WORLD
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;


void * ThreadHandler(void * arg)
{
	srand(time(NULL));	
	while (1) {
		pthread_mutex_lock(&mutex);
		printf("hello ");		
		sleep(rand() % 3);
		printf("world \n");		
		pthread_mutex_unlock(&mutex);
		sleep(rand() % 3);
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	
	pthread_mutex_init(&mutex, NULL);
	
	pthread_t tid; 
	pthread_create(&tid, NULL, ThreadHandler, NULL);
	
	srand(time(NULL));	
	while (1) {
		pthread_mutex_lock(&mutex);
		printf("HELLO ");		
		sleep(rand() % 3);
		printf("WORLD \n");		
		pthread_mutex_unlock(&mutex);

		sleep(rand() % 3);
	}
	
	pthread_join(tid, NULL);
	pthread_mutex_destroy(&mutex);

	return 0;
}
