/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 28 Jun 2020 05:22:44 PM CST
 @ File Name	: test_log.c
 @ Description	: 
 ************************************************************************/
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

/***************************************************
* global variable, marco 
***************************************************/
int port = 8888;
char * ip_addr = "0.0.0.0";

/***************************************************
* other 
***************************************************/
void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode)
{

#if (-1 == my_errno)
	fprintf(stream, "%s\n", headStr);	
#else 
	if (!my_errno) {
		my_errno = errno;
	}	
	fprintf(stream, "%s : %s\n", headStr, strerror(my_errno));	
#endif
	if (exitCode) {
		exit(exitCode);	
	}
}

// 处理main传参, 有需要自定义
void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:")) != -1) {
		switch (opt) {
			case 'h':
				ip_addr = optarg; 
				break;
			case 'p':
				port = atoi(optarg);
				break;	
               		default: 
               		    fprintf(stderr, "Usage: %s [-h ip][-p port]\n", argv[0]);
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

void PrintAddr(FILE * stream, struct sockaddr_in * addr, const char * headStr)
{
	char arr[INET_ADDRSTRLEN];	
	inet_ntop(AF_INET, &(addr->sin_addr), arr, sizeof(arr)); 
	fprintf(stream, "%s, %s:%d\n", headStr, arr, ntohs(addr->sin_port));	
}

void sigint(evutil_socket_t fd, short what, void * arg)
{	
	struct timeval t1 = {1, 0};
	struct event_base * base = (struct event_base *)arg;
	event_base_loopexit(base, &t1);
	printf("loop exit after 1 secs\n");
}

void read_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer *readbuf, *writebuf;
	readbuf = evbuffer_new();
	writebuf = evbuffer_new();
	if (NULL == readbuf || NULL == writebuf) {
		PrintError(stderr, -1, "read_cb, evbuffer_new error", EXIT_FAILURE);
	}

	// 读buff
	if (-1 == bufferevent_read_buffer(bev, readbuf)) {
		PrintError(stderr, -1, "read_cb, bufferevent_read_buffer error", EXIT_FAILURE);
	}

	// do something	
	evbuffer_add_buffer(writebuf, readbuf);

	// 写buff
	if (-1 == bufferevent_write_buffer(bev, writebuf)) {
		PrintError(stderr, -1, "read_cb, bufferevent_write_buffer error", EXIT_FAILURE);
	}

	evbuffer_free(readbuf);
	evbuffer_free(writebuf);	
}

void write_cb(struct bufferevent *bev, void *ctx)
{
	int cfd = bufferevent_getfd(bev);
	printf("fd %d write data successfully\n", cfd);
}

void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	if (BEV_EVENT_EOF & what) {
		bufferevent_free(bev);
		int cfd = bufferevent_getfd(bev);
		printf("close fd %d\n", cfd);	
	} else if (BEV_EVENT_TIMEOUT& what) {

	} else {
		PrintError(stderr, -1, "event_cb, some errors happen", EXIT_FAILURE);
	}
}

void connect_call_back(struct evconnlistener * evlistener, evutil_socket_t cfd, struct sockaddr * cli_addr, int socklen, void * arg)
{
	const char * aa = (const char *)arg;
	PrintAddr(stdout, (struct sockaddr_in *)cli_addr, aa);
	
	// 添加读写bufferevent
	struct event_base * base = evconnlistener_get_base(evlistener);	
	struct bufferevent * bev = bufferevent_socket_new(base, cfd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bev) {
		PrintError(stderr, -1, "bufferevent_socket_new error", EXIT_FAILURE);
	}
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

	// TODO, 默认就开启write， 不要在这里指定write， 居然会在连接建立后马上调用write_cb
	if (-1 == bufferevent_enable(bev, EV_READ)) {
		PrintError(stderr, -1, "bufferevent_enable error", EXIT_FAILURE);
	}
}

struct evconnlistener * BindListener(struct event_base *base)
{
	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(port);
	
	struct in_addr tmpAddr; 
	int ret  = inet_pton(AF_INET, ip_addr, &tmpAddr);
	if (-1 == ret) {
		PrintError(stderr, 0, "inet_pton error", EXIT_FAILURE);				
	} else if (0 == ret) {
		PrintError(stderr, 0, "inet_pton error, invalid ip", EXIT_FAILURE);				
	}	
	svrAddr.sin_addr.s_addr = tmpAddr.s_addr;
	
	struct evconnlistener * evlistener = evconnlistener_new_bind(base, connect_call_back, "new connection", LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (struct sockaddr *)&svrAddr, sizeof(svrAddr));
	if (NULL == evlistener) {
		PrintError(stderr, -1, "evconnlistener_new_bind error", EXIT_FAILURE);
	}

	return evlistener;
}


void log_cb(int severity, const char *msg) {
	FILE * fp = fopen("tmp_log_file", "a");

	switch (severity) {
		case EVENT_LOG_DEBUG:
			printf("this is a EVENT_LOG_DEBUG message\n");
			break;
		case EVENT_LOG_MSG:
			printf("this is a EVENT_LOG_MSG message\n");
			break;
		case EVENT_LOG_WARN:
			printf("this is a EVENT_LOG_WARN message\n");
			break;
		case EVENT_LOG_ERR:
			printf("this is a EVENT_LOG_ERR message\n");
			break;
			
	}
	fprintf(fp, "%s\n", msg);
	fclose(fp);
}


int main(int argc, char *argv[]) {		
	HandleOpt(argc, argv);
	
	event_enable_debug_logging(EVENT_DBG_ALL);
	event_set_log_callback(log_cb);


	struct event_base * base = event_base_new();
	struct event * sigEv = event_new(base, SIGINT, EV_SIGNAL | EV_PERSIST, sigint, base); 
	event_add(sigEv, NULL);
	struct evconnlistener * evlistener = BindListener(base);

	printf("begin event loop\n");
	event_base_dispatch(base);

	event_free(sigEv);
	evconnlistener_free(evlistener);
	event_base_free(base);

	return 0;
}
