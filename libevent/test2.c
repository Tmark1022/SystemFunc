/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 03 Jun 2020 02:50:59 PM CST
 @ File Name	: test2.c
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

void timeout1(evutil_socket_t fd, short what, void * arg)
{
	char * aa = (char *) arg;
	printf("i am t1, fd %d, what %x, arg : %s\n", fd, what, aa);
}

void timeout2(evutil_socket_t fd, short what, void * arg)
{
	char * aa = (char *) arg;
	printf("i am t2, fd %d, what %x, arg : %s\n", fd, what, aa);
}

void sig_int(evutil_socket_t fd, short what, void * arg)
{
	static int cnt = 0;
	struct event * event = (struct event *)arg;
	struct timeval t1;
	memset(&t1, 0, sizeof(t1));
	int ret = event_pending(event, EV_TIMEOUT | EV_READ|EV_WRITE | EV_SIGNAL, &t1);
	printf("ret %d, sec %ld\n", ret, t1.tv_sec);		

	if (ret) {
		// 加入了, del ta
		event_del(event);
		printf("event del successfully\n");
	} else {
		// 没有加入
		struct event_base *base_out;
		evutil_socket_t fd_out;
		short events_out;	
		event_get_assignment(event, &base_out, &fd_out, &events_out, NULL, NULL);		

		printf("event_get_assignment, fd_out %d, what %d\n", fd_out, events_out);

		if (cnt % 2) {
			event_assign(event, base_out, fd_out - 1, events_out & ~EV_READ, timeout1, "i am timeout 1");
		} else {
			event_assign(event, base_out, fd_out - 1, events_out | EV_READ, timeout2, "i am timeout 2");
		}

		printf("event assign successfully\n");	
		struct timeval t2 = {2, 0};
		event_add(event, &t2);
		printf("event add successfully\n");

		++cnt;
	}

}

int main(int argc, char *argv[]) {

	struct event_base * base = event_base_new(); 
	if (NULL == base) {
		PrintError(stderr, -1, "event_base_new error", EXIT_FAILURE);				
	}
	
	struct event *event = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, timeout1, "i am timeout 1");  	
	struct event * evsig = evsignal_new(base, SIGINT, sig_int, event); 
	
	struct timeval t1 = {2,0};
	event_add(event, &t1);
	evsignal_add(evsig, NULL);

	int ret = event_base_dispatch(base);
	printf("event_base_dispatch ret is %d\n", ret);

	event_base_free(base);
	event_free(event);
	event_free(evsig);

	return 0;
}
