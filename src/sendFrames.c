/*
 * Aplikacja pozwalająca na stworzenie i wysłanie dowolnego pakietu IP
 * Obsługiwany stos protokołów: IPv4 + TCP
 * Program ma zezwalać na wybranie interfejsu sieciowego
 * Biblioteki poszczególnych protokołów ładowane dynamiczne
 * Pola nie podane przez użytkownika zostanę wypełnione zgodnie z RFC
 * Stworzone pakiety zostaną załadowane do listy wiązanej
 * Pakiety z listy zostaną wysłane do wskazanego interfejsu
 */


/*
 * Application for sending raw tcp->ip packet
 * Program let you choose interface and all field of tcp and ip packet
 * Libraries are dynamic alocated
 * Fields that are left will be fill in with RFC regulation
 * Created packets will be loaded to linked list and then sent to chosen interface(ip addr)
 */


#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "ipv4.h"
#include "tcp.h"
#include <dlfcn.h>
#include <unistd.h>
#include <getopt.h>
#define DEFAULT_IF	"lo"	// Picked loopback as default interface because it is universal
#define BUF_SIZ	65536	// Max size of buffer and possible packet to sent
extern int optind;	// variable is the index value of the next argument that should be handled by the getopt()


/* Linked list to store frames */
struct Frames{
    unsigned char data[BUF_SIZ];
    struct Frames* next;
};


/* 96 bit (12 bytes) pseudo header needed for tcp header checksum calculation */
struct pseudo_header
{
	u_int32_t source_address;
	u_int32_t dest_address;
	u_int8_t placeholder;
	u_int8_t protocol;
	u_int16_t tcp_length;
};


void append(struct Frames** head_ref,unsigned char new_data[]);
unsigned short csumtcp(unsigned short *ptr,int nbytes); 
unsigned short csum(unsigned short *buf, int nwords);
void help();



int main(int argc, char *argv[]){

	int i = 0;	// temporary helper variable 
	int times = 1;	// How many times packet should be sent, default = 1;
	int opt;	// variable for getopt function
	int sockfd;	// raw socket
	struct ifreq if_idx;	// interface index to send on
	struct ifreq if_mac;	// interface mac address
	struct ifreq if_ip;	// interface ip address
	void *dl_handle;	// variable to handle dynamic alocation of libraries
	int tx_len = 0;	// Send N bytes of BUF on socket FD to peer at address ADDR 


	struct Frames* head = (struct Frames*) malloc(sizeof(struct Frames));	// Empty frame list
	char MY_DEST_MAC[6] = {0x00,0x11,0x22,0x33,0x44,0x55};	// Destination MAC 
	unsigned char sendbuf[BUF_SIZ];	// Buffer that will store all information about packets


	/* Variables used to calculate TCP checksum */
	char *pseudogram;	
	struct pseudo_header psh;


	/* Universal definition of do option function, used for both libraries */
	void (*do_opt)(unsigned char *sendbuf, struct ifreq if_ip, int tx_len, char * const argv[], int argc);						


	/* Get default interface name, if -b option will be filled then this variable will be overritten */
	char ifName[IFNAMSIZ];
	strcpy(ifName, DEFAULT_IF);


	/* Get options. I have started with this becouse when -h option is chose then program will display help and stop without sending packets etc. */
	while((opt = getopt(argc, argv, "a:b:d:e:f:g:i:j:n:p:q:r:s:t:u:w:x:y:z:h")) != -1){
		switch(opt){
			case 'h':
				help();
				exit(EXIT_SUCCESS);
				break;
			case 'z':
				times = (u_int8_t)strtoul(optarg, (char **)NULL,0);
				break;
			case 'b':
				snprintf( ifName, IF_NAMESIZE, "%s", optarg );	
				break;
			case '?':
				break;
		}
	} 


	/* Define ethernet frame */
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct sockaddr_ll socket_address;


	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    perror("socket");
	}


	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");


	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");


	/* Get interface IP */
	memset(&if_ip, 0, sizeof(struct ifreq));
	strncpy(if_ip.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFADDR, &if_ip) < 0)
		perror("SIOCGIFADDR");


	/* Construct the Ethernet header */
	memset(sendbuf, 0, BUF_SIZ);
	for (i = 0; i < 6; i++){
		eh->ether_shost[i] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[i];
		eh->ether_dhost[i] = MY_DEST_MAC[i];
		socket_address.sll_addr[i] = MY_DEST_MAC[i];
	}
	eh->ether_type = htons(ETH_P_IP);	// Ethertype field
	socket_address.sll_ifindex = if_idx.ifr_ifindex;	// Index of the network device
	socket_address.sll_halen = ETH_ALEN;	// Address length
	tx_len += sizeof(struct ether_header);	// Added size of ethernet header to length of buffer


	/* Create IPv4 header */
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	dl_handle=dlopen("./src/ipv4.so",RTLD_LAZY);
		if (!dl_handle) {
				printf("Error: Can't open library");
				exit(EXIT_FAILURE);
		}
	do_opt=dlsym(dl_handle,"do_opt");
	do_opt(sendbuf, if_ip,tx_len, argv, argc);
	tx_len += sizeof(struct iphdr);
	iph->check = csum((unsigned short *)(sendbuf+sizeof(struct ether_header)), sizeof(struct iphdr)/2);
	dlclose(dl_handle);


	/* Create TCP header */
	struct tcphdr *tcph = (struct tcphdr *) (sendbuf + sizeof(struct ether_header) + sizeof(struct iphdr));
	dl_handle=dlopen("./src/tcp.so",RTLD_LAZY);
	if (!dl_handle) {
			printf("Error: Can't open library");
			exit(EXIT_FAILURE);
		}
	do_opt=dlsym(dl_handle,"do_opt");
	do_opt(sendbuf, if_ip,tx_len, argv, argc);
	tx_len += sizeof(struct tcphdr);
	dlclose(dl_handle);


	/* Creating pseudo header and parse it with tcph to csumtcp() to calculate TCP checksum */
	psh.source_address = inet_addr(inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
	psh.dest_address = iph->daddr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr));
	int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr);
	pseudogram = malloc(psize);
	memcpy(pseudogram , (char*)&psh , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr));
	tcph->check = csumtcp( (unsigned short*) pseudogram , psize);


	/* Add frame to list */
	for (i = 0; i < times; i++){
		append(&head,sendbuf);
	}


	/* Send frame */
	while(head->next!=NULL){
		head = head->next;
		/* As buffer I parse frame from our linked list */
		if (sendto(sockfd, head->data, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
			printf("Send failed\n");
	}
	return 0;
}


/* Simple alghoritm to calculate checksum */
unsigned short csum(unsigned short *buf, int nwords){
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}


/* Function to append new frame to list  */
void append(struct Frames** head_ref,unsigned char new_data[])
{
    struct Frames* new_node = (struct Frames*) malloc(sizeof(struct Frames));
    struct Frames *last = *head_ref;
    memcpy(new_node->data,new_data,1024);
    new_node->next = NULL;
    if (*head_ref == NULL)
    {
       *head_ref = new_node;
       return;
    }
    while (last->next != NULL)
        last = last->next;
    last->next = new_node;
    return;
}


/* Generic checksum calculation function*/
unsigned short csumtcp(unsigned short *ptr,int nbytes) 
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
		*((unsigned char*)&oddbyte)=*(unsigned char*)ptr;
		sum+=oddbyte;
	}
	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	return(answer);
}


void help(){
	printf("\nSend raw TCP packet with IPv4.\n\n");
	printf("Parametrs: \n");
	printf("	a - acknowledgment number\n");
	printf("	b - interface name \n");
	printf("	d - destination ip address\n");
	printf("	e - sequence number	\n");
	printf("	f - don't fragment 	\n");
	printf("	g - fin flag	\n");
	printf("	h - help	\n");
	printf("	i - identification	\n");
	printf("	j - fragment offset \n");
	printf("	n - destination port\n");
	printf("	o - type of service  	\n");
	printf("	p - push flag	\n");
	printf("	q - ack flag\n");
	printf("	r - rst flag	\n");
	printf("	s - source port\n");
	printf("	t - time to live  \n");
	printf("	u - urg flag \n");
	printf("	w - window size\n");
	printf("	x - urgent pointer\n");
	printf("	y - syn flag\n");
	printf("	z - number of frames\n\n");
}
