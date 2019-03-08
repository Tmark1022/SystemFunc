#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>


char * buf[1024];

int main(int argc, char *argv[]){
	
	int fd = open(argv[1], O_RDWR | O_CREAT, 0664);
	if(-1 == fd){
		perror("open error");
		exit(1);
	}

	lseek(fd, 5, SEEK_SET);
	
	write(fd, "i am tmark\n", strlen("i am tmark\n"));

	lseek(fd, 0, SEEK_SET);
	int n;
	while((n = read(fd, buf, 1)) != 0){
		if (-1 == n){
			perror("read error");
			exit(1);
		}
		write(STDOUT_FILENO, buf, n);
	}
	return 0;
}
