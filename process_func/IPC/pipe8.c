#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 兄弟进程利用管道实现 ls | wc -l, 父进程waitpid进行回收, 并打印回收状态
int main(int argc, char *argv[]) {
	// create pipe
	int pipeFd[2];
	if  (-1 == pipe(pipeFd)) {
		perror("fork error");
		exit(EXIT_FAILURE);	
	}

	// fork child process
	int pid;
	int idx;
	for(idx = 0; idx < 2; ++idx) {
		pid = fork();
		if (-1 == pid) {
			// errror
			perror("fork error");
			exit(EXIT_FAILURE);
		}
		if (0 == pid) {
			// child
			printf("create new process, pid=%d, ppid = %d\n", getpid(), getppid());
			break;
		}
	}

	//  分进程处理
	if (0 == idx) {
		// child 1, do command "ls"
		dup2(pipeFd[1], STDOUT_FILENO);
		close(pipeFd[0]);
		close(pipeFd[1]);
		sleep(2);
		execlp("ls", "ls", "-alF", "/home/tmark", NULL);
	} else if (1 == idx) {
		// child 2, do command "wc -l"
		dup2(pipeFd[0], STDIN_FILENO);
		close(pipeFd[0]);
		close(pipeFd[1]);
		sleep(1);
		execlp("wc", "wc", "-l", NULL);
	} else if (2 == idx) {
		// parent
		close(pipeFd[0]);
		close(pipeFd[1]);
		
		int status, res;
		while (1) {
			res = waitpid(-1, &status, WNOHANG);
			if (-1 == res) {
				perror("waitpid error");
				exit(EXIT_FAILURE);
			} else if (res > 0) {
				if ( WIFEXITED(status)) {	
					printf("wait pid %d, exit code %d\n", res, WEXITSTATUS(status));
				}
			} else if (0 == res) { 	
				sleep(1);
			}
		}
	}




	return 0;
}
