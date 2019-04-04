/* Server program  */

// setsockopt call - for address inuse error, port can reused, set before it

// bug - server runs before, put sleep
// bug: last packet left in n/w and server closed, put sleep in client

#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <strings.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

#include <fcntl.h>
#include <sys/stat.h>

#include <netdb.h>
extern int h_errno;

// #define PORTX 50000 
#define MAXLINE 80
#define BLOCKSIZE 100


int main() 
{ 
	int PORTY;
	int listenfd, i, newsockfd, datasockfd, flag=0; 
	int clilen;
	char buf[MAXLINE]; 
	pid_t childpid; 
	fd_set rset; 
	ssize_t n; 
	socklen_t len; 
	const int on = 1; 
	struct sockaddr_in cliaddr, servaddr; 
	char* message = "Hello Client";

	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	int k=10; 
  
	/* create listening TCP socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0); 


	int enable = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");


	bzero(&servaddr, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(50000); 
	
	// binding server addr structure to listenfd 
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	listen(listenfd, 1); 

  
	while (1) 
	{
		int start =0;
		clilen = sizeof(cliaddr);
		newsockfd = accept(listenfd, (struct sockaddr *) &cliaddr,
					(socklen_t*)&clilen) ;
		printf("Client connected\n");

		if (newsockfd < 0) 
		{
			printf("Accept error\n");
			exit(0);
		} 

		while(1)
		{
			printf("< ");			
			int t = recv(newsockfd, buf, MAXLINE, 0);
			if(t==0)
				break; // break connection 

			// printf("size: %d\n", t );
			printf("%s\n", buf);

			char* token;
			token = strtok(buf, " ");
			if(strcmp(token, "port")==0)
			{
				if(start==0)
				{
					token = strtok(NULL, " ");
					if(token!= NULL)
					{
						int y = atoi(token);
						if(1024<= y && y<= 65535)
						{
							PORTY = y;
							int tosend = htonl(200);
							send(newsockfd, &tosend, sizeof(tosend),0 );
						}
						else
						{
							int tosend = htonl(550);
							send(newsockfd, &tosend, sizeof(tosend),0 );
							break;
						}
					}
					else
					{
						int tosend = htonl(501); // no argument for port command
						send(newsockfd, &tosend, sizeof(tosend),0 );
						break;
					}	
					start=1;
				}
				else
				{
					printf("port already set\n");
					int tosend = htonl(503);
					send(newsockfd, &tosend, sizeof(tosend),0 );
					// break; // breaks connection
				}
			}
			else if(strcmp(token, "cd")==0)
			{
				token = strtok(NULL, "\0");
				if(chdir(token)!=0)
				{
					perror("cd not successful");
					int tosend = htonl(501);
					send(newsockfd, &tosend, sizeof(tosend),0 );
				}
				else
				{
					printf("cd successful\n");
					int tosend = htonl(200);
					send(newsockfd, &tosend, sizeof(tosend),0 );
				}
			}
			else if(strcmp(token, "get")==0 || strcmp(token, "put") ==0 )
			{
				int childpid, fntexist=0, gp;

				if(strcmp(token, "get")==0)
					gp=1;
				else
					gp =0;

				token = strtok(NULL, "\n");
				if(token==NULL)
				{
					int tosend = htonl(501);
					send(newsockfd, &tosend, sizeof(tosend),0 ); 
					continue;
				}
				if(gp)
				{
					int fd = open(token, O_RDONLY);
					int fd1 = open(token, O_RDONLY);
					if(fd<0)
					{
						perror("file open error");
						int tosend = htonl(550);
						send(newsockfd, &tosend, sizeof(tosend),0 );
						continue;                   
					}
					close(fd);
					close(fd1);
				}
				else
				{ // put
					int fd = open(token, O_WRONLY| O_CREAT|O_TRUNC, S_IRWXU);
					if(fd<0)
					{
						perror("file create error");
						int tosend = htonl(550);
						send(newsockfd, &tosend, sizeof(tosend),0 );
						continue;
					}
					close(fd);

				}

				if((childpid=fork())==0)
				{
					// close(newsockfd);

					int  sockfd ,n, received_code;
					struct sockaddr_in  serv_addr;                        

					if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
					{
						perror("Unable to create socket\n");
						exit(0);
					}
					serv_addr.sin_family    = AF_INET;

					inet_aton("127.0.0.1", &serv_addr.sin_addr);
					serv_addr.sin_port  = htons(PORTY);

					sleep(1); // gives time for client to create it's data and wait

					if ((connect(sockfd, (struct sockaddr *) &serv_addr,
										sizeof(serv_addr))) < 0) 
					{
						perror("Unable to connect to server\n");
						exit(0);
					}
					if(gp)
					{  // if get
						int fd = open(token, O_RDONLY);
						int fd1 = open(token, O_RDONLY);
						short int k;
						char temp[BLOCKSIZE], temp1[BLOCKSIZE];
						char c = 'M';
						k = read(fd1, temp1, BLOCKSIZE);
						while(1)
						{
							bzero(temp, sizeof(temp));
							short int t = read(fd, temp, BLOCKSIZE);
							k = read(fd1, temp1, BLOCKSIZE);
							if(k==0)
							{
								c='L';
							}
							if(t==0)
							{
								break;
							}
							else if(t>0)
							{
								printf("%c %hd %s\n",c,t,temp );
								send(sockfd, &c, sizeof(c), 0);
								send(sockfd, &t, sizeof(t), 0);
								send(sockfd, temp, t, 0);
							}
							else
							{
								perror("file read error\n");
								exit(0);   
							}
						}
						close(fd);
						close(fd1); // close in all processes
						// fd=-1;
						// fd1=-1;

					}
					else
					{// put case
						int fd = open(token, O_WRONLY| O_CREAT|O_TRUNC, S_IRWXU);
						char temp[100];
						short int t, len;
						char c;
						int index=0;
						int v1,v2;
						while(1)
						{
							v1 = recv(sockfd, &c, 1, 0);
							if(v1==0)
								break;
							v2 = recv(sockfd, &len, sizeof(len), 0);
							while(1)
							{
								if(len>BLOCKSIZE)
								{
									t = recv(sockfd, temp, BLOCKSIZE, 0);
									if(t>0)
									{
										write(fd, temp, t);
										len = len-t;
									}
									else
										perror("receive error");

								}
								else
								{
									t = recv(sockfd, temp, len , 0);
									if(t==len)
									{
										write(fd, temp, t);
										break;
									}
									else if(t<len)
									{
										len=len-t;
									}
									else
										perror("receive error");

								}
								for (i = 0; i < BLOCKSIZE; ++i)
								{
									temp[i]='\0';
								}
							}
						}

						close(fd);
						// fd=-1;

					}

					close(sockfd);
					
					exit(2);
				}
				
				else
				{
					int status, tosend=0;
					waitpid(childpid, &status, 0);
					// if(fd!=-1)
					// 	close(fd);
					// if(gp)
					// {
					// 	if(fd1!=-1)
					// 		close(fd1);
					// }
					
					if(WIFEXITED(status))
					{
						if( WEXITSTATUS(status) ==2)
						{
							tosend = htonl(250);
							printf("data transfer successful\n");       
						}
						else if( WEXITSTATUS(status) ==0 )
						{
							printf("Error during transmission\n");
							tosend = htonl(550); 
						}						
					}
					else
					{
						tosend = htonl(550);
						printf("Error during transmission\n");
					}
					send(newsockfd, &tosend, sizeof(tosend),0 );
					// printf("sent: %d\n", ntohl(tosend));

				}

			}
			else if(strcmp(token, "quit")==0)
			{
				int tosend = htonl(421);
				send(newsockfd, &tosend, sizeof(tosend),0 );
				flag=1;
				break;
			}
			else
			{  // invalid command
				int tosend = htonl(502);
				send(newsockfd, &tosend, sizeof(tosend),0 );
			}
			
			for(i=0; i < MAXLINE; i++)
			{
				buf[i] = '\0';
			}
			// printf("waiting for next command\n");

		}

		chdir(cwd);
		close(newsockfd);
		printf("Client disconnected\n");
			
	}

	close(listenfd);

}
