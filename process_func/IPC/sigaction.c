#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void PrintBitMap(const sigset_t *set)
{
	printf ("now set, ");
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

void handler(int signum)
{
	sigset_t set;

	int i = 0;
	while (i <= 5) {
		if (-1 == sigpending(&set)) {
			perror("sigpending error");
			exit(EXIT_FAILURE);
		}
		PrintBitMap(&set);
		printf("handler %d\n", i++);
		sleep(1);
	}


}



int main(int argc, char *argv[]) {
	struct sigaction new;
	new.sa_flags = 0;
	sigemptyset(&(new.sa_mask));
	new.sa_handler = handler;
	sigaction(SIGINT, &new, NULL);	

	int i = 0;
	while (1) {
		printf("main %d\n", i++);
		sleep(1);
	}

	return 0;
}
