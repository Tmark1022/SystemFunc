/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 12 Jun 2020 06:24:01 PM CST
 @ File Name	: ps_lwp_tid.c
 @ Description	: pthread_self只能保证进程内unique， 在unix中， 有tgid， 就是ps -eLf 的LWP
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/syscall.h>

// 获取tgid, linux specific
pid_t sys_gettid()
{	
	return syscall(SYS_gettid);
}

void * thread_handler(void *arg) 
{
	printf("pid : %d, process tid : %ld, kernel tid : %d\n", getpid(), pthread_self(), sys_gettid());
	pause();
	return NULL;
}


#define PRODUCER_CNT	3
#define CONSUMER_CNT	2
int main(int argc, char *argv[]) {
	
	pthread_t tids[PRODUCER_CNT + CONSUMER_CNT];
	
	for(int i = 0; i < PRODUCER_CNT; ++i) {
		pthread_create(tids+i, NULL, thread_handler, NULL);
		pthread_detach(tids[i]);
	}	

	for(int i = 0; i < CONSUMER_CNT; ++i) {
		pthread_create(tids+i+PRODUCER_CNT, NULL, thread_handler, NULL);
		pthread_detach(tids[i]);
	}	

	printf("i am main, pid : %d, process tid : %ld, kernel tid : %d\n", getpid(), pthread_self(), sys_gettid());
	pause();

	return 0;
}
