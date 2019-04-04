#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

extern int errno;

struct client_Id
{
	char ip[32];
	short int port;
};

int main()
{
	int sockfd, i,j, newsockfd,k;
	struct sockaddr_in cliaddr, servaddr;
	socklen_t len;
	char buf[10];
	
	if( (sockfd= socket(AF_INET, SOCK_STREAM,0))==0)
	{
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port=htons(20000);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr) ) <0)
	{
		perror("unable to connect to server");
		exit(EXIT_FAILURE);
	}

	listen(sockfd, 5);

	fcntl(sockfd, F_SETFD, O_NONBLOCK);

	int clientsockfd[5];
	struct client_Id clientid[5]; 
	int numclient=0;

	for(i=0; i<5; i++)
		clientsockfd[i]=-1;
	

	while(1)
	{
		sleep(1);

		while( numclient < 5)
		{
			if( (newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len)) < 0 )
			{
				if(errno == EAGAIN || errno == EWOULDBLOCK)
				{
					break;
				}
			}
			else
			{
				//printf("new connection\n");
				for(i=0; i<5; i++)
				{
					if(clientsockfd[i] ==-1)
					{
						clientsockfd[i] = newsockfd;
						strcpy( clientid[i].ip ,  inet_ntoa(cliaddr.sin_addr));
						clientid[i].port = cliaddr.sin_port;
						printf("Received new connection from client < %s :  %d>\n", clientid[i].ip, clientid[i].port);
						numclient++;
						break;
					}
				}
			}
		}
			
		

		for(i=0 ; i<5; )
		{
			if(clientsockfd[i]!=-1)
			{
				k = recv(clientsockfd[i], buf, 10, MSG_DONTWAIT);
				if(k<0)
				{
					if(errno == EWOULDBLOCK || errno== EAGAIN)
					{
						i++;
					}
				}
				else if(k>0)
				{
					printf("Server: Received message %s from client <%s:%d>\n", buf, clientid[i].ip, clientid[i].port);
					if(numclient ==1)
					{
						printf("Server: Insufficient clients, %s from client <%d:%d>\n",buf, clientid[i].ip, clientid[i].port );
					}
					else
					{
						for(j=0; j<5; j++)
						{
							if(clientsockfd[j]!=-1 && j!=i)
							{
								send(clientsockfd[j], clientid[i].ip , sizeof(int) , 0);
								send(clientsockfd[j], &clientid[i].port, sizeof(short int), 0);
								send(clientsockfd[j], buf, 10, 0);
								printf("Server: Sent message %s from client <%d:%d> to client <%d:%d>\n", buf, clientid[i].ip, clientid[i].port, clientid[j].ip, clientid[j].port);
							}
						}
					}
				}
				else
				{
					i++;
				}
				
			}
		}

		
		
		
		
	}
		

	
	
}
