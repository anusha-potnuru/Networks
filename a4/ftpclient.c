
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
#include <signal.h>

// #define PORTX 50000
#define MAXLINE 80

int main()
{
	int			sockfd ,n, received_code;
	struct sockaddr_in	serv_addr;

	int i, start=0;
	char buf[100];
	char command[MAXLINE];

	int PORTY;

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("Unable to create socket\n");
		exit(0);
	}

	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(50000);


	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	while(1)
	{
		printf("> ");
		fgets(command, MAXLINE, stdin);
        command[strlen(command)-1] = '\0';

        
        if(strlen(command)==0)
            continue;
        // printf("\n");
        // printf("in client: %s\n", command );
        int k = send(sockfd, command, strlen(command) + 1, 0);
        char* token = strtok(command, " ");

        printf("token: %s\n", token );
        if(strcmp(token, "port")==0)
        {
        	if(start==0)
        	{
        		token = strtok(NULL, " ");
        		if(token != NULL)
        		{
        			PORTY = atoi(token);
        			printf("port set to %d\n",PORTY );
        		}
        		else
        			printf("error\n");
        		start =1;
        	}
        	else
        	{
        		printf("port already set\n");
        	}

        }

        if(strcmp(token, "get")==0 || strcmp(token, "put")==0)
        {
        	int childpid, gp;
        	if(strcmp(token, "get")==0)
        		gp =1;
        	else
        		gp =0;

        	token = strtok(NULL, "\n");

        	if((childpid=fork())==0)
        	{

        		int listenfd, i, newsockfd; 
			    int clilen;
			    char buf[MAXLINE]; 
			    ssize_t n; 
			    socklen_t len; 
			    struct sockaddr_in cliaddr, servaddr; 

				listenfd = socket(AF_INET, SOCK_STREAM, 0); 

				int enable = 1;
				if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
				    perror("setsockopt(SO_REUSEADDR) failed");

			    bzero(&servaddr, sizeof(servaddr)); 

			    servaddr.sin_family = AF_INET; 
			    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
			    servaddr.sin_port = htons(PORTY); 
			  
			    // binding server addr structure to listenfd 
			    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
			    if(listen(listenfd, 1)<0)
			    	perror("error on listen");    

			    clilen = sizeof(cliaddr);
		        newsockfd = accept(listenfd, (struct sockaddr *) &cliaddr,
		                    (socklen_t*)&clilen) ;

		        if ( newsockfd < 0) 
		        {
		            printf("Accept error\n");
		            exit(0);
		        } 

		        if(gp)
				{
			        int fd = open(token, O_WRONLY| O_CREAT|O_TRUNC, S_IRWXU);
			        while(1)
			        {
			        	int t = recv(newsockfd, buf, 100, 0);
				        if(t==0)
				        	break;
				        else if(t>0)
				        {
				        	write(fd, buf, t);
				        	printf("buf: %s\n", buf );

				        }
				        else
				        	perror("receive error");
				        for(i=0; i<100; i++)
				        	buf[i] = '\0';

			        }	
		        }
		        else
		        {
		        	char temp[100];
                    int fd= open(token, O_RDONLY);
                    if(fd==-1)
                    {
                        perror("file open error");
                        exit(0);
                    }
                    else
                    {
                        char temp[100];
                        while(1)
                        {
                            int t = read(fd, temp, 100);
                            if(t==0)
                            {
                                break;
                            }
                            else if(t>0)
                                send(newsockfd, temp, t, 0);
                            else
                                perror("file read error\n");
                            bzero(&temp, sizeof(temp));                        

                        }
                    }
                    close(fd);

		        }

		        close(newsockfd);
	
			    close(listenfd);
			    printf("child exit here\n");
			    exit(0);			    

        	}
        	else
        	{        		
        		int status;
        		//waitpid(childpid, &status, 0);    
        		int n = recv(sockfd, &received_code, sizeof(received_code), 0);
		        printf("%d\n",ntohl(received_code));
		        if(ntohl(received_code) == 550)
		        	kill(childpid, SIGKILL);
        	}

        }
        else
        {
	        int n = recv(sockfd, &received_code, sizeof(received_code), 0);
	        printf("%d\n",ntohl(received_code));
	        if(ntohl(received_code) == 421)
	        	break;
    	}
		
	}

	close(sockfd);

}
