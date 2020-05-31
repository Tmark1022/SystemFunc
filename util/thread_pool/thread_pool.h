#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <queue.h>
#include <pthread.h>

/*
 *	thread_poll 依赖queue库， 编译时确保把lqueue依赖加入进来使用
 *
 */

#define THREAD_POOL_FAILED	0
#define THREAD_POOL_SUCCESS	1

#define CTRL_THREAD_RUN_INTERVAL	5

typedef struct thread_pool_task_t {
	void * (*task_handler)(void *);
	void * arg;
	void (*task_complete_call_back)(void *);		// 任务完成回调函数， 由处理任务的线程调用， 参数是task_handler的返回值
} thread_pool_task_t;

typedef struct thread_pool_t {
	// 任务队列相关
	pthread_mutex_t	global_lock;				// 整个结构体全局锁
	pthread_cond_t	queue_not_full; 
	pthread_cond_t	queue_not_empty; 
	struct queue_t	task_queue;				// 任务队列，长度为 2 * max_thread_cnt 

	// 线程池控制线程相关
	pthread_t	ctrl_tid;				// 控制线程id
	pthread_t *	tid_list;				// 线程池数组, pthread_t 是unsigned long 类型， 0 代表没有使用

	unsigned int	min_thread_cnt;				// 最少线程个数
	unsigned int	max_thread_cnt;				// 最多线程个数
	unsigned int	cur_thread_cnt;				// 当前线程个数
	unsigned int	act_thread_cnt;				// 正在活跃(work)的线程个数
	unsigned int	wait_exit_thread_cnt;			// 等待退出的线程个数, (被唤醒的线程会先去检查这个值， 如果非0就主动退出)
	int		shutdown_flag;				// 线程池关闭标记

} thread_pool_t;

void thread_pool_print_error(FILE * stream, int my_errno, const char * headStr, int exitCode);

int thread_pool_init(thread_pool_t * tp, unsigned int min_thread_cnt, unsigned int max_thread_cnt);
int thread_pool_destroy(thread_pool_t * tp);
void thread_pool_print_struct(FILE * stream, thread_pool_t * tp);

thread_pool_task_t * task_create(void * (*task_handler)(void *), void * arg , void (*task_complete_call_back)(void *));

int thread_pool_add_task(thread_pool_t * tp, thread_pool_task_t * task);

#endif
