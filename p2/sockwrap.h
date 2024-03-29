#ifndef __SOCKWRAP_H_
#define __SOCKWRAP_H_

int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr_in *sa, socklen_t salen);
int Connect(int fd, const struct sockaddr_in *sa, socklen_t salen);
int Accept(int fd, struct sockaddr_in *sa, socklen_t *salenptr);
void Listen(int fd, int backlog);
ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr_in *sa, socklen_t *salenptr);
void Sendto(int fd, const void *ptr, int nbytes, int flags, const struct sockaddr_in *sa, socklen_t salen);
ssize_t Read(int fd, char *ptr, size_t nbytes);
void Write(int fd, char *ptr, int nbytes);
void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
void Close(int fd);
pid_t Fork();

#endif

