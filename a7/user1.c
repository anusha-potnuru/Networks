#include "rsocket.h"

#define MAXLINE 150

#define PORT1 53027
#define PORT2 53028

int main() 
{ 
	int sockfd; 
	struct sockaddr_in servaddr ,cliaddr; 
  
	// Creating socket file descriptor 
	sockfd = r_socket(AF_INET, SOCK_MRP, 0);
	
	if ( sockfd < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
  
	memset(&servaddr, 0, sizeof(servaddr)); 
	  
	// Server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT1); 
	servaddr.sin_addr.s_addr = INADDR_ANY;

	cliaddr.sin_family = AF_INET; 
	cliaddr.sin_port = htons(PORT2); 
	cliaddr.sin_addr.s_addr = INADDR_ANY; 

	if ( r_bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
	  
	int i, k;
	char buffer[100];
	printf("Give input: \n");
	scanf("%[^\n]%*c", buffer);
	printf("%s\n",buffer);
	for(i=0; i<strlen(buffer) ; i++)
	{
		k = r_sendto(sockfd, (const char *)&buffer[i], 1, 0, 
				(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
		// printf("sendto return value in user1: %d\n",k );
		// printf("sent from client: %c\n", buffer[i]); 
	}
		   
	r_close(sockfd); 
	return 0; 
} 
