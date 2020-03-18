// flag设置为shared， 多个进程共享实时的mmap数据, 写端

// shared 返回的地址不一样， 怎么做到写端的内容一修改， 马上同步到读端呢（无须重新调用mmap分配地址）

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
	char * point = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);		// shared
	//char * point = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);		// private
	printf("地址 : %x", (int)point);
	close(fd);	
	char input[1024];
	while(EOF != scanf("%s", input)) {
		// write
		strcpy(point, input);	
		printf("aspect writing: %s\n", point);
	}

	munmap(point, length);

	return 0;
}
