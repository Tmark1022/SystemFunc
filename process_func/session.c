// 进程组， 会话
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	
	if (0 == fork()) {	
		// child
		printf("ppid :%d, pid : %d, pgid : %d, sid : %d\n", getppid(), getpid(), getpgid(0), getsid(0));
		setsid();
		printf("ppid :%d, pid : %d, pgid : %d, sid : %d\n", getppid(), getpid(), getpgid(0), getsid(0));
		sleep(10);
	} else {
		printf("ppid :%d, pid : %d, pgid : %d, sid : %d\n", getppid(), getpid(), getpgid(0), getsid(0));
		sleep(10);
	}


	return 0;
}
