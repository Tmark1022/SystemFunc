//  利用信号回收子进程，不过调用handler时因为wait阻塞， 直到回收所有子进程, 不然父进程逻辑还是在阻塞中，
// 稍微优化, 使用waitpid 非阻塞， 每次回收cnt次， 让父进程不会一直阻塞

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define USE_WAITPID 0
int cnt = 15;

void PrintBitMap(const sigset_t *set)
{	
	printf ("now set, ");
	for (int i = 1; i <= 32; ++i)
	{
		if (sigismember(set, i)) {
			putchar('1');
		} else {
			putchar('0');
		}
	}
	putchar('\n');
}

void handler(int signum)
{
	static int hcnt = 0;
	sigset_t set;
	int pid;
	
#ifdef USE_WAITPID 
	int remain = cnt - hcnt; 

	for (int idx = 1; idx <= remain; ++idx) {
		pid = waitpid(-1, NULL, WNOHANG);

		if (-1 == pid) {
			break;
		} else if (0 == pid) {
			continue;
		} else {
			printf("wait pid %d, now hcnt is %d\n", pid, ++hcnt);
		}	
	}
#else
	while (-1 != (pid = wait(NULL))) {
		printf("wait pid %d, now hcnt is %d\n", pid, ++hcnt);
	}
#endif
}

int main(int argc, char *argv[]) {

	setbuf(stdout, NULL);

	// 屏蔽sigchld信号， 避免父进程信号处理函数还没有注册好就子进程就结束发送SIGCHLD
	sigset_t mySet, set;
	sigemptyset(&mySet);	
	sigaddset(&mySet, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mySet, NULL);	// set block set
	
	int pid;
	int i;
	for (i = 0; i < cnt; ++i) {
		if ((pid = fork()) == 0) {
			break;
		}
	}
	
	if (i == cnt) {
		sleep(3);
		struct sigaction new;
		new.sa_flags = 0;
		sigemptyset(&(new.sa_mask));
		new.sa_handler = handler;
		sigaction(SIGCHLD, &new, NULL);	

		// 解除信号屏蔽
		sigpending(&set);
		PrintBitMap(&set);
		sigprocmask(SIG_UNBLOCK, &mySet, NULL);	// set block set

		printf("i am parent, my pid is %d\n", getpid());

		int ha = 0;
		while (1) {
			printf("main, ha = %d\n", ha++);
			sleep(1);
		}	

	} else {
		printf("i am child, my ppid is %d, my pid is %d\n", getppid(), getpid());
		sleep(i);
	}

	return 0;
}
