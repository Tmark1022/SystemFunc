#ifndef INET_WRAP_H
#define INET_WRAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/un.h>

/***************************************************
* marco 
***************************************************/
#define MAX_BUF_SIZE 1024
#define MAX_LINE_SIZE 1024
#define max(a,b) (a) > (b) ? (a) : (b)


/***************************************************
* generic  
***************************************************/
void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode);
void PrintAddr(FILE * stream, struct sockaddr_in * addr, const char * headStr);
void FcntlAddFlag(int fd, int flag);
void SetNonBlocking(int fd);
void SockAddrToHumanStr(struct sockaddr * addr, socklen_t addrlen, char * host, socklen_t hostlen, char *port, socklen_t portlen);

char *
Sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
int
sock_get_port(const struct sockaddr *sa, socklen_t salen);

/***************************************************
* socket 
***************************************************/
int Socket(int domain, int type, int protocol);
int Bind(int socket, const struct sockaddr *address, socklen_t address_len);
int Listen(int socket, int backlog);
int Accept(int socket, struct sockaddr *address, socklen_t * address_len);
int Connect(int socket, const struct sockaddr *address, socklen_t address_len);
int Close(int fd);

/***************************************************
* io function 
***************************************************/
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
ssize_t ReadCount(int fd, void *buf, size_t count);
ssize_t WriteCount(int fd, const void *buf, size_t count);

/***************************************************
* dns && protocol independence
***************************************************/
struct addrinfo *
Host_serv(const char *host, const char *serv, int family, int socktype);




#endif
