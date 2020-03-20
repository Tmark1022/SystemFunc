#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


void handler(int signum)
{
	alarm(1);
	static int a = 1;
	printf("hello world, %d\n", a++);
}

int main(int argc, char *argv[]) {
	int idx = 0;		
	alarm(1);
	signal(SIGALRM, handler);
	while(1) {	
	}

	return 0;
}
