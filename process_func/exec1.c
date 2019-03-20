#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

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
		//execl("/bin/ps", "aux", NULL);    // 命令行参数传递错误， 导致仅仅是执行了ps， 并不是生效 ps aux
		
		
		//execl("/bin/ps", "ps", "aux", NULL);     // right call
		execlp("ps", "ps", "aux", NULL);           // right call

		perror("exec error");
		exit(1);

	}

	return 0;
}
