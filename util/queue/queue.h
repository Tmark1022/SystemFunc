#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>

#define QUEUE_SUCCESS	1
#define QUEUE_FAILED	0

// NOTE
/*
 *	plist 是void * 数组, 指向的内存空间是用户传参时指定的【并没有进行开辟新空间并进行内容copy】
 *	使用者有义务保证数据的有效性【杜绝生命周期短的局部变量赋值】
 */
typedef struct queue_t {
	unsigned int head;	
	unsigned int rear;
	unsigned int max_size;
	unsigned int cur_size;
	void ** plist;
}queue_t;

int queue_init(struct queue_t * que, unsigned int size);
int queue_destroy(struct queue_t * que);
int queue_max_size(struct queue_t * que);
int queue_cur_size(struct queue_t * que);
int queue_is_empty(struct queue_t * que);

int queue_enqueue(struct queue_t * que, const void * ptr);
int queue_dequeue(struct queue_t * que, void ** ptr);

void queue_print(FILE *stream, struct queue_t * que);

#endif
