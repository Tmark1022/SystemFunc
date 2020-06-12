/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 12 Jun 2020 11:41:58 AM CST
 @ File Name	: named_semaphore.c
 @ Description	: 命名信号量实现没有关系的进程间（线程）通信（同步）
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
#include <sys/types.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
 
int dkey;
int proId = 1022; 
char * path = "/";
int shmid;
int mode = 0;	// 0 生产者， 1 消费者
void * ptr = NULL;
char * head;

sem_t *sem_space, *sem_product;

#define SEMAPHORE_SPACE_PATH	"/semaphore_space"
#define SEMAPHORE_PRODUCT_PATH	"/semaphore_product"

// 队列放在共享内存中
#define QUEUE_MAX_SIZE	5
union data_elem_t {
	long num; 
	char str[128];
};
struct queue_data_t {
	unsigned int front;
	unsigned int rear;	
	unsigned int size;
	union data_elem_t arr[QUEUE_MAX_SIZE];  
};

void queue_init(struct queue_data_t * queue)
{
	if (NULL == queue) {
		fprintf(stderr, "parameter error\n");
		exit(EXIT_FAILURE);
	}

	bzero(queue, sizeof(struct queue_data_t));
}

// 剩余空间数
unsigned int queue_get_free_space(struct queue_data_t * queue)
{
	if (NULL == queue) {
		fprintf(stderr, "parameter error\n");
		exit(EXIT_FAILURE);
	}
	
	return QUEUE_MAX_SIZE - queue->size;
}

void queue_add(struct queue_data_t * restrict queue, const union data_elem_t * restrict tmp) 
{
	if (NULL == queue || NULL == tmp) {
		fprintf(stderr, "parameter error\n");
		exit(EXIT_FAILURE);
	}
	
	if (queue_get_free_space(queue) <= 0) {
		fprintf(stderr, "no free space\n");
		return ;
	}

	memcpy(&(queue->arr[queue->rear]), tmp, sizeof(union data_elem_t));		
	queue->rear = (queue->rear + 1) % QUEUE_MAX_SIZE; 
	++queue->size;						
}

void queue_pop(struct queue_data_t * queue, union data_elem_t  * tmp) 
{
	if (NULL == queue) {
		fprintf(stderr, "parameter error\n");
		exit(EXIT_FAILURE);
	}

	if (queue->size <=  0) {
		fprintf(stderr, "no elem data\n");
		return ;
	}

	memcpy(tmp, &(queue->arr[queue->front]), sizeof(union data_elem_t));		
	queue->front = (queue->front+ 1) % QUEUE_MAX_SIZE; 
	--queue->size;						
}

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
	int size =  sizeof(struct queue_data_t); 

	// 先尝试获取， 不存在就创建
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

				queue_init((struct queue_data_t *)tmp);

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
	struct queue_data_t * tmp = (struct queue_data_t *)ptr;
	int id = 1;	
	srand(time(NULL));

	union data_elem_t  elem;
	while(1) {					
		sprintf(elem.str, "%s:%d <--- from %d", head, id++, getpid());	
		sem_wait(sem_space);
		queue_add(tmp, &elem); 
		printf("produce successfully, %s\n", elem.str);
		sem_post(sem_product);
		sleep(rand() % 5);
	}	
}

void Consumer()  
{
	struct queue_data_t * tmp = (struct queue_data_t *)ptr;

	union data_elem_t  elem;
	while(1) {					
		sem_wait(sem_product);
		queue_pop(tmp, &elem); 
		printf("%d consume, %s\n", getpid(), elem.str);
		sem_post(sem_space);
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

	// unlink 过后， 用相同的name再也不能得到原来的那个命名semaphore对象，使用sem_open得到的是另外一个新的semaphore实体
	sem_unlink(SEMAPHORE_SPACE_PATH);
	sem_unlink(SEMAPHORE_PRODUCT_PATH);
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
	
	sem_close(sem_space);
	sem_close(sem_product);

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

	sem_space = sem_open(SEMAPHORE_SPACE_PATH, O_CREAT, 0600, QUEUE_MAX_SIZE);
	sem_product = sem_open(SEMAPHORE_PRODUCT_PATH, O_CREAT, 0600, 0);
	if (SEM_FAILED == sem_space || SEM_FAILED == sem_product) {
		perror("sem_open error");
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
