/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 11 Jun 2020 02:53:26 PM CST
 @ File Name	: shm.c
 @ Description	: 开辟共享内存， 多个无关系进程进行通行（简单生产者消费者）
		  共享内存中使用pthread_mutex实现进程间的同步
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <errno.h>

#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
 
int dkey;
int proId = 1022; 
char * path = "/";
int shmid;
int mode = 0;	// 0 生产者， 1 消费者
void * ptr = NULL;
char * head;

// 共享内存空间组成
// mutex + pid_t + data_t, pid_t 非0 代表有数据
struct data_t {
	unsigned int size;		// 数据大小
	char buf[1024];  
};

#define DATA_SIZE	(sizeof(struct data_t))
#define OFFSET_MUTEX	(0)
#define OFFSET_PID	(OFFSET_MUTEX + sizeof(pthread_mutex_t))
#define OFFSET_DATA	(OFFSET_PID + sizeof(pid_t))

void HandleOpt(int argc, char * argv[]) 
{
	int opt, tmp;
	while ((opt = getopt(argc, argv, "mhp:i:k:")) != -1) {
		switch (opt) {
			case 'p':
               		    path = optarg;
               		    break;

               		case 'i':
               		    proId= atoi(optarg);
               		    break;

               		case 'k':
               		    dkey = atoi(optarg);
               		    break;

               		case 'h':
               		    fprintf(stderr, "Usage: %s [-p path] [-i proj_id] [-k key] string\n", argv[0]);
               		    exit(EXIT_SUCCESS);

			case 'm':
			    // 设置消费者模式
			    mode = 1;
			    break;

               		default: 
               		    fprintf(stderr, "Usage: %s [-p path] [-i proj_id] [-k key] string\n", argv[0]);
               		    exit(EXIT_FAILURE);
               	}
	}

	if (optind >= argc) {
		fprintf(stderr, "need more argument\n");
		exit(EXIT_FAILURE);
	}

	head = argv[optind];
}

int GetShmId()
{
	int size =  OFFSET_DATA + DATA_SIZE; 

	// 先从获取， 不存在就创建
	int shmid_tmp = shmget(dkey, size, 0);
	if (-1 == shmid_tmp) {
		if (ENOENT == errno) {
			printf("not exist shm, try to create one\n");
			shmid_tmp = shmget(dkey, size, IPC_CREAT | 0600);
			if (-1 != shmid_tmp) {
				// 进行初始化
				void *tmp = shmat(shmid_tmp, NULL, 0);
				if ((void *)-1 == tmp) {
					perror("init data error, shmat");
					exit(EXIT_FAILURE);
				}

				pthread_mutex_t * pmutex = (pthread_mutex_t *)(tmp + OFFSET_MUTEX);
				pthread_mutex_init(pmutex, NULL);
				pid_t * pidp= (pid_t *)(tmp + OFFSET_PID);
				*pidp = 0;
				shmdt(tmp);
				tmp = NULL;	
				printf("init shm data successfully\n");
			}
		}
	}

	if (-1 == shmid_tmp) {
		perror("shmget error");
		exit(EXIT_FAILURE);	
	}

	return shmid_tmp;
}

void Producer()  
{
	void * tmp = ptr;
	pthread_mutex_t * pmutex = (pthread_mutex_t *)(tmp + OFFSET_MUTEX);
	pid_t * pidp= (pid_t *)(tmp + OFFSET_PID);
	struct data_t * pdata = (struct data_t *)(tmp + OFFSET_DATA);  

	int id = 1;	
	srand(time(NULL));
	while(1) {					
		pthread_mutex_lock(pmutex);
		if (!(*pidp)) {
			// 没有数据		
			sprintf(pdata->buf, "%s:%d", head, id++);				
			pdata->size = strlen(pdata->buf);
			*pidp = getpid();
			printf("produce successfully, %s\n", pdata->buf);
		}
		pthread_mutex_unlock(pmutex);
		sleep(rand() % 5);
	}	
}

void Consumer()  
{
	void * tmp = ptr;
	pthread_mutex_t * pmutex = (pthread_mutex_t *)(tmp + OFFSET_MUTEX);
	pid_t * pidp= (pid_t *)(tmp + OFFSET_PID);
	struct data_t * pdata = (struct data_t *)(tmp + OFFSET_DATA);  

	char cache[1024];	
	while(1) {					
		pthread_mutex_lock(pmutex);
		if (*pidp) {
			// 有数据		
			memcpy(cache, pdata->buf, pdata->size);
			cache[pdata->size] = '\0'; 
			printf("%d consume --> %s <-- from %d\n", getpid(), cache, *pidp);
			*pidp = 0;	
		}
		pthread_mutex_unlock(pmutex);
		sleep(1);
	}	
}

void sig_quit(int signo) 
{
	int ret = shmctl(shmid, IPC_RMID, NULL);
	if (-1 == ret) {	
		perror("shmctl failed");
		if (EINVAL != errno) {
			exit(EXIT_FAILURE);
		}
	}
}

void sig_int(int signo) 
{
	if (ptr) {
		int ret = shmdt(ptr);
		if (-1 == ret) {
			perror("shmdt failed");
			exit(EXIT_FAILURE);
		}	
	}
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

	HandleOpt(argc, argv);	
	dkey = ftok(path, proId);
	if (-1 == dkey) {
		perror("ftok error");
		exit(EXIT_FAILURE);
	}
	
	shmid = GetShmId();	
	printf("begin, key : %d, id : %d, mode : %s\n", dkey, shmid, mode == 0 ? "producer" : "consumer");

	signal(SIGQUIT, sig_quit);
	signal(SIGINT, sig_int);

	ptr = shmat(shmid, NULL, 0);
	if ((void *)-1 == ptr) {
		perror("shmat failed");
		exit(EXIT_FAILURE);	
	}

	if (0 == mode) {
		// producer
		Producer();	
	} else {
		// consumer	
		Consumer();
	}	

	return 0;
}
