#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFF_SIZE 1024
char buff[BUFF_SIZE];

void PrintNowOffset(int fd, const char *ptr) 
{
	off_t of = lseek(fd, 0, SEEK_CUR);
	if (-1 == of) {
		sprintf(buff, "lseek failed");
		perror(buff);
		exit(EXIT_FAILURE);
	}
	printf("%s ==> now offset : %ld\n", ptr, of);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		perror("usage: command <filename>\n");
		exit(1);
	}
		
	int fd = open(argv[1], O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO); 
	if (-1 == fd) {
		sprintf(buff, "open file(%s) error", argv[1]);
		perror(buff);
		exit(1);
	}	

	PrintNowOffset(fd, "1111");
	
	if (-1 == read(fd, buff, 15)) {
		sprintf(buff, "read file error");
		perror(buff);
		exit(1);
	}

	PrintNowOffset(fd, "2222");
	
	if (-1 == write(fd, "==><==", 6)) {
		perror("write error");
		exit(1);
	}

	PrintNowOffset(fd, "3333");

	close(fd);
	return 0;
}
