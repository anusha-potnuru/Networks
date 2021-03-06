
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
	int k= 50; //chunk size
	char buf[k], temp[k];

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


	int fd;
	fd  = open("clientfile.txt", O_TRUNC|O_CREAT); // |O_CREAT, 0640

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

	int x;
	for (int i = 0; i < k; ++i) buf[i] = '\0';
	int bytecount =0,wordcount=0;
	char* tok, *lasttok, buf1[200];
	lasttok = "";

	char s[] = {'\t', ' ', ',', ';',':', '.', '\n'}; // delimiters used
	while(1)
	{
		x = recv(sockfd,buf, k, 0);
		bytecount = bytecount + x;	
		
		if(x==0)
		{
			if(bytecount==0)
				printf("File not found\n");
			break;
		}
		else if(x>0)
		{
			printf("BUFFER IS: (%d size)\n%s\n", x, buf);
			for (int i = 0; i < k; ++i)
			{
				temp[i] = buf[i];
			}

			// printf("lasttok is %s\n", lasttok );
			
			/*
			 if last token is not null and first character is not a delimiter use the previous 
			token and tokenise
			*/
			if(lasttok[0]!='\0' && strchr(s,buf[0])==NULL)
			{ // condition for last token is not null and first character in buf is not a delimiter
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
			{ // if ending letter is a delimiter
				strcpy(lasttok,"");
			} // make lasttok null


			if(write(fd, temp, x)==-1)
			{
				perror("write file error");
			}
		}		
		else
			perror("error");

		for (int i = 0; i < k; ++i) buf[i] = '\0';
	}

	if(bytecount>0)
		printf("The file transfer is successful. Size of the file = %d bytes, no. of words = %d\n", bytecount, wordcount);

	close(fd);
	close(sockfd);

}

