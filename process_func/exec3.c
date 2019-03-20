#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	pid_t pid = fork();
	if(-1 == pid){
		perror("fork error");
		exit(1);
	}

	if (0 != pid){
		// parent
		sleep(1);
		printf("i am parent, my pid = %d\n", getpid());

	}
	else{
		// child
		int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if(-1 == fd){
			perror("open file error");
			exit(1);
		}
		
		// 重定向标准输出
		if (-1 == dup2(fd, STDOUT_FILENO)){
			perror("dup2 error");
			exit(1);
		}

		execlp("ps", "ps", "aux", NULL);           // right call

		perror("exec error");
		exit(1);

	}

	return 0;
}
