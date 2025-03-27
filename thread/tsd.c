/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 27 Mar 2025 07:41:01 PM CST
 @ File Name	: tsd.c
 @ Description	: thread specific data
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFF_SIZE 1024 

pthread_once_t once_control = PTHREAD_ONCE_INIT;
pthread_key_t key1;


void DeallocateBuff(void *p)
{	
	char tmp[1024];
	pthread_t tid = pthread_self();
	sprintf(tmp, "tid %lu, tsd addr %lx, call DeallocateBuff\n", tid, (long)p);
	write(STDOUT_FILENO, tmp, strlen(tmp));
	free(p);
}

// 初始化key， 只会执行一遍
void CreateKey()
{
	printf("tid %lu, call CreateKey\n", pthread_self());
	pthread_key_create(&key1, DeallocateBuff);
}

void InitTsd()
{
       pthread_once(&once_control, CreateKey);
       pthread_setspecific(key1, malloc(BUFF_SIZE));
}

char *GetTsd()
{
	return (char *)pthread_getspecific(key1);
}

void * ThreadRoutine(void *arg) 
{
	char tmp[1024];
	InitTsd();
	char *tsd = GetTsd();
	pthread_t tid = pthread_self();
	sprintf(tmp, "tid %lu, tsd addr %lx\n", tid, (long)tsd);
	write(STDOUT_FILENO, tmp, strlen(tmp));
		
	strcpy(tsd, (char *)arg);

	int a = 9;
	while (a--) {
		tsd = GetTsd();
		sprintf(tmp, "tid %lu, tsd addr %lx, value: %s\n", tid, (long)tsd, tsd);
		write(STDOUT_FILENO, tmp, strlen(tmp));
		sleep(5);
	}
	return (void *)NULL;
}

int main(int argc, char *argv[]) {

	pthread_t tid;
	pthread_create(&tid, NULL, ThreadRoutine, "first thread");
	pthread_create(&tid, NULL, ThreadRoutine, "second thread");

	sleep(1);
	InitTsd();
	char *tsd = GetTsd();
	strcpy(tsd, "main thread");
	int a = 10;
	while (a--) {
		tsd = GetTsd();
		printf("tid %lu, tsd addr %lx, value: %s\n", pthread_self(), (long)tsd, tsd);
		sleep(5);
	}

	pthread_key_delete(key1);
	return 0;
}
