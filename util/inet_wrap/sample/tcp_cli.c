/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 19 Aug 2020 02:12:32 PM CST
 @ File Name	: tcp_cli.c
 @ Description	: 
 ************************************************************************/
#include "../inet_wrap.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		PrintError(stderr, -1, "need more argument", EXIT_FAILURE);
	}

	int cfd = Tcp_connect(argv[1], argv[2]);

	SS peer_addr;
	bzero(&peer_addr, sizeof(SS));
	socklen_t peer_addr_len = sizeof(SS);
	int ret = getpeername(cfd, (SA *)&peer_addr, &peer_addr_len);
	if (-1 == ret) {
		PrintError(stderr, 0, "getpeername error", EXIT_FAILURE);
	}
	PrintAddr(stdout, (SA*)&peer_addr, "connect to");
	
	char buf[MAX_BUF_SIZE];
	while (NULL != fgets(buf, MAX_BUF_SIZE, stdin)) {	
		Write(cfd, buf, strlen(buf));
		int n = Read(cfd, buf, MAX_BUF_SIZE);
		if (0 == n) {
			break;
		} else {
			Write(STDOUT_FILENO, buf, n);
		}	
	}

	close(cfd);

	return 0;
}
