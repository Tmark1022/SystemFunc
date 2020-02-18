#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	int pid = fork();
	if (-1 == pid) {
		perror("fork error");
	} else if (0 == pid) {
		// child
		sleep(100);
		return 10;	
	} else {
		// parent
		int wpid, status;	
		wpid = wait(&status);
		if (-1 == wpid) {
			perror("wait error");
		}
		if (WIFEXITED(status)) {
			printf("wpid %d, return code is %d\n", wpid, WEXITSTATUS(status));	
		}
		if (WIFSIGNALED(status)) {
			printf("wpid %d, sig is %d\n", wpid, WTERMSIG(status));	
		}
	}

	return 0;
}
