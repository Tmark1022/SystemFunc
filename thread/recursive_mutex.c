/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 22 May 2020 07:44:53 PM CST
 @ File Name	: recursive_mutex.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_once_t once_control = PTHREAD_ONCE_INIT;
pthread_mutex_t mutex;

int conditon = 1;

void init_once(void)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);	
	// set recursive mutext lock
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);

}

void sig_int(int signo)
{	
	pthread_mutex_lock(&mutex);
	printf("sig_int, conditon is %d\n", conditon++);
	pthread_mutex_unlock(&mutex);	
}

int main(int argc, char *argv[]) {

	pthread_once(&once_control, init_once);
	
	signal(SIGINT, sig_int);

	pthread_mutex_lock(&mutex);
	while (1) {
		printf("main, conditon is %d\n", conditon);
		sleep(1);	
	}
	pthread_mutex_unlock(&mutex);		
	pthread_mutex_destroy(&mutex);
	return 0;
}
