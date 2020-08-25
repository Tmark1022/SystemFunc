#include "inet_wrap.h"

int
writeable_timeo(int fd, int sec)
{
	fd_set			wset;
	struct timeval	tv;

	FD_ZERO(&wset);
	FD_SET(fd, &wset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;

	return(select(fd+1, NULL, &wset, NULL, &tv));
		/* 4> 0 if descriptor is writeable */
}
/* end writeable_timeo */

int
Writeable_timeo(int fd, int sec)
{
	int		n;

	if ( (n = writeable_timeo(fd, sec)) < 0) {
		PrintError(stderr, -1, "writeable_timeo error", EXIT_FAILURE);	
	}
	return(n);
}
