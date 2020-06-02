/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 02 Jun 2020 11:23:23 AM CST
 @ File Name	: mmap_4.c
 @ Description	: fork 共享问题
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {

	int fd = open("./testmmp.txt", O_RDWR | O_CREAT, 0644);
	if (-1 == fd) {
		perror("open failed");
		exit(1);
	}

	struct stat buf;
	if (-1 == fstat(fd, &buf)) {
		perror("fstat failed");
		exit(1);
	}

	int len = buf.st_size;
	if ( len <= 0) {
		ftruncate(fd, 100);
		printf("file is empty, ftruncate to 100 byte");
		len = 100;
	}

//	char * ptr = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//	char * ptr = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);
	char * ptr = mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (MAP_FAILED == ptr) {
		perror("mmap error");
		exit(1);
	}
	
	int pid = fork();		
	if (-1 == pid) {
		perror("fork error");
		exit(1);
	} else if ( 0 == pid) {
		// child	
		while (1) {
			write(STDOUT_FILENO, ptr, len); 
			sleep(2);
		}	
	} else {
		// parent
		while (1) {
			read(STDIN_FILENO, ptr, len);
		}	
	}

	munmap(ptr, len);
	return 0;
}
