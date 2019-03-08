#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char *argv[]){
	DIR * dp = opendir(argv[1]);
	if (dp == NULL){
		perror("opendir error");
		exit(1);
	}

	struct dirent * p_data;
	while((p_data = readdir(dp)) != NULL){
		printf("%s\t%ld\n", p_data->d_name, p_data->d_ino);
	}
	closedir(dp);
	return 0;
}
