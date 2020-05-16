/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sat 16 May 2020 10:56:48 PM CST
 @ File Name	: signal_interrupt_sleep.c
 @ Description	: sleep函数调用阻塞时会被信号中断激活（中断慢速系统调用?）, 因为sleep的实现有使用pause？？
		sleep()  causes the calling thread to sleep either until the number of real-time seconds specified in seconds have elapsed 
			or until a signal arrives which is not ignored.
		使用三个信号来进行测试, 其中SIGCHLD的默认处理动作是ignore

 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

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
	
	alarm(10);

	int ret = sleep(15);

	printf("sleep 's ret --> %d\n", ret);

	return 0;
}
