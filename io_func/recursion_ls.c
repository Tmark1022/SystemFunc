#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

struct stat buf;

char ww[256];

// 递归ls子目录， 做出tree的效果
int  LsDir(const char * dir_path, unsigned level){
	
	DIR * dp = opendir(dir_path);
	if (NULL == dp){
		perror("open dir error");
		exit(1);
	}

	int file_cnt = 1;
	struct dirent * data;
	while ((data = readdir(dp)) != NULL){
		if (strcmp(data->d_name, "..") == 0 || strcmp(data->d_name, ".") == 0)
			continue;
	
			
		if (-1 == lstat(data->d_name, &buf)){
			perror("lstat error");
			exit(1);
		}
		
		for (int i = 0; i < level; ++i)
			printf("\t");
		printf("%s, size is %ld\n", data->d_name, buf.st_size);	
		
		if (S_ISDIR(buf.st_mode)){
			char temp[10] = "./";
			strcat(temp,data->d_name);
			chdir(temp);
			LsDir("./", level + 1);	
		}

	}
	closedir(dp);
	
	chdir("../");
	
	return file_cnt;

}




int main(int argc, char *argv[]){
	chdir(argv[1]);
	LsDir("./", 0);
	return 0;
}
