#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
	char buf[100];
	int sockfd;
	struct sockaddr_in cliaddr, servaddr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		perror("sock error");
		exit(0);
	}

	servaddr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(8181);

	if( connect(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) <0)
	{
		perror("errr");
		exit(0);
	}
	
	recv(sockfd, buf, 100, 0);
	printf("%s\n",buf );
	
	return 0;
}