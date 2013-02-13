#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "sockwrap.h"
#include "tcpconn.h"

#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); }

int kbhit()
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
    {
        return 1;
    }

    return 0;
}

int socksel(int fd)
{
    struct timeval tv;
    fd_set read_fd;

    tv.tv_sec=0;
    tv.tv_usec=0;
    FD_ZERO(&read_fd);
    FD_SET(fd,&read_fd);

    if(select(fd+1, &read_fd, NULL, NULL, &tv) == -1)
        return 0;

    if(FD_ISSET(fd,&read_fd))
        return 1;

    return 0;
}

int Socket(int family, int type, int protocol)
{
	int	n;
	if ( (n = socket(family, type, protocol)) < 0)
		ERROR ("socket error");
	return n;
}

void Bind(int fd, const struct sockaddr_in *sa, socklen_t salen)
{
	if (bind(fd, (struct sockaddr*)sa, salen) < 0)
		ERROR ("bind error");
}

int Connect(int fd, const struct sockaddr_in *sa, socklen_t salen)
{
	if (connect(fd, (struct sockaddr*)sa, salen) < 0)
    {
        ERROR ("connect error");
        return -1;
    }
    return 0;
}
	
int Accept(int fd, struct sockaddr_in *sa, socklen_t *salenptr)
{
    int i;
    while(!kbhit()&&!(i=socksel(fd))) {};
    if(i==1)
    {
        if((i=accept(fd,(struct sockaddr*)sa,salenptr))<0)
            ERROR("accept error");
        return i;
    }
    return -2;
}

void Listen(int fd, int backlog)
{
    if (listen(fd, backlog) < 0)
		ERROR ("listen error");
}

ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr_in *sa, socklen_t *salenptr)
{
	ssize_t	n;
	if ( (n = recvfrom(fd, ptr, nbytes, flags, (struct sockaddr*)sa, salenptr)) < 0)
		ERROR ("recvfrom error");
	return n;
}

void Sendto(int fd, const void *ptr, int nbytes, int flags, const struct sockaddr_in *sa, socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, (struct sockaddr*)sa, salen) != nbytes)
		ERROR ("sendto error");
}

ssize_t Read(int fd, char *ptr, size_t nbytes) 
{
	int n;
	if ( (n = read(fd, ptr, nbytes)) < 0)
		ERROR ("read error");
	ptr[n] = 0;
	return n;
}

void Write(int fd, char *ptr, int nbytes) 
{
	// to niekoniecznie jest blad!
	if (write(fd, ptr, nbytes) < nbytes)
		ERROR ("write error");	
}

void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(sockfd,level,optname,optval,optlen) < 0) 
		ERROR ("setsockopt error");
}

void Close(int fd)
{
	if (close(fd) < 0)
		ERROR ("close error");
}

pid_t Fork() 
{
	pid_t n;
	if ((n = fork()) < 0)
		ERROR ("fork error");
	return n;
}
