#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	
	int res = rename(argv[1], argv[2]);
	if (-1 == res){
		perror("rename error");
		exit(1);
	}

	return 0;
}
