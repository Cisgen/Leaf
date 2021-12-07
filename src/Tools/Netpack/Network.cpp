#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

#define BUFFER_MAX 2048

typedef int int32;
typedef unsigned int u_int32;
typedef unsigned char u_char;
typedef unsigned short u_short;

//�ο��ں� struct eth_hdr ���� (include/uapi/linux/if_ether.h��
struct eth_hdr {
	char h_dest[6];	
	char h_source[6];
	short h_proto;
}__attribute__((packed));

//�ο��ں� struct iphdr ���壨include/uapi/linux/ip.h��
struct iphdr{
#ifdef __LITTLE_ENDIAN_BIFIELD
	u_char ip_len:4, ip_ver:4;
#else
	u_char ip_ver:4, ip_len:4;
#endif
	u_char  tos;
	u_short total_len;
	u_short id;
	u_short flags_off;
	u_char  ttl;
	u_char  protocol;
	u_short check;
	u_int32 saddr;
	u_int32 daddr;
}__attribute__((packed));

struct udphdr {
	unsigned short source;
	unsigned short dest;
	unsigned short len;
	unsigned short check;
} __attribute__ ((packed));

struct udppkt {
	struct eth_hdr eh;
	struct iphdr ip;
	struct udphdr udp;
	unsigned char body[128];
} __attribute__ ((packed));


struct tcphdr {
	unsigned short source;
	unsigned short dest;
	unsigned int seq;
	unsigned int ack_seq;

	unsigned short res1:4, 
doff:4, 
fin:1,
syn:1,
rst:1,
psh:1,
ack:1,
urg:1,
ece:1,
cwr:1;
	unsigned short window;
	unsigned short check;
	unsigned short urg_ptr;
} __attribute__ ((packed));

struct arphdr {
	unsigned short h_type;
	unsigned short h_proto;
	unsigned char h_addrlen;
	unsigned char protolen;
	unsigned short oper;
	unsigned char smac[ETH_ALEN];
	unsigned int sip;
	unsigned char dmac[ETH_ALEN];
	unsigned int dip;
} __attribute__ ((packed));


struct icmphdr {
	unsigned char type;
	unsigned char code;
	unsigned short check;
	unsigned short identifier;
	unsigned short seq;
	unsigned char data[32];
} __attribute__ ((packed));


struct icmppkt {
	struct eth_hdr eh;
	struct iphdr ip;
	struct icmphdr icmp;
} __attribute__ ((packed));

int main()
{
	int  sock;
	char buffer[BUFFER_MAX];
	int len;

	struct eth_hdr* mac_hdr; 
	struct iphdr* ip_hdr;
	char* p;

	if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
	{
		printf("Create socket error.\n");
		exit(0);
	}

	while(1)
	{
		len = recvfrom(sock, buffer, BUFFER_MAX, 0, NULL, NULL);
		if (len < 46) 
		{
			printf("Catch packet length error %d.\n", len);
			continue;
		}

		mac_hdr = (struct eth_hdr*)buffer;	
		ip_hdr = (struct iphdr*)(buffer + sizeof(struct eth_hdr));

		if(ip_hdr->protocol != IPPROTO_UDP) 
		{
			continue;
		}

		struct udppkt* udp = (struct udppkt*) buffer;
		int udplen = ntohs((udp->udp.len));
		if (ntohs(udp->udp.dest) != 24503)
		{
			continue;
		}

		printf("�ػ����ݳ��� %d\n", len);
		printf("Դ MAC:%X:%X:%X:%X:%X:%X",
			(u_char)mac_hdr->h_source[0],
			(u_char)mac_hdr->h_source[1],
			(u_char)mac_hdr->h_source[2],
			(u_char)mac_hdr->h_source[3],
			(u_char)mac_hdr->h_source[4],
			(u_char)mac_hdr->h_source[5]
		);

		printf(" ==> Ŀ�� MAC:%X:%X:%X:%X:%X:%X\n",
			(u_char)mac_hdr->h_dest[0],
			(u_char)mac_hdr->h_dest[1],
			(u_char)mac_hdr->h_dest[2],
			(u_char)mac_hdr->h_dest[3],
			(u_char)mac_hdr->h_dest[4],
			(u_char)mac_hdr->h_dest[5]
		);

		p = (char*)&ip_hdr->saddr;
		printf("Դ IP: %d.%d.%d.%d",
			(u_char)p[0], 
			(u_char)p[1], 
			(u_char)p[2],
			(u_char)p[3]
		);

		p = (char*)&ip_hdr->daddr;
		printf(" ==> Ŀ�� IP: %d.%d.%d.%d\n",
			(u_char)p[0], 
			(u_char)p[1], 
			(u_char)p[2],
			(u_char)p[3]
		);

		printf("Э�����ͣ�");
		switch(ip_hdr->protocol) 
		{
		case IPPROTO_ICMP: 
			printf("ICMP"); 
			break;
		case IPPROTO_IGMP: 
			printf("IGMP"); 
			break;
		case IPPROTO_IPIP: 
			printf("IPIP"); 
			break;
		case IPPROTO_TCP: 
			break;
		case IPPROTO_UDP:
			{
				printf("UDP %d %d \n", ntohs(udp->udp.source), ntohs(udp->udp.dest));
				int ilen = ntohs(udp->udp.len);
				udp->body[ilen-8] = '\0';
				printf("Body: %s\n", udp->body);
				break;
			}
		case IPPROTO_RAW: 
			printf("RAW"); 
			break;
		default: 
			printf("Unknown type"); 
			break;
		}

		printf("\n");
	}

	close(sock);
	return 0;
}
