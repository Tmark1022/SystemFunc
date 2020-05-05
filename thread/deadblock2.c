/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 05 May 2020 03:15:16 PM CST
 @ File Name	: deadblock2.c
 @ Description	: 死锁情况2，完成一个操作需要两个以上的资源， 两个线程各持有一个锁（不释放）的情况下去请求另外一个锁， 导致死锁
		  解决方法， 同时申请到所有需要的资源的锁才开始下面逻辑， 不然释放所有获取的锁， 看deadblock2_solve.c
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex, mutex2;
int var1 = 10, var2 = 20;

void * ThreadHandler(void * arg)
{
	// 线程先请求锁2，再请求锁1 
	srand(time(NULL));	
	while (1) {
		pthread_mutex_lock(&mutex2);
		sleep(1);
		pthread_mutex_lock(&mutex);
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
		pthread_mutex_lock(&mutex);
		sleep(1);
		pthread_mutex_lock(&mutex2);
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
