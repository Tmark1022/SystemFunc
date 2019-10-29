#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if (3 != argc) {		
		printf("usage: commond <oldpath> <newpath>\n");
		exit(1);
	}	

	int res = link(argv[1], argv[2]);
	if (-1 == res) {
		printf("link error");
		exit(1);
	}
	
	if (-1 == unlink(argv[1])) {
		printf("unlink error");
		exit(1);
	}
	
		



	return 0;
}
