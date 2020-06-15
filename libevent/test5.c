/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 05 Jun 2020 06:14:15 PM CST
 @ File Name	: test5.c
 @ Description	: 
 ************************************************************************/
#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <ctype.h>

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#define AF_FAMILY	AF_INET

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

void PrintAddr(FILE * stream, struct sockaddr_in * addr, const char * headStr)
{
	char arr[INET_ADDRSTRLEN];	
	inet_ntop(AF_INET, &(addr->sin_addr), arr, sizeof(arr)); 
	fprintf(stream, "%s, %s:%d\n", headStr, arr, ntohs(addr->sin_port));	
}

void sigint(evutil_socket_t fd, short what, void * arg)
{	
	struct timeval t1 = {2, 0};
	struct event_base * base = (struct event_base *)arg;
	event_base_loopexit(base, &t1);
	printf("loop exit after 2 secs\n");
}


/***************************************************
* other 
***************************************************/
void read_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer * input_evbuff = bufferevent_get_input(bev);
	int total_cnt = evbuffer_get_length(input_evbuff);

	int cfd = bufferevent_getfd(bev);
	char buf[100];

	while (total_cnt > 0) {
		int cnt = bufferevent_read(bev, buf, 100);
		printf("read %d data from fd %d, total data is %d bytes\n", cnt, cfd, total_cnt);
		// toupper
		for(int i = 0; i < cnt; ++i)
			buf[i] = toupper(buf[i]);

		if (-1 == bufferevent_write(bev, buf, cnt)) {
			PrintError(stderr, -1, "read_cb, bufferevent_write error", EXIT_FAILURE);
		}
		
		total_cnt -= cnt;
	}
}

void write_cb(struct bufferevent *bev, void *ctx)
{
	int cfd = bufferevent_getfd(bev);
	printf("fd %d write data successfully\n", cfd);
}

#define PRINT_BEV(flag, what)    \
	if ( flag& what) { \
		printf("event_cb --> "#flag"\n"); \
	}

void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	PRINT_BEV(BEV_EVENT_READING, what)
	PRINT_BEV(BEV_EVENT_WRITING, what)
	PRINT_BEV(BEV_EVENT_EOF, what)
	PRINT_BEV(BEV_EVENT_ERROR, what)
	PRINT_BEV(BEV_EVENT_TIMEOUT, what)
	PRINT_BEV(BEV_EVENT_CONNECTED, what)

	if (BEV_EVENT_EOF & what) {
		bufferevent_free(bev);
		int cfd = bufferevent_getfd(bev);
		printf("close fd %d\n", cfd);	
	} else if (BEV_EVENT_TIMEOUT& what) {

	} else {
		PrintError(stderr, -1, "event_cb, someerror happen", EXIT_FAILURE);
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
	if (-1 == bufferevent_enable(bev, EV_READ)) {
		PrintError(stderr, -1, "bufferevent_enable error", EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {	
	struct event_base * base = event_base_new();

	struct event * sigEv = event_new(base, SIGINT, EV_SIGNAL | EV_PERSIST, sigint, base); 
	event_add(sigEv, NULL);

	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(8888);
	svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	struct evconnlistener * evlistener = evconnlistener_new_bind(base, connect_call_back, "new connection", LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (struct sockaddr *)&svrAddr, sizeof(svrAddr));
	if (NULL == evlistener) {
		PrintError(stderr, -1, "evconnlistener_new_bind error", EXIT_FAILURE);
	}
		
	printf("begin event loop\n");
	event_base_dispatch(base);

	event_free(sigEv);
	evconnlistener_free(evlistener);
	event_base_free(base);

	return 0;
}
