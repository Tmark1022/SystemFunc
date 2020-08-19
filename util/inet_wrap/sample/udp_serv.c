/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 19 Aug 2020 02:57:24 PM CST
 @ File Name	: sample/udp_serv.c
 @ Description	: 
 ************************************************************************/
#include "../inet_wrap.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		PrintError(stderr, -1, "need more argument", EXIT_FAILURE);
	}

	int lfd = Udp_server(argv[1], argv[2], NULL);
	char buf[MAX_BUF_SIZE];	

	while (1) {
		SS cli_addr;
		bzero(&cli_addr, sizeof(SS));
		socklen_t cli_addr_len = sizeof(SS);
			
		int cnt = recvfrom(lfd, buf, sizeof(buf), 0, (SA*)&cli_addr, &cli_addr_len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call recvfrom failed", EXIT_FAILURE);		
		}
		PrintAddr(stdout,  (SA *)&cli_addr, "new client");

		cnt = sendto(lfd, buf, cnt, 0, (struct sockaddr * )&cli_addr, cli_addr_len);
		if (-1 == cnt) {
			PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
		}
	}

	return 0;
}
