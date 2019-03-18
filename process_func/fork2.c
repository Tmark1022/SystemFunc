#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[]){
	
	int n;
	scanf("%d", &n);
	if (n < 0){
		printf("n must be bigger than 0\n");
		return 0;
	}

	for (int i = 0; i < n; ++i)
	{
		pid_t pid = fork();
		if (pid == -1){
			perror("fork error");
			exit(1);
		}

		if (0 == pid){
			// 子进程
			printf("i am child %d, my pid = %d, my parent pid = %d\n", i + 1, getpid(), getppid());
			break;
		}
		else{
			if (i == n - 1){
				sleep(1);
				printf("i am parent, my pid = %d\n", getpid());
			}
		}
	}

	return 0;
}
