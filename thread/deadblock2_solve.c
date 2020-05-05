/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 05 May 2020 03:22:41 PM CST
 @ File Name	: deadblock2_solve.c
 @ Description	: 解决deadblock2.c的死锁的写法
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t mutex, mutex2;
int var1 = 10, var2 = 20;

void * ThreadHandler(void * arg)
{
	// 线程先请求锁2，再请求锁1 
	srand(time(NULL));	
	while (1) {
		int ret = pthread_mutex_trylock(&mutex2);		// 使用非阻塞的trylock
		if (EBUSY == ret) {
			continue;
		}
		ret = pthread_mutex_lock(&mutex);
		if (EBUSY == ret) {
			// 上一步获取的锁
			pthread_mutex_unlock(&mutex2);
			sleep(rand()%3);		
			continue;
		}

		printf("thread, %d\n", var1 + var2);
		++var1;
		++var2;

		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutex2);
		sleep(rand() % 3);
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex2, NULL);
	
	pthread_t tid; 
	pthread_create(&tid, NULL, ThreadHandler, NULL);
	
	// 主线程先请求锁1，再请求锁2, 与子线程相反
	srand(time(NULL));	
	while (1) {
		int ret = pthread_mutex_trylock(&mutex);		// 使用非阻塞的trylock
		if (EBUSY == ret) {
			continue;
		}
		ret = pthread_mutex_lock(&mutex2);
		if (EBUSY == ret) {
			// 上一步获取的锁
			pthread_mutex_unlock(&mutex);
			sleep(rand()%3);			// 随机休眠一下
			continue;
		}

		printf("main, %d\n", var1 + var2);
		++var1;
		++var2;

		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutex2);
		sleep(rand() % 3);
	}
	
	pthread_join(tid, NULL);
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex2);

	return 0;
}
