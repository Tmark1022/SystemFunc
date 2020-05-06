/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 06 May 2020 10:04:28 AM CST
 @ File Name	: rwlock.c
 @ Description	: 读写锁简单demo, 创建5个读线程和3个写线程，使用读写锁来进行线程同步。
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

// static initialization
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

int counter = 100;

void * ReadHandler(void *arg)
{
	srand(time(NULL));
	while (1) {
		pthread_rwlock_rdlock(&rwlock);	
		printf("%dth read thread, counter is %d\n", (int)arg, counter);
		pthread_rwlock_unlock(&rwlock);	
		usleep(rand() % 200000);
	}
	return NULL;
}

void * WriteHandler(void *arg)
{
	srand(time(NULL));
	int old = counter;
	while (1) {
		pthread_rwlock_wrlock(&rwlock);	
		printf("%dth write thread, counter is %d, ++counter is %d\n", (int)arg, old, ++counter);
		pthread_rwlock_unlock(&rwlock);	
		usleep(rand() % 1000000);
	}
	return NULL;
}

int main(int argc, char *argv[]) {

	// dynamic initialization
	// int pthread_rwlock_init(&rwlock, NULL);

	int readCnt = 5;
	int writeCnt = 3;
	pthread_t * tidList = calloc(sizeof(pthread_t), readCnt + writeCnt);
	if (NULL == tidList) {
		exit(EXIT_FAILURE);
	}
		
	for(int i = 0; i < readCnt; ++i) {
		pthread_create(tidList + i, NULL, ReadHandler, (void *)i);
	}

	for(int i = 0; i < writeCnt; ++i) {
		pthread_create(tidList + readCnt + i, NULL, WriteHandler, (void *)i);
	}
	
	for(int i = 0; i < readCnt + writeCnt; ++i) {
		pthread_join(*(tidList+i), NULL);
	}

	pthread_rwlock_destroy(&rwlock);

	return 0;
}
