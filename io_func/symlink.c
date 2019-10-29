#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if (3 != argc) {		
		printf("usage: commond <oldpath> <newpath>\n");
		exit(1);
	}	
	
	if (-1 == symlink(argv[1], argv[2])) {
		printf("symlink error\n");
		exit(1);	
	}

	char buff[1024];	
	sprintf(buff, "file(%s) is xxxx\n", argv[1]);
	printf("hello world, %s", buff);

	return 0;
}
