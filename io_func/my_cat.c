#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFF_SIZE 1024
char buff[BUFF_SIZE];

int main(int argc, char *argv[]) {	
	int fdIn = STDIN_FILENO;
	int fdOut = STDOUT_FILENO;
	if (1 == argc) {

	} else if (2 == argc) {
		fdIn = open(argv[1], O_RDONLY); 
		if (-1 == fdIn) {
			sprintf(buff, "open file(%s) error", argv[1]);
			perror(buff);
			exit(1);
		}	
	} else {
		perror("usage: command <filename>\n");
		exit(1);
	}
	
	ssize_t size; 	
	while (0 != (size = read(fdIn, buff, BUFF_SIZE))) {
		if (-1 == write(fdOut, buff, strlen(buff))) {
			perror("write error");
			exit(1);
		}
	}		
	close(fdIn);
	close(fdOut);
	
	


	return 0;
}
