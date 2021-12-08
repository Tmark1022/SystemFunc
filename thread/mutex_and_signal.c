/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 08 Dec 2021 02:06:43 PM CST
 @ File Name	: mutex_and_signal.c
 @ Description	: signal handler 死锁问题
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t mutex;
long global_val = 100;

void sigint(int signo)
{	
	printf("catch signo %d\n", signo);

	printf("begin sigint lock\n");	
	pthread_mutex_lock(&mutex);			// signal handler异常控制流 与 main逻辑流并发， 进来这里导致重复加锁, 因为没有处理完信号处理sigint 不会返回main，导致main永远不会释放锁， 导致死锁现象

	global_val += 1000;
	pthread_mutex_unlock(&mutex);
	printf("end sigint lock\n");	
}

int main(int argc, char *argv[]) {	
	pthread_mutex_init(&mutex, NULL);
	signal(SIGINT, sigint);
	signal(SIGQUIT, sigint);
	
	printf("begin main lock\n");	
	pthread_mutex_lock(&mutex);
	printf("begin main sleep\n");		
	global_val = 2222;
	sleep(5);
	printf("end main sleep\n");	
	pthread_mutex_unlock(&mutex);
	printf("end main lock\n");	

	pthread_mutex_destroy(&mutex);	
	printf("exit\n");
	return 0;
}
