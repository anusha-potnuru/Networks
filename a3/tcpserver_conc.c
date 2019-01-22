
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
			/* THE SERVER PROCESS */

int main()
{
	int			sockfd, newsockfd ; /* Socket descriptors */
	int			clilen;
	struct sockaddr_in	cli_addr, serv_addr;

	int i,x,prev, index;
	int k = 100;
	char buf[100], temp[200];		/* We will use this buffer for communication */

	/* The following system call opens a socket. The first parameter
	   indicates the family of the protocol to be followed. For internet
	   protocols we use AF_INET. For TCP sockets the second parameter
	   is SOCK_STREAM. The third parameter is set to 0 for user
	   applications.
	*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Cannot create socket\n");
		exit(0);
	}

	/* The structure "sockaddr_in" is defined in <netinet/in.h> for the
	   internet family of protocols. This has three main fields. The
 	   field "sin_family" specifies the family and is therefore AF_INET
	   for the internet family. The field "sin_addr" specifies the
	   internet address of the server. This field is set to INADDR_ANY
	   for machines having a single IP address. The field "sin_port"
	   specifies the port number of the server.
	*/
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(20000);

	/* With the information provided in serv_addr, we associate the server
	   with its port using the bind() system call. 
	*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		printf("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 1); /* This specifies that up to 5 concurrent client
			      requests will be queued up while the system is
			      executing the "accept" system call below.
			   */

	/* In this program we are illustrating a concurrent server -- one
	   which forks to accept multiple client connections concurrently.
	   As soon as the server accepts a connection from a client, it
	   forks a child which communicates with the client, while the
	   parent becomes free to accept a new connection. To facilitate
	   this, the accept() system call returns a new socket descriptor
	   which can be used by the child. The parent continues with the
	   original socket descriptor.
	*/
	while (1) 
	{

		/* The accept() system call accepts a client connection.
		   It blocks the server until a client request comes.

		   The accept() system call fills up the client's details
		   in a struct sockaddr which is passed as a parameter.
		   The length of the structure is noted in clilen. Note
		   that the new socket descriptor returned by the accept()
		   system call is stored in "newsockfd".
		*/
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
					(socklen_t *)&clilen) ;

		if (newsockfd < 0) 
		{
			printf("Accept error\n");
			exit(0);
		}

		/* Having successfully accepted a client connection, the
		   server now forks. The parent closes the new socket
		   descriptor and loops back to accept the next connection.
		*/
		if (fork() == 0) 
		{

			/* This child process will now communicate with the
			   client through the send() and recv() system calls.
			*/

			// char s[] = {'\0', '\n'};
			close(sockfd);

			for(i=0; i < 100; i++) buf[i] = '\0';
			recv(newsockfd, buf, 100, 0);
			printf("%s\n", buf);

			if(strcmp(buf, "Request for words")==0)
			{
				int fd = open("file.txt", O_RDONLY);
				if(fd==-1)
				{
					perror("file open error");
					exit(1);
				}
				int done=0;
				while(1)
				{
					for(i=0; i < 100; i++) buf[i] = '\0';
					x = read(fd, buf, 100);
					prev = -1;
					index =0;
					if(x==0)
					{
						break;
					}
					else if(x>0)
					{
						printf("BUFFER IS: (%d size)\n%s\n", x, buf);
						for (int i = 0; i < 100; ++i)
						{
							if(buf[i] == '\n')
							{
								strncpy(temp+index, buf+prev+1, i-prev-1 );
								temp[i-prev-1] = '\0';
								printf("TEMP:\n%s\n",temp );
								int j = send(newsockfd, temp, i-prev, 0);
								printf("j is %d\n\n",j);
								temp[0] = 0; //empty
								if(j==-1)
									perror("error sending");
								prev = i;
							}
							if(buf[i] == '\0')
								done=1;
						}
						if(prev<99 && done!=1)
						{//prev+1, 99
							strncpy(temp, buf+prev+1, 99-prev);
							index = 99-prev;
						}
					}
					else
					{
						perror("file read error");
						exit(1);
					}
					if(done==1)
					{
						// int j = send(newsockfd, "\0", 1, 0);
						// printf("j is %d\n",j);
						break;
					}

				}

			}
			printf("END\n");
			close(newsockfd);
			exit(0);
		}

		close(newsockfd);
	}
}
			
