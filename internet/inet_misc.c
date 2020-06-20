/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sat 20 Jun 2020 02:28:04 PM CST
 @ File Name	: inet_misc.c
 @ Description	: 网络api中一些杂项
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {	

	// hostname
	char name[1024];
	if (-1 == gethostname(name, 1024)) {
		perror("gethostname failed");
		exit(EXIT_FAILURE);
	}

	printf("host name is %s\n", name);

	// unix host identifier
	long hostid = gethostid();
	printf("hostid is %ld\n", hostid);

	return 0;
}
