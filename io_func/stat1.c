#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 1024
char * buf[SIZE];

int (*func_p)(const char *, struct stat *) = lstat;

int main(int argc, char *argv[]){
	
	struct stat *data = malloc(sizeof(struct stat));
	if (NULL == data){
		printf("overflow\n");
		exit(1);
	}

	int res = func_p(argv[1], data);
	if (-1 == res){
		perror("error");
		exit(1);
	}

	printf("file %s 's size is %ld\n", argv[1], data->st_size);
	if (S_ISREG(data->st_mode))
		printf("file %s is a regular file\ni", argv[1]);
	else if(S_ISDIR(data->st_mode))
		printf("file %s is a dir file\n", argv[1]);
	else if(S_ISCHR(data->st_mode))
		printf("file %s is a character device\n", argv[1]);
	else if(S_ISBLK(data->st_mode))
		printf("file %s is a BLOCK device\n", argv[1]);
	else if((data->st_mode & S_IFMT) == S_IFLNK )
		printf("file %s is a symbol file\n", argv[1]);
	
	free(data);
	data = NULL;

	return 0;
}
