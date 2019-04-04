// A Simple Client Implementation with has conversation

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MAXLINE 1024
  
int main() { 
    int sockfd; 
    struct sockaddr_in servaddr; 
  
    // Creating socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(20000); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
      
    int n;
    socklen_t len; 
    char* domain = "www.youtube.com";
	// printf("%lu\n", strlen(domain) );     
    sendto(sockfd, (const char *)domain, strlen(domain), 0, 
			(const struct sockaddr *) &servaddr, sizeof(servaddr)); // SEND domain name
    printf("File name sent from client: %s\n",domain); 

    char buffer[MAXLINE];

    while(1)
    {
    	for (int i = 0; i < MAXLINE; ++i)
    	{
    		buffer[i] = '\0';
    	}
	    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
				( struct sockaddr *) &servaddr, &len); //recieving ip address
	    // printf("%d\n",n );
	    if(n==0)
	    	break;
	    else if(n>0)
		{
			if(strcmp(buffer, "ip sent")==0)
			{
				break;
			}
			printf("IP address: %s\n", buffer);
		}
		else
		{
			perror("recieving error");
		}
	}		

    close(sockfd); 
    return 0; 
} 




	
