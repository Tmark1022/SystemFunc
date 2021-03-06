/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 18 Aug 2020 05:28:15 PM CST
 @ File Name	: generic_wrap.c
 @ Description	: 
 ************************************************************************/
#include "inet_wrap.h"
#include <netdb.h>
#include <sys/socket.h>

void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode)
{
	if (!my_errno) {
		my_errno = errno;
	}	
	
	if (-1 == my_errno) {
		fprintf(stream, "%s\n", headStr);
	} else {
		fprintf(stream, "%s : %s\n", headStr, strerror(my_errno));
	}
	if (exitCode) {
		exit(exitCode);	
	}
}

void SockAddrToHumanStr(struct sockaddr * addr, socklen_t addrlen, char * host, socklen_t hostlen, char *port, socklen_t portlen)
{
	int ret = getnameinfo(addr, addrlen, host, hostlen, port, portlen, NI_NUMERICHOST | NI_NUMERICSERV);
	if (0 != ret) {
		fprintf(stderr, "SockAddrToHumanStr error : %s\n", gai_strerror(ret));
		exit(EXIT_FAILURE);
	}
}

void PrintAddr(FILE * stream, SA * addr, const char * headStr)
{
	char ipstr[100], portstr[10];
	SockAddrToHumanStr(addr, sizeof(struct sockaddr_storage), ipstr, sizeof(ipstr), portstr, sizeof(portstr));
	fprintf(stream, "%s, %s:%s\n", headStr, ipstr, portstr);	
}

void FcntlAddFlag(int fd, int flag)
{
	int flags = fcntl(fd, F_GETFL);
	if (-1 == flags) {		
		PrintError(stderr, 0, "fcntl(fd, F_GETFL)", EXIT_FAILURE);	
	}

	flags |= flag;
	int ret = fcntl(fd, F_SETFL, flags);
	if (-1 == ret) {
		PrintError(stderr, 0, "fcntl(fd, F_SETFL, flags)", EXIT_FAILURE);	
	}

}	

void SetNonBlocking(int fd)
{
	FcntlAddFlag(fd, O_NONBLOCK);
}

void * Malloc(size_t size)
{
	void * res = malloc(size);
	if (NULL == res) {	
		PrintError(stderr, 0, "malloc error", EXIT_FAILURE);	
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

	return ret;
}

int Connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	int ret = connect(socket, address, address_len);
	if (-1 == ret) {
		PrintError(stderr, 0, "call connect failed", EXIT_FAILURE);		
	}

	return ret;
}

int Close(int fd)
{
	int ret = close(fd);
	if (-1 == ret) {
		PrintError(stderr, 0, "call close failed", EXIT_FAILURE);		
	}

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

ssize_t Send(int sockfd, const void *buf, size_t len, int flags)
{
	ssize_t cnt = send(sockfd, buf, len, flags);
	if (-1 == cnt) {
		PrintError(stderr, 0, "call Send failed", EXIT_FAILURE);		
	}
	return cnt;
}

ssize_t Recv(int sockfd, void *buf, size_t len, int flags)
{
	ssize_t cnt = recv(sockfd, buf, len, flags);
	if (-1 == cnt) {
		PrintError(stderr, 0, "call recv failed", EXIT_FAILURE);		
	}
	return cnt;
}

// @buf : 缓冲区指针
// count : 要读取的字节数
ssize_t ReadCount(int fd, void *buf, size_t count)
{
	// 与普通read一般, 不做buf大小合法判断		
	size_t left = count;	
	size_t index = 0;
	while (left > 0) {
		ssize_t cnt = read(fd, buf + index, left);
		if (0 == cnt) {
			// EOF
			break;
		}
		else if (-1 == cnt) {
			// error
			if (EAGAIN == errno || EWOULDBLOCK == errno) {
				continue;
			} else if (EINTR == errno) {
				// 信号中断慢速系统调用
				continue;
			} else {	
				PrintError(stderr, 0, "call ReadCount failed", EXIT_FAILURE);		
			}	
		} else {
			// 正常读取
			left -= cnt;	
			index += cnt;
		}
	}
	
	return index;
}

// @buf : 缓冲区指针
// @count : 写的字节数
ssize_t WriteCount(int fd, const void *buf, size_t count)
{
	// 该接口不完善， 后续继续优化
	// 与普通write一般, 不做buf大小合法判断		
	size_t left = count;	
	size_t index = 0;
	while (left > 0) {
		ssize_t cnt = write(fd, buf + index, left);
		if (0 == cnt) { 
			break;
		}
		else if (-1 == cnt) {
			// error
			if (EAGAIN == errno || EWOULDBLOCK == errno) {
				// 设置非阻塞
				continue;
			} else if (EINTR == errno) {
				// 信号中断慢速系统调用
				continue;
			} else {	
				PrintError(stderr, 0, "call WriteCount failed", EXIT_FAILURE);		
			}	
		} else {
			// 正常写
			left -= cnt;	
			index += cnt;
		}
	}
	
	return index;
}

