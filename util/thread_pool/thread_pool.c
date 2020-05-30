/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sat 30 May 2020 08:18:17 PM CST
 @ File Name	: thread_pool.c
 @ Description	: 
 ************************************************************************/
#include <asm-generic/errno-base.h>
#include <pthread.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "thread_pool.h"

void thread_pool_print_error(FILE * stream, int my_errno, const char * headStr, int exitCode)
{
	if (!my_errno) {
		my_errno = errno;
	}	
	fprintf(stream, "%s : %s\n", headStr, strerror(my_errno));	
	if (exitCode) {
		exit(exitCode);	
	}
}

/* 控制线程处理函数 */
void * ctrl_thread_handler(void * arg)
{
	// TODO
	// 监听信号处理shutdown_flag
	/*
	 int sigtimedwait(const sigset_t *set, siginfo_t *info,
                        const struct timespec *timeout);
			*/
	//sigaction	

	// 根据活跃数线程动态调整线程池大小  
	

	return NULL;
}

/* 工作线程出来了函数 */
void * work_thread_handler(void * arg)
{
	// 获取任务队列锁
	
	// 阻塞任务队列条件变量
	
	// shutdown flag 检测

	// wait_exit 退出线程
	
	// 处理任务

	// TODO
	return NULL;
}

int thread_pool_init(thread_pool_t * tp, unsigned int min_thread_cnt, unsigned int max_thread_cnt)
{
	if (NULL == tp || min_thread_cnt <= 0 || max_thread_cnt <= 0 || min_thread_cnt > max_thread_cnt) {
		thread_pool_print_error(stderr, EINVAL, "thread_pool_init error (argument)", EXIT_FAILURE);
	}
	
	int ret;	
	ret = pthread_mutex_init(&(tp->task_queue_lock), NULL);
	if (ret) {
		thread_pool_print_error(stderr, ret, "thread_pool_init error (pthread_mutex_init)", EXIT_FAILURE);
	}
	
	ret = pthread_cond_init(&(tp->queue_not_full), NULL);
	if (ret) {
		thread_pool_print_error(stderr, ret, "thread_pool_init error (pthread_cond_init)", EXIT_FAILURE);
	}
	ret = pthread_cond_init(&(tp->queue_not_empty), NULL);
	if (ret) {
		thread_pool_print_error(stderr, ret, "thread_pool_init error (pthread_cond_init)", EXIT_FAILURE);
	}

	ret = queue_init(&(tp->task_queue), 2 * max_thread_cnt);
	if (!ret) {
		thread_pool_print_error(stderr, ENOMEM, "thread_pool_init error (queue_init)", EXIT_FAILURE);
	}

	// 线程池控制线程相关
	ret = pthread_mutex_init(&(tp->ctrl_thread_lock), NULL);
	if (ret) {
		thread_pool_print_error(stderr, ret, "thread_pool_init error (pthread_mutex_init)", EXIT_FAILURE);
	}

	// 所有线程都屏蔽SIGQUIT, 线程池控制线程使用sigtimedwait 来捕捉SIGQUIT信号
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	ret = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (ret) {
		thread_pool_print_error(stderr, ret, "thread_pool_init error (pthread_sigmask)", EXIT_FAILURE);
	}

	tp->shutdown_flag		= 0;
	tp->min_thread_cnt		= min_thread_cnt;	
	tp->max_thread_cnt		= max_thread_cnt;				
	tp->cur_thread_cnt		= 0;
	tp->act_thread_cnt		= 0;				
	tp->wait_exit_thread_cnt	= 0;			

	ret = pthread_mutex_lock(&tp->ctrl_thread_lock);
	
	// 控制线程
	ret = pthread_create(&tp->ctrl_tid, NULL, ctrl_thread_handler, (void *)tp);
	if (ret) {
		thread_pool_print_error(stderr, ret, "thread_pool_init error (pthread_create)", EXIT_FAILURE);
	}
	
	// 线程池线程
	int byte_cnt = max_thread_cnt * sizeof(pthread_t);
	pthread_t *	tid_list = malloc(byte_cnt);
	if (NULL == tid_list) {
		thread_pool_print_error(stderr, ret, "thread_pool_init error (malloc)", EXIT_FAILURE);
	}
	tp->tid_list = tid_list;
	memset(tid_list, 0, byte_cnt);
	for (int idx = 0; idx < min_thread_cnt; ++idx) {
		ret = pthread_create(tid_list + idx, NULL, work_thread_handler, (void *)tp);
		if (ret) {
			thread_pool_print_error(stderr, ret, "thread_pool_init error (pthread_create)", EXIT_FAILURE);
		}	
	}	
	
	ret = pthread_mutex_unlock(&tp->ctrl_thread_lock);

	return THREAD_POOL_SUCCESS;
}

int thread_pool_destroy(thread_pool_t * tp)
{
	pthread_mutex_destroy(&tp->task_queue_lock);
	pthread_cond_destroy(&tp->queue_not_full);
	pthread_cond_destroy(&tp->queue_not_empty);
	queue_destroy(&tp->task_queue);
	pthread_mutex_destroy(&tp->ctrl_thread_lock);

	if (tp->tid_list) {
		free(tp->tid_list);
	}

	return THREAD_POOL_SUCCESS;
}

void thread_pool_print_struct(thread_pool_t * tp);

int thread_pool_add_task(thread_pool_t * tp, thread_pool_task_t * task);
