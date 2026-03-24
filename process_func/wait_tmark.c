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
		//sleep(100);
		while(1);
		printf("------ lalala\n");
		return 10;	
	} else {
		// parent
		int wpid, status;	
		printf("child pid is %d\n", pid);
		while(1) {
			// wpid = wait(&status);
			wpid = waitpid(-1, &status, WUNTRACED|WCONTINUED);
			if (-1 == wpid) {
				perror("wait error");
			}
			if (WIFEXITED(status)) {
				printf("wpid %d, return code is %d\n", wpid, WEXITSTATUS(status));	
				break;
			}
			if (WIFSIGNALED(status)) {
				printf("wpid %d, sig is %d\n", wpid, WTERMSIG(status));	
				break;
			}
			if (WIFSTOPPED(status)) {
				printf("wpid %d, stop sig is %d\n", wpid, WSTOPSIG(status));	
			}
			if (WIFCONTINUED(status)) {
				printf("wpid %d, continue", wpid);	
			}
		}					
			
	}

	return 0;
}
