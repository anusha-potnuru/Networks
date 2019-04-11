#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/udp.h> /* for udp header */ 
#include <linux/ip.h> /* for ipv4 header */ 
#include <linux/icmp.h> /* for icmp header */ 

#include <unistd.h>
#include <string.h> 
#include <signal.h> 
#include <sys/wait.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <netdb.h>
#include <errno.h>

extern int errno;

#define T_SEC 1
#define T_USEC 0
#define HOP_LIMIT 64
#define PORT1 50000 //udp port
#define PORT2 50001 //icmp port
/*

DATA STRUCTURES:

struct iphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8	ihl:4,
		version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
	__u8	version:4,
  		ihl:4;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	__u8	tos;
	__u16	tot_len;
	__u16	id;
	__u16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__u16	check;
	__u32	saddr;
	__u32	daddr;
	/*The options start here. 
};


struct udphdr
__u16 	source
__u16 	dest
__u16 	len
__u16 	check


struct sockaddr_in {
               sa_family_t    sin_family; /* address family: AF_INET
               in_port_t      sin_port;   /* port in network byte order 
               struct in_addr sin_addr;   /* internet address 
           };

*/

// CHECKSUM for ip
unsigned short checksum(void *b, int len) 
{    
	unsigned short *buf = b; 
    unsigned int sum=0; 
    unsigned short result; 
  
    for ( sum = 0; len > 1; len -= 2 ) 
        sum += *buf++; 
    if ( len == 1 ) 
        sum += *(unsigned char*)buf; 
    sum = (sum >> 16) + (sum & 0xFFFF); 
    sum += (sum >> 16); 
    result = ~sum; 
    return result; 
} 

uint16_t udp_checksum(const void *buff, size_t len, in_addr_t src_addr, in_addr_t dest_addr)
{
	const uint16_t *buf=buff;
	uint16_t *ip_src=(void *)&src_addr, *ip_dst=(void *)&dest_addr;
	uint32_t sum;
	size_t length=len;

	// Calculate the sum                                            //
	sum = 0;
	while (len > 1)
	{
	     sum += *buf++;
	     if (sum & 0x80000000)
	             sum = (sum & 0xFFFF) + (sum >> 16);
	     len -= 2;
	}

	if ( len & 1 )
	// Add the padding if the packet length is odd          //
	    sum += *((uint8_t *)buf);

	// Add the pseudo-header                                        //
	sum += *(ip_src++);
	sum += *ip_src;

	sum += *(ip_dst++);
	sum += *ip_dst;

	sum += htons(IPPROTO_UDP);
	sum += htons(length);

	// Add the carries                                              //
	while (sum >> 16)
	     sum = (sum & 0xFFFF) + (sum >> 16);

	// Return the one's complement of sum                           //
	return ( (uint16_t)(~sum)  );
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	struct in_addr dest_ip_addr;
	char domain_name[200];
	if(argv[1])
	{
		strcpy(domain_name, argv[1]);
	}
	else
	{
		printf("Enter domain name:\n");
		scanf("%s", domain_name);	
	}
	
	int flag=0;
	char ip[100];
	for (int i = 0; i < 100; ++i) ip[0] = '\0';
	struct hostent* x;
	x = gethostbyname(domain_name);
	if(x)
	{
		printf("IP for given domain:\n");
		for (int i = 0; x->h_addr_list[i]!=NULL ; ++i)
		{
			printf("%s\n",  inet_ntoa(*((struct in_addr*)x->h_addr_list[i])) );
			strcpy(ip, inet_ntoa(*((struct in_addr*)x->h_addr_list[i])));
			memcpy(&dest_ip_addr,((struct in_addr*)x->h_addr_list[i]), sizeof(struct in_addr));
			flag=1;
		}
	}
	else
	{
		herror("gethostbyname error");
	}
	printf("\n");

	if(flag==0)
	{
		printf("Invalid domain name\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in saddr_udp, saddr_icmp;
	int saddr_len_udp, saddr_len_icmp;
	

	int udpfd, icmpfd;
	udpfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if(udpfd < 0)
	{
		perror("socket creation failed");
		exit(0);
	}
	int enable=1;
	setsockopt(udpfd, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(int));

	saddr_udp.sin_family = AF_INET;
	saddr_udp.sin_port = htons(PORT1);
	saddr_udp.sin_addr.s_addr = INADDR_ANY;
	saddr_len_udp = sizeof(saddr_udp);

	if(bind(udpfd, (struct sockaddr*)&saddr_udp, saddr_len_udp ) <0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	icmpfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(icmpfd < 0)
	{
		perror("icmp raw socket creation failed");
		exit(EXIT_FAILURE);
	}
	saddr_icmp.sin_family = AF_INET;
	saddr_icmp.sin_port = htons(PORT2);
	saddr_icmp.sin_addr.s_addr = INADDR_ANY;
	saddr_len_icmp = sizeof(saddr_icmp);

	if(bind(icmpfd, (struct sockaddr*)&saddr_icmp, saddr_len_icmp ) <0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	printf("Sockets created and binded\n");
	

	struct iphdr *hdrip, *icmp_hdrip;
	struct udphdr *hdrudp;
	struct icmphdr *hdricmp;

	struct sockaddr_in destaddr, checkaddr;
	destaddr.sin_family = AF_INET;
	destaddr.sin_port = 32164; //given
	destaddr.sin_addr =  dest_ip_addr; /* address in network byte order */

	// printf("%s\n", inet_ntoa(destaddr.sin_addr) );

	int ttl=1,r, ret, len, usend, k;
	char buffer[sizeof(struct iphdr)+sizeof(struct udphdr)+52];
	// char mesg[52];
	for (k = sizeof(struct iphdr)+sizeof(struct udphdr); k < sizeof(struct iphdr)+sizeof(struct udphdr)+52 ; ++k)
	{
		/* code */
		buffer[k] = 'A' + (random() % 26);
	}

	printf("Payload:\n%s\n", &buffer[sizeof(struct iphdr)+sizeof(udphdr)]);
	printf("Starting traceroute...\n\n");
	fd_set rset;
	clock_t time;
	double time_taken;

	hdrip = (struct iphdr*)buffer;
	hdrudp = (struct udphdr*)(buffer+sizeof(struct iphdr));
	// IP
	hdrip->ihl = 5;
	hdrip->version = 4;
	hdrip->protocol = IPPROTO_IP;
	// hdrip->tot_len = sizeof(struct iphdr)+sizeof(struct udphdr);
	//ip header sets tot_len and check
	hdrip->saddr =  INADDR_ANY;
	hdrip->daddr = dest_ip_addr.s_addr;
	hdrip->ttl = ttl;

	//UDP
	hdrudp->source = htons(PORT1);
	hdrudp->dest = htons(32164);
	hdrudp->len = sizeof(struct udphdr)+52; //added
	// hdrudp->check = 0; // doesn't check if 0

	hdrudp->check = udp_checksum(&hdrudp, sizeof(struct udphdr)+52 , INADDR_ANY, dest_ip_addr.s_addr);
	// printf("%d\n", hdrudp->check );
	// len doubt

	struct timeval t;
	t.tv_sec=T_SEC;
	t.tv_usec= T_USEC;
	char icmp_buffer[1024];

	int count=0;
	while(ttl < HOP_LIMIT)
	{
		FD_ZERO(&rset);
		FD_SET(icmpfd, &rset);
		usend = sendto(udpfd, buffer, sizeof(buffer),  0, (const struct sockaddr *)&destaddr , sizeof(destaddr) );
		// printf("udp sendto ret value: %d\n",usend );
		// start time
		time = clock();
		r = select(icmpfd+1, &rset , 0,0, &t);
		// end time
		time = clock()-time;
		time_taken = ((double)time) / CLOCKS_PER_SEC;

		if(r<0 && (errno!=EINTR))
		{
			perror("error in select function");
		}

		if(r==0)
		{
			// printf("timeout\n");
			printf("Hop_Count(TTL Value) %d, * *\n", hdrip->ttl);
			if(count==3)
			{
				ttl++;
				count=0;
			}
			count++;
			t.tv_sec = T_SEC;
			t.tv_usec = T_USEC;
		}
		if(FD_ISSET(icmpfd, &rset))
		{
			len = sizeof(checkaddr);
			ret = recvfrom(icmpfd, icmp_buffer, 1024, 0, (struct sockaddr*)&checkaddr, &len);
			icmp_hdrip = (struct iphdr*)icmp_buffer;
			hdricmp = (struct icmphdr*)(icmp_buffer + sizeof(struct iphdr));

			if(icmp_hdrip->protocol == 1)
			{//icmp packet
				printf("icmp pcket\n");
			}

			if(hdricmp->type == 3)
			{ // DEST_UNREACHABLE
				
				// printf("%d %d %d %d \n", checkaddr.sin_family , destaddr.sin_family , checkaddr.sin_port, destaddr.sin_port);
				if( checkaddr.sin_addr.s_addr == destaddr.sin_addr.s_addr)
				{// check family, port, sin_addr
					printf("Hop_Count(TTL Value) %d, IP_Address %s Response_time %fms\n", hdrip->ttl , inet_ntoa(checkaddr.sin_addr), time_taken*1000 );
					printf("Destination reached, %s\n", inet_ntoa(checkaddr.sin_addr));
					break;
				}
				else
				{
					printf("not reached correct destination\n");
				}
				ttl++;
			}
			else if(hdricmp->type ==11)
			{//TIME EXCEEDED
				printf("Hop_Count(TTL Value) %d, IP_Address %s Response_time %f\n", hdrip->ttl , inet_ntoa(checkaddr.sin_addr), time_taken );
				ttl++;
			}
			else
			{
				printf("spurious icmp packet: %d\n", hdricmp->type );
			}
			
			// ignore all other types of icmp packets
		}

		
		hdrip->ttl = ttl;
	}

	close(icmpfd);
	close(udpfd);

}