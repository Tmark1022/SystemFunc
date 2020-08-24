/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 24 Aug 2020 11:57:58 AM CST
 @ File Name	: xinetd_echo.c
 @ Description	: 运行在xinetd上的echo demo
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	char tmp[2048];
	if (argc > 1) {
		snprintf(tmp, 2048, "%s : ", argv[1]);
	}
	
	char buf[1024];
	while (1) {
		int n = read(STDIN_FILENO, buf, sizeof(buf));	
		if (-1 == n) {
			// error log
			exit(EXIT_FAILURE);	
		} else if (0 == n) {
			exit(EXIT_SUCCESS);	
		} else {

			if (argc > 1) {
				write(STDOUT_FILENO, tmp, strlen(tmp));
			}

			if (-1 == write(STDOUT_FILENO, buf, n)) {
				// error log
				exit(EXIT_FAILURE);	
			}
		}	
	}
	
	return 0;
}
