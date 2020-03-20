#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

void handler(int signum)
{
	struct itimerval aa;
	getitimer(ITIMER_REAL, &aa);
	char str[1024];
	sprintf(str, "%ld,%ld\n%ld,%ld\n", aa.it_interval.tv_sec, aa.it_interval.tv_usec, aa.it_value.tv_sec, aa.it_value.tv_usec);
	fprintf(stderr, "signum is %d, alarm ret is %s\n", signum, str);	
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
	int idx = 0;		
	struct itimerval * aa = malloc(sizeof(struct itimerval));
	struct itimerval bb;
	aa->it_value.tv_sec = 2;
	aa->it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, aa, &bb);

	signal(SIGALRM, handler);
	while(1) {	
		printf("%d\n", ++idx);
	}

	return 0;
}
