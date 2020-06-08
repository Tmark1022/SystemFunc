/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 08 Jun 2020 05:16:37 PM CST
 @ File Name	: test6.c
 @ Description	: debug_mode 测试
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <signal.h>

#define DEBUG_MODE 

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


void timeout1(evutil_socket_t fd, short what, void * arg)
{
	printf("i am t1, fd %d, what %d\n", fd, what);	

}

void timeout2(evutil_socket_t fd, short what, void * arg)
{
	printf("i am t2, fd %d, what %d, ================\n", fd, what);

	struct event * ev = (struct event *)arg;
	struct event_base * base  = event_get_base(ev);

	// debug mode 检测是否有
	// An event is re-assigned while it is added
	event_assign(ev, base, -3, EV_TIMEOUT | EV_PERSIST, timeout2, event_self_cbarg());

}

int main(int argc, char *argv[]) {

#ifdef DEBUG_MODE 
	event_enable_debug_mode();
#endif

	struct event_base * base = event_base_new(); 
	if (NULL == base) {
		PrintError(stderr, -1, "event_base_new error", EXIT_FAILURE);				
	}

	struct event *aa = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, timeout1, NULL);  		
	struct event bb;
	if (-1 == event_assign(&bb, base, -2, EV_TIMEOUT | EV_PERSIST, timeout2, event_self_cbarg())) {
		PrintError(stderr, -1, "event_assign error", EXIT_FAILURE);			
	}
	
	struct timeval t1 = {3, 0};
	event_add(aa, &t1);
	event_add(&bb, &t1);
	
	int ret = event_base_dispatch(base);
	printf("event_base_dispatch ret is %d\n", ret);

	event_free(aa);
	event_base_free(base);

	return 0;
}
