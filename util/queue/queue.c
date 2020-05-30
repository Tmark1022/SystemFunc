/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sat 30 May 2020 05:13:40 PM CST
 @ File Name	: queue.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "queue.h"

#define QUEUE_SUCCESS	1
#define QUEUE_FAILED	0
typedef struct queue_t {
	unsigned int head;
	unsigned int rear;
	unsigned int max_size;
	unsigned int cur_size;
	void **plist;
}queue_t;

int queue_init(struct queue_t * que, unsigned int size)
{

	if (NULL == que || size <= 0) {
		return QUEUE_FAILED;
	}		

	void **ptr = malloc(size * sizeof(void *));
	if (NULL == ptr) {
		return QUEUE_FAILED;
	}
	
	que->head = 0;	
	que->rear = 0;	
	que->max_size = size;	
	que->cur_size = 0;	
	que->plist = ptr;	
	
	return QUEUE_SUCCESS;
}


int queue_destroy(struct queue_t * que)
{
	if (NULL == que) {
		return QUEUE_FAILED;
	}		
	
	que->head = 0;	
	que->rear = 0;	
	que->max_size = 0;	
	que->cur_size = 0;	
	if (que->plist) {
		free(que);
	}
	que->plist = NULL;	
	
	return QUEUE_SUCCESS;

}

int queue_max_size(struct queue_t * que)
{
	if (NULL == que) {
		return QUEUE_FAILED;
	}		
	
	return que->max_size;
}

int queue_cur_size(struct queue_t * que)
{
	if (NULL == que) {
		return QUEUE_FAILED;
	}		
	
	return que->cur_size;
}

int queue_is_empty(struct queue_t * que)
{
	if (NULL == que) {
		return QUEUE_FAILED;
	}		
	
	return que->cur_size < que->max_size;
}

int queue_enqueue(struct queue_t * que, const void * ptr)
{
	if (NULL == que || NULL == ptr) {
		return QUEUE_FAILED;
	}		
	
	if (!queue_is_empty(que)) {
		return QUEUE_FAILED;		
	}
	
	++(que->cur_size);
	que->plist[que->rear] = (void *)ptr;
	que->rear = (que->rear + 1) % que->max_size;  

	return QUEUE_SUCCESS;
}

int queue_dequeue(struct queue_t * que, void ** ptr)
{
	if (NULL == que || NULL == ptr) {
		return QUEUE_FAILED;
	}		
	
	if (que->cur_size <= 0) {
		return QUEUE_FAILED;		
	}
	
	--(que->cur_size);
	*ptr = que->plist[que->head];
	que->head = (que->head + 1) % que->max_size;   

	return QUEUE_SUCCESS;
}

void queue_print(FILE *stream, struct queue_t * que)
{
	if (NULL == que || NULL == stream) {
		return ;
	}		
	fprintf(stream, "head : %d, rear : %d, max_size : %d, cur_size : %d\n", que->head, que->rear, que->max_size, que->cur_size);
}

