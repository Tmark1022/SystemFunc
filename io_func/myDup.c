#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char buff[1024];

int main(int argc, char *argv[]) {

	if (argc != 2) {
		perror("usage : command <filename>\n");
		exit(1);
	}
	
	int oldFd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);    
	if (-1 == oldFd) {
		sprintf(buff, "open file(%s) error.\n", argv[1]); 
		perror(buff);
		exit(1);
	}

	if (-1 == write(oldFd, "old fd\n", strlen("old fd\n"))) {
		perror("write error");
		exit(1);
	}
	
	int newFd = dup(oldFd); 
	if (-1 == newFd) {
		perror("dup error");
		exit(1);
	}

	if (-1 == write(newFd, "new fd\n", strlen("new fd\n"))) {
		perror("write error");
		exit(1);
	}

	close(oldFd);			
	close(newFd);			

	return 0;
}
