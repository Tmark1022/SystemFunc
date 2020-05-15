/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 11:30:39 AM CST
 @ File Name	: test_struct_server.c
 @ Description	: 
 ************************************************************************/
#pragma pack(4)

#include "wrap/wrap.h"
#include "test_struct.h"

int stdoutFd;

void DoReadWrite(int cfd) 
{
	int cnt;
	Proto pto;

	while ((cnt = ReadCount(cfd, &pto, sizeof(Proto))) > 0) {
		if (cnt != sizeof(Proto)) {
			fprintf(stderr, "cnt != sizeof(Proto)\n");
			exit(EXIT_FAILURE);
		}
		
		// invalid type	
		if (pto.type != TYPE_ECHO && pto.type != TYPE_CALL) {
			fprintf(stderr, "type(%d) error\n", pto.type);
			exit(EXIT_FAILURE);
		}	

		// 后续的内容读出来
		int size = pto.size;	
		if (size < 0) {
			fprintf(stderr, "pto size error\n");
			exit(EXIT_FAILURE);
		}
	
		printf("get pto (%d, %d)\n", pto.type, pto.size);

		if (0 == size) {
			continue;
		}	
		
		// +1 兼容最后一个\0
		char * tmp = malloc(size + 1); 	
		if (NULL == tmp) {
			fprintf(stderr, "malloc error \n");
			exit(EXIT_FAILURE);

		}

		cnt = ReadCount(cfd, tmp, size);
		if (cnt != size) {
			fprintf(stderr, "miss content\n");
			continue;
		}
		
		if (pto.type == TYPE_ECHO) {	
			WriteCount(cfd, tmp, cnt);

			tmp[cnt] = '\0'; 
			printf("get string : %s\n", tmp);

		} else if (pto.type == TYPE_CALL) {
			// 重定向标准输出到连接到的client	
			if ( -1 == dup2(cfd, STDOUT_FILENO)) {
				perror("dup2 error");
				exit(EXIT_FAILURE);
			}
			
			tmp[cnt] = '\0';
			int status = system(tmp);

			// 恢复标准输出
			if ( -1 == dup2(stdoutFd, STDOUT_FILENO)) {
				perror("dup2 error, 2222222");
				exit(EXIT_FAILURE);
			}
			
			printf("call : %s\nreturn status : %d\n", tmp, status);
		}

		free(tmp);
		tmp = NULL;
	}
}

int main(int argc, char * argv[]) 
{
	int port = 8888;
	if (argc == 2) {
		port = atoi(argv[1]);
	}

	int fd = Socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	Bind(fd, (struct sockaddr *)&bindAddr, sizeof(bindAddr));  	

	Listen(fd, SOMAXCONN);

	socklen_t addrlen;
	struct sockaddr_in clientAddr;
	int cfd = Accept(fd, (struct  sockaddr * )&clientAddr, &addrlen);
	
	// dup标准输出，保存一份
	stdoutFd = dup(STDOUT_FILENO);
	if (-1 == stdoutFd) {
		perror("dup error");
		exit(EXIT_FAILURE);
	}

	DoReadWrite(cfd);

	close(cfd);	
	close(fd);

	return 0;
}


