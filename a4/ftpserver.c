// Server program 

//check port, quit - not working


// setsockopt call - for address inuse error, port can reused, set before it-- DO

// int part, breakup -- 
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
    listen(listenfd, 2); 

  
    while (1) 
    {
        int start =0;
        clilen = sizeof(cliaddr);
        newsockfd = accept(listenfd, (struct sockaddr *) &cliaddr,
                    (socklen_t*)&clilen) ;

        if (newsockfd < 0) 
        {
            printf("Accept error\n");
            exit(0);
        } 

        while(1)
        {     
            printf("<\n");
            for(i=0; i < 100; i++) buf[i] = '\0';
            int index =0;
            int t = recv(newsockfd, buf+index, 100, 0);
            printf("t is %d\n", t );
            if(t==0)
                break; // parts

            printf("%s\n", buf);

            char* token;
            token = strtok(buf, " ");
            while(token!=NULL)
            {
                if(strcmp(token, "port")==0)
                {
                    if(start==1)
                    {
                        int tosend = htonl(503);
                        send(newsockfd, &tosend, sizeof(tosend),0 );
                        break;
                    }
                    if(start==0)
                        start=1;
                    token = strtok(NULL, " ");
                    if(token!= NULL)
                    {
                        int x = atoi(token);
                        if(1024<= x && x<= 65535)
                        {
                            PORTY = x;
                            int tosend = htonl(200);
                            send(newsockfd, &tosend, sizeof(tosend),0 );
                        }
                        else
                        {
                            int tosend = htonl(550);
                            send(newsockfd, &tosend, sizeof(tosend),0 );
                        }
                    }
                    else
                    {
                        break;
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
                    break;
                }
                else if(strcmp(token, "get")==0 || strcmp(token, "put") ==0 )
                {
                    int childpid, fntexist=0, gp, fd;
                    if(strcmp(token, "get")==0)
                        gp=1;
                    else
                        gp =0;
                    token = strtok(NULL, "\n");
                    if(gp)
                    {
                        fd = open(token, O_RDONLY);
                        if(fd==-1)
                        {
                            perror("file open error");
                            int tosend = htonl(550);
                            send(newsockfd, &tosend, sizeof(tosend),0 );                        
                        }
                    }
                    else
                    {
                        int fd = open(token, O_WRONLY| O_CREAT|O_TRUNC, S_IRWXU);
                        if(fd<0)
                        {
                            perror("file create error");
                            int tosend = htonl(550);
                            send(newsockfd, &tosend, sizeof(tosend),0 ); 

                        }
                    }

                    if(fd!=-1 && (childpid=fork())==0)
                    {
                        close(newsockfd);

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

                        sleep(1);

                        if ((connect(sockfd, (struct sockaddr *) &serv_addr,
                                            sizeof(serv_addr))) < 0) 
                        {
                            perror("Unable to connect to server\n");
                            exit(0);
                        }
                        if(gp)
                        { // if get

                            char temp[100];
                            while(1)
                            {
                                int t = read(fd, temp, 100);
                                if(t==0)
                                {
                                    break;
                                }
                                else if(t>0)
                                    send(sockfd, temp, t, 0);
                                else
                                    perror("file read error\n");                                
                            }

                        }
                        else
                        {
                            while(1)
                            {
                                int t = recv(sockfd, buf, 100, 0);
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

                        close(fd);
                        close(sockfd);

                        exit(2);

                    }
                    else
                    {
                        int status, tosend=0;
                        waitpid(childpid, &status, 0);
                        close(fd);

                        printf("back to parent\n");
                        if(WIFEXITED(status))
                        {
                            if( WEXITSTATUS(status) ==0 )
                                tosend = htonl(250);       
                            else if( WEXITSTATUS(status) ==0 )
                                tosend = htonl(550);                                                        
                        }
                        else
                        {
                            tosend = htonl(550);
                            printf("child not normal\n");
                        }
                        send(newsockfd, &tosend, sizeof(tosend),0 );
                        printf("sent: %d\n", tosend);

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
                {
                    break;
                }

            }

            if(flag==1) // quit case
                break;

        }
        

        close(newsockfd);
            
    }

    close(listenfd);

}
