#include <sys/socket.h>
#include <sys/types.h>
#include <search.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define DROP_PROB 0.2
#define T 2
#define SOCK_MRP 20

int r_socket(int domain, int type, int protocol);
int r_bind(int socket, const struct sockaddr *address, socklen_t address_len);
ssize_t r_sendto(int socket, const void *buffer, size_t length, int flags, const struct sockaddr *dest_addr, 
socklen_t dest_len);
ssize_t r_recvfrom(int socket, void *restrict buffer, size_t length, int flags,
struct sockaddr *restrict address, socklen_t *restrict address_len);
int r_close(int fildes);

int dropMessage(float p);
void HandleRetransmit(int socket);
void HandleAppMsgRecv(int socket, int id, char* buffer,int length, struct sockaddr_in dest_addr);
void HandleACKMsgRecv(int socket, int id);
void HandleReceive(int socket);

