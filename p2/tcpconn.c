#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include "sockwrap.h"
#include "tcpconn.h"

#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(1); }

/*
 * Returns a socket connected with a given IP and port.
 */
int TCPConnect (char* ip_address, int port) 
{
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
    memset(&server_address,sizeof(server_address),0);
	server_address.sin_family   = AF_INET;
	server_address.sin_port     = htons(port);
	inet_pton(AF_INET, ip_address, &server_address.sin_addr);
	int e=Connect (sockfd, &server_address, sizeof(server_address)); 
    if(e==0)
        return sockfd;
    else
        return -1;
}

/*
 * Retries sending data through socket till the complete buffer is written.
 */
void WriteAll (int fd, const char *buf, size_t n) 
{
	signal(SIGPIPE, SIG_IGN);	// Signal SIGPIPE won't end the program but 
								// cause error in write()
	size_t nleft = n;
	while (nleft > 0) {
		int result = write(fd, buf, nleft);
		if (result < 0) ERROR("write error");
		printf ("DEBUG (WriteAll): %d bytes written\n", result);
		nleft -= result;
		buf   += result;
	}
}

/*
 * Creates a listening socket for the server, binds to concrete port and start listening.
 */
int StartTCPServer (int port)
{
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    
	struct sockaddr_in server_address;
    memset(&server_address,sizeof(server_address),0);
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
	Bind (sockfd, &server_address, sizeof(server_address));
	Listen (sockfd, 64);
	return sockfd;
}


/* 
 * Accepts a waiting connection from a queue, returns the descriptor of the connected socket.
 */
int MyAccept (int fd)
{
	struct sockaddr_in client_address;
	socklen_t len = sizeof(client_address);
	int fd_conn = Accept (fd, &client_address, &len);
    if(fd_conn==-2)
        return -2;
	
	char ip_address[20];
	inet_ntop (AF_INET, &client_address.sin_addr, ip_address, sizeof(ip_address));
	printf ("A new client %s:%d\n", ip_address, ntohs(client_address.sin_port));

	return fd_conn;
}
