/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 10 Dec 2021 12:03:56 PM CST
 @ File Name	: a.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	gid_t rgid = getgid();
	gid_t egid = getegid();

	gid_t list[64];
	int cnt = getgroups(64, list);
	char buf[1024] = "";
	for(int i = 0; i < cnt; ++i) {
		sprintf(buf, "%s%d,", buf, list[i]);
	}
	printf("rgid = %d, egid = %d, supplementary groups %s\n", rgid, egid, buf);
	return 0;
}
