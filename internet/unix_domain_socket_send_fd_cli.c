/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 28 Aug 2020 11:53:40 AM CST
 @ File Name	: unix_domain_socket_send_fd_cli.c
 @ Description	: 
 ************************************************************************/
#include <asm-generic/errno-base.h>
#include <asm-generic/socket.h>
#include <inet_wrap.h>
#include <sys/socket.h>

/***************************************************
* global variables or marco
***************************************************/
#define SERVER_SOCKET_FILE "/tmp/9Lq7BNBnBycd6nxy_server.socket"
fd_set allRSet; 
int max_fd = -1;

/***************************************************
* select  
***************************************************/
void add_to_fd_set(int fd) 
{
	FD_SET(fd, &allRSet);	
	if (fd > max_fd) {
		max_fd = fd;
	}
}

void remove_from_fd_set(int fd) 
{
	FD_CLR(fd, &allRSet);	
}


/***************************************************
* echo 
***************************************************/
void do_write(int fd, int cfd)
{
	char buf[MAX_BUF_SIZE];
	int n = Read(fd, buf, MAX_BUF_SIZE); 
	if (0 == n) {
		printf("stdin close\n");
		exit(EXIT_SUCCESS);
	}

	Write(cfd, buf, n);
}

void do_read(int cfd)
{	
	char buf[MAX_BUF_SIZE];
	int n = Read(cfd, buf, MAX_BUF_SIZE); 
	if (0 == n) {
		printf("peer close\n");
		exit(EXIT_SUCCESS);
	}

	Write(STDOUT_FILENO, buf, n);
}


int main(int argc, char *argv[]) {	
	// connect to unix domain server
	int ufd = Socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sun_family = AF_UNIX; 
	strcpy(svrAddr.sun_path, SERVER_SOCKET_FILE);
	Connect(ufd, (SA *)&svrAddr, sizeof(svrAddr));

	// recv fd
	struct msghdr msgh = { 0 };
	char iobuf[MAX_BUF_SIZE];
	struct iovec io = {
	    .iov_base = iobuf,
	    .iov_len = sizeof(iobuf)
	};	
	union {         /* Ancillary data buffer, wrapped in a union
	                   in order to ensure it is suitably aligned */
	    char buf[CMSG_SPACE(sizeof(int))];
	    struct cmsghdr align;
	} u;
	msgh.msg_iov = &io;
	msgh.msg_iovlen = 1;
	msgh.msg_control = u.buf;
	msgh.msg_controllen = sizeof(u.buf);

	struct cmsghdr *cmsg;
	int send_fd = -1;	
	if (-1 == recvmsg(ufd, &msgh, 0)) {
		printf("errno is %d\n", errno);
		PrintError(stderr, 0, "recvmsg error", EXIT_FAILURE);		
	}	

	for (cmsg = CMSG_FIRSTHDR(&msgh); cmsg != NULL; cmsg = CMSG_NXTHDR(&msgh, cmsg)) {
	    if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
	        send_fd = *((int *) CMSG_DATA(cmsg));
	        break;
	    }
	}
	if (-1 == send_fd) {
		PrintError(stderr, -1, "recv send_fd failed", EXIT_FAILURE);	
	} else {
		printf("get send_fd successfully, send_fd = %d\n", send_fd);
	}
	close(ufd);
	
	/***************************************************
	* do somethine over send_fd 
	***************************************************/
	// stdin	
	FD_ZERO(&allRSet);
	add_to_fd_set(STDIN_FILENO);
	add_to_fd_set(send_fd);

	fd_set runRSet; 
	while (1) {
		runRSet = allRSet;
		int nreadys = select(max_fd + 1,  &runRSet, NULL, NULL, NULL);	
		if (-1 == nreadys) {
			if (EINTR == errno) {
				continue;
			} else {
				PrintError(stderr, 0, "call select failed", EXIT_FAILURE);		
			}
		}	

		if (nreadys <= 0) {
			continue;
		}
		
		// stdin
		if (FD_ISSET(STDIN_FILENO, &runRSet)) {
			do_write(STDIN_FILENO, send_fd);
		}

		if (FD_ISSET(send_fd, &runRSet)) {
			do_read(send_fd);
		}
	}

	return 0;
}
