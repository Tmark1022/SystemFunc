#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

char buf[1024];

int main(int argc, char *argv[]){
	
	int fd[2];
	if (-1 == pipe(fd)){
		perror("pipe error");
		exit(1);
	}
	
	printf("read end = %d, write end %d\n", fd[0], fd[1]);

	pid_t cpid = fork();
	if (-1 == cpid){
		perror("fork error");
		exit(1);
	}
	
	if (0 == cpid){
		// child
		printf("i am child, my pid is %d\n", getpid());
		sprintf(buf, "hello guys, i am child");
		write(fd[1], buf, strlen(buf));
		
		sleep(1);

		//sleep(1);
		int n = read(fd[0], buf, 1024);
		printf("i am child, i get string size n = %d\n", n);
		sprintf(buf + strlen(buf) + 1, "i am child, i get (%s)\n", buf);
		write(STDOUT_FILENO, buf + strlen(buf) + 1, strlen(buf + strlen(buf) + 1));
	}
	else{
		// parent
		printf("i am parent, my pid is %d\n", getpid());
		sprintf(buf, "hello guys, i am parent");
		write(fd[1], buf, strlen(buf));

		sleep(1);
		sleep(1);
		int n = read(fd[0], buf, 1024);
		printf("i am parent, i get string size n = %d\n", n);
		sprintf(buf + strlen(buf) + 1, "i am parent, i get (%s)\n", buf);
		write(STDOUT_FILENO, buf + strlen(buf) + 1, strlen(buf + strlen(buf) + 1));

	}



	return 0;
}
