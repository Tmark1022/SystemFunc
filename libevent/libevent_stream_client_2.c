/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 22 Sep 2020 05:36:36 PM CST
 @ File Name	: libevent_stream_client_2.c
 @ Description	: libevent 客户端2, bufferevent
 ************************************************************************/
#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <signal.h>

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <inet_wrap.h>

/***************************************************
* global variable, marco 
***************************************************/
char * host = NULL;
int port = 8888; 
unsigned int tcp_send_buf = 0, tcp_recv_buf= 0;
int keepalive = 0;

/***************************************************
* other 
***************************************************/
// 处理main传参, 有需要自定义
void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:S:R:K")) != -1) {
		switch (opt) {
			case 'h':
				host = optarg; 
				break;
			case 'p':
				port = atoi(optarg);
				break;	
			case 'S':
				tcp_send_buf = atoi(optarg);	
				break;
			case 'R':
				tcp_recv_buf = atoi(optarg);	
				break;
			case 'K':
				keepalive = 1;
				break;
               		default: 
               		    fprintf(stderr, "Usage: %s [-h ip][-p port][-S send_buff][-R recv_buff][-K]\n", argv[0]);
               		    exit(EXIT_FAILURE);
               	}
	}
	
	/*
	if (optind >= argc) {
		fprintf(stderr, "need more argument\n");
		exit(EXIT_FAILURE);
	}
	*/
	printf("args : host(%s), port(%d)\n", host, port);

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
	
	if (tcp_send_buf > 0) {
		if (-1 == setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &tcp_send_buf, send_len)) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}	
}

void set_socket_keep_alive(int fd)
{
	int flags =1;
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags))) { 
		perror("ERROR: setsocketopt(), SO_KEEPALIVE"); 
		exit(0); 
	}

	printf("fd %d KEEP ALIVE\n", fd);
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

void sigint(evutil_socket_t fd, short what, void * arg)
{	
	struct timeval t1 = {1, 0};
	struct event_base * base = (struct event_base *)arg;
	event_base_loopexit(base, &t1);
	printf("loop exit after 1 secs\n");
}







/***************************************************
*   
***************************************************/
void read_cb(struct bufferevent *bev, void *ctx)
{
	char buf[MAX_BUF_SIZE];
	
	// eof 不会到这里， 到event_cb
	int size = bufferevent_read(bev, buf, MAX_BUF_SIZE);
	if (size > 0) {
		Write(STDOUT_FILENO, buf, size);
	}
}


void stdin_read(evutil_socket_t fd, short what, void * arg)
{		
	struct bufferevent *bev = (struct bufferevent *)arg;
	char buf[MAX_BUF_SIZE];
	if (NULL != fgets(buf, MAX_BUF_SIZE, stdin)) {
		// 写到socket
		bufferevent_write(bev, buf, strlen(buf)); 		
	}
}

void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	if (BEV_EVENT_EOF & what) {
		bufferevent_free(bev);
		int cfd = bufferevent_getfd(bev);
		printf("close fd %d by peer\n", cfd);	
		sigint(SIGINT, EV_SIGNAL, bufferevent_get_base(bev));
	} else if (BEV_EVENT_TIMEOUT& what) {
	
	} else if (BEV_EVENT_CONNECTED & what) {
		printf("connect successfully\n");
	} else {
		PrintError(stderr, 0, "some error happen", EXIT_FAILURE);
		bufferevent_free(bev);
		sigint(SIGINT, EV_SIGNAL, bufferevent_get_base(bev));
	}
}

int main(int argc, char *argv[]) {		
	HandleOpt(argc, argv);
	
	// sigint
	struct event_base * base = event_base_new();
	struct event * sigEv = event_new(base, SIGINT, EV_SIGNAL | EV_PERSIST, sigint, base); 
	event_add(sigEv, NULL);

	// connect 
	struct bufferevent * bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);	
    	bufferevent_setcb(bev, read_cb, NULL, event_cb, base);
    	bufferevent_enable(bev, EV_READ);
    	bufferevent_socket_connect_hostname(bev, NULL, AF_UNSPEC, host, port);
	evutil_socket_t sockfd = bufferevent_getfd(bev);

	// 设置发送和接收缓冲buf
	set_socket_buf_value(sockfd);
	print_socket_buf_value(sockfd);
	// set keep alive timer 
	if (keepalive) {
		set_socket_keep_alive(sockfd);
	}
	
	// stdin 
	struct event * std_read = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, stdin_read, bev); 
	event_add(std_read, NULL);

	printf("begin event loop\n");
	event_base_dispatch(base);

	event_free(sigEv);
	event_free(std_read);
	event_base_free(base);

	return 0;
}
