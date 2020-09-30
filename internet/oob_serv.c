/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 30 Sep 2020 03:43:33 PM CST
 @ File Name	: oob_serv.c
 @ Description	: 带外数据 - 服务器
 ************************************************************************/ #include <inet_wrap.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

/***************************************************
* global variable, marco 
***************************************************/
char * serv = NULL;
char * host = NULL;

int tcp_recv_buf = 0;
/***************************************************
* other 
***************************************************/
void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:R:")) != -1) {
		switch (opt) {
			case 'h':
				host = optarg; 
				break;
			case 'p':
				serv = optarg;
				break;	
			case 'R' :
				tcp_recv_buf = atoi(optarg);
				break;
               		default: 
               		    fprintf(stderr, "Usage: %s [-h host][-p serv][-R buf]\n", argv[0]);
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

void set_socket_buf_value(int fd) 
{
	socklen_t recv_len, send_len; 
	recv_len = send_len = sizeof(int);
	if (tcp_recv_buf > 0) {
		if (-1 == setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &tcp_recv_buf, recv_len)) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}	
}

void print_socket_buf_value(int fd) 
{
	int recv_value, send_value;
	socklen_t recv_len, send_len; 
	recv_len = send_len = sizeof(int);
	if (-1 == getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &recv_value, &recv_len)) {
		PrintError(stderr, 0, "call getsockopt failed", EXIT_FAILURE);		
	}
	if (-1 == getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &send_value, &send_len)) {
		PrintError(stderr, 0, "call getsockopt failed", EXIT_FAILURE);		
	}
	
	printf("fd (%d), send buf is %d, recv buf is %d\n", fd, send_value, recv_value);
}

/***************************************************
* 
***************************************************/
int cfd = -1;
void sig_urg(int signo)
{
	if (-1 == cfd) {
		PrintError(stderr, 0, "cfd invalid", EXIT_FAILURE);		
	}

	int counter = 0;
	char tmp[MAX_BUF_SIZE];
	while (1) {
		if (sockatmark(cfd)) {
			printf("sock at mark\n");
			break;
		}		

		int cnt = Recv(cfd, tmp, MAX_BUF_SIZE, 0); 
		if (0 == cnt) {
			printf("in sig_urg, close by peer, ???????\n");
			exit(EXIT_SUCCESS);	
		}

		counter += cnt;
	}

	char oob[MAX_BUF_SIZE];
	int oobCnt = Recv(cfd, &oob, MAX_BUF_SIZE, MSG_OOB); 
	oob[oobCnt] = '\0';

	printf("\nget oob data %s, discard %d bytes data\n", oob, counter);
}


char buf[MAX_BUF_SIZE];
int main(int argc, char *argv[]) {
	HandleOpt(argc, argv); 
	int lfd = Tcp_listen(host, serv, NULL);

	set_socket_buf_value(lfd);
	print_socket_buf_value(lfd); 
	
	cfd = Accept(lfd, NULL, NULL);		
	
	fcntl(cfd, F_SETOWN, getpid());
	signal(SIGURG, sig_urg);	
	
	sleep(100);

	while (1) {
		int cnt = Recv(cfd, buf, MAX_BUF_SIZE, 0);		
		if (0 == cnt) {
			printf("\nclose by peer\n");
			close(cfd);
			break;
		}
		Write(STDOUT_FILENO, buf, cnt);
	}

	close(lfd);
	return 0;
}
