#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void PrintBitMap(const sigset_t *set)
{
	for (int i = 1; i <= 32; ++i)
	{
		if (sigismember(set, i)) {
			putchar('1');
		} else {
			putchar('0');
		}
	}
	putchar('\n');
}

int main(int argc, char *argv[]) {
	sigset_t mySet, pending;
	// sigemptyset(&mySet);	
	sigaddset(&mySet, SIGINT);

	sigfillset(&mySet);

	sigprocmask(SIG_BLOCK, &mySet, NULL);	// set block set
		
	while(1) {
		if (-1 == sigpending(&pending)) {
			perror("sigpending error");
			exit(EXIT_FAILURE);
		}
		PrintBitMap(&pending);
		sleep(1);
	}
	
	return 0;
}
