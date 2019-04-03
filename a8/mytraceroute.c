#include<stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/udp.h> /* for ipv4 header */ 
#include <linux/ip.h> /* for upd header */ 
#include <unistd.h>
#include <string.h> 
#include <signal.h> 
#include <sys/wait.h>
#include <netdb.h>

extern int h_errno;

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


*/
int main()
{
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
			int j = sendto(udpfd, (const char*)ip, strlen(ip)+1, 0, 
						(struct sockaddr*)&cliaddr, sizeof(cliaddr));
			// printf("%d\n",j );
			if(j<0)
				perror("udp send error"); 
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

	struct iphdr *hdrip;
	struct updhdr *hdrudp;

	int ttl=1,r;
	char buffer[sizeof(struct iphdr)+sizeof(struct updhdr)+52];
	char mesg[52];

	hdrip = (struct iphdr*)buffer;
	hdrudp = (struct updhdr*)(buffer+sizeof(struct iphdr));
	// IP
	hdrip.ihl =;
	hdrip.version = ;
	hdrip.protocol =;
	hdrip.saddr = ;
	hdrip.daddr =  ;

	//UDP
	hdrudp.source = ;
	hdrudp.dest = ;


	while(1)
	{
		
		sendto(udpfd, buffer, sizeof(buffer),  , );
		r = select(icmpfd,  , , );
		if(r<0)
		{
			perror("error in select function");
		}
		else if(r==0)
		{
			t.tv_sec = T_SEC;
			t.tv_usec = T_USEC;
		}
		else
		{
			DEST_UNREACHABLE
			break;
		}

		hdrip.ttl++;
	}


}