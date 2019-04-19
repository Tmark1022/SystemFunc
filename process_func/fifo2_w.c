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
	int w_fd = open(argv[1], O_WRONLY);
	if (-1 == w_fd){
		perror("open error");
		exit(1);
	}
	
	write(STDOUT_FILENO, "find read end", strlen("find read end"));

	for(int j = 0; j < 30; ++j){
		sprintf(buf, "i am tmark, this is index %d\n", j);
		int n = write(w_fd, buf, strlen(buf));
		if (-1 == n){
			perror("write error");
			exit(1);
		}
		sleep(1);
	}
	close(w_fd);

	return 0;
}
