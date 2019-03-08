#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

#define SIZE 100
char buf[SIZE];

int main(int argc, char *argv[]){
	
	char * res = getcwd(buf, SIZE);
	if( NULL == res){
		perror("getcwd error");
		exit(1);
	}
	else{
		printf("return value is %s, buf is %s\n", res, buf);
	}


	return 0;
}
