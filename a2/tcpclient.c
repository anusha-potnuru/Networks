
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
	int			sockfd ;
	struct sockaddr_in	serv_addr;

	int i,n;
	char buf[100], temp[100];

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}

	/* Recall that we specified INADDR_ANY when we specified the server
	   address in the server. Since the client can run on a different
	   machine, we must specify the IP address of the server. 

	   In this program, we assume that the server is running on the
	   same machine as the client. 127.0.0.1 is a special address
	   for "localhost" (this machine)
	   
		IF YOUR SERVER RUNS ON SOME OTHER MACHINE, YOU MUST CHANGE 
           THE IP ADDRESS SPECIFIED BELOW TO THE IP ADDRESS OF THE 
           MACHINE WHERE YOU ARE RUNNING THE SERVER. 
    	*/

	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(20000);

	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/
	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	/* After connection, the client can send or receive messages.
	   However, please note that recv() will block when the
	   server is not sending and vice versa. Similarly send() will
	   block when the server is not receiving and vice versa. For
	   non-blocking modes, refer to the online man pages.
	*/

	printf("Enter file name:\n");
	scanf("%s", buf);
	printf("Given file name is : %s\n\n",buf);

	send(sockfd, buf, strlen(buf) + 1, 0);

	// for(i=0; i < 100; i++) buf[i] = '\0';
	// printf("%s\n",buf );
	// n = recv(sockfd, temp, 50  , 0);
	// printf("%s %d\n", temp,n);

	int fd;
	fd  = open("clientfile.txt", O_TRUNC|O_CREAT); // |O_CREAT, 0640
	// S_IRWXO|S_IRWXU|S_IRWXG
	if(fd==-1)
	{
		perror("file create error");
	}
	close(fd);
	fd = open("clientfile.txt",O_WRONLY, 0666);
	if(fd==-1)
	{
		perror("file use error");
	}
	int x,sz;
	for (int i = 0; i < 100; ++i) buf[i] = '\0';
	int bytecount =0,wordcount=0, k=1;
	char* tok, *lasttok, buf1[200];
	lasttok = "";
	// char temp[100];
	char s[] = {'\t', ' ', ',', ';',':', '.', '\n'};
	while(1)
	{
		x = recv(sockfd,buf, 100,0);
		// sz = x;	
		bytecount = bytecount + x;	
		printf("BUFFER IS: (%d size)\n%s\n", x, buf);
		if(x>0)
		{
			// strcpy(temp, buf);
			for (int i = 0; i < 100; ++i)
			{
				temp[i] = buf[i];
			}

			// printf("lasttok is %s\n", lasttok );
			
			
			if(lasttok[0]!='\0')
			{
				wordcount--; 
				strcpy(buf1, lasttok);
				strcat(buf1, buf);
				printf("New BUFFER is \n%s\n",buf1);
				tok = strtok(buf1, s);
			}
			else
			{
				tok = strtok(buf, s);
			}

			// tok = strtok(buf1, s);
			printf("Tokens are: \n");
			while(tok!=0)
			{
				wordcount++;
				printf("%s ",tok);
				lasttok = strdup(tok);
				tok = strtok(0, s);
			}
			printf("\nlast token is %s\n", lasttok);
			printf("wordcount: %d\n", wordcount );
			printf("\n\n");


			if(strchr(s, temp[x-1]))
			{ // ending letter not in delimiter
				strcpy(lasttok,"");
			} // remove last token as word, count


			if(write(fd, temp, x)==-1)
			{
				perror("write file error");
			}
		}
		else if(x==0)
		{
			if(bytecount==0)
				printf("file not found\n");
			break;
		}
		else
			perror("error");

		for (int i = 0; i < 100; ++i) buf[i] = '\0';
	}

	printf("The file transfer is successful. Size of the file = %d bytes, no. of words = %d\n", bytecount, wordcount);

	close(fd);
	close(sockfd);



}

