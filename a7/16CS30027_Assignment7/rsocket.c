#include "rsocket.h"

extern int errno; 

typedef struct receive_buffer
{
	char buffer[100];
	int mesg_len;
	struct in_addr ip;
	in_port_t port;
	struct receive_buffer *next;
}receive_buffer;

typedef struct unack_message
{
	int id;
	char buffer[100];
	int mesg_len;
	struct in_addr ip;
	in_port_t port;
	struct timeval timestamp;
}unack_message;

typedef struct received_message_id_table
{
	int id;
}received_message_id_table;

typedef struct receive_buffer_queue
{
	struct receive_buffer *front, *rear;
}receive_buffer_queue;

int id=0;
int unack_msg_count, no_of_transmissions;
pthread_mutex_t lock; 
pthread_t X;

receive_buffer_queue *recvbuf_queue;
unack_message *unack_msg;
received_message_id_table *recv_msg_id_table;

int recv_msg_id_table_id;

// receive buffer - buffer, source_ip - NO ID
// unack messg - id, buffer, inet_aton(dest_addr.sin_port), time
// received message id table - id


void insert(receive_buffer_queue* root,  receive_buffer* node)
{
	// printf("inserting in receive_buffer_queue\n");
	receive_buffer *temp = node; 

	if(root->rear==NULL)
	{
		root->rear = root->front = temp;
		return;
	}
	root->rear->next = temp;
	root->rear = temp;
}

receive_buffer* delete(receive_buffer_queue* root)
{// returns deleted element

	if(root->front == NULL)
		return NULL;

	receive_buffer* temp = root->front;
	root->front = root->front->next;

	if(root->front == NULL)
		root->rear = NULL;

	return temp; 
}

int dropMessage(float p)
{
	float r;
	r = (float)rand()/((float)RAND_MAX+1);
	int ret = r<p ? 1: 0;
	return ret;
}

void HandleRetransmit(int socket)
{
	// scan unack for which mesg
	struct sockaddr_in dest_addr;
	int i, dest_len, k;
	dest_addr.sin_family = AF_INET;

	struct timeval res, curr_time;
	gettimeofday(&curr_time, NULL);

	for(i=0; i < 100 ;i++)
	{
		pthread_mutex_lock(&lock);
		if( unack_msg[i].id>=0 && curr_time.tv_sec - unack_msg[i].timestamp.tv_sec >= T )
		{
			// retransmit
			printf("retransmit %d \n", i);
			dest_addr.sin_addr =  unack_msg[i].ip;
			dest_addr.sin_port = unack_msg[i].port;
			dest_len = sizeof(dest_addr);

			char send_buf[1+sizeof(id)+unack_msg[i].mesg_len];
			
			strcpy(send_buf, "N");

			int conv_id = htonl(unack_msg[i].id);
			memcpy(&send_buf[1], &conv_id, sizeof(id));
			memcpy(&send_buf[1+sizeof(id)], unack_msg[i].buffer, unack_msg[i].mesg_len);

			k = sendto(socket, send_buf, 1+sizeof(id)+unack_msg[i].mesg_len, 0, 
				(const struct sockaddr*)&dest_addr, dest_len); 

			if(k<0)
			{
				perror("unable to send, in sendto");
				exit(EXIT_FAILURE);
			}
			no_of_transmissions++;
			gettimeofday(&unack_msg[i].timestamp, NULL);
		}
		pthread_mutex_unlock(&lock);
	}
}

void HandleAppMsgRecv(int socket, int id, char* buffer, int buf_length, struct sockaddr_in dest_addr)
{
	int i, dup_flag=0;
	char ack_buf[sizeof(id)+1];
	int dest_len = sizeof(dest_addr);
	char integer_string[32];

	for(i=0 ; i < recv_msg_id_table_id+1 ; i++)
	{
		if(recv_msg_id_table[i].id == id)
		{// duplicate mesg
			// send ack
			
			strcpy(ack_buf, "A");
			int conv_id = htonl(id);
			memcpy(&ack_buf[1], &conv_id, sizeof(conv_id));

			sendto(socket, ack_buf, 1+sizeof(id) , 0, 
				(const struct sockaddr*)&dest_addr, dest_len); //flags

			dup_flag = 1;
			break;
		}
	}
	if(dup_flag==0)
	{
		// send ack		
		strcpy(ack_buf, "A");
		int conv_id = htonl(id);
		memcpy(&ack_buf[1], &conv_id, sizeof(conv_id));

		sendto(socket, ack_buf, 1+sizeof(id) , 0, 
			(const struct sockaddr*)&dest_addr, dest_len); //flags

		// add to recv_mesg_id_table
		recv_msg_id_table[recv_msg_id_table_id++].id=id;

		// add to receive buffer
		
		struct receive_buffer* node = (struct receive_buffer*)malloc(sizeof(receive_buffer));
		node->ip = dest_addr.sin_addr;
		node->port = dest_addr.sin_port;
		memcpy(node->buffer, buffer, buf_length);
		node->mesg_len = buf_length;
		node->next = NULL;

		pthread_mutex_lock(&lock);

		insert(recvbuf_queue, node);

		pthread_mutex_unlock(&lock);
	}
}

void HandleACKMsgRecv(int socket, int id)
{
	int i;
	for(i=0; i< 100 ; i++)
	{
		pthread_mutex_lock(&lock); 		
		if(id == unack_msg[i].id)
		{
			printf("received ack for %d \n", id);
			// remove from unack_msg
			unack_msg_count--;
			unack_msg[i].id = -1;
			bzero(unack_msg[i].buffer, sizeof(unack_msg[i].buffer));
			bzero(&unack_msg[i].ip , sizeof(unack_msg[i].ip));
			unack_msg[i].port =0;
			unack_msg[i].timestamp = (struct timeval){0};
			
		}
		pthread_mutex_unlock(&lock);	
	}
	return;
}

void HandleReceive(int socket)
{
	char buffer[133], integer_string[32];
	struct sockaddr_in cliaddr;

	socklen_t socklen = sizeof(cliaddr);
	int len = recvfrom(socket, buffer, 133, 0,
				(struct sockaddr *)&cliaddr, &socklen); //flags

	int t = dropMessage(DROP_PROB);
	// printf("T: %d\n",t);
	if(!t)
	{
		int id;
		memcpy(&id, &buffer[1], sizeof(id));
		id = ntohl(id);
		// printf("id in receive: %d\n",id );
		if(buffer[0] == 'N')
		{// appl mesg  - to do id 
			HandleAppMsgRecv(socket, id, &buffer[sizeof(id)+1], len-sizeof(id)-1,  cliaddr);
		}
		else if(buffer[0] == 'A')
		{// ack mess
			HandleACKMsgRecv(socket, id);
		}
		else
		{
			printf("error b0: %c\n", buffer[0]);
		}
	}
	else
	{
		printf("dropping message\n");
	}


}

void *runner(void* arg)
{
	int socket = *((int*)arg);
	free(arg);

	struct timeval t;
	t.tv_sec=T;
	t.tv_usec=0;

	fd_set fds;
	int r;
	while(1)
	{
		FD_ZERO(&fds);
		FD_SET(socket, &fds);
		r = select(socket+1, &fds ,0, 0 ,&t);
		// printf("select value: %d\n",r );
		if(r==-1 && (errno!=EINTR))
		{
			perror("error in select");
		}
		if(r==0)
		{
			HandleRetransmit(socket);
			t.tv_sec = T;
			t.tv_usec=0;
		}
		if(FD_ISSET(socket, &fds))		
		{
			HandleReceive(socket);
		}		
	}

}

int r_socket(int domain, int type, int protocol)
{
	int i;
	if(type!=SOCK_MRP)
	{
		printf("Socket Type error\n");
		return -1;
	}
	int sockfd = socket(domain, SOCK_DGRAM, protocol);
	if(sockfd==-1)
	{
		perror("error in socket creation");
		exit(EXIT_FAILURE);
	}
	srand(time(NULL));

	// initialise tables

	unack_msg = (unack_message*)malloc(sizeof(unack_message)*100);
	recv_msg_id_table = (received_message_id_table*)malloc(sizeof(received_message_id_table)*100);
	recvbuf_queue = (receive_buffer_queue*)malloc(sizeof(receive_buffer_queue));

	for(i=0;i<100;i++)
	{
		unack_msg[i].id=-1;
		recv_msg_id_table[i].id=-1;
	}

	recvbuf_queue->front = recvbuf_queue->rear = NULL;
	unack_msg_count=0;
	recv_msg_id_table_id=0;
	no_of_transmissions=0;

	if(pthread_mutex_init(&lock, NULL) != 0) 
	{ 
		printf("\n mutex init has failed\n"); 
		return -1; 
	}  

	int* arg=(int*)malloc(sizeof(int));
	*arg = sockfd;
	pthread_create(&X, NULL, runner, arg );

	printf("rsocket done \n");
	return sockfd;
}


int r_bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	int k = bind(socket, address, address_len);
	return k;
}

ssize_t r_sendto(int socket, const void *buffer, size_t length, int flags, 
const struct sockaddr *dest_addr, socklen_t dest_len)
{
	struct timeval curr_time;
	int i;
	struct sockaddr_in addr;

	char newbuf[133];
	char buf_wlen[length+1];

	strcpy(newbuf, "N");

	int conv_id = htonl(id);
	memcpy(&newbuf[1], &conv_id, sizeof(conv_id));

	// take only till length
	memcpy(&newbuf[1+sizeof(id)], buffer, length );

	int k = sendto(socket, newbuf, 1+sizeof(id)+length, flags, 
			dest_addr, dest_len);

	if(k<0)
	{
		perror("unable to send");
	}

	no_of_transmissions++;
	gettimeofday(&curr_time, NULL);

	// add to unack messg - id, buffer, dest_addr.sin_port, time
	for(i=0; i < 100; i++)
	{
		pthread_mutex_lock(&lock); 		
		if(unack_msg[i].id==-1)
		{			
			unack_msg_count++;
			unack_msg[i].id = id++;
			memcpy(unack_msg[i].buffer, buffer, length);
			unack_msg[i].mesg_len = length;
			unack_msg[i].ip = ((struct sockaddr_in*)dest_addr)->sin_addr;
			unack_msg[i].port = ((struct sockaddr_in*)dest_addr)->sin_port;
			unack_msg[i].timestamp = curr_time;
			pthread_mutex_unlock(&lock); 
			break;
		}
		pthread_mutex_unlock(&lock); 
	}
	return k;
}

ssize_t r_recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct 
sockaddr *restrict address, socklen_t *restrict address_len)
{
	ssize_t ret;
	while(1)
	{
		// check the recv buffer , !empty
		// use lock, as when this deletes, other may insert
		pthread_mutex_lock(&lock);		
		if(recvbuf_queue->front!=NULL)
		{ // read first string
			
			receive_buffer* node = delete(recvbuf_queue);
			memcpy(buffer, node->buffer, node->mesg_len);
			ret = node->mesg_len;
			pthread_mutex_unlock(&lock);
			return ret;
		}
		else
		{
			pthread_mutex_unlock(&lock);
			sleep(1);
		}
	}
}

int r_close(int fildes)
{
	// free tables
	while(unack_msg_count>0)
	{
		sleep(1);
	}

	printf("NUMBER OF TRANSMISSSIONS: %d\n", no_of_transmissions);

	free(recvbuf_queue);
	free(unack_msg);
	free(recv_msg_id_table);

	pthread_mutex_destroy(&lock); 
	// have to close thread
	pthread_cancel(X);
	int ret=close(fildes);
	return ret;
}

