#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// 重定向stdout

int main(int argc, char *argv[]){
	
	int fd = open(argv[1], O_RDWR  | O_CREAT, 0664);
	if(-1 == fd){
		perror("open error");
		exit(1);
	}
	
	int stdout_cache_fileno = dup(STDOUT_FILENO);
	if (stdout_cache_fileno == -1){
		perror("dup error");
		exit(1);
	}
	
	// 重定向标准输出
	if (-1 == dup2(fd, STDOUT_FILENO)){
		perror("dup2 error");
		exit(1);
	}
	printf("hello world, i am tmark\n");
	fflush(stdout); // 刷新缓冲区，下面有close，使得在程序结束前就关闭了文件描述
					// 使得下面的printf不能写进文件 
	// 打印标准输出
	write(stdout_cache_fileno, "123456789\n", 10);
	printf("test close\n");
	
	close(fd);
	close(STDOUT_FILENO);
	return 0;
}
