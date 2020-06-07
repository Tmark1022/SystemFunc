/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 07 Jun 2020 01:02:42 PM CST
 @ File Name	: msgqueue.c
 @ Description	: 
 ************************************************************************/
#include <asm-generic/errno-base.h>
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
 
#define COMMUNICATION_MODE 0
#define MONITOR_MODE 1

int mode = COMMUNICATION_MODE; 
int dkey;
int proId = 1022; 
char * path = "/";
long cliId;

struct msgbuf {
	long mtype;       /* message type, must be > 0 */
	char mtext[1024];    /* message data */
};

void HandleOpt(int argc, char * argv[]) 
{
	int opt, tmp;
	while ((opt = getopt(argc, argv, "hp:i:k:m:")) != -1) {
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

               case 'm':
                   tmp = atoi(optarg);
		   if (tmp) {
			mode = MONITOR_MODE;
		   } else {
			mode = COMMUNICATION_MODE; 	
		   }
                   break;

               case 'h':
                   fprintf(stderr, "Usage: %s [-p path] [-i proj_id] [-k key] [-m mode] client\n", argv[0]);
                   exit(EXIT_SUCCESS);

               default: 
                   fprintf(stderr, "Usage: %s [-p path] [-i proj_id] [-k key] [-m mode] client\n", argv[0]);
                   exit(EXIT_FAILURE);
               }
           }

           if (optind >= argc) {
               fprintf(stderr, "Expected argument after options\n");
               exit(EXIT_FAILURE);
           }

	   cliId = atoi(argv[optind]); 
}

void print_perm(struct ipc_perm * perm)
{
	printf("perm struct is :\n");
	printf("		__key\t%d\n		uid\t%d\n		gid\t%d\n		cuid\t%d\n		cgid\t%d\n		mode\t%o\n		__seq\t%d\n", perm->__key, perm->uid, perm->gid, perm->cuid, perm->cgid, perm->mode, perm->__seq);
	
}

#define STIME(x) asctime(localtime(&x))
void print_msqid_ds(struct msqid_ds *buf)
{
	printf("==============================%ld===============================\n", time(NULL));
	printf("msg_stime %smsg_rtime %smsg_ctime %s", STIME(buf->msg_stime), STIME(buf->msg_rtime), STIME(buf->msg_ctime)); 
	printf("__msg_cbytes %lu, msg_qnum %lu, msg_qbytes %lu\n", buf->__msg_cbytes, buf->msg_qnum, buf->msg_qbytes);
	printf("msg_lspid %d, msg_lrpid %d\n", buf->msg_lspid, buf->msg_lrpid);	

	print_perm(&buf->msg_perm);
}

void monitor(int msgid) 
{
	struct msqid_ds buf;
	int ret = msgctl(msgid, IPC_STAT, &buf);
	if (-1 == ret) {
		perror("msgctl error");
		exit(EXIT_FAILURE);
	}
	print_msqid_ds(&buf);
}

void * read_from_msgq(void * arg)
{	
	int msgid = (int)arg;
	struct msgbuf dataBuf;
	
	while (1) {	
		ssize_t cnt = msgrcv(msgid, &dataBuf, 1023, cliId, 0);
		if (-1 == cnt) { 
			perror("msgrcv failed");
			if (EINTR != errno) {
				exit(EXIT_FAILURE);
			}
		}
		dataBuf.mtext[cnt] = '\0';
			
		printf("-------------------------begin------------------------\n");
		printf("recv %ld data, type is %ld\n", cnt, dataBuf.mtype);		
		printf("%s", dataBuf.mtext);
		printf("-------------------------end--------------------------\n");	
	}

	return NULL;
}

void communicate(int msgid) 
{
	// create 一个新的线程来接受数据	
	
	pthread_t tid;
	pthread_create(&tid, NULL, read_from_msgq, (void *)(long)msgid);

	struct msgbuf dataBuf;	
	while (1) {	
		scanf("%ld", &dataBuf.mtype);	
		getchar();
		fgets(dataBuf.mtext, 1024, stdin);

		int ret = msgsnd(msgid, &dataBuf, strlen(dataBuf.mtext), IPC_NOWAIT);
		if (-1 == ret) 
		{ perror("msgsnd failed");
			if (EAGAIN != errno && EINTR != errno) {
				exit(EXIT_FAILURE);
			}
		} else {
			printf("send successfully\n");
		}
	}

}

int main(int argc, char *argv[]) {

	HandleOpt(argc, argv);	
	int dkey  = ftok(path, proId);
	if (-1 == dkey) {
		perror("ftok error");
		exit(EXIT_FAILURE);
	}

	int msgid =  msgget(dkey, IPC_CREAT | 0660);
	if (-1 == msgid) {
		perror("msgget error");
		exit(EXIT_FAILURE);
	}

	printf("begin do it, mode : %d, key : %d, msgid : %d, cliId : %ld\n", mode, dkey, msgid, cliId);

	if (COMMUNICATION_MODE == mode) {
		communicate(msgid);
	} else {
		while (1) {
			monitor(msgid);
			sleep(1);
		}
	}
	
	return 0;
}
