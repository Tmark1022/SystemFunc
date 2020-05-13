/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 12 May 2020 11:55:10 AM CST
 @ File Name	: wrap.c
 @ Description	: 常用网络编程函数接口的封装
 ************************************************************************/
#include "wrap.h"

void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode)
{
	if (!my_errno) {
		my_errno = errno;
	}	
	fprintf(stream, "%s : %s\n", headStr, strerror(my_errno));	
	if (exitCode) {
		exit(exitCode);	
	}
}

void PrintAddr(FILE * stream, struct sockaddr_in * addr, const char * headStr)
{
	char arr[1024];	
	inet_ntop(AF_INET, &(addr->sin_addr), arr, sizeof(arr)); 
	fprintf(stream, "%s, %s:%d\n", headStr, arr, ntohs(addr->sin_port));	
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

int Connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	int ret = connect(socket, address, address_len);
	if (-1 == ret) {
		PrintError(stderr, 0, "call connect failed", EXIT_FAILURE);		
	}

#if defined(AF_FAMILY) && (AF_FAMILY) == (AF_INET)
	// 这里只考虑ipv4	
	PrintAddr(stdout, (struct sockaddr_in *)address, "connect success, the peer address");
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

ssize_t ReadCount(int fd, void *buf, size_t count)
{
	// 与普通read一般, 不做buf大小合法判断		
	char * tmp = (char *)buf;			// rid警告
	size_t left = count;	
	size_t index = 0;
	while (left > 0) {
		ssize_t cnt = read(fd, buf + index, left);
		if (0 == cnt) {
			// EOF
			tmp[index] = '\0';
			return index;	
		}
		else if (-1 == cnt) {
			// error
			if (EAGAIN == errno || EWOULDBLOCK == errno) {
				// read 设置非阻塞
				usleep(1000);
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
	
	// 尾部增加'\0'
	tmp[index] = '\0';
	return index;
}

ssize_t ReadLine(int fd, void *buf, size_t count);

ssize_t WriteCount(int fd, const void *buf, size_t count);

