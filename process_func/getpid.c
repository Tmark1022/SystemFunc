#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("start fork\n");
	
	pid_t pid = fork();
	if (-1 == pid){
		perror("fork error");
		exit(1);
	}
	
	if (pid == 0){
		// 子进程
		printf("i am child, the return pid = %d, my pid = %d, my parent pid = %d\n", pid, getpid(), getppid());
	}
	else{
		// 父进程
		printf("i am parent, the return pid = %d, my pid = %d, my parent pid = %d\n", pid, getpid(), getppid());
		sleep(1);  // 为了打印美观，控制父进程被子进程迟结束（父进程一结束shell提示就出来了， fuck）
	}

	printf("fork end \n");

	return 0;
}
