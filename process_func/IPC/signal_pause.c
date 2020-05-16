/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sat 16 May 2020 11:14:52 PM CST
 @ File Name	: signal_pause.c
 @ Description	: 测试下pause函数
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void sig_int(int signo)
{
	printf("call sig_int\n");
}

void sig_alarm(int signo)
{
	printf("call sig_alarm\n");
}

int main(int argc, char *argv[]) {
	signal(SIGINT, sig_int);
	signal(SIGALRM, sig_alarm);

	printf("begin, my pid is %d\n", getpid());
	
	alarm(15);

	int ret = pause();
	printf("pause ret is %d\n, strerr is %s\n", ret, strerror(errno));
	return 0;
}
