/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 05 May 2020 03:04:33 PM CST
 @ File Name	: mutex2.c
 @ Description	: 使用trylock加锁而不是lock， 重写mutex.c的代码
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t mutex;

void * ThreadHandler(void * arg)
{
	srand(time(NULL));	
	while (1) {
		int ret = pthread_mutex_trylock(&mutex);
		if (EBUSY == ret) {
			/* Device or resource busy */			
			fprintf(stderr, "thread busy\n");
			sleep(1);			
			continue;
		} else if (0 != ret) {
			// error
			exit(EXIT_FAILURE);	
		}
			
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
		int ret = pthread_mutex_trylock(&mutex);
		if (EBUSY == ret) {
			/* Device or resource busy */			
			fprintf(stderr, "main busy\n");
			sleep(1);			
			continue;
		} else if (0 != ret) {
			// error
			exit(EXIT_FAILURE);	
		}
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
