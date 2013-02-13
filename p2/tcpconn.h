#ifndef __TCPCONN_H_
#define __TCPCONN_H_

int TCPConnect (char* ip_address, int port);
void WriteAll (int fd, const char *buf, size_t n);
int StartTCPServer (int port);
int MyAccept (int fd);

#endif
