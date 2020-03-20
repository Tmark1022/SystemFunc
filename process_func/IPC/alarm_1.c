#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


void handler(int signum)
{
	fprintf(stderr, "signum is %d, alarm ret is %d\n", signum, alarm(0));	
	raise(SIGSEGV);
}


int main(int argc, char *argv[]) {
	int idx = 0;		
	alarm(10);
	signal(SIGQUIT, handler);
	while(1) {	
		printf("%d\n", ++idx);
	}

	return 0;
}
