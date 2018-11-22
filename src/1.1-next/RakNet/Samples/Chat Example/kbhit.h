/* 
 * kbhit function on linux
 * i dont know who implemented it, but thanks anyway, i just found it on google 
 * romulo fernandes
 *
 */

#ifndef KBHIT
#define KBHIT

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int kbhit(void)
{
	struct timeval tv;
	fd_set read_fd;
 
	tv.tv_sec=0;
	tv.tv_usec=0;
	FD_ZERO(&read_fd);
	FD_SET(0,&read_fd);
 
	if(select(1, &read_fd, NULL, NULL, &tv) == -1)
	return 0;
 
	if(FD_ISSET(0,&read_fd))
	return 1;
 
	return 0;
}

#endif
