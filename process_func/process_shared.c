#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int a = 100;


int main(int argc, char *argv[]){
	
	static int  b = 200;
	int * p = malloc(sizeof(int));
	*p = 300;

	pid_t pid = fork();
	if(-1 == pid){
		perror("fork error");
		exit(1);
	}

	if (0 != pid){
		// parent
		sleep(1);
		printf("i am parent, my pid = %d\n", getpid());
		printf("a = %d, b = %d, pc = %ld, c = %d\n", a, b, p, *p);

	}
	else{
		// child
		printf("a = %d, b = %d, pc = %ld, c = %d\n", a, b, p, *p);
		
		// change value
		a += 1;
		b += 1;
		*p += 1;

		printf("a = %d, b = %d, pc = %ld, c = %d\n", a, b, p, *p);

	}



	return 0;
}
