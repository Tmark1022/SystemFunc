/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 28 Aug 2020 11:24:09 AM CST
 @ File Name	: unix_domain_socket_send_fd_server.c
 @ Description	: 
 ************************************************************************/
#include <inet_wrap.h>
#include <sys/socket.h>

/***************************************************
* global variables or marco
***************************************************/
#define SERVER_SOCKET_FILE "/tmp/9Lq7BNBnBycd6nxy_server.socket"

void send_fd_func(int cfd, int send_fd)
{
	struct msghdr msg = { 0 };
	struct cmsghdr *cmsg;
	int *fdptr;
	char iobuf[1];
	struct iovec io = {
	    .iov_base = iobuf,
	    .iov_len = sizeof(iobuf)
	};
	
	union {         /* Ancillary data buffer, wrapped in a union
	                   in order to ensure it is suitably aligned */
	    char buf[CMSG_SPACE(sizeof(int))];
	    struct cmsghdr align;
	} u;

	msg.msg_iov = &io;
	msg.msg_iovlen = 1;
	msg.msg_control = u.buf;
	msg.msg_controllen = sizeof(u.buf);
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	fdptr = (int *) CMSG_DATA(cmsg);    /* Initialize the payload */
	memcpy(fdptr, &send_fd, sizeof(int));

	if (-1 == sendmsg(cfd, &msg, 0)) {
		PrintError(stderr, 0, "sendmsg error", EXIT_FAILURE);	
	}
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("usage : %s host serv\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// unix domain server
	int lfd = Socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sun_family = AF_UNIX; 
	strcpy(svrAddr.sun_path, SERVER_SOCKET_FILE);
	unlink(SERVER_SOCKET_FILE);
	Bind(lfd, (struct sockaddr *)&svrAddr, sizeof(svrAddr)); 
	Listen(lfd, SOMAXCONN);

	while (1) {		
		int cfd = Accept(lfd, NULL, 0);	
		printf("new connect\n");	

		// connect to server
		int send_fd = Tcp_connect(argv[1], argv[2]);	
		
		// send fd to client
		send_fd_func(cfd, send_fd);
		printf("send fd %d successfully\n", send_fd);	

		close(send_fd);	// 无须等client recvmsg	完成
		close(cfd);
	}

	close(lfd);	
	return 0;
}
