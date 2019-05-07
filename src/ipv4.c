#include "ipv4.h"


void c_ip(unsigned char *sendbuf, struct ifreq if_ip,int tx_len){
   	char input_char[32];
	int input_int = 0;
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));


   	/* IP Header */
	printf("---------------- IPv4 -----------------\n");
	printf("0 for default\n");


	/* Type of service */
	printf("Type of service: ");
	scanf("%d",&input_int);
	if (input_int!=0){
		iph->tos = input_int;
		input_int = 0;
	}
	else iph->tos = 0;


	/* Flags */
	printf("Don't fragment (0,1): ");
	scanf("%d",&input_int);
	if (input_int!=0){
		iph->frag_off = 0b01000000;
		input_int = 0;
	}
	else iph->frag_off = 0;




	/* Identification */
    printf("Identification: ");
	scanf("%d",&input_int);
	if (input_int!=0){
		iph->id = htons(input_int);
		input_int = 0;
	}
	else iph->id = htonl(54321); 


	/* Time to live */
	printf("Time to live: ");
	scanf("%d",&input_int);
	if (input_int!=0){
		iph->ttl = input_int;
		input_int = 0;
	}
	else iph->ttl = 128;


	/* Destination IP address */
    printf("Destination IP address: ");
    scanf("%s",&input_char);
	if (input_char!="0"){
		iph->daddr = inet_addr(input_char);
	}
	else iph->daddr = inet_addr("127.0.0.1");


	/* Those parametrs can only be filled that way */
	iph->tot_len = htons(sizeof(struct iphdr) + sizeof (struct tcphdr));
	iph->version = 4; // IPv4
	iph->check = 0; // Set to 0 before calculating checksum
	iph->ihl = 5;
	iph->protocol = 6; // TCP
	iph->saddr = inet_addr(inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
}

