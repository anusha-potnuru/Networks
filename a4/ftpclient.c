
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

// #define PORTX 50000
#define MAXLINE 80

int main()
{
	int			sockfd ,n, received_code;
	struct sockaddr_in	serv_addr;

	int i;
	char buf[100];
	char command[MAXLINE];

	int PORTY = 55000;

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
		printf(">");
		fgets(command, MAXLINE, stdin);
        command[strlen(command)-1] = '\0';
        if(strcmp(command, "quit")==0)
        	break;
        printf("\n");
        if(strlen(command)==0)
            continue;
        // printf("in client: %s\n", command );
        int k = send(sockfd, command, strlen(command) + 1, 0);
        char* token = strtok(command, " ");
        if(strcmp(token, "get")==0)
        {
        	int childpid;
        	if((childpid=fork())==0)
        	{

        		int listenfd, i, newsockfd; 
			    int clilen;
			    char buf[MAXLINE]; 
			    ssize_t n; 
			    socklen_t len; 
			    struct sockaddr_in cliaddr, servaddr; 

				listenfd = socket(AF_INET, SOCK_STREAM, 0); 
			    bzero(&servaddr, sizeof(servaddr)); 

			    servaddr.sin_family = AF_INET; 
			    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
			    servaddr.sin_port = htons(PORTY); 
			  
			    // binding server addr structure to listenfd 
			    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
			    listen(listenfd, 1);    

			    clilen = sizeof(cliaddr);
		        newsockfd = accept(listenfd, (struct sockaddr *) &cliaddr,
		                    (socklen_t*)&clilen) ;

		        if (newsockfd < 0) 
		        {
		            printf("Accept error\n");
		            exit(0);
		        } 

		        int t = recv(newsockfd, buf, 100, 0);
		        if(t==0)
		        	break;

		        printf("buf: %s\n", buf );

		        close(newsockfd);
	
			    close(listenfd);

			    exit(0);
			    printf("child exit here\n");

        	}
        	else
        	{
        		
        		int status;
        		waitpid(childpid, &status, 0);
        		// printf("child ended\n");
        		// printf("%d\n", status);
        		// if (WIFEXITED(status)) 
        		// {
        		// 	printf("exited with status: %d\n", WEXITSTATUS(stat));
        			
        		// }
        		// else
        		// {
        		// 	printf("error\n");
        		// }
        		
        	}

        }

        int n = recv(sockfd, &received_code, sizeof(received_code), 0);
        printf("%d\n",ntohl(received_code));

		
	}

	close(sockfd);

}
