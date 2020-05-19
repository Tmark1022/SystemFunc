#ifndef WRAP_H
#define WRAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>

#define AF_FAMILY AF_INET  

void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode);
void PrintAddr(FILE * stream, struct sockaddr_in * addr, const char * headStr);
void SetNonBlocking(int fd);




int Socket(int domain, int type, int protocol);
int Bind(int socket, const struct sockaddr *address, socklen_t address_len);
int Listen(int socket, int backlog);
int Accept(int socket, struct sockaddr *address, socklen_t * address_len);
int Connect(int socket, const struct sockaddr *address, socklen_t address_len);
int Close(int fd);


ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
ssize_t ReadCount(int fd, void *buf, size_t count);
ssize_t WriteCount(int fd, const void *buf, size_t count);
ssize_t ReadLine(int fd, void *buf, size_t size);





#endif
