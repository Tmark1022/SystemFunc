#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

char str[1024];
int main(int argc, char *argv[]) {
	int fd = open("testmmp.txt", O_RDWR|O_CREAT, 0664);
	
	// resize 
	/*
	lseek(fd, 9, SEEK_END);
	write(fd, "\0", 1);
	int length = lseek(fd, 0, SEEK_END);
	*/
	int length = 100; 
	ftruncate(fd, length);

	sprintf(str, "%d\n", length);
	write(STDOUT_FILENO, str, strlen(str));

	//char * point = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);		// 只读，下面有读写行为， 运行会出segmentation fault
	//char * point = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);	// private
	char * point = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);		// shared
	close(fd);	
	
	// read
	printf("1...%s\n", point);
	
	// write
	strcpy(point, "haha, i am tmark.");	
	printf("2...%s\n", point);



	return 0;
}
