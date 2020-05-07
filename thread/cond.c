/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 07 May 2020 10:37:18 AM CST
 @ File Name	: cond.c
 @ Description	: 条件变量实现生产者消费者demo，【支持多生产者多个消费者】
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int producerCnt = 3;
int consumerCnt = 5;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct Node{
	struct Node * next;
	int no;
} Node;
Node * head = NULL;

void * ThreadProducer(void *arg) 
{
	srand(time(NULL));
	while (1) {
		Node * tmp = malloc(sizeof(Node));	
		if (NULL == tmp) {
			perror("");
			exit(EXIT_FAILURE);
		}
		tmp->no = rand() % 100;
		tmp->next = NULL;

		pthread_mutex_lock(&mutex);

		// produce
		tmp->next = head;
		head = tmp;
		printf("producer, %lu, no is %d\n", pthread_self(), tmp->no);
	
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);

		//sleep(rand() % 3);
	}
	
	return NULL;
}

void * ThreadConsumer(void *arg) 
{	
	srand(time(NULL));

	while (1) {
		pthread_mutex_lock(&mutex);

		while (NULL == head) {
			pthread_cond_wait(&cond, &mutex);
		}
		
		// consume 
		Node * tmp = head;
		head = head->next;
		printf("==================consumer, %lu, no is %d\n", pthread_self(), tmp->no);

		pthread_mutex_unlock(&mutex);

		sleep(rand() % 3);
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	pthread_t * tidList = calloc(producerCnt + consumerCnt, sizeof(pthread_t));
	
	for(int i = 0; i < producerCnt; ++i)
		pthread_create(tidList + i, NULL, ThreadProducer, NULL);

	for(int i = 0; i < consumerCnt; ++i)
		pthread_create(tidList + i + producerCnt, NULL, ThreadConsumer, NULL);

	// join
	for (int i = 0; i < producerCnt + consumerCnt; ++i) {
		pthread_join(*(tidList + i), NULL);	
	}

	pthread_cond_destroy(&cond);
	return 0;
}
