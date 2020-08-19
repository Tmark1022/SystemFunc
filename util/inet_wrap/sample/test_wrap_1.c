/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 18 Aug 2020 06:05:04 PM CST
 @ File Name	: test_wrap_1.c
 @ Description	: 
 ************************************************************************/
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../inet_wrap.h"

int main(int argc, char *argv[]) {
	
	char *host = argv[1];
	char *serv = argv[2];
	struct addrinfo * res = Host_serv(host, serv, 0, 0);
	struct addrinfo * save_res = res; 
	
	for (; save_res != NULL; save_res = save_res->ai_next) {
		// char * ipstr = Sock_ntop_host(save_res->ai_addr, save_res->ai_addrlen);
		//printf("canonical name : %s, ip : %s, port %d\n", save_res->ai_canonname, ipstr, ntohs(sock_get_port(save_res->ai_addr, save_res->ai_addrlen)));	
		char ipstr[100], portstr[10];
		SockAddrToHumanStr(save_res->ai_addr, save_res->ai_addrlen, ipstr, sizeof(ipstr), portstr, sizeof(portstr));
		printf("canonical name : %s, ip : %s, port %s\n", save_res->ai_canonname, ipstr, portstr);	

	}		

	freeaddrinfo(save_res);
	return 0;
}
