/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 03 Jun 2020 07:11:15 PM CST
 @ File Name	: test4.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <signal.h>

#define AF_FAMILY	AF_INET

void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode)
{

#if (-1 == my_errno)
	fprintf(stream, "%s\n", headStr);	
#else 
	if (!my_errno) {
		my_errno = errno;
	}	
	fprintf(stream, "%s : %s\n", headStr, strerror(my_errno));	
#endif
	if (exitCode) {
		exit(exitCode);	
	}
}

void PrintAddr(FILE * stream, struct sockaddr_in * addr, const char * headStr)
{
	char arr[INET_ADDRSTRLEN];	
	inet_ntop(AF_INET, &(addr->sin_addr), arr, sizeof(arr)); 
	fprintf(stream, "%s, %s:%d\n", headStr, arr, ntohs(addr->sin_port));	
}

void SetNonBlocking(int fd)
{
	int flag = fcntl(fd, F_GETFL);
	if (-1 == flag) {		
		PrintError(stderr, 0, "fcntl(fd, F_GETFL)", EXIT_FAILURE);	
	}
	flag |= O_NONBLOCK;
	int ret = fcntl(fd, F_SETFL, flag);
	if (-1 == ret) {
		PrintError(stderr, 0, "fcntl(fd, F_SETFL, flag)", EXIT_FAILURE);	
	}
}

int Socket(int domain, int type, int protocol)
{
	int ret = socket(domain, type, protocol);
	if (-1 == ret) {
		PrintError(stderr, 0, "call socket failed", EXIT_FAILURE);		
	}
	return ret;

}

int Bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	int ret = bind(socket, address, address_len);
	if (-1 == ret) {
		PrintError(stderr, 0, "call bind failed", EXIT_FAILURE);		
	}

#if defined(AF_FAMILY) && (AF_FAMILY) == (AF_INET)
	// 这里只考虑ipv4	
	PrintAddr(stdout, (struct sockaddr_in *)address, "bind address");
#endif
	return ret;
}

int Listen(int socket, int backlog)
{
	int ret = listen(socket, backlog);
	if (-1 == ret) {
		PrintError(stderr, 0, "call listen failed", EXIT_FAILURE);		
	}
	return ret;
}

int Accept(int socket, struct sockaddr *address, socklen_t * address_len)
{
	int ret = accept(socket, address, address_len);
	if (-1 == ret) {
		PrintError(stderr, 0, "call accept failed", EXIT_FAILURE);		
	}

#if defined(AF_FAMILY) && (AF_FAMILY) == (AF_INET)
	if (NULL != address) {
		// 这里只考虑ipv4	
		PrintAddr(stdout, (struct sockaddr_in *)address, "new client");
	}
#endif
	return ret;
}

int Close(int fd)
{
	int ret = close(fd);
	if (-1 == ret) {
		PrintError(stderr, 0, "call close failed", EXIT_FAILURE);		
	}

#if defined(AF_FAMILY) && (AF_FAMILY) == (AF_INET)
	// 这里只考虑ipv4	
	printf("close fd %d\n", fd);
#endif
	return ret;
}

ssize_t Read(int fd, void *buf, size_t count)
{
	ssize_t cnt = read(fd, buf, count);
	if (-1 == cnt) {
		// error
		PrintError(stderr, 0, "call Read failed", EXIT_FAILURE);		
	}

	return cnt;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
	ssize_t cnt = write(fd, buf, count);
	if (-1 == cnt) {
		PrintError(stderr, 0, "call Write failed", EXIT_FAILURE);		
	}
	return cnt;

}

void readcb(evutil_socket_t fd, short what, void * arg)
{
	char buf[50];		
	int cnt = Read(fd, buf, 50);
	printf("读到%d数据\n", cnt);
	if (0 == cnt) {
		close(fd);
		struct event * selfEv = (struct event *) arg;
		event_del(selfEv);
		event_free(selfEv);
		printf("关闭%d\n", fd);		
		return ;
	}

	Write(fd, buf, cnt);		
}

void acceptcb(evutil_socket_t fd, short what, void * arg)
{
	struct sockaddr_in cliAddr;
	socklen_t len = sizeof(cliAddr);
	int cfd = Accept(fd, (struct sockaddr *)&cliAddr, &len);

	struct event * selfEv = (struct event *) arg;
	struct event_base * base = event_get_base(selfEv);
	
	struct event * ev = event_new(base, cfd, EV_READ | EV_PERSIST, readcb, event_self_cbarg());
	event_add(ev, NULL);			
}

void sigint(evutil_socket_t fd, short what, void * arg)
{	
	struct event_base * base = (struct event_base *)arg;
	event_base_loopexit(base, NULL);
}

int main(int argc, char *argv[]) {
	

	int lfd = Socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		PrintError(stderr, 0, "call setsockopt failed", EXIT_FAILURE);		
	}

	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(8888);
	svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind(lfd, (struct sockaddr *)&svrAddr, sizeof(svrAddr));
	Listen(lfd, SOMAXCONN);

	struct event_base * base = event_base_new();
	struct event * listenEv = event_new(base, lfd, EV_READ | EV_PERSIST, acceptcb, event_self_cbarg());
	event_add(listenEv, NULL);

	struct event * sigEv = event_new(base, SIGINT, EV_SIGNAL | EV_PERSIST, sigint, base); 
	event_add(sigEv, NULL);
	
	event_base_dispatch(base);

	printf("loop exit\n");
	event_free(listenEv);
	event_free(sigEv);
	event_base_free(base);

	return 0;
}
