/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 08 May 2020 11:15:50 AM CST
 @ File Name	: semaphore.c
 @ Description	: 信号量实现生产者消费者模型 
 ************************************************************************/
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM 5
#define PRODUCER_CNT 5
#define CONSUMER_CNT 5
pthread_t tidList[PRODUCER_CNT + CONSUMER_CNT];

sem_t sem_place, sem_product;

typedef struct Node{
	struct Node * next;
	int no;
} Node;

Node * head = NULL;


void * ThreadProduce(void * arg)
{
	srand(time(NULL));
	while(1) {
		Node * tmp = malloc(sizeof(Node));
		tmp->next = NULL;
		tmp->no = rand() % 1000;  

		sem_wait(&sem_place);		// 阻塞会被信号软件中断打破？？？？？？, 等待以后解决
		tmp->next = head;
		head = tmp;
		printf("===========(%dth) produce, no is %d\n", (int)arg, tmp->no);
		sem_post(&sem_product);

		sleep(rand() % 3);
	}

	return NULL;
}

void * ThreadConsume(void * arg)
{
	srand(time(NULL));
	while(1) {
		sem_wait(&sem_product);		// 阻塞会被信号软件中断打破？？？？？？, 等待以后解决
		Node * tmp = head;
		head = head->next;
		printf("(%dth) consume, no is %d\n", (int)arg, tmp->no);
		sem_post(&sem_place);
		free(tmp);

		sleep(rand() % 3);
	}

	return NULL;
}

int main(int argc, char *argv[]) {	

	if (-1 == sem_init(&sem_place, 0, NUM) || -1 == sem_init(&sem_product, 0, 0)) {
		perror("sem_init error");
		exit(EXIT_FAILURE);
	}
	
	for (int i = 0; i < PRODUCER_CNT; ++i) {
		pthread_create(tidList + i, NULL, ThreadProduce, (void *)i);
	}	
	for (int i = 0; i < CONSUMER_CNT; ++i) {
		pthread_create(tidList + i + PRODUCER_CNT, NULL, ThreadConsume, (void *)i);
	}	
	
	for (int i = 0; i < PRODUCER_CNT + CONSUMER_CNT; ++i) {
		pthread_join(tidList[i], NULL);
	}

	sem_destroy(&sem_place);
	sem_destroy(&sem_product);

	return 0;
}
