/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 21 May 2020 02:06:17 PM CST
 @ File Name	: thread_signal_2.c
 @ Description	: sigwait 与 信号处理函数
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

void sig_int_on_sigwait(int signo)
{
	if (pthread_equal(tid, pthread_self())) {
		printf("sig_int_on_sigwait, i am other thread, %lu, %lu\n", tid, pthread_self());
	} else {
		printf("sig_int_on_sigwait, i am main thread, %lu, %lu\n", tid, pthread_self());
	}	
}

void * ThreadHandler(void * arg) 
{	
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);	
	sigaddset(&set, SIGQUIT);	

	int signalNo;
	while (1) {	
		sigwait(&set, &signalNo);
		printf("sigwait return , signalNo is %d\n", signalNo);
		if (SIGINT == signalNo) {
			sig_int_on_sigwait(0);
		} else if (SIGQUIT == signalNo) {
			printf("hello , sigwait get SIGQUIT\n");
		} else {
			printf("hello , sigwait get error\n");
		}
	}
	return NULL;
}

int main(int argc, char *argv[]) {

	signal(SIGINT, sig_int);
	signal(SIGQUIT, SIG_IGN);

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);	
	sigaddset(&set, SIGQUIT);	
	
	// 在创建子线程时设置屏蔽
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	
	pthread_create(&tid, NULL, ThreadHandler, NULL);

	// 在创建子线程后设置屏蔽
	// pthread_sigmask(SIG_BLOCK, &set, NULL);

	sigset_t old;
	pthread_sigmask(SIG_BLOCK, NULL, &old);
	PrintBitMap(&old, "main thread");

	pthread_join(tid, NULL);
	
	return 0;
}
