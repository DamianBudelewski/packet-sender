#include "ipv4.h"
#include <getopt.h>


void do_opt(unsigned char *sendbuf, struct ifreq if_ip, int tx_len, char * const argv[], int argc){

	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	optind = 1;				// variable is the index value of the next argument that should be handled by the getopt()
	int opt;				// variable will handle getopt func
   	char input_char[32] = "0";
	int input_int = 0;


	/* Predefinition */
	iph->frag_off = 0;
	iph->tos = 0;
	iph->id = htonl(54321); 
	iph->ttl = 128;
	iph->daddr = inet_addr("127.0.0.1");


	/* Fields which need to be filled like this, if not packet will be corrupt */
	iph->tot_len = htons(sizeof(struct iphdr) + sizeof (struct tcphdr));
	iph->version = 4; // IPv4
	iph->check = 0; // Set to 0 before calculating checksum
	iph->ihl = 5;
	iph->protocol = 6; // TCP
	iph->saddr = inet_addr(inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));


	/* Get option */
	while((opt = getopt(argc, argv, "a:b:d:e:f:g:i:j:n:p:q:r:s:t:u:w:x:y:z:h")) != -1){
		switch(opt){
			case 'd':
				iph->daddr = inet_addr(optarg);
				break;
			case 'f':
				iph->frag_off = 0b01000000;
				break;
			case 'o':
				iph->tos = (u_int8_t)strtoul(optarg, (char **)NULL, 0);
				break;
			case 't':
				iph->ttl = (u_int8_t)strtoul(optarg, (char **)NULL,0);
				break;
			case 'i':
				iph->id = htons((u_int16_t)strtoul(optarg, (char **)NULL, 0));
				break;
			case '?':
				break;
		}
	}
}