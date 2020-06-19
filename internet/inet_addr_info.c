/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 19 Jun 2020 09:12:59 AM CST
 @ File Name	: inet_addr_info.c
 @ Description	: 获取地址（主机，域名），端口（服务）等接口
 ************************************************************************/
#include <netinet/in.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int method = 0;
char * host;
char * srv; 
struct sockaddr_in appoint_addr;

void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "m")) != -1) {
		switch (opt) {
			case 'm':
			    method = 1;
			    break;

               		default: 
               		    fprintf(stderr, "Usage: %s [-p path string\n", argv[0]);
               		    exit(EXIT_FAILURE);
               	}
	}

	if (argc - optind != 2) {
		fprintf(stderr, "need more argument\n");
		exit(EXIT_FAILURE);
	}
		
	if (method) {
		// ip port -> host service
		appoint_addr.sin_family = AF_INET;		// 只测试af_inet
		inet_pton(AF_INET, argv[optind], &appoint_addr.sin_addr);
		appoint_addr.sin_port = htons(atoi(argv[optind + 1]));
	} else {
		// host service -> ip port
		host = argv[optind];		
		srv = argv[optind+1];		
	}
}
	
int main(int argc, char *argv[]) {

	HandleOpt(argc, argv); 	

	struct addrinfo hint;
	struct addrinfo * res = NULL;
	
	printf("run mode : %s\n", method ? "(ip:port) -> (host:service)":"(host:service) -> (ip:port)");
	
	if (method) {
		char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
		//int ret = getnameinfo((struct sockaddr *)&appoint_addr, sizeof(struct sockaddr_in), hbuf, NI_MAXHOST, sbuf, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
		int ret = getnameinfo((struct sockaddr *)&appoint_addr, sizeof(struct sockaddr_in), hbuf, NI_MAXHOST, sbuf, NI_MAXSERV, 0);
		if (ret) {
			fprintf(stderr, "getnameinfo failed : %s\n", gai_strerror(ret));
			exit(EXIT_FAILURE);
		}

		printf("host : %s, srv : %s\n", hbuf, sbuf);
	} else {
		bzero(&hint, sizeof(struct addrinfo));
		hint.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
		hint.ai_family = AF_UNSPEC;
		hint.ai_socktype = 0;
		hint.ai_protocol = 0;
		hint.ai_addrlen = 0;
		hint.ai_addr = NULL;
		hint.ai_canonname = NULL;
		hint.ai_next = NULL;
	
		int ret = getaddrinfo(host, srv, &hint, &res);
		if (ret) {
			fprintf(stderr, "getaddrinfo failed : %s\n", gai_strerror(ret));
			exit(EXIT_FAILURE);
		}
	
		for (struct addrinfo * p = res; p != NULL; p = p->ai_next) {
			const void * addr;
			int port;
			if (AF_INET == p->ai_family) {
				struct sockaddr_in * tmp = (struct sockaddr_in *)p->ai_addr;
				addr = (void *)(&tmp->sin_addr);
				port = ntohs(tmp->sin_port); 
			} else {
				struct sockaddr_in6 * tmp = (struct sockaddr_in6 *)p->ai_addr;
				addr = (void *)(&tmp->sin6_addr);
				port = ntohs(tmp->sin6_port); 
			}
			char straddr[64]; 
			inet_ntop(p->ai_family, addr, straddr, sizeof(straddr));
			printf("af=%d, socktype=%d, pro=%d, addr=%s, port=%d\n", p->ai_family, p->ai_socktype, p->ai_protocol, straddr, port);	
		}

		freeaddrinfo(res);
		res = NULL;
	}








	return 0;
}
