#include "tcp.h"
#include <getopt.h>


void do_opt(unsigned char *sendbuf, struct ifreq if_ip, int tx_len, char * const argv[], int argc){
	int opt;
	int input;
	/* TCP Header */
	struct tcphdr *tcph = (struct tcphdr *) (sendbuf + sizeof(struct ether_header) + sizeof(struct iphdr));
	optind = 1;				// variable is the index value of the next argument that should be handled by the getopt()

	/* Predefinition */
	tcph->source = htons(80);
	tcph->dest = htons(80);
	tcph->doff = 5; // tcp header size
	tcph->window = htons(5840); // Maximum allowed window size 
	tcph->check = 0; // will be calculate later 


	while((opt = getopt(argc, argv, "a:b:d:e:f:g:i:j:n:p:q:r:s:t:u:w:x:y:z:h")) != -1){
		switch(opt){
			case 's':
				tcph->source = htons((u_int16_t)strtoul(optarg, (char **)NULL, 0));
				break;
			case 'n':
				tcph->dest = htons((u_int16_t)strtoul(optarg, (char **)NULL, 0));
				break;
			case 'e':
				tcph->seq = htonl((u_int32_t)strtoul(optarg, (char **)NULL, 0));
				break;
			case 'a':
				tcph->ack_seq = htonl((u_int32_t)strtoul(optarg, (char **)NULL, 0));
				break;
			case 'x':
				tcph->urg_ptr = htons((u_int16_t)strtoul(optarg, (char **)NULL, 0));
				break;
			case 'w':
				tcph->window = htons((u_int16_t)strtoul(optarg, (char **)NULL, 0));
				break;
			case 'g':
				tcph->fin=(u_int16_t)*optarg&1;
				break;
			case 'y':
				tcph->syn=(u_int16_t)*optarg&1;
				break;
			case 'r':
				tcph->rst=(u_int16_t)*optarg&1;
				break;
			case 'p':
				tcph->psh=(u_int16_t)*optarg&1;
				break;
			case 'q':
				tcph->ack=(u_int16_t)*optarg&1;
				break;
			case 'u':
				tcph->urg=(u_int16_t)*optarg&1;
				break;
			case '?':
				break;
		}
	}
}



