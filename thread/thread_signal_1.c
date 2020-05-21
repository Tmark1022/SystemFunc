/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 21 May 2020 01:41:06 PM CST
 @ File Name	: thread_signal_1.c
 @ Description	: 线程与信号demo， 信号屏蔽字
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_t tid;

void PrintBitMap(const sigset_t *set, const char * str)
{
	printf ("%s, now set, ", str);
	for (int i = 1; i <= 32; ++i)
	{
		if (sigismember(set, i)) {
			putchar('1');
		} else {
			putchar('0');
		}
	}
	putchar('\n');
}

void sig_int(int signo)
{
	if (pthread_equal(tid, pthread_self())) {
		printf("sig_int, i am other thread, %lu, %lu\n", tid, pthread_self());
	} else {
		printf("sig_int, i am main thread, %lu, %lu\n", tid, pthread_self());
	}	
}

void * ThreadHandler(void * arg) 
{	
	sigset_t old;

	while (1) {	
		pthread_sigmask(SIG_BLOCK, NULL, &old);
		PrintBitMap(&old, "other thread");
		pause();
	}
	return NULL;
}

int main(int argc, char *argv[]) {

	signal(SIGINT, sig_int);
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);	
	
	// 在创建子线程时设置屏蔽
	// pthread_sigmask(SIG_BLOCK, &set, NULL);
	
	pthread_create(&tid, NULL, ThreadHandler, NULL);

	// 在创建子线程后设置屏蔽
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	sigset_t old;
	pthread_sigmask(SIG_BLOCK, NULL, &old);
	PrintBitMap(&old, "main thread");

	pthread_join(tid, NULL);
	
	return 0;
}
