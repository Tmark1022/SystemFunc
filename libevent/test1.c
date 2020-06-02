/*************************************************************************
  @ Author	: tmark
  @ Created Time	: Mon 01 Jun 2020 03:26:41 PM CST
  @ File Name	: test1.c
  @ Description	: 
 ************************************************************************/
#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#include <event2/event.h>

struct Node {
	struct event_base * base;
	struct event * ev;
};

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

void PrintArg(evutil_socket_t fd, short what, void * arg)
{
	printf("fd : %d\n", fd);
	if (EV_READ & what) 
		printf("EV_READ trigger\n");
	if (EV_WRITE & what) 
		printf("EV_WRITE trigger\n");
	if (EV_SIGNAL & what) 
		printf("EV_SIGNAL trigger\n");
	if (EV_TIMEOUT & what) 
		printf("EV_TIMEOUT trigger\n");
	if (EV_ET & what) 
		printf("EV_ET trigger\n");

	fflush(NULL);	
}


void readcb(evutil_socket_t fd, short what, void * arg)
{
	PrintArg(fd, what, arg);		
	
	if (EV_SIGNAL & what) {
		struct event_base * base = (struct event_base *) arg; 
		event_base_loopexit(base, NULL);
		printf("call loop exit\n");
		return ;
	}

	char buf[1024];
	int cnt = read(fd, buf, 1024);
	printf("read cnt is %d\n", cnt);
	if (-1 == cnt) {					
		perror("read failed");
		return ;
	} else if ( 0 == cnt) {
		// fifo 管道没有写端了
		struct Node * pnode = (struct Node *)arg;	
		event_del(pnode->ev);	
		printf("del read event\n");
		return ;
	}
	
	write(STDOUT_FILENO, buf, cnt);
}

void timeout(evutil_socket_t fd, short what, void * arg)
{
	printf("=====================================\n");
	struct event_base * base = (struct event_base *)arg;
	PrintArg(fd, what, arg);		

	event_base_dump_events(base, stdout);
	printf("=====================================\n");
}

int main(int argc, char *argv[]) {

	// version
	printf("libevent version : %s\n", event_get_version());

	// support method
	const char ** supportList = event_get_supported_methods();  
	int no = 1;
	while (NULL != *supportList) {
		printf("%d.%s\n", no++, *(supportList++));
	}		

	struct event_base * base = event_base_new();
	if (NULL == base) {
		PrintError(stderr, -1, "event_base_new failed", EXIT_FAILURE);
	}

	printf("Using Libevent with backend method %s.\n", event_base_get_method(base)); 

	int feature = event_base_get_features(base);
	if ((feature & EV_FEATURE_ET)) 
		printf(" Edge-triggered events are supported.\n"); 
	if ((feature & EV_FEATURE_O1)) 
		printf(" O(1) event notification is supported.\n"); 
	if ((feature & EV_FEATURE_FDS)) 
		printf(" All FD types are supported.\n");	
	
	// set nonblocking	
	int fifo_fd = open("fifo_tmp", O_RDONLY); 				
	int fl = fcntl(fifo_fd, F_GETFL); 
	fl |= O_NONBLOCK;
	fcntl(fifo_fd, F_SETFL, fl); 

	if (-1 == fifo_fd) {
		exit(1);	
	}

	struct Node node;
	struct event * even1 = event_new(base, fifo_fd, EV_READ | EV_TIMEOUT | EV_PERSIST, readcb, (void *)&node);  
	node.base = base;
	node.ev = even1;

	struct event * even2 = evsignal_new(base, SIGINT, readcb, (void *)base);  
	struct event * even3 = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, timeout, (void *)base);  

	struct timeval t1 = {5, 0};
	struct timeval t3 = {4, 0};
	event_add(even1,  &t1);
	event_add(even2,  NULL);
	event_add(even3,  &t3);	

	printf("begin dispatch\n");
	int ret = event_base_dispatch(base);
	printf("event_base_dispatch ret is %d\n", ret);
	printf("got exit %d, got break %d\n", event_base_got_exit(base), event_base_got_break(base));

	event_free(even1);
	event_free(even2);
	event_free(even3);

	event_base_free(base);

	close(fifo_fd);

	return 0;
}
