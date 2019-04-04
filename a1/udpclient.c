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
    servaddr.sin_port = htons(8181); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
      
    int n;
    socklen_t len; 
    char *hello = "file.txt"; 
     
    sendto(sockfd, (const char *)hello, strlen(hello), 0, 
			(const struct sockaddr *) &servaddr, sizeof(servaddr)); // SEND file name
    printf("File name sent from client: %s\n",hello); 

    char buffer[MAXLINE];

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			( struct sockaddr *) &servaddr, &len); //recieving hello
	printf("recieved: %s\n", buffer);		
	buffer[n] = '\0'; 
    int i=1;
        
    if(strcmp(buffer, "HELLO") == 0 )
    {
    	FILE *cfp = fopen("clientfile.txt", "w");
    	// fprintf(cfp, "%s" ,buffer); 

        char *word = "WORD";      
        char temp[MAXLINE];
        char recdword[MAXLINE]; 
        char snum[5];

        while(1)
        {
            sprintf(snum, "%d", i);
            strcpy(temp, word);
            strcat(temp, snum);
            // printf("sending: %s\n",temp );

            //sending wordi request
            sendto(sockfd, (const char *)(temp), strlen(temp), 0, 
                (const struct sockaddr *) &servaddr, sizeof(servaddr));
            printf("sent: %s\n",temp );

            len = sizeof(servaddr);
            n = recvfrom(sockfd, (char *)recdword, MAXLINE, 0, 
                    ( struct sockaddr *) &servaddr, &len); // recieving wordi requested  
            recdword[n] = '\0'; 

            // keep sending requests until it recieves END
            printf("recieved : %s\n\n", recdword );
            if(strcmp(recdword, "END") != 0)
                fprintf(cfp, "%s\n" ,recdword);
            else
                break; 
            i++;
        }
        
        fclose(cfp);        
    	
    }
    else if(strcmp(buffer, "FILENOTFOUND") == 0)
    {
        printf("File not found\n");
    }
           
    close(sockfd); 
    return 0; 
} 
