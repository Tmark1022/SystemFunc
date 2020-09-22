/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 24 Aug 2020 12:52:18 PM CST
 @ File Name	: xinetd_echo_udp.c
 @ Description	: xinetd_echo udp 版本
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	char tmp[2048];
	if (argc > 1) {
	}
	
	char buf[1025];
	struct sockaddr_storage src_addr;
	bzero(&src_addr, sizeof(src_addr));
	socklen_t addrlen = sizeof(src_addr);
	int n = recvfrom(STDIN_FILENO, buf, 1024, 0, (struct sockaddr *)&src_addr, &addrlen);	

	if (-1 == n) {
		// error log
		exit(EXIT_FAILURE);	
	} else {
		buf[n] = '\0';
		if (argc > 1) {
			snprintf(tmp, 2048, "%s : %s", argv[1], buf);
		} else {
			snprintf(tmp, 2048, "%s", buf);
		}

		if (-1 == sendto(STDOUT_FILENO, tmp, strlen(tmp), 0, (struct sockaddr *)&src_addr, addrlen)) {
			// error log
			exit(EXIT_FAILURE);	
		}
	}	
	
	return 0;
}
