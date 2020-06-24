#ifndef DAEMON_H
#define DAEMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <syslog.h>
#include <fcntl.h>

#define DAEMON_PID_PATH "/var/run/"

void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode);
void Daemonize(const char * cmd, int facility);
void SingletonDaemon(const char * cmd);









#endif
