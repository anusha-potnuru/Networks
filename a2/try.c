#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
	char buf[] = {"anusha\nanusha\n\n\nanusha"};

	printf("%lu\n", strlen(buf) );
	return 0;
}

	// while(1)
	// {
	// 	for(i=0; i < 100; i++) temp[i] = '\0';
	// 	n = recv(sockfd, temp, 100, 0);
	// 	printf("buf is : %s, %d\n", temp, n);
	// 	if(strlen(temp)==0)
	// 		break;
	// 	if(n==0)
	// 	{
	// 		printf("File not found\n");
	// 	}
	// 	else
	// 	{
	// 		printf("found\n");
	// 		printf("file found\n");
	// 		int fd = open("client.txt", O_WRONLY | O_CREAT); 
	// 		while(1)
	// 		{
	// 			for(i=0; i < 100; i++) buf[i] = '\0';
	// 			n = recv(sockfd, buf, 50, 0);
	// 			if(n<=0)
	// 				break;
	// 			else
	// 			{
	// 				printf("%s\n", buf);
	// 				write(fd, buf, strlen(buf)+1);
	// 			}
	// 		}

	// 	}
	// }
	