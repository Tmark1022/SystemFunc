#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "sys_err.h"

int main(int argc, char *argv[]){
	int n =2;
	int i;
	for (i = 0; i < n; ++i){
		pid_t pid = fork();
		if (-1 == pid){
			perror("fork error");
			exit(1);
		}

		if (0 == pid){
			// child 
			break;
		}
	}
	
	int status;
	int ret;

	if (i == 0){
		// child 1
		ret = wait(&status);
		if (-1 == ret){
			perror("wait error");
			exit(1);
		}
	}
	else if (1 == i){
		// child 2
		ret = wait(&status);
		if (-1 == ret){
			perror("wait error");
			exit(1);
		}
	}
	else{
		// parent
		while (-1 != (ret = wait(&status))){
			printf("wait child pid %d, ", ret);
			if (WIFEXITED(status))
				printf("normal end, return value is %d\n", WEXITSTATUS(status));
			else if(WIFSIGNALED(status))
				printf("singal end, signal value is %d\n", WTERMSIG(status));





		}
		
	}
	
	
		



	return 0;
}
