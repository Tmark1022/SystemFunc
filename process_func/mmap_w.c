#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	
	int fd = open("mmap_temp", O_RDWR | O_CREAT | O_TRUNC, 0644);
	
	mmap()



	return 0;
}
