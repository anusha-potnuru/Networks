#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <sys/time.h>

int main()
{
	int sockfd, newsockfd, len;
	struct sockaddr_in cliaddr, servaddr;

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("sock creation error");
		exit(0);
	}

	int enable=1;

	if (setsockopt(sockfd,SOL_SOCKET , SO_REUSEADDR, &enable, sizeof(int)) <0)
	{
		printf("setsockopt error\n");
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(8181);

	if(bind(sockfd, (const struct sockaddr*)(&servaddr), sizeof(servaddr) ) <0)
	{
		perror("unable to connect to server");
		exit(0);
	}

	printf("Server running\n");

	fd_set fds;
	int nready, maxfd;
	

	listen(sockfd, 4);

	char buf[100];
	strcpy(buf, "hello");

	while(1)
	{
		FD_ZERO(&fds);	
		FD_SET(sockfd, &fds);
		maxfd = sockfd+1;

		nready = select(maxfd, &fds, NULL, NULL, NULL);

		if(nready < 0)
		{
			perror("error");
		}

		if(FD_ISSET(sockfd, &fds))
		{
			len = sizeof(cliaddr);
			newsockfd =accept(sockfd, (struct sockaddr*)&cliaddr, (socklen_t*)&len);
			printf("new client connection\n");
			send(newsockfd, buf, 100, 0);
			close(newsockfd);
		}

	}


	return 0;
}