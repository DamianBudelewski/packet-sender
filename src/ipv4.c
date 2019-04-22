#include "ipv4.h"


void c_ip(unsigned char *sendbuf, struct ifreq if_ip,int tx_len){
   	char input_char[32];
	int input_int;
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));

   	/* IP Header */
	printf("---------------- IPv4 -----------------\n");
	printf("input 0 to choose default options\n");
	input_int = 0;
	printf("Type of service: ");
	scanf("%d",&input_int);
	if (input_int!=0){
		iph->tos = htons(input_int);
	}
	else iph->tos = 16;
	input_int = 0;
	printf("Id: ");
	scanf("%d",&input_int);
	if (input_int!=0){
		iph->id = htons(input_int);
	}
	else iph->id = htons(50000); //Function to convert between host and network byte order (htons)
	input_int = 0;
	printf("Time to live: ");
	scanf("%d",&input_int);
	if (input_int!=0){
		iph->ttl = htons(input_int);
	}
	else iph->ttl = 128;
	input_int = 0;
    printf("Destination IP address: ");
    scanf("%s",&input_char);
	iph->daddr = inet_addr(input_char);


	iph->ihl = 5;
	iph->tot_len = htons(tx_len - sizeof(struct ether_header));
	iph->version = 4;
	iph->protocol = 6; // TCP
	iph->saddr = inet_addr(inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
}

