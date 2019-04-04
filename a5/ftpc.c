
/*    THE CLIENT PROCESS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
	int	sockfd ;
	struct sockaddr_in	serv_addr;

	int i,n;
	int k= 50; //chunk size
	char buf[k], temp[k];


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}


	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(20000);


	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}


	printf("Enter file name:\n");
	scanf("%s", buf);
	printf("Given file name is : %s\n\n", buf);

	send(sockfd, buf, strlen(buf) + 1, 0);

	int x, fsize, b=20;
	for (int i = 0; i < k; ++i) buf[i] = '\0';

	x = recv(sockfd,buf, 1, 0);
	if(strcmp(buf, "E")==0)
	{
		printf("Error: File not found\n");
	}
	else if(strcmp(buf, "L")==0)
	{
		int fd  = open("clientfile.txt", O_TRUNC|O_CREAT|O_WRONLY, S_IRWXU); // |O_CREAT, 0640
		if(fd==-1)
		{
			perror("file create error");
		}
		x = recv(sockfd, &fsize, sizeof(fsize), MSG_WAITALL);
	
		int no_of_blocks=fsize/b;
		int j=0;
		while(j<no_of_blocks)
		{		
			for (int i = 0; i < k; ++i) buf[i] = '\0';
			recv(sockfd, buf, b, MSG_WAITALL);
			write(fd, buf, b);
			j++;
		}
		if(fsize%b!=0)
		{
			recv(sockfd, buf, fsize%b, MSG_WAITALL);
			write(fd, buf, fsize%b);
			no_of_blocks++;
		}
		close(fd);
		printf("The file transfer is successful. Total number of blocks = %d, Last block size = %d\n", no_of_blocks, fsize%b);
	}
	
	close(sockfd);

}

