#include <stdio.h>
#include <stdlib.h>


#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 

#include <strings.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

#include <fcntl.h>
#include <sys/stat.h>
int main()
{
	// char* token="\0";
	// int x = atoi(30000);
	// printf("%d\n", x);

	// char* token;
	// token = itoa(10000);
	// printf("%s\n",token );



	// char* a;
	// short int i=10;
	// memcpy('L', a, sizeof(char));
	// memcpy(&i, a+1, sizeof(i));



	// char c= 'L';
	// short int i= 329;
	// char a[5];

	// char c1;
	// short int i1;

	// sprintf(a, "%c%hd", c,i);
	// sscanf(a,"%c%hd",&c1,&i1 );

	// printf("%c %hd\n",c1,i1 );



	int fd = open("abc.txt", O_RDONLY);
	int fd1 = open("abc.txt", O_RDONLY);
	int k;
	char temp[100], temp1[100];

	k = read(fd1, temp1,100);
	
	char c='M';
	while(1)
	{
		bzero(temp, sizeof(temp));
		short int t = read(fd, temp, 100);
		k = read(fd1, temp1, 100);
		if(k==0)
		{
			c='L';
		}
		if(t==0)
		{
			break;
		}
		else if(t>0)
		{			
			printf("%c %d %s\n", c, t,temp);
			// send(sockfd, temp, t, 0);
		}
		else
		{
			perror("file read error\n");
			exit(0);   
		}
	}


	return 0;
}
