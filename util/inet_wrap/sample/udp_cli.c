/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 19 Aug 2020 03:18:44 PM CST
 @ File Name	: sample/udp_cli.c
 @ Description	: 
 ************************************************************************/
#include "../inet_wrap.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		char tmp[1024];
		snprintf(tmp, 1024, "usage : %s host service", argv[0]);
		PrintError(stderr, -1, tmp, EXIT_FAILURE);
	}

	SA * saptr;
	socklen_t len;
	int cfd = Udp_client(argv[1], argv[2], &saptr, &len);
	char buf[MAX_BUF_SIZE];	

	while (NULL != fgets(buf, MAX_BUF_SIZE, stdin)) {	

		int cnt = sendto(cfd, buf, strlen(buf), 0, saptr, len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
		}

		cnt = recvfrom(cfd, buf, sizeof(buf), 0, NULL, NULL);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call recvfrom failed", EXIT_FAILURE);		
		}

		Write(STDOUT_FILENO, buf, cnt);
	}

	free(saptr);

	return 0;
}
