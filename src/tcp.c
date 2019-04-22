#include "tcp.h"

void c_tcp(unsigned char *sendbuf){
    /* TCP Header */
 	struct tcphdr *tcph = (struct tcphdr *) (sendbuf + sizeof(struct ether_header) + sizeof(struct iphdr));
	int input;


    printf ("Source Port : ");
    scanf("%d", &input);
	tcph->source = htons(input);
    printf ("Destination Port : ");
    scanf("%d", &input);
	tcph->dest = htons(input);
    printf ("Sequence number: ");
    scanf("%d",&input);
    tcph->seq = htons(input);
    printf ("Acknowledgment number: ");
    scanf("%d",&input);
    tcph->ack_seq = htons(input);
}



