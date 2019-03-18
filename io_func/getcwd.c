#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024
char buf[SIZE];

int main(int argc, char *argv[]){
	memset(buf, 0, SIZE);
	
	getcwd(buf, SIZE);

	printf("%s\n", buf);

	return 0;
}
