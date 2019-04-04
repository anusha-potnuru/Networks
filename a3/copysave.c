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

#define PORT 20000 
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
    int listenfd, connfd, udpfd, nready, maxfdp1; 
    char buffer[MAXLINE]; 
    pid_t childpid; 
    fd_set rset; 
    ssize_t n; 
    socklen_t len; 
    const int on = 1; 
    struct sockaddr_in cliaddr, servaddr; 
    char* message = "Hello Client"; 
    void sig_chld(int); 
  
    /* create listening TCP socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    bzero(&servaddr, sizeof(servaddr)); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // binding server addr structure to listenfd 
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
    listen(listenfd, 2); 
  
    /* create UDP socket */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0); 

    // if ( udpfd < 0 ) { 
    //     perror("socket creation failed"); 
    //     exit(EXIT_FAILURE); 
    // }

    // binding server addr structure to udp sockfd 
    bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
  
    // clear the descriptor set 
    FD_ZERO(&rset); 
  
    // get maxfd 
    maxfdp1 = max(listenfd, udpfd) + 1; 
    for (;;) 
    { 
        FD_ZERO(&rset); 
        // set listenfd and udpfd in readset 
        FD_SET(listenfd, &rset); 
        FD_SET(udpfd, &rset); 
  
        // select the ready descriptor 
        nready = select(maxfdp1, &rset, 0, 0, 0); 
  
        // if tcp socket is readable then handle 
        // it by accepting the connection 
        if (FD_ISSET(listenfd, &rset)) 
        { 
            len = sizeof(cliaddr); 
            connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len); 
            if ((childpid = fork()) == 0) 
            { 
                int i;
                close(listenfd); 
                for (i = 0; i < MAXLINE; ++i) buffer[i] = '\0';
                recv(connfd, buffer, sizeof(buffer)+1,0); 
                if(strcmp(buffer, "Request for words")==0)
                {
                    int fd = open("word.txt", O_RDONLY);
                    if(fd==-1)
                    {
                        perror("file open error");
                        exit(1);
                    }
                    int done=0,prev,index,x;
                    char buf[100], temp[200];
                    prev = -1;
                    index=0;
                    while(1)
                    {
                        for(i=0; i < 100; i++) buf[i] = '\0';
                        x = read(fd, buf, 100);
                        printf("x: %d\n",x );
                        // prev = -1;
                        // index =0;
                        if(x==0)
                        {
                            break;
                        }
                        else if(x>0)
                        {
                            printf("BUFFER IS: (%d size)\n%s\n", x, buf);
                            for (int i = 0; i < 100; ++i)
                            {
                                if(buf[i] == '\n' || (i<99 && buf[i]=='\0' && buf[i+1]=='\0'))
                                {
                                    strncpy(temp+index, buf+prev+1, i-prev-1);
                                    temp[index+i-prev-1] = '\0';
                                    printf("TEMP: %s\n",temp );
                                    int j = send(connfd, temp, i-prev, 0);
                                    printf("j is %d\n",j);

                                    temp[0] = 0; //empty
                                    if(j==-1)
                                        perror("error sending");
                                    prev = i;
                                    if(i<99 && buf[i]=='\0' && buf[i+1]=='\0')
                                    {
                                        if(buf[i]!= '\n')
                                        {
                                            int j = send(connfd, "\0", 1, 0);
                                            printf("j is %d\n",j);   
                                        }
                                        break;
                                    }
                                }
                            }

                            if(prev< x && done!=1)
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

                    }
                    // int j = send(connfd, "\0", 1, 0);
                    // printf("j is %d\n",j);

                    // printf("Message From TCP client: "); 
                    // recv(connfd, buffer, sizeof(buffer)+1,0); 
                    // printf("%s\n",buffer );
                    // send(connfd, (const char*)message, strlen(message)+1,0); 
            
                }

                close(connfd); 
                exit(0); 
            }
            close(connfd); 
            
        } 
        // if udp socket is readable receive the message. 
        if (FD_ISSET(udpfd, &rset)) 
        {
            if(fork()==0)
            { 
                len = sizeof(cliaddr); 
                bzero(buffer, sizeof(buffer)); 
                printf("\nMessage from UDP client: "); 
                n = recvfrom(udpfd, buffer, sizeof(buffer), 0, 
                             (struct sockaddr*)&cliaddr, &len); 
                puts(buffer); 
                char ip[100];
                struct hostent* x;
                x = gethostbyname(buffer);
                if(x)
                {
                    for (int i = 0; x->h_addr_list[i]!=NULL ; ++i)
                    {
                        printf("%s\n",  inet_ntoa(*((struct in_addr*)x->h_addr_list[i])));
                        strcpy(ip, inet_ntoa(*((struct in_addr*)x->h_addr_list[i])));
                        int j = sendto(udpfd, (const char*)ip, strlen(ip)+1, 0, 
                                    (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
                    }
                }
                else
                {
                    printf("%d\n", h_errno);
                    herror("get host error");
                }
            }                        
        } 

    } 
} 