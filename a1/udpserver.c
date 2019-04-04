// A Simple UDP Server that sends has a conversation!
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <dirent.h>
  
#define MAXLINE 1024 
  
int main() { 
    int sockfd; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Create socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family    = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(8181); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    printf("\nServer Running....\n");
  
    int n; 
    socklen_t len;
    char buffer[MAXLINE]; 
    
    len = sizeof(cliaddr);
 
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			( struct sockaddr *) &cliaddr, &len);  // recieve file name!
    // printf("first n is %d\n",n );
    buffer[n] = '\0'; 
    printf("recieved file name: %s\n", buffer); 
	int flag=0;
    struct dirent *de;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
 
    // for readdir() 
    while ((de = readdir(dr)) != NULL) 
    {
        //printf("%s %s\n", de->d_name, buffer); 
		if( strcmp(de->d_name, buffer) ==0)
		{
			flag = 1;
		}
	}
  
    closedir(dr); 
    
    char line[MAXLINE];    
    
    if(flag==0)
    {
    	// printf("file not found\n");
        char *str = "NOTFOUND";
        sendto(sockfd, str , strlen(str), 0, 
             (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
    	return 0;
    }
    else
    {
    	printf("file found\n");
    	FILE *fp = fopen(buffer, "r");// buffer - file name
    	fscanf(fp, "%s", line); 
    	
    	// printf("read line from file : %s\n", line);    	
    	sendto(sockfd, line , strlen(line), 0, 
			 (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); // sending hello
        printf("sent: %s",line);

        char temp[MAXLINE];
        char rword[100];
        int read;

        len = sizeof(cliaddr);

        // cliaddr is filled and 
        // len is modified to store the actual size of the address stored there.
        n = recvfrom(sockfd, (char *)temp, MAXLINE, 0, 
                  ( struct sockaddr *) &cliaddr, &len);
        printf("recieved: %s\n",temp );

        // printf("n is %d\n", n);
        // write recieving word1.. till client sends  ... recieves end
        while(n>0) // recvfrom =0 connection is closed
        {
            
            read = fscanf(fp, "%s", rword);
            // printf("sending: %s\n", glline);

            sendto(sockfd, (char*)rword , strlen(rword), 0, 
                 (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); //while it's recieving  

            printf("sent: %s\n", rword);          

            if(strcmp(rword, "END")!=0)
            {
                n = recvfrom(sockfd, (char *)temp, MAXLINE, 0, 
                  ( struct sockaddr *) &cliaddr, &len);
                printf("recieved: %s\n\n", temp);
            }
            else
            { // when word read is END, break cause the client is not gonna reply anymore
                break;
            }

        }
        fclose(fp);    	
    }

    close(sockfd); 
      
    return 0; 
} 
