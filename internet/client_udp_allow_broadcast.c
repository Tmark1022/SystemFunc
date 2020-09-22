/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 22 Sep 2020 11:47:36 AM CST
 @ File Name	: client_udp_allow_broadcast.c
 @ Description	: 允许广播的udp客户端 
 ************************************************************************/

#include <asm-generic/socket.h>
#include <inet_wrap.h>
#include <sys/socket.h>

/***************************************************
* global variable, marco 
***************************************************/
char * serv = NULL;
char * host = NULL;

int broadcast = 0;
/***************************************************
* other 
***************************************************/
void HandleOpt(int argc, char * argv[]) 
{
	int opt;
	while ((opt = getopt(argc, argv, "h:p:b")) != -1) {
		switch (opt) {
			case 'h':
				host = optarg; 
				break;
			case 'p':
				serv = optarg;
				break;	
			case 'b':
				broadcast = 1;
				break;
               		default: 
               		    fprintf(stderr, "Usage: %s [-h host][-p serv][-b]\n", argv[0]);
               		    exit(EXIT_FAILURE);
               	}
	}
	
	/*
	if (optind >= argc) {
		fprintf(stderr, "need more argument\n");
		exit(EXIT_FAILURE);
	}
	*/
}

void str_cli(FILE *fp, int cfd, SA * dst, socklen_t dst_len)
{
	char buf[MAX_BUF_SIZE];
	fd_set all_read_set;
	FD_ZERO(&all_read_set);	
	int stdin_fileno = fileno(fp);
	FD_SET(stdin_fileno, &all_read_set);
	FD_SET(cfd, &all_read_set);
		
	while (1) {
		int nfds = max(stdin_fileno, cfd) + 1; 
		fd_set readfds  = all_read_set;
		int ret = select(nfds, &readfds, NULL, NULL, NULL);
		if (-1 == ret) {
			if (EINTR == errno) {
				continue;
			} else {
				PrintError(stderr, 0, "select failed", EXIT_FAILURE);		
			}
		}
	
		// udp socket
		if (FD_ISSET(cfd, &readfds)) {
			struct sockaddr_in cliAddr;
			bzero(&cliAddr, sizeof(struct sockaddr_in));
			socklen_t len = sizeof(cliAddr);
			int cnt = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&cliAddr, &len);
			if (-1 == cnt) {
				PrintError(stderr, 0, "call recvfrom failed", EXIT_FAILURE);		
			}
			char tmp[MAX_BUF_SIZE];	
			sprintf(tmp, "addr : %s, port : %d ==> ", Sock_ntop_host((SA *)&cliAddr, len), sock_get_port((SA *)&cliAddr, len));
			
			write(STDOUT_FILENO, tmp, strlen(tmp));
			write(STDOUT_FILENO, buf, cnt);
		}

		// stdin 
		if (FD_ISSET(stdin_fileno, &readfds)) {
			int cnt = read(stdin_fileno, buf, MAX_BUF_SIZE);
			if (cnt == -1) {
				if (EINTR != errno) {
					PrintError(stderr, 0, "read failed", EXIT_FAILURE);		
				} else {
					continue;
				}
			} else if (cnt == 0) {
				// eof
				return ;
			}				
				
			int ret = sendto(cfd, buf, cnt, 0, dst, dst_len);
			if (-1 == ret) {
				PrintError(stderr, 0, "call sendto failed", EXIT_FAILURE);		
			}
		}
	}
}


int main(int argc, char *argv[]) {
	HandleOpt(argc, argv);
	SA  * saptr;  
	socklen_t len; 
	int sockfd = Udp_client(host, serv, &saptr, &len);
	
	// broadcast
	if (broadcast) {
		int flag = 1;		
		if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag))) {
			PrintError(stderr, 0, "setsockopt failed", EXIT_FAILURE);		
		}
	}	

	str_cli(stdin, sockfd, saptr, len);
	close(sockfd);
	return 0;
}
