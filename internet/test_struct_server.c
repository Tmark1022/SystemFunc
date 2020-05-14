/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 11:30:39 AM CST
 @ File Name	: test_struct_server.c
 @ Description	: 
 ************************************************************************/
#include "wrap/wrap.h"
#include "test_struct.h"

char buf[1024];


int main(int argc, char *argv[]) {
	
	printf("size is %lu\n", sizeof(Proto));

	return 0;




	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("call socket failed");
		exit(EXIT_FAILURE);
	}
	
	int ret;	
	struct sockaddr_in serverSockAddr;
	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_port = htons(8888);
	serverSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	ret = bind(sockfd, (const struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr_in));
	if (-1 == ret) {
		perror("call bind failed");
		exit(EXIT_FAILURE);
	}

	ret = listen(sockfd, SOMAXCONN);
	if (-1 == ret) {
		perror("call listen failed");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in clientSockAddr;
	socklen_t len;
	int clientfd;
	clientfd = accept(sockfd, (struct sockaddr *)&clientSockAddr, &len);
	if (-1 == clientfd) {
		perror("call accept failed");
		exit(EXIT_FAILURE);
	}
	
	char clentAddrStr[1024];
	const char * aaaa;
	if (NULL == (aaaa = inet_ntop(AF_INET, (void * )&clientSockAddr.sin_addr.s_addr, clentAddrStr, sizeof(clentAddrStr)))) {
		perror("inet_ntop error");
		exit(EXIT_FAILURE);
	}


	// 重定向标准输出到连接到的client	
	if ( -1 == dup2(clientfd, STDOUT_FILENO)) {
		perror("dup2 error");
		exit(EXIT_FAILURE);
	}

	while( (ret = read(clientfd, buf, 1024 - 1))) {
		if (-1 == ret) {
			abort();
		}
		
		// 尾部出入\0
		buf[ret] = '\0';
		int status = system(buf);		
		fprintf(stderr, "call %s, status is %d\n", buf, status);
	}
	
	close(sockfd);
	close(clientfd);
	
	return 0;
}
