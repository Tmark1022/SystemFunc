#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

char buf[1024];

int main(int argc, char *argv[]){
	// open file
	int w_fd = open(argv[1], O_WRONLY | O_NONBLOCK);
	if (-1 == w_fd){
		perror("open error");
		exit(1);
	}
	sprintf(buf, "w_fd is %d\n", w_fd);	
	write(STDOUT_FILENO, buf, strlen(buf));
	
	int r_fd = open(argv[1], O_RDONLY);
	if (-1 == r_fd){
		perror("open error");
		exit(1);
	}
	sprintf(buf, "r_fd is %d\n", r_fd);	
	write(STDOUT_FILENO, buf, strlen(buf));

	// fork
	int i;
	for (i = 0; i < 2; ++i){
		pid_t pid = fork();
		if (-1 == pid){
			perror("fork error");
			exit(1);
		}

		if (0 == pid)
			break;

	}

	
	if (0 == i){
		// child 1
		printf("i am child 1, my pid = %d, my ppid = %d\n", getpid(), getppid());
		close(r_fd);
		for(int j = 0; j < 30; ++j){
			sprintf(buf, "i am tmark, this is index %d\n", j);
			write(w_fd, buf, strlen(buf));
			sleep(1);
		}
		close(w_fd);
	}
	else if (1 == i){
		// child 2
		printf("i am child 2, my pid = %d, my ppid = %d\n", getpid(), getppid());
		close(w_fd);
		int n;
		while(0 != (n = read(r_fd, buf, 1024))){
			if(-1 == n){
				perror("read error");
				exit(1);
			}
			write(STDOUT_FILENO, buf, n);
		}
		close(r_fd);
	}
	else{
		// parent
		printf("i am parent, my pid = %d, my ppid = %d\n", getpid(), getppid());
		close(r_fd);
		close(w_fd);
		
		int ret;
		while (-1 != (ret = wait(NULL))){
			printf("get child process, child pid is %d\n", ret);
		}
	}

	return 0;
}
