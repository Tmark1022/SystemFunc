/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 25 May 2020 02:06:04 PM CST
 @ File Name	: barrier.c
 @ Description	: 线程同步之barrier
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <limits.h>

#define REDIRECT_OUTPUT

#define NUM_ARRAY 50000000

#define USE_BARRIER 
#define NUM_THREAD 5
#define NUM_HANDLER_PER_THREAD (NUM_ARRAY / NUM_THREAD)

int nums[NUM_ARRAY];
int snums[NUM_ARRAY];

pthread_barrier_t barrier;

void PrintArray(int * arr)
{
#ifdef REDIRECT_OUTPUT
	int cache = dup(STDOUT_FILENO);
	int fd = open("./output", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	dup2(fd, STDOUT_FILENO); 
	close(fd);
#endif
	for (int i = 0; i < NUM_ARRAY; ++i) {
		printf("%d ", arr[i]);
	}
	printf("\n");
	fflush(NULL);

#ifdef REDIRECT_OUTPUT
	dup2(cache, STDOUT_FILENO); 
	close(cache);
#endif
}

int comp_asc(const void * p1, const void * p2)
{
	return *((int *)p1) <= *((int *)p2) ? -1 : 1;			
}

void * ThreadHandler(void * arg)
{	
	int idx = (int)arg;  
	qsort(nums + idx, NUM_HANDLER_PER_THREAD, sizeof(int), comp_asc);	

	printf("thread %lu, sort finished ,range %d ~ %d\n", pthread_self(), idx, idx + NUM_HANDLER_PER_THREAD - 1);
#ifdef USE_BARRIER
	pthread_barrier_wait(&barrier);
#endif
	return NULL;
}

void merge()
{	
	int indexArr[NUM_THREAD];
	for (int i = 0; i < NUM_THREAD; ++i) {
		indexArr[i] = NUM_HANDLER_PER_THREAD * i;
	}

	long minVal, minTid;
	for (int i = 0; i < NUM_ARRAY; ++i) {
		minTid = -1;
		minVal = LONG_MAX; 
		for (int j = 0; j < NUM_THREAD; ++j) {
			if (indexArr[j] < ((j + 1) * NUM_HANDLER_PER_THREAD) && nums[indexArr[j]] < minVal) {
				minVal = nums[indexArr[j]];	
				minTid = j;
			}	
		}

		if (minTid < 0) {
			fprintf(stderr, "minTid < 0\n");
			exit(EXIT_FAILURE);
		}

		++indexArr[minTid];
		snums[i] = minVal;
	}	
}

int main(int argc, char *argv[]) {
	
	struct timeval start_tv, end_tv;
	// init
	srand(1);		// 固定随机队列
	for (int i = 0; i < NUM_ARRAY; ++i) {
		nums[i] = rand() % 10000;
	}
	gettimeofday(&start_tv, NULL);		

#ifdef USE_BARRIER
	pthread_barrier_init(&barrier, NULL, NUM_THREAD + 1);

	pthread_t tid;	
	for (int i = 0; i < NUM_THREAD; ++i) {	
		pthread_create(&tid, NULL, ThreadHandler, (void *)(NUM_HANDLER_PER_THREAD * i));
		pthread_detach(tid);
	}	
	
	pthread_barrier_wait(&barrier);
	merge();			
	pthread_barrier_destroy(&barrier);
#else
	for (int i = 0; i < NUM_THREAD; ++i) {	
		ThreadHandler((void *)(NUM_HANDLER_PER_THREAD * i));
	}		
	merge();			
#endif 


	gettimeofday(&end_tv, NULL);			
	PrintArray(snums);
	long long elapse = (end_tv.tv_sec * 1000 * 1000 + end_tv.tv_usec) - (start_tv.tv_sec * 1000 * 1000 + start_tv.tv_usec); 
	printf("elapse : %lld\n", elapse);

	return 0;
}
