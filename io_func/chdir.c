#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


char * filepath = "./aaa/hello.txt";
char buf[1024];


int main(int argc, char *argv[]){
	
	printf("now world path is %s\n", getcwd(buf, 1024));
	int fd = open(filepath, O_RDONLY);
	if (-1 == fd){
		perror("open file error");
		exit(1);
	}
	
	int n = read(fd, buf, 1024);
	write(STDOUT_FILENO, buf, n);
	
		
	close(fd);
	

	int res = chdir("../");
	if (-1 == res){
		perror("chdir error");
		exit(1);
	}

	printf("now world path is %s\n", getcwd(buf, 1024));
	getchar();
	fd = open(filepath, O_RDONLY);        // error, not such file 
	if (-1 == fd){
		perror("open file error");
		exit(1);
	}

	close(fd);
	
	return 0;
}
