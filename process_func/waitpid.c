#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sys_err.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	int n = 5;
	pid_t pid;

	for (int i = 1; i <= n; ++i){
		pid = fork();
		if (-1 == pid){
			sys_err("fork error");
		}

		if (0 == pid){
			// child
			printf("i am child %d, my pid is %d, my ppid is %d\n", i, getpid(), getppid());

			if(i == 1){
				// call a program
				execl("myenv", "myenv", NULL);
				sys_err("execl error");
			}
			else if (2 == i){
				// segmentation fault
				char * temp = "hahaha";
				temp[2] = 'd';
				break;
			}
			else if (3 == i){
				sleep(2);
				int fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
				dup2(fd, STDOUT_FILENO);
				execlp("ps", "ps", "aux", NULL);
				sys_err("execlp error");
			}
			else if (4 == i){

				// 用信号kill掉
				while(1)
					sleep(10);
				break;
			}
			else{
				break;
			}
		}
		else{
			// parent
			if (5 == i){
				sleep(1);
				printf("i am parent, my pid is %d, my ppid is %d\n", getpid(), getppid());
				
				// wait				
				int ret;
				int status;
				while(-1 != (ret = waitpid(-1, &status, WNOHANG))){	
					if (0 == ret){
						printf("sleep 3\n");
						sleep(3);
						continue;
					}

					printf("wait child pid = %d, ", ret);
					if (WIFEXITED(status))
						printf("exit number is %d\n", WEXITSTATUS(status));
					else if (WIFSIGNALED(status))
						printf("exit singal is %d\n", WTERMSIG(status));
					else
						printf("other exit status\n");
				}
				if (ret == -1){
					perror("wait");
				}

			}
		}
	}
	return 0;
}
