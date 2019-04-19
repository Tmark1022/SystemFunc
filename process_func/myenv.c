#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char ** environ;

int main(int argc, char *argv[]){
	
	char ** temp = environ;
	while(*temp != NULL){	
		printf("%s\n", *temp);
		++temp;
	}


	return 0;
}
