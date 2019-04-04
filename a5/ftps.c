/* THE SERVER PROCESS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <errno.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

extern int errno;

int main()
{
	int	sockfd, newsockfd ; 
	int	clilen;
	struct sockaddr_in	cli_addr, serv_addr;

	int i,flag, n ;
	char buf[100],filename[100];		

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}

	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(20000);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 1); 
	while (1) 
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
					(socklen_t *)&clilen) ;

		if (newsockfd < 0) {
			perror("socket Accept error\n");
			exit(0);
		}
		printf("\nClient connected\n");

		for(i=0; i < 100; i++) buf[i] = '\0';
			
		recv(newsockfd, buf, 100, 0);
		printf("File name: %s\n", buf);
		strcpy(filename, buf);

		int fp = open(filename, O_RDONLY);

		if(fp==-1)
		{
			printf("File not found\n");
			strcpy(buf, "E");
			send(newsockfd, buf, strlen(buf), 0);   
		}
		else
		{
			int fsize=0, b=20,j;
			n=0;
			while(1)
			{
				n = read(fp, buf, 100);
				if(n>0)
					fsize +=n;
				else if(n==0)
					break;
				else
					perror("file read error\n");
			}
			int no_of_blocks = fsize/b;
			int last_block_size = fsize%b;
			j = no_of_blocks;
			close(fp);
			
			fp = open(filename,O_RDONLY);
			strcpy(buf, "L");
			send(newsockfd, buf, strlen(buf), 0);
			send(newsockfd, &fsize, sizeof(fsize),0);
			printf("Sending\n");
			while(j--)
			{
				for(i=0; i < 100; i++) buf[i] = '\0';
				n = read(fp, buf, b);
				if(n>0)
				{
					printf("\nBLOCK(%d size):\n%s\n",b, buf );
					send(newsockfd, buf, b, 0); // remove null
				}
				else if(n==0)
				{
					printf("error\n");
					break;
				}
				else
					printf("error\n");
			}
			if(last_block_size!=0)
			{
				n = read(fp, buf, last_block_size);
				printf("\nBLOCK(%d size):\n%s\n\n",last_block_size, buf );
				send(newsockfd, buf, last_block_size, 0);
			}

		}
		printf("File transfer successful\n");
		close(fp);
		close(newsockfd);
		printf("\nClient disconnected\n");
	}

	close(sockfd);
}

