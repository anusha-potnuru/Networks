
/*    THE CLIENT PROCESS */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
	int			sockfd ,n;
	struct sockaddr_in	serv_addr;

	int i;
	char buf[100];

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}

	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(20000);

	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/
	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	/* After connection, the client can send or receive messages.
	   However, please note that recv() will block when the
	   server is not sending and vice versa. Similarly send() will
	   block when the server is not receiving and vice versa. For
	   non-blocking modes, refer to the online man pages.
	*/
	
	for(i=0; i < 100; i++) buf[i] = '\0';
	
	strcpy(buf,"Request for words");
	send(sockfd, buf, strlen(buf) + 1, 0);
	int count=0, bytecount=0;

	while(1)
	{
		for(i=0; i < 100; i++) buf[i] = '\0';
		n = recv(sockfd, buf, 100, 0); //error due to not doing +1
		bytecount = bytecount+n;
		if(n==0)
			break;
		else if(n>0)
		{
			printf("bytes recieved: %d\n",n);
			for (int i = 0; i < n; ++i)
			{
				if(buf[i] == '\0' )
				{// && (i<n-1? buf[i+1]!='\0':0 || i>0? buf[i-1]!='\0':0)
					// printf("index: %d\n",i );
					count++;
				}
			}

			// printf("%s, %c\n\n",buf, buf[n-1]);
			for (int i = 0; i < n; ++i)
			{
				/* code */
				// if(buf[i]=='\0')
				// 	printf("N");
				// else
					printf("%c", buf[i]);
			}
			printf("\n");
		}
		else
		{
			perror("line recieve error");
		}
	}
	
	printf("total bytes recieved: %d\n\n", bytecount);
	printf("WORD COUNT: %d\n",count-1);
	close(sockfd);
}
