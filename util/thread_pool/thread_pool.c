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

/***************************************************
* declaration 
***************************************************/
void * work_thread_handler(void * arg);

/***************************************************
* 这些函数的实现没有加锁【没有把锁设置成recursive lock】, 使用前请加锁
***************************************************/
int FindTidIdx(thread_pool_t * tp,  pthread_t tid)
{	
	if (NULL == tp) {
		return -1;	
	}

	for (int idx = 0; idx < tp->max_thread_cnt; ++idx) {
		if (pthread_equal(tp->tid_list[idx], tid)) {
			return idx;
		}
	}
	
	return -1;
}

int GetUnusedTidIdx(thread_pool_t * tp)
{	
	if (NULL == tp) {
		return -1;	
	}

	for (int idx = 0; idx < tp->max_thread_cnt; ++idx) {
		if (pthread_equal(tp->tid_list[idx], 0)) {
			return idx;
		}
	}
	
	return -1;
}

int IsNeedExpand(thread_pool_t * tp)
{	
	if (NULL == tp) {
		return 0;	
	}

	// 正在忙的线程有70%, 那么就尝试拓展线程	
	float rate = tp->act_thread_cnt * 1.0 / tp->cur_thread_cnt;	
	if (rate >= 0.7) {
		return 1;
	}	

	return 0;
}

/* 拓张策略， 返回变化个数 */
int ExpandStrategy(thread_pool_t * tp)
{
	int defaultCnt = 5;
	int canExpandCnt = tp->max_thread_cnt - tp->cur_thread_cnt;
	if (canExpandCnt > defaultCnt) {
		return defaultCnt; 
	} else {
		return canExpandCnt;
	}
}

int IsNeedShrink(thread_pool_t * tp)
{	
	if (NULL == tp) {
		return 0;	
	}

	// 正在忙的线程少于30%, 删减
	float rate = tp->act_thread_cnt * 1.0 / tp->cur_thread_cnt;	
	if (rate < 0.3) {
		return 1;
	}	

	return 0;
}

/* 缩小策略， 返回变化个数 */
int ShrinkStrategy(thread_pool_t * tp)
{
	// 最终目标保持忙线程占比大概50%左右
	int resCnt = tp->act_thread_cnt * 2;
	if (resCnt < tp->min_thread_cnt) {
		resCnt = tp->min_thread_cnt;
	}		

	int diffCnt = tp->cur_thread_cnt - resCnt;
	if (diffCnt > 0) 
		return diffCnt;
	else 
		return 0;

}

/***************************************************
* definition
***************************************************/
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
	thread_pool_t * tp = (thread_pool_t *)arg;
	if (NULL == tp) {
		thread_pool_print_error(stderr, EINVAL, "ctrl_thread_handler(argument)", EXIT_FAILURE);
	}	

	sigset_t set;	
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	struct timespec timeout;
	timeout.tv_nsec = 0;
	timeout.tv_sec = 0;
	while (1) {	
		sleep(CTRL_THREAD_RUN_INTERVAL);

		// 监听信号处理shutdown_flag	
		int getSigNo = sigtimedwait(&set, NULL, &timeout);
		if (-1 == getSigNo) {
			if (errno != EAGAIN) {
				thread_pool_print_error(stderr, 0, "ctrl_thread_handler(sigtimedwait)", EXIT_FAILURE);
			}
		}

		if ( SIGQUIT == getSigNo) {
			pthread_mutex_lock(&tp->global_lock);
			tp->shutdown_flag = 1;
			pthread_mutex_unlock(&tp->global_lock);
			printf("set shutdown_flag\n");
		}

		pthread_mutex_lock(&tp->global_lock);
		if (tp->shutdown_flag && tp->cur_thread_cnt <= 0) {
			// 所有工作线程已经退出	
			pthread_mutex_unlock(&tp->global_lock);		
			printf("ctrl thread exit...\n");
			break;
		}

		// 动态调整线程池大小
		if (!tp->shutdown_flag) {
			if (IsNeedExpand(tp)) {
				// 拓张线程池
				int expandCnt = ExpandStrategy(tp);
				if (expandCnt) {
					printf("try to expand %d cnt thread \n", expandCnt);
					for (int i = 0; i < expandCnt; ++i) {
						int unusedIdx = GetUnusedTidIdx(tp);
						if (-1 == unusedIdx) {
							thread_pool_print_error(stderr, EINVAL, "ctrl_thread_handler (too much thread?)", 0);
							continue;
						}

						int ret = pthread_create(tp->tid_list + unusedIdx, NULL, work_thread_handler, (void *)tp);
						if (ret) {
							thread_pool_print_error(stderr, ret, "ctrl_thread_handler error (pthread_create)", EXIT_FAILURE);
						}	
						pthread_detach(tp->tid_list[unusedIdx]);
						++tp->cur_thread_cnt;
					}
				}
			} else if (IsNeedShrink(tp)) {
				// 缩小线程池
				int shrinkCnt = ShrinkStrategy(tp);
				if (shrinkCnt > 0) {
					printf("try to shrink %d cnt thread \n", shrinkCnt);
					tp->wait_exit_thread_cnt += shrinkCnt;
				}
			}	
		}

		// 关闭所有线程
		if (tp->shutdown_flag) {
			tp->wait_exit_thread_cnt = tp->cur_thread_cnt;
			// 激活阻塞的add_task	
			pthread_cond_broadcast(&tp->queue_not_full);
		} 

		if (tp->wait_exit_thread_cnt) {
			// 激活工作线程
			pthread_cond_broadcast(&tp->queue_not_empty);
		}	

		pthread_mutex_unlock(&tp->global_lock);		
	}
	

	printf("ctrl thread, all done, try to exit\n");	
	return NULL;
}


/* 工作线程出来了函数 */
void * work_thread_handler(void * arg)
{
	thread_pool_t * tp = (thread_pool_t *)arg;
	if (NULL == tp) {
		thread_pool_print_error(stderr, EINVAL, "work_thread_handler(argument)", EXIT_FAILURE);
	}	
	
	while (1) {	
		pthread_mutex_lock(&tp->global_lock);
	
		while (queue_is_empty(&tp->task_queue) && !tp->shutdown_flag) {
			// 任务队列空了 且 没shutdown
			pthread_cond_wait(&tp->queue_not_empty, &tp->global_lock);

			if (tp->shutdown_flag || tp->wait_exit_thread_cnt) {
				// 跳出检测任务队列循环
				break;
			}
		}	

		if (tp->shutdown_flag || tp->wait_exit_thread_cnt) {
			// 检测到要关闭线程池 或 有待关闭的线程
			int idx = FindTidIdx(tp, pthread_self());
			if (-1 == idx) {
				thread_pool_print_error(stderr, EINVAL, "work_thread_handler(FindTidIdx)", EXIT_FAILURE);
			}

			// 标记未使用
			tp->tid_list[idx] = 0;
			if (tp->wait_exit_thread_cnt) {
				--tp->wait_exit_thread_cnt;
			}
			--tp->cur_thread_cnt;
			
			printf("tid %lu, idx %d auto exit \n", pthread_self(), idx);
		
			pthread_mutex_unlock(&tp->global_lock);
			break;
		}
			
		// 从任务队列获取任务, 设置忙状态
		thread_pool_task_t * taskp;
		if (!queue_dequeue(&tp->task_queue, (void **)&taskp)) {
			thread_pool_print_error(stderr, EINVAL, "work_thread_handler(queue_dequeue)", EXIT_FAILURE);
		}
		++tp->act_thread_cnt;	
		pthread_cond_signal(&tp->queue_not_full);
		pthread_mutex_unlock(&tp->global_lock);

		// 执行任务
		void * res = taskp->task_handler(taskp->arg); 
	
		// 任务完成回调
		if (NULL != taskp->task_complete_call_back) {	
			taskp->task_complete_call_back(res);
		}
		
		// 重置忙状态
		pthread_mutex_lock(&tp->global_lock);
		--tp->act_thread_cnt;
		pthread_mutex_unlock(&tp->global_lock);	
	}

	return NULL;
}

int thread_pool_init(thread_pool_t * tp, unsigned int min_thread_cnt, unsigned int max_thread_cnt)
{
	if (NULL == tp || min_thread_cnt <= 0 || max_thread_cnt <= 0 || min_thread_cnt > max_thread_cnt) {
		thread_pool_print_error(stderr, EINVAL, "thread_pool_init error (argument)", EXIT_FAILURE);
	}
	
	int ret;	
	ret = pthread_mutex_init(&(tp->global_lock), NULL);
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

	// 这里模拟通过信号设置shutdown_flag, 这里将主线程设置屏蔽, 后续pthread_create的线程都会继承
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

	ret = pthread_mutex_lock(&tp->global_lock);
	
	// 控制线程
	ret = pthread_create(&tp->ctrl_tid, NULL, ctrl_thread_handler, (void *)tp);
	if (ret) {
		thread_pool_print_error(stderr, ret, "thread_pool_init error (pthread_create)", EXIT_FAILURE);
	}
	pthread_detach(tp->ctrl_tid);
	
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
		pthread_detach(tid_list[idx]);
		++tp->cur_thread_cnt;
	}	
	
	ret = pthread_mutex_unlock(&tp->global_lock);

	return THREAD_POOL_SUCCESS;
}

int thread_pool_destroy(thread_pool_t * tp)
{
	pthread_mutex_destroy(&tp->global_lock);
	pthread_cond_destroy(&tp->queue_not_full);
	pthread_cond_destroy(&tp->queue_not_empty);
	queue_destroy(&tp->task_queue);

	if (tp->tid_list) {
		free(tp->tid_list);
	}

	return THREAD_POOL_SUCCESS;
}

void thread_pool_print_struct(FILE * stream, thread_pool_t * tp)
{
	// 单纯读和打印， 不用加锁
	fprintf(stream, "\nthread_pool_print_struct ---------------\n task_queue_cnt %d\n shutdown_flag %d\n min_thread_cnt %d\n max_thread_cnt %d\n cur_thread_cnt %d\n act_thread_cnt %d\n wait_exit_thread_cnt %d\n -------------------------------end\n\n", queue_cur_size(&tp->task_queue), tp->shutdown_flag, tp->min_thread_cnt, tp->max_thread_cnt, tp->cur_thread_cnt, tp->act_thread_cnt, tp->wait_exit_thread_cnt);	

	fprintf(stream, "tid_list : ");
	for (int idx = 0; idx < tp->max_thread_cnt; ++idx) {
		fprintf(stream, "%lu ", tp->tid_list[idx]);
	}
	fprintf(stream, "\n");
	fflush(stream);
}

thread_pool_task_t * task_create(void * (*task_handler)(void *), void * arg , void (*task_complete_call_back)(void *))
{
	if (NULL == task_handler) {
		return NULL;
	}
	thread_pool_task_t * ptr = malloc(sizeof(thread_pool_task_t));
	if (NULL == ptr) {
		return NULL;
	}

	ptr->task_handler = task_handler;
	ptr->arg = arg;
	ptr->task_complete_call_back = task_complete_call_back;

	return ptr;
}

int thread_pool_add_task(thread_pool_t * tp, thread_pool_task_t * task)
{
	if (NULL == tp || NULL == task) {
		thread_pool_print_error(stderr, EINVAL, "thread_pool_add_taskerror (argument)", 0);
		return THREAD_POOL_FAILED;
	}	
	
	pthread_mutex_lock(&tp->global_lock);
	
	while (!queue_is_not_full(&tp->task_queue) && !tp->shutdown_flag) {
		// 任务队列满了 且 没shutdown
		pthread_cond_wait(&tp->queue_not_full, &tp->global_lock);
	}	
	
	if (tp->shutdown_flag) {
		pthread_mutex_unlock(&tp->global_lock);
		return THREAD_POOL_SUCCESS;	
	}

	// 加入任务队列
	int ret = queue_enqueue(&tp->task_queue, task);	
	if (!ret) {
		thread_pool_print_error(stderr, EINVAL, "thread_pool_add_taskerror (queue_enqueue)", 0);
		pthread_mutex_unlock(&tp->global_lock);
		return THREAD_POOL_FAILED;	
	}

	printf("add task successfully\n");

	pthread_cond_signal(&tp->queue_not_empty);

	pthread_mutex_unlock(&tp->global_lock);
	return THREAD_POOL_SUCCESS;	
}

