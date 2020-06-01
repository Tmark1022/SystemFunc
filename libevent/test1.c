/*************************************************************************
  @ Author	: tmark
  @ Created Time	: Mon 01 Jun 2020 03:26:41 PM CST
  @ File Name	: test1.c
  @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <event2/event.h>

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
	


	int ret = event_base_dispatch(base);
	printf("event_base_dispatch ret is %d\n", ret);
	printf("got exit %d, got break %d\n", event_base_got_exit(base), event_base_got_break(base));

	event_base_dump_events(base, stdout);

	event_base_free(base);


	return 0;
}
