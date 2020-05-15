/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 14 May 2020 03:10:22 PM CST
 @ File Name	: test_struct_client.c
 @ Description	: 
 ************************************************************************/
#include "wrap/wrap.h"
#include "test_struct.h"

char inbuf[1024];
char outbuf[10240];

int DoInput(Proto * pto) 
{
	if (NULL == pto) {
		exit(EXIT_FAILURE);	
	}

	int type;
	while (1) {
		scanf("%d", &type);
		getchar();
		if (type != TYPE_CALL && type != TYPE_ECHO) {
			printf("type error, repeat input\n");
			continue;
		}

		if (NULL == fgets(inbuf, sizeof(inbuf), stdin)) {
			exit(EXIT_FAILURE);
		}
		
		if (strlen(inbuf) > 0) {
			if (inbuf[strlen(inbuf) - 1] == '\n') {
				// 去掉换行
				inbuf[strlen(inbuf) - 1] = '\0';
			}
		}	

		pto->type = type;	
		pto->size = strlen(inbuf);

		break;
	}		

	return 1;
}

int main(int argc, char *argv[]) {

	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("call socket failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverSockAddr;
	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serverSockAddr.sin_addr);
	Connect(sockfd, (const struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr_in));


	Proto pto;
	int ret;
	while(1) {	
		DoInput(&pto); 

		
		// 先写头，然后写数据部分	
		WriteCount(sockfd, &pto, sizeof(Proto));
		WriteCount(sockfd, inbuf, strlen(inbuf));
	
		ret = read(sockfd, outbuf, sizeof(outbuf) - 1);
		if (-1 == ret) {
			perror("read error");
			exit(EXIT_FAILURE);
		}	

		// 尾部出入\0
		outbuf[ret] = '\0';	
		printf("%s\n", outbuf);
			
	}
	
	close(sockfd);
	
	return 0;
}
