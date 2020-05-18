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
	char arr[INET_ADDRSTRLEN];	
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
	
	return index;
}

// @buf : 缓冲区指针 
// @size : 缓冲区大小 
// #return : line size
// buf读入\n
ssize_t ReadLine(int fd, void *buf, size_t size)
{
	size_t index = 0;
	char ch;
	char * tmp = (char *)buf;			

	while (1) {
		if ( index >= size ) {	
			PrintError(stderr, EFAULT, "call ReadLine failed", 0);		
			return -1;
		}

		ssize_t cnt = read(fd, &ch, 1);
		if (0 == cnt) {
			// EOF
			break;
		} else if (-1 == cnt) {
			// error
			if (EAGAIN == errno || EWOULDBLOCK == errno) {
				// read 设置非阻塞
				usleep(1000);
				continue;
			} else if (EINTR == errno) {
				// 信号中断慢速系统调用
				continue;
			} else {	
				PrintError(stderr, 0, "call ReadLine failed", EXIT_FAILURE);		
			}	
		} else {
			tmp[index] = ch;	
			index += cnt;

			// 正常读取
			if ('\n' == ch) {			
				break;
			}
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
			// 磁盘满了?, 无东西可以写？
			break;
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

ssize_t ReadLoop(int fd, void *buf, size_t count)
{	
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
				// read 设置非阻塞
				// 非阻塞读到底
				break;
			} else if (EINTR == errno) {
				// 信号中断慢速系统调用
				continue;
			} else {	
				PrintError(stderr, 0, "call ReadLoop failed", EXIT_FAILURE);		
			}	
		} else {
			// 正常读取
			left -= cnt;	
			index += cnt;
		}
	}
	
	return index;
}

