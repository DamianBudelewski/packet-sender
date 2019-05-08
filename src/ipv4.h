#ifndef IPV4_H_
#define IPV4_H_

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


void do_opt(unsigned char *sendbuf, struct ifreq if_ip, int tx_len, char * const argv[], int argc);

#endif /* IPV4_H_ */
