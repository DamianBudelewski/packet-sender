/*
 * Aplikacja pozwalająca na stworzenie i wysłanie dowolnego pakietu IP
 * Obsługiwany stos protokołów: IPv4 + TCP
 * Program ma zezwalać na wybranie interfejsu sieciowego
 * Biblioteki poszczególnych protokołów ładowane dynamiczne
 * Pola nie podane przez użytkownika zostanę wypełnione zgodnie z RFC
 * Stworzone pakiety zostaną załadowane do listy wiązanej
 * Pakiety z listy zostaną wysłane do wskazanego interfejsu
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
#define DEFAULT_IF	"lo"
#define BUF_SIZ		1024
unsigned short csum(unsigned short *buf, int nwords);


/* Linked list to store frames */
struct Frames{
    unsigned char data[BUF_SIZ];
    struct Frames* next;
};


/* Given a reference (pointer to pointer) to the head of a list and an char, appends a new node at the end  */
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


int main(int argc, char *argv[]){
	struct Frames* head = (struct Frames*) malloc(sizeof(struct Frames)); 	// Empty frame list
	int i = 0;
	char MY_DEST_MAC[6] = {0x00,0x11,0x22,0x33,0x44,0x55};
	int sockfd;				// raw socket
	struct ifreq if_idx;	// interface index to send on
	struct ifreq if_mac;	// interface mac address
	struct ifreq if_ip;		// interface ip address
	void *dl_handle;		// variable to handle dynamic alocation of libraries
	int tx_len = 0;  		// Send N bytes of BUF on socket FD to peer at address ADDR (
	unsigned char sendbuf[BUF_SIZ];


	/* Define ethernet frame */
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct sockaddr_ll socket_address;
	char ifName[IFNAMSIZ];


	/* Get interface name */
	if (argc > 1)
		strcpy(ifName, argv[1]);
	else
		strcpy(ifName, DEFAULT_IF);


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
	eh->ether_type = htons(ETH_P_IP);					// Ethertype field
	socket_address.sll_ifindex = if_idx.ifr_ifindex;	// Index of the network device
	socket_address.sll_halen = ETH_ALEN; 				// Address length
	tx_len += sizeof(struct ether_header);


	/* Create IPv4 header */
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	dl_handle=dlopen("/home/damian/Workspace/eclipse-workspace/c-socket-programming/sendFrames/src/ipv4.so",RTLD_LAZY);
		if (!dl_handle) {
		        printf("Error: Can't open library");
		        exit(EXIT_FAILURE);
		}
	void (*c_ip)(unsigned char *sendbuf, struct ifreq if_ip, int tx_len);
	c_ip=dlsym(dl_handle,"c_ip");
	c_ip(sendbuf,if_ip,tx_len);
	tx_len += sizeof(struct iphdr);
	iph->check = csum((unsigned short *)(sendbuf+sizeof(struct ether_header)), sizeof(struct iphdr)/2);
	dlclose(dl_handle);


	/* Create TCP header */
 	struct tcphdr *tcph = (struct tcphdr *) (sendbuf + sizeof(struct ether_header) + sizeof(struct iphdr));
	dl_handle=dlopen("/home/damian/Workspace/eclipse-workspace/c-socket-programming/sendFrames/src/tcp.so",RTLD_LAZY);
	if (!dl_handle) {
	        printf("Error: Can't open library");
			exit(EXIT_FAILURE);
		}
	void (*c_tcp)(unsigned char *sendbuf);
	c_tcp=dlsym(dl_handle,"c_tcp");
	c_tcp(sendbuf);
	tx_len += sizeof(struct tcphdr);
	dlclose(dl_handle);


	/* Add frame to list */
	int times;
	i = 0;
	printf("Number of frames: ");
	scanf("%d",&times);
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



unsigned short csum(unsigned short *buf, int nwords){
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}



