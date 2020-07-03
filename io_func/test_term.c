/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 03 Jul 2020 09:31:43 PM CST
 @ File Name	: test_term.c
 @ Description	: 终端IO一些函数使用
 ************************************************************************/
#include <asm-generic/ioctls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>

#include <sys/ioctl.h>

void sig_win_ch(int signo)
{
	printf("change the size of window\n");
	
	struct winsize aa;	
	if (-1 == ioctl(STDIN_FILENO, TIOCGWINSZ, &aa)) {
		perror("ioctl error");
		exit(EXIT_FAILURE);
	}

	printf("new size si row %d, col %d\n", aa.ws_row, aa.ws_col);
}

int main(int argc, char *argv[]) {

	char tty_path[L_ctermid];
	ctermid(tty_path);
	fprintf(stderr, "tty name is %s\n", tty_path);

	int fd = STDIN_FILENO;	
	if (!isatty(fd)) {
		fprintf(stderr, "fd is not a tty\n");
		exit(EXIT_FAILURE);	
	}

	char * res = ttyname(fd);
	if (res) {
		fprintf(stderr, "fd 's tty name is %s\n", res);
	}

	char *passwd = getpass("input passwd : ");
	if (NULL == passwd) {
		perror("getpass error");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "the passwd is  %s\n", passwd);
	
	signal(SIGWINCH, sig_win_ch);

	while(1) {
		pause();
	}

	return 0;
}
