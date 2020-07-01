/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 01 Jul 2020 04:39:52 PM CST
 @ File Name	: libevent_stream_client.c
 @ Description	: 
 ************************************************************************/
#include <asm-generic/errno-base.h>
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

#include <pthread.h>

/***************************************************
* global variable, marco 
***************************************************/
int port = 8888;
char * ip_addr = "0.0.0.0";

int bind_port = 0;
char * bind_ip_addr = NULL;

int reuse_addr = 0;
int do_linger = 0;

struct bufferevent * gbev = NULL;

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
	while ((opt = getopt(argc, argv, "h:p:s:b:ALv")) != -1) {
		switch (opt) {
			case 'h':
				ip_addr = optarg; 
				break;
			case 'p':
				port = atoi(optarg);
				break;	
			case 's':
				bind_ip_addr = optarg; 
				break;
			case 'b':
				bind_port = atoi(optarg);
				break;	
			case 'A':
				reuse_addr = 1;
				break;
			case 'L':
				do_linger = 1;
				break;
			case 'v':
               		    fprintf(stdout, "Usage: %s [-h ip][-p port][-s src_addr][-b src_port][-A][-L]\n", argv[0]);
               		    exit(EXIT_SUCCESS);
               		default: 
               		    fprintf(stderr, "Usage: %s [-h ip][-p port][-s src_addr][-b src_port][-A][-L]\n", argv[0]);
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
	struct evbuffer *readbuf;
	readbuf = evbuffer_new();
	if (NULL == readbuf) {
		PrintError(stderr, -1, "read_cb, evbuffer_new error", EXIT_FAILURE);
	}
	if (-1 == bufferevent_read_buffer(bev, readbuf)) {
		PrintError(stderr, -1, "read_cb, bufferevent_read_buffer error", EXIT_FAILURE);
	}

	size_t len = evbuffer_get_length(readbuf);
	if (len > 0) {
		char * tmp = malloc(len + 1); 
		if (NULL == tmp) {
			PrintError(stderr, 0, "read_cb, malloc error", EXIT_FAILURE);
		}
		int cnt = evbuffer_remove(readbuf, tmp, len);
		if (cnt != len) {
			PrintError(stderr, -1, "read_cb, evbuffer_remove error", EXIT_FAILURE);
		}
		tmp[cnt] = '\0';

		printf("get data : %s\n", tmp);
	}
}

void write_cb(struct bufferevent *bev, void *ctx)
{
	int cfd = bufferevent_getfd(bev);
	printf("fd %d write data successfully\n", cfd);
}

void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	if (BEV_EVENT_EOF & what) {
		struct event_base * base = bufferevent_get_base(bev);
		int cfd = bufferevent_getfd(bev);
		printf("close fd %d\n", cfd);	
		event_base_loopexit(base, NULL);

	} else if (BEV_EVENT_TIMEOUT& what) {

	} else {
		printf("error no is %d, error string is %s\n", errno, strerror(errno));
		PrintError(stderr, 0, "event_cb, some errors happen", EXIT_FAILURE);
	}
}

struct sockaddr_in  ConstructSockaddr(const char * addr, int p)
{
	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(p);
	struct in_addr tmpAddr; 
	int ret  = inet_pton(AF_INET, addr, &tmpAddr);
	if (-1 == ret) {
		PrintError(stderr, 0, "inet_pton error", EXIT_FAILURE);				
	} else if (0 == ret) {
		PrintError(stderr, 0, "inet_pton error, invalid ip", EXIT_FAILURE);				
	}	
	svrAddr.sin_addr.s_addr = tmpAddr.s_addr;

	return svrAddr;
}

int  DoConnect()
{
	// 构造服务器的地址struct
	struct sockaddr_in svrAddr = ConstructSockaddr(ip_addr, port);

	// 客户端socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		PrintError(stderr, 0, "socket failed", EXIT_FAILURE);		
	}
		
	if (reuse_addr) {
		// 端口复用
		int opt = 1;
		if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
		}
	}
	
	if (do_linger) {	
		struct linger sl;
		sl.l_onoff = 1;		/* non-zero value enables linger option in kernel */
		sl.l_linger = 0;	/* timeout interval in seconds */
		setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));
	}

	if (bind_ip_addr != NULL && port != 0) {
		struct sockaddr_in bindAddr = ConstructSockaddr(bind_ip_addr, bind_port);
		bind(sockfd, (struct sockaddr *)&bindAddr, sizeof(struct sockaddr_in));
	}	

	connect(sockfd, (const struct sockaddr *)&svrAddr, sizeof(struct sockaddr_in));

	return sockfd;
}

void * do_read_stdin(void * arg)
{

	struct bufferevent * bev = (struct bufferevent *)arg;

	char buf[1024];
	while(1) {
		if (NULL == fgets(buf, 1024, stdin)) {
			break;
		}
		
		bufferevent_write(bev, buf, strlen(buf));
	}
	
	printf("stop write\n");
	return NULL;
}

int main(int argc, char *argv[]) {		
	HandleOpt(argc, argv);

	struct event_base * base = event_base_new();
	struct event * sigEv = event_new(base, SIGINT, EV_SIGNAL | EV_PERSIST, sigint, base); 
	event_add(sigEv, NULL);

	int sockfd = DoConnect();

	gbev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == gbev) {
		PrintError(stderr, -1, "bufferevent_socket_new error", EXIT_FAILURE);
	}
	bufferevent_setcb(gbev, read_cb, write_cb, event_cb, NULL);
	// TODO, 默认就开启write， 不要在这里指定write， 居然会在连接建立后马上调用write_cb
	if (-1 == bufferevent_enable(gbev, EV_READ)) {
		PrintError(stderr, -1, "bufferevent_enable error", EXIT_FAILURE);
	}

	// 新的线程来进行输入
	pthread_t tid;
	pthread_create(&tid, NULL, do_read_stdin, (void *)gbev);
	pthread_detach(tid);

	printf("begin event loop\n");
	event_base_dispatch(base);

	event_free(sigEv);
	bufferevent_free(gbev);
	event_base_free(base);

	return 0;
}
