#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

char buf[100];  // bss, 初始化为0

int main(int argc, char *argv[]){
	
	int pipefd[2];
	if (pipe(pipefd) == -1){
		perror("pipe error");
		exit(1);
	}

	printf("read end = %d, write end %d\n", pipefd[0], pipefd[1]);
	
	
	// close(pipefd[1]);   // 在读之前关闭写端
	sprintf(buf, "i am %s, i am so handsome\n", "tmark");

	write(pipefd[1], buf, strlen(buf));
	close(pipefd[1]);    // 这里不关闭， 下边的read会阻塞

	while(read(pipefd[0], buf, 1) > 0){
		 write(STDOUT_FILENO, buf, 1);
	}
	

	printf ("test end\n");
	return 0;
}
