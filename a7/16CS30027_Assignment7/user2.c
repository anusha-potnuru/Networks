#include "rsocket.h" 

#define MAXLINE 100

#define PORT1 53027
#define PORT2 53028
  
int main() { 
	int sockfd; 
	struct sockaddr_in servaddr, cliaddr; 
  
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

	if(r_bind(sockfd, (const struct sockaddr *)&cliaddr,  
            sizeof(cliaddr)) < 0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

	int n, i=1;
	socklen_t len; 
	char buffer[100];

	while(1)
	{
		len = sizeof(servaddr);
		n = r_recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			  (struct sockaddr *) &servaddr, &len);
		// printf("ret value %d\n", n); 
		buffer[n] = '\0'; 
		printf("character recieved: %s\n", buffer);
		bzero(buffer, sizeof(buffer));
	}
				   
	r_close(sockfd); 
	return 0; 
} 
