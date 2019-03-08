#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

char * buf[1024];
#define READSIZE 1024

int main(int argc, char *argv[]){
	if (argc != 3){
		printf("need two params\n");
		exit(1);
	}
	
	if(strcmp(argv[1], argv[2]) == 0){
		printf("'%s' and '%s' are the same file\n", argv[1], argv[2]);
		exit(1);
	}

	int fd1, fd2;
	fd1 = open(argv[1], O_RDONLY);
	if (fd1 == -1){
		perror("open argv[1] file failed:");
		exit(1);
	}

	fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd2 == -1){
		perror("open argv[2] file failed:");
		exit(1);
	}
	
	int n;
	while ( (n = read(fd1, buf, READSIZE)) != 0 ){
		if (n == -1){
			perror("read error");
			exit(1);
		}

		if (write(fd2, buf, n) == -1){
			perror("write error");
			exit(1);
		}
	}

	close(fd1);
	close(fd2);
		
	return 0;
}
