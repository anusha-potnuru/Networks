
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
        printf("\n");
        if(strlen(command)==0)
            continue;
        printf("in client: %s\n", command );
        int k = send(sockfd, command, strlen(command) + 1, 0);
        printf("k is %d\n", k);
        int n = recv(sockfd, &received_code, sizeof(received_code), 0);
        printf("%d\n",ntohs(received_code));

		
	}

	close(sockfd);

}
