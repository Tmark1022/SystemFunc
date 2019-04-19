#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc, char *argv[]){
	// create pipe
	int fd[2];
	if (-1 == pipe(fd)){
		perror("pipe error");
		exit(1);
	}

	// fork
	pid_t cpid = fork();
	if (-1 == cpid){
		perror("fork error");
		exit(1);
	}
	if (0 == cpid){
		// child
		sleep(1);
		close(fd[1]); // close write end
		
		if (-1 == dup2(fd[0], STDIN_FILENO)){  // redirect stdin
			perror("dup2 error");
			exit(1);
		}

		// execute wc -l  programe
		execlp("wc", "wc","-l", NULL);
		perror("execlp error");
		exit(1);
	}
	else{
		// parent
		close(fd[0]); // close read end
		
		if (-1 == dup2(fd[1], STDOUT_FILENO)){  // redirect stdout
			perror("dup2 error");
			exit(1);
		}

		// execute ls programe
		execlp("ls", "ls", NULL);
		perror("execlp error");
		exit(1);
	}

	return 0;
}
