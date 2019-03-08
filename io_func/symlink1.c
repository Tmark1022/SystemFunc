#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	int res = symlink(argv[1], argv[2]);
	if (-1 == res){
		perror("symlink error");
		exit(1);
	}

	return 0;
}
