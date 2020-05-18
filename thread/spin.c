/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 18 May 2020 07:16:33 PM CST
 @ File Name	: spin.c
 @ Description	: 自旋锁(平时不使用， 多用于各种同步锁的底层实现, 与mutex的差别是mutex会阻塞线程， 而spin却一直在忙等loop，所以自旋锁不能等太久， 不然会耗费好多资源)实现线程同步简单demo , 程序希望无论父进程还是子进程， 都能同时打印完hello world 或 HELLO WORLD
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_spinlock_t lock;


void * ThreadHandler(void * arg)
{
	srand(time(NULL));	
	while (1) {
		pthread_spin_lock(&lock);
		printf("hello ");		
		sleep(rand() % 3);
		printf("world \n");		
		pthread_spin_unlock(&lock);
		sleep(rand() % 3);
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	
	pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
	
	pthread_t tid; 
	pthread_create(&tid, NULL, ThreadHandler, NULL);
	
	srand(time(NULL));	
	while (1) {
		pthread_spin_lock(&lock);

		printf("HELLO ");		
		sleep(rand() % 3);
		printf("WORLD \n");		

		pthread_spin_unlock(&lock);

		sleep(rand() % 3);
	}
	
	pthread_join(tid, NULL);
	
	pthread_spin_destroy(&lock);

	return 0;
}
