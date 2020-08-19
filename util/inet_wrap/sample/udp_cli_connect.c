/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 19 Aug 2020 03:35:18 PM CST
 @ File Name	: sample/udp_cli_connect.c
 @ Description	: 
 ************************************************************************/
#include "../inet_wrap.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		PrintError(stderr, -1, "need more argument", EXIT_FAILURE);
	}

	int cfd = Udp_connect(argv[1], argv[2]);
	char buf[MAX_BUF_SIZE];	

	while (NULL != fgets(buf, MAX_BUF_SIZE, stdin)) {	

		int cnt = sendto(cfd, buf, strlen(buf), 0, NULL, 0);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
		}

		cnt = recvfrom(cfd, buf, sizeof(buf), 0, NULL, NULL);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call recvfrom failed", EXIT_FAILURE);		
		}

		Write(STDOUT_FILENO, buf, cnt);
	}

	return 0;
}
