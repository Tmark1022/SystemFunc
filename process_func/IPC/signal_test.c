// 产生信号时， 内核中断当前执行逻辑, 进入系统空间处理未决信号
// 进入用户空间， 执行handler
// 执行完handler进入系统空间， sys_return 
// 返回用户空间， 从中断位置继续执行  

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum)
{
	printf("get signum %d\n", signum);	
	int i = 1;
	while (i <= 10) {
		printf("handler %d\n", i++);
		sleep(1);
	}
}

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);	
	signal(SIGINT, handler);	
	
	int i = 0;
	while (1) {
		printf("main %d\n", i++);
		sleep(1);
	}

	return 0;
}
