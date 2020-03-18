// flag设置为shared， 多个进程共享实时的mmap数据, 读端

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
	
	int length = 100; 
	//char * point = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);		// shared
	char * point = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);		// private
	printf("地址 : %x", (int)point);
	close(fd);	
	int idx = 1;
	while(1) {
		if (idx == 10) {
			strcpy(point, argv[1]);
			printf("aspect reading, write it : %s\n", point);
		}
		printf("aspect reading: %s\n", point);
		sleep(2);	
		++idx;

	}
	munmap(point, length);

	return 0;
}
