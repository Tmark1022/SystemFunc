#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sys_err.h"


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
			sleep(60);
			printf("i am child %d, my pid is %d, my ppid is %d\n", i, getpid(), getppid());
			break;
		}
		else{
			// parent
			if (5 == i){
				printf("i am parent, my pid is %d, my ppid is %d\n", getpid(), getppid());
				sleep(10);
			}
		}
	}





	return 0;
}
