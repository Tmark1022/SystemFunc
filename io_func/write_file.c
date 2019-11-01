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
	if (argc != 2) {
		perror("usage: command <filename>\n");
		exit(1);
	}
		
	int fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO); 
	if (-1 == fd) {
		sprintf(buff, "open file(%s) error", argv[1]);
		perror(buff);
		exit(1);
	}	
	
	while ((NULL != gets(buff))) {
		if (-1 == write(fd, buff, strlen(buff))) {
			perror("write error");
			exit(1);
		}
		// 插入\n
		if (-1 == write(fd, "\n", strlen("\n"))) {
			perror("write error");
			exit(1);
		}
	}		
	close(fd);
	return 0;
}
