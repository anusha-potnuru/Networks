#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
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

extern int errno;

#DEFINE T_SEC 1
#DEFINE T_USEC 0
/*
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
int main()
{
	struct in_addr dest_ip_addr;
	char domain_name[200];
	scanf("%s", domain_name);

	char ip[100];
	for (int i = 0; i < 100; ++i) ip[0] = '\0';
	struct hostent* x;
	x = gethostbyname(domain_name);
	if(x)
	{
		printf("IP:\n");
		for (int i = 0; x->h_addr_list[i]!=NULL ; ++i)
		{
			printf("%s\n",  inet_ntoa(*((struct in_addr*)x->h_addr_list[i])) );
			strcpy(ip, inet_ntoa(*((struct in_addr*)x->h_addr_list[i])));
			memcpy(&dest_ip_addr, x->h_addr_list[i], sizeof(struct in_addr));

		}
	}

	struct sockaddr_in saddr_udp, saddr_icmp;
	int saddr_len_udp, saddr_len_icmp;
	int udphdrlen = sizeof(hdrudp);

	int udpfd, icmpfd;
	udpfd = socket(AF_INET, SOCK_RAW, IPROTO_UDP);
	if(udpfd < 0)
	{
		perror("socket creation failed");
		exit(0);
	}
	int enable=1;
	setsockopt(udpfd, IPROTO_IP, IPHDR_INCL, &enable, sizeof(int));

	saddr_udp.sin_family = AF_INET;
	saddr_udp.sin_port = htons(PORT1);
	saddr_udp.sin_addr.s_addr = INADDR_ANY;
	saddr_len_udp = sizeof(saddr_udp);

	if(bind(udpfd, (struct sockaddr*)&saddr_udp, saddr_len_udp ) <0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	icmpfd = socket(AF_INET, SOCK_RAW, IPROTO_ICMP);
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

	struct iphdr *hdrip, *icmp_hdrip;
	struct updhdr *hdrudp;
	struct icmphdr *hdricmp;

	struct sockaddr_in destaddr, checkaddr;
	destaddr.sin_family = AF_INET;
	destaddr.sin_port = 32164; //given
	destaddr.sin_addr =  dest_ip_addr; /* address in network byte order */

	int ttl=1,r, ret, len;
	char buffer[sizeof(struct iphdr)+sizeof(struct updhdr)+52];
	char mesg[52];
	fd_set rset;
	clock_t t;
	double time_taken;

	hdrip = (struct iphdr*)buffer;
	hdrudp = (struct updhdr*)(buffer+sizeof(struct iphdr));
	// IP
	hdrip.ihl = INET_HDR_LEN;
	hdrip.version = 4;
	hdrip.protocol = IPROTO_UDP;
	hdrip.tot_len = sizeof(struct iphdr)+sizeof(struct udphdr);
	hdrip.saddr =  INADDR_ANY;
	hdrip.daddr = dest_ip_addr.s_addr;
	hdrip.ttl = ttl;
	//UDP
	hdrudp.source = ;
	hdrudp.dest = ;

	struct timeval t;
	t.tv_sec=T_SEC;
	t.tv_usec= T_USEC;

	int count=0;
	while(1)
	{
		FD_ZERO(&rset);
		FD_SET(&icmpfd);
		sendto(udpfd, buffer, sizeof(buffer),  (const struct sockaddr *)&destaddr , sizeof(destaddr) );
		// start time
		t = clock();
		r = select(icmpfd+1, &rset , 0,0, &t);
		// end time
		t = clock()-t;
		time_taken = (double)t / CLOCKS_PER_SEC;

		if(r<0 && (errno!=EINTR))
		{
			perror("error in select function");
		}

		if(r==0)
		{
			if(count==3)
			{
				printf("Hop_Count(TTL Value) %d, * *\n", ttl);
			}
			count++;
			t.tv_sec = T_SEC;
			t.tv_usec = T_USEC;
		}
		if(FD_ISSET(icmpfd, &rset))
		{
			len = sizeof(checkaddr);
			ret = recvfrom(icmpfd, icmp_buffer, sizeof(icmp_buffer), 0, (struct sockaddr*)&checkaddr, &len);
			icmp_hdrip = (struct iphdr*)icmp_buffer;

			hdricmp = (struct icmphdr*)(icmp_buffer + sizeof(struct iphdr));

			if(hdricmp.type == 3)
			{ // DEST_UNREACHABLE
				if(checkaddr == destaddr)
				{
					printf("Hop_Count(TTL Value) %d, IP_Address %s Response_time %f\n", ttl , inet_ntoa(checkaddr.sin_addr), time_taken );
					break;
				}
				else
				{
					printf("not reached correct dest\n");
				}
			}
			else if(hdricmp.type ==11)
			{//TIME EXCEEDED
				printf("Hop_Count(TTL Value) %d, IP_Address %s Response_time %f\n", ttl , inet_ntoa(checkaddr.sin_addr), time_taken );
			}
			// ignore all other types of icmp packets
		}

		ttl++;
		hdrip.ttl = ttl;
	}

	close(icmpfd);
	close(udpfd);

}