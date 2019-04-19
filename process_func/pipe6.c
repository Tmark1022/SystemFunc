#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){
	// create pipe
	int fd[2];
	if (-1 == pipe(fd)){
		perror("pipe error");
		exit(1);
	}

	// fork
	int i;
	for(i = 0; i < 2; ++i){
		pid_t cpid = fork();
		if (-1 == cpid){
			perror("fork error");
			exit(1);
		}
		
		if (0 == cpid)
			break;
	}
	
	if (0 == i){
		// child 1
		printf("my pid is %d, my ppid is %d\n", getpid(), getppid());
		
		close(fd[0]); // close read end
		
		if (-1 == dup2(fd[1], STDOUT_FILENO)){  // redirect stdout
			perror("dup2 error");
			exit(1);
		}

		// execute ls programe
		execlp("ps", "ps", "ajx", NULL);
		perror("execlp error");
		exit(1);
	}
	else if (1 == i){
		// child 2
		printf("my pid is %d, my ppid is %d\n", getpid(), getppid());
		
		close(fd[1]); // close write end
		
		if (-1 == dup2(fd[0], STDIN_FILENO)){  // redirect stdin
			perror("dup2 error");
			exit(1);
		}

		// execute wc -l  programe
		execlp("grep", "grep","pipe6", NULL);
		perror("execlp error");
		exit(1);
	}
	else{
		// parent
		printf("my pid is %d, my ppid is %d\n", getpid(), getppid());

		close(fd[0]); // close read end
		close(fd[1]); // close write end
		

		// wait for child
		sleep(10);
		int ret;
		while(-1 != (ret = wait(NULL))){
			printf("wait for child pid %d\n", ret);
		}
	}

	return 0;
}
