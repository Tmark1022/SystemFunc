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
	int r_fd = open(argv[1], O_RDONLY);
	if (-1 == r_fd){
		perror("open error");
		exit(1);
	}

	write(STDOUT_FILENO, "find write end", strlen("find write end"));
	
	int n;
	while(0 != (n = read(r_fd, buf, 1024))){
		if (-1 == n){
			perror("read error");
			exit(1);
		}
		
		write(STDOUT_FILENO, buf, n);
	}

	close(r_fd);

	return 0;
}
