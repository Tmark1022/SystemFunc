#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
		
	int pid, kill_pid;
	int i;
	for(i = 0; i < 5; ++i) {
		pid = fork();

		if (-1 == pid) {
			perror("fork error");
			exit(EXIT_FAILURE);
		} else if (0 == pid) {
			break;
		}else {
			if (i == 3) {
				kill_pid = pid;
			}	
		}
		
	}	
	
	if (5== i) {
		sleep(10);
		printf("kill %d\n", kill_pid);
		kill(kill_pid, SIGINT);
	} else {
		sleep(60);
	}

	return 0;
}
