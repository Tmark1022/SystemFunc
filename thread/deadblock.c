/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 05 May 2020 03:12:19 PM CST
 @ File Name	: deadblock.c
 @ Description	: 死锁情况1：加了两次锁, 下面demo中线程加了两次锁， 导致陷入死锁
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

		pthread_mutex_lock(&mutex);		// 锁还没释放，加了第二次锁 ,导致死锁
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
