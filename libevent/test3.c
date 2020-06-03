/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 03 Jun 2020 04:00:18 PM CST
 @ File Name	: test3.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <event2/event.h>
#include <signal.h>

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


void active_cb(evutil_socket_t fd, short what, void * arg)
{
	char * aa = (char *) arg;
	printf("i am active_cb, fd %d, what %x, arg : %s\n", fd, what, aa);
}

void timeout1(evutil_socket_t fd, short what, void * arg)
{
	struct event  * aa = (struct event *) arg;
	printf("i am t1, fd %d, what %d\n", fd, what);	

	event_active(aa, EV_READ | EV_WRITE | EV_TIMEOUT, 0);
}

void timeout2(evutil_socket_t fd, short what, void * arg)
{
	struct event * event = (struct event *)arg; 
	event_del(event);
	printf("i am t2, fd %d, what %d, try to exit\n", fd, what);
}

int main(int argc, char *argv[]) {

	struct event_base * base = event_base_new(); 
	if (NULL == base) {
		PrintError(stderr, -1, "event_base_new error", EXIT_FAILURE);				
	}
	


	struct event *aa = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, active_cb, "hahaha");  		
	struct event *event = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, timeout1, aa);  		
	struct timeval t1 = {1,0};
	event_add(event, &t1);
	
	struct timeval t2 = {10,0};
	event_base_once(base, -1, EV_TIMEOUT, timeout2, event, &t2);

	int ret = event_base_dispatch(base);
	printf("event_base_dispatch ret is %d\n", ret);

	event_base_free(base);
	event_free(event);

	return 0;
}
