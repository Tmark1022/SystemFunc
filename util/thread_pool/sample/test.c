/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 31 May 2020 03:45:16 PM CST
 @ File Name	: test.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <thread_pool.h>

thread_pool_t *tp;

void * task_handler(void *arg)
{
	long sec = (long)arg;
	printf("tid %lu, try sleep %ld\n", pthread_self(), sec);
	sleep(sec);

	char * ha = malloc(50);
	sprintf(ha, "%lu -> %ld secs", pthread_self(), sec);
	return ha;
}

void complete_call_back(void *arg) 
{
	char * res = (char *) arg;
	printf("tid %lu, res is %s\n", pthread_self(), res);
}

void * print_data(void * arg) 
{

	FILE * stream = fopen("output", "w+");	
	while (1) {
		thread_pool_print_struct(stream, tp);
		sleep(1);
	}

	fclose(stream);
	return NULL;
}


int main(int argc, char *argv[]) {
	tp = malloc(sizeof(thread_pool_t));
	thread_pool_init(tp, 5, 15);
	
	pthread_t tidTmp;
	pthread_create(&tidTmp, NULL, print_data, NULL);
	
	long choice;
	while (EOF != scanf("%ld", &choice)) {
		if (choice > 0) {
			thread_pool_task_t * tmp = task_create(task_handler, (void *)choice, complete_call_back); 
			thread_pool_add_task(tp, tmp);
		}
	}
		
	thread_pool_destroy(tp);
	free(tp);
	tp = NULL;

	return 0;
}
