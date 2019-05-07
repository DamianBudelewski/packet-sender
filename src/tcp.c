#include "tcp.h"

void c_tcp(unsigned char *sendbuf){
    /* TCP Header */
 	struct tcphdr *tcph = (struct tcphdr *) (sendbuf + sizeof(struct ether_header) + sizeof(struct iphdr));
	int input;

	printf("---------------- TCP ------------------\n");
    printf ("Source Port : ");
    scanf("%d", &input);
	if (input!=0){
    	tcph->source = htons(input);
        input = 0;
    }
    else tcph->source = htons(80);

    printf ("Destination Port : ");
    scanf("%d", &input);
	if (input!=0){
    	tcph->dest = htons(input);
    }
    else tcph->dest = htons(80);


    printf ("Sequence number: ");
    scanf("%d",&input);
    tcph->seq = input;

    printf ("Acknowledgment number: ");
    scanf("%d",&input);
    tcph->ack_seq = input;

    printf ("FIN: ");
    scanf("%d",&input);
   	tcph->fin=input;
    
    printf ("SYN: ");
    scanf("%d",&input);
	tcph->syn=input;

    printf ("RST: ");
    scanf("%d",&input);
	tcph->rst=input;

    printf ("PUSH: ");
    scanf("%d",&input);
	tcph->psh=input;

    printf ("ACK: ");
    scanf("%d",&input);
	tcph->ack=input;

    printf ("URG: ");
    scanf("%d",&input);
	tcph->urg=input;


    tcph->doff = 5; // tcp header size
    tcph->window = htons(5840); // Maximum allowed window size 
    tcph->check = 0; // will be calculate later 
}



