/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 12 Jun 2020 05:22:20 PM CST
 @ File Name	: semaphore_mutex.c
 @ Description	: 使用匿名信号量原语实现mutex锁
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>

struct sem_mutex_t {
	sem_t sem;		// 二进制信号量	
	// pthread_t cptid;	// 当前加锁线程（限定只能加锁的线程来进行解锁）, Thread  IDs  are  guaranteed  to be unique only within a process
	pid_t	lwp;		// kernel tid, linux specific		
};

struct sem_mutex_t sm;

// 获取tgid, linux specific
pid_t sys_gettid()
{	
	return syscall(SYS_gettid);
}

void sem_mutex_init(struct sem_mutex_t * sem_mutex)
{
	if (NULL == sem_mutex) {
		fprintf(stderr, "sem_mutex_init parameter invalid");
		exit(EXIT_FAILURE);
	}	
	
	bzero(sem_mutex, sizeof(struct sem_mutex_t));

	// NOTE, 如果单纯只在单个进程中使用， 那么pshared 就设置0
	int ret = sem_init(&sem_mutex->sem, 1, 1);
	if (-1 == ret) {
		perror("sem_init error");	
		exit(EXIT_FAILURE);
	}
}

void sem_mutex_destroy(struct sem_mutex_t * sem_mutex)
{	
	if (NULL == sem_mutex) {
		fprintf(stderr, "sem_mutex_destroy parameter invalid");
		exit(EXIT_FAILURE);
	}	

	int ret = sem_destroy(&sem_mutex->sem);
	if (-1 == ret) {
		perror("sem_destroy error");	
		exit(EXIT_FAILURE);
	}	
}

void sem_mutex_lock(struct sem_mutex_t * sem_mutex)
{
	if (NULL == sem_mutex) {
		fprintf(stderr, "sem_mutex_lock parameter invalid");
		exit(EXIT_FAILURE);
	}	

	sem_wait(&sem_mutex->sem);	
	sem_mutex->lwp = sys_gettid();	
}

void sem_mutex_unlock(struct sem_mutex_t * sem_mutex)
{
	if (NULL == sem_mutex) {
		fprintf(stderr, "sem_mutex_unlock parameter invalid");
		exit(EXIT_FAILURE);
	}	

	// 判断当前是否在加锁
	int val;
	if (-1 == sem_getvalue(&sem_mutex->sem, &val)) {
		fprintf(stderr, "sem_mutex_unlock sem_getvalue error");
		exit(EXIT_FAILURE);	
	}	
	if (val > 0) {
		// 没有加锁， 不能解锁
		return ;
	}

	// 判断tgid
	if (sys_gettid() != sem_mutex->lwp) {
		return ;
	}

	sem_post(&sem_mutex->sem);	
	sem_mutex->lwp = 0;
}


void * thread_producer(void *arg) 
{
	long tmp = (long)arg;
	srand(time(NULL));
	while (1) {
		sem_mutex_lock(&sm);
		printf("======= thread %ld, pid : %d,", tmp, getpid());
		usleep(10);
		printf(" process tid : %ld, kernel tid : %d =======\n", pthread_self(), sys_gettid());	
		sem_mutex_unlock(&sm);
		
		sleep(rand() % 3);
	}

	return NULL;
}

void * thread_consumer(void *arg) 
{
	long tmp = (long)arg;
	srand(time(NULL));
	while (1) {
		sem_mutex_lock(&sm);
		printf("******* thread %ld, pid : %d,", tmp, getpid());
		usleep(10);
		printf(" process tid : %ld, kernel tid : %d *******\n", pthread_self(), sys_gettid());	
		sem_mutex_unlock(&sm);
		
		sleep(rand() % 3);
	}
}

#define PRODUCER_CNT	3
#define CONSUMER_CNT	2
int main(int argc, char *argv[]) {
	
	sem_mutex_init(&sm);

	pthread_t tids[PRODUCER_CNT + CONSUMER_CNT];
	
	for(int i = 0; i < PRODUCER_CNT; ++i) {
		pthread_create(tids+i, NULL, thread_producer, (void *)(long)(i+1));
		pthread_detach(tids[i]);
	}	

	for(int i = 0; i < CONSUMER_CNT; ++i) {
		pthread_create(tids+i+PRODUCER_CNT, NULL, thread_consumer, (void *)(long)(i+1));
		pthread_detach(tids[i]);
	}	

	printf("i am main, pid : %d, process tid : %ld, kernel tid : %d\n", getpid(), pthread_self(), sys_gettid());
	pause();

	return 0;
}
