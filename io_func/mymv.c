#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char *argv[]){
	
	int res1, res2;
	res1 = link(argv[1], argv[2]);
	if (-1 == res1){
		perror("link error");
		exit(1);
	}

	res2 = unlink(argv[1]);
	if (-1 == res2){
		perror("unlink error");
		exit(1);
	}

	return 0;
}
