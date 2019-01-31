// Server program 
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
#define MAXLINE 1024 

int max(int x, int y) 
{ 
    if (x > y) 
        return x; 
    else
        return y; 
} 

int main() 
{ 
    int PORTY, start=0;
    int listenfd, i, newsockfd; 
    int clilen;
    char buf[MAXLINE]; 
    pid_t childpid; 
    fd_set rset; 
    ssize_t n; 
    socklen_t len; 
    const int on = 1; 
    struct sockaddr_in cliaddr, servaddr; 
    char* message = "Hello Client"; 

    int k=10; 
  
    /* create listening TCP socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    bzero(&servaddr, sizeof(servaddr)); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(50000); 
  
    // binding server addr structure to listenfd 
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
    listen(listenfd, 2); 

  
    while (1) 
    {

        clilen = sizeof(cliaddr);
        newsockfd = accept(listenfd, (struct sockaddr *) &cliaddr,
                    (socklen_t*)&clilen) ;

        if (newsockfd < 0) {
            printf("Accept error\n");
            exit(0);
        } 

        while(1)
        {     
            // printf("receive\n");
            for(i=0; i < 100; i++) buf[i] = '\0';
            int index =0;
            int t = recv(newsockfd, buf+index, 100, 0);
            printf("t is %d\n", t );
            if(t==0)
                break;
            // while(1)
            // {
            //     int t = recv(newsockfd, buf+index, 100, 0);
            //     if(t==0)
            //         break;
            //     index = index + t;
            // }
            
            printf("%s\n", buf);

            char* token;
            token = strtok(buf, " ");
            while(token!=NULL)
            {
                if(strcmp(token, "PORT")==0)
                {
                    if(start==1)
                    {
                        int tosend = htons(503);
                        send(newsockfd, &tosend, sizeof(tosend),0 );
                        break;
                    }
                    if(start==0)
                        start=1;
                    token = strtok(NULL, " ");
                    int x = atoi(token);
                    if(1024<= x && x<= 65535)
                    {
                        PORTY = x;
                        int tosend = htons(200);
                        send(newsockfd, &tosend, sizeof(tosend),0 );
                    }
                    else
                    {
                        int tosend = htons(550);
                        send(newsockfd, &tosend, sizeof(tosend),0 );
                    }
                }

                if(strcmp(token, "get")==0)
                {
                    char temp[100];
                    strcpy(temp, "I have received it");
                    send(newsockfd, temp, 100, 0);
                }
                token = strtok(NULL, " ");            

            }
        }

        close(newsockfd);
            
    }
    
    close(listenfd);

} 