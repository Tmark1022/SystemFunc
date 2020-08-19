/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 19 Aug 2020 01:57:56 PM CST
 @ File Name	: tcp_serv.c
 @ Description	: 
 ************************************************************************/

#include "../inet_wrap.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		PrintError(stderr, -1, "need more argument", EXIT_FAILURE);
	}

	int lfd = Tcp_listen(argv[1], argv[2], NULL);
	
	while (1) {
		SS cli_addr;
		bzero(&cli_addr, sizeof(SS));
		socklen_t cli_addr_len = sizeof(SS);
		int cfd = Accept(lfd, (SA *)&cli_addr, &cli_addr_len);
		PrintAddr(stdout, (SA *)&cli_addr, "new connect");
		
		char buf[MAX_BUF_SIZE];	
		while (1) {
			int n = Read(cfd, buf, MAX_BUF_SIZE);
			if (0 == n) {
				Close(cfd);
				break;
			}
			Write(cfd, buf, n);
		}
	}

	return 0;
}
