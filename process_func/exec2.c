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
		execl("./print_argv", "./print_argv", "param1", "param2", NULL);     // right call

		perror("exec error");
		exit(1);

	}

	return 0;
}
