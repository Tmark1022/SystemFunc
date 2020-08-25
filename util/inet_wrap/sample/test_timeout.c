/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 25 Aug 2020 03:53:34 PM CST
 @ File Name	: sample/test_timeout.c
 @ Description	: 
 ************************************************************************/
#include "../inet_wrap.h"

int main(int argc, char *argv[]) {
	int buf[MAX_BUF_SIZE];
	while (1) {
		int ret = Readable_timeo(STDIN_FILENO, 5); 
		if (ret > 0) {
			int n = Read(STDIN_FILENO, buf, MAX_BUF_SIZE);
			if (0 == n) {
				printf("eof\n");
				exit(EXIT_SUCCESS);
			} else {
				Write(STDOUT_FILENO, buf, n);
			}
		} else {
			printf("read timeout\n");
		}
	}
	return 0;
}
