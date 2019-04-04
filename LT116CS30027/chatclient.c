#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>

#define STDIN 0

int max(int a, int b)
{
	return a>b ? a : b;
}
int main()
{
	int sockfd , maxfd, nready, k;
	char buf[10];
	struct sockaddr_in cliaddr, servaddr;
	
	if( (sockfd= socket(AF_INET, SOCK_STREAM,0))==0)
	{
		perror("socket error");
		exit(EXIT_FAILURE);
	}
	servaddr.sin_family = AF_INET;
	servaddr.sin_port=htons(20000);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if( connect(sockfd, (struct sockaddr*)&servaddr , sizeof(servaddr) ) <0)
	{
		perror("error");
		exit(EXIT_FAILURE);
	}
	
	fd_set readfds;

	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);

		FD_SET(STDIN, &readfds);
		
		maxfd = max(STDIN, sockfd)+1;
		nready = select(maxfd, &readfds, 0,0,0);
		
		if(FD_ISSET(STDIN, &readfds))
		{
			read(STDIN, buf, 10, 0);
			buf[9]='\0';
			send(sockfd, buf, 10, 0);
			printf("Client message %s sent to server\n", buf);
		}
		if(FD_ISSET(sockfd, &readfds))
		{
			char ip[32];
			short int port;
			bzero(buf, sizeof(buf));
			k =recv(sockfd, ip, sizeof(ip), 0);
			if(k==0)
				break;
			k = recv(sockfd, &port, sizeof(port), 0);
			if(k==0)
				break;
			k = recv(sockfd, buf, 10, 0);
			if(k==0)
				break;
			printf("Client: Received %s from <%s:%d>\n", buf, ip, port );
			
		}

	}
	
}
