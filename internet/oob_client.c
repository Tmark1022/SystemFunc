/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 30 Sep 2020 03:43:04 PM CST
 @ File Name	: oob_client.c
 @ Description	: 发送带外数据（tcp 紧急模式）客户端
 ************************************************************************/
#include <inet_wrap.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

/***************************************************
* global variable, marco 
***************************************************/
char * serv = NULL;
char * host = NULL;

/***************************************************
* other 
***************************************************/
void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:b")) != -1) {
		switch (opt) {
			case 'h':
				host = optarg; 
				break;
			case 'p':
				serv = optarg;
				break;	
               		default: 
               		    fprintf(stderr, "Usage: %s [-h host][-p serv]\n", argv[0]);
               		    exit(EXIT_FAILURE);
               	}
	}
	
	/*
	if (optind >= argc) {
		fprintf(stderr, "need more argument\n");
		exit(EXIT_FAILURE);
	}
	*/
}

/***************************************************
* 
***************************************************/

int sockfd = -1;
int sendFlag = 0;
void sig_int(int signo)
{
	if (sendFlag) {
		write(STDOUT_FILENO, "sigint\n", 7);	
		return ;
	}


	// one byte oob				
	if (sockfd >= 0) {
		printf("send oob data\n");
		Send(sockfd, "oob t", 5, MSG_OOB);			
		sendFlag = 1;
	}
}

void sig_quit(int signo)
{
}

int main(int argc, char *argv[]) {
	HandleOpt(argc, argv); 

	sockfd = Tcp_connect(host, serv);
	
	// 10000 bytes discard data
	char tmp[10000];
	for (int idx = 0; idx < 10000; ++idx) {
		tmp[idx] = 'x';
	}
	printf("begin send discard data\n");
	Send(sockfd, tmp, 10000, 0);
	
	signal(SIGINT, sig_int);
	
	// stop until sending oob data
	pause();

	// normal data
	char buf[MAX_BUF_SIZE];
	for (int idx = 0; idx < MAX_BUF_SIZE; ++idx) {
		buf[idx] = 'a';	
	}
	printf("begin send normal data\n");
	Send(sockfd, buf, MAX_BUF_SIZE, 0);

	pause();
	close(sockfd);
	return 0;
}
