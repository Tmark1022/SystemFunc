#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <error.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	int pid;
	for (int i = 0; i < 3; ++i) {
		pid = fork();
		if (-1 == pid) {
			// error
			perror("fork error");
			exit(EXIT_FAILURE);
		} else if (0 == pid) {
			// child
			if (i == 0) { 		
				int fd = open("haha1", O_CREAT | O_WRONLY | O_APPEND, 0664);
				if (-1 == fd) {
					perror("open error");
					exit(EXIT_FAILURE);
				}	
				dup2(fd, STDOUT_FILENO);
				close(fd);
				if (-1 == execlp("ps", "ps", NULL)) {
					perror("execlp error");	
					exit(EXIT_FAILURE);
				}
			} else if (i == 1) {
				int fd = open("haha2", O_CREAT | O_WRONLY | O_APPEND, 0664);
				if (-1 == fd) {
					perror("open error");
					exit(EXIT_FAILURE);
				}	
				dup2(fd, STDOUT_FILENO);
				close(fd);
				if (-1 == execlp("yes", "yes", NULL)) {
					perror("execlp error");	
					exit(EXIT_FAILURE);
				}
			} else {
				int fd = open("haha3", O_CREAT | O_WRONLY | O_APPEND, 0664);
				if (-1 == fd) {
					perror("open error");
					exit(EXIT_FAILURE);
				}	
				dup2(fd, STDOUT_FILENO);
				close(fd);
				if (-1 == execlp("./tmp", "./tmp", NULL)) {
					perror("execlp error");	
					exit(EXIT_FAILURE);
				}


			}
		}
	}

	// parent
	int status, res;
	while (1) {
		res = waitpid(-1, &status, WNOHANG);
		if (-1 == res) {
			perror("waitpid -1");
			break;
		} else if (0 == res) {	
			printf("sleep 2 secs\n");
			sleep(2);
		} else {
			if (WIFEXITED(status)) {
				printf("wait child(%d), exit code is %d\n", res, WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				printf("wait child(%d), signal is %d\n", res, WTERMSIG(status));
			} else {
				printf("wait child(%d)\n", res);
			}
		}

	}

	return 0;
}
