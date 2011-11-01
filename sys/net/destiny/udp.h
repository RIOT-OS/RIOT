/*
 * udp.h
 *
 *  Created on: 05.09.2011
 *      Author: Oliver
 */

#ifndef UDP_H_
#define UDP_H_

#define UDP_HDR_LEN 				8


// TODO: Probably stack size too high
#define UDP_STACK_SIZE 				4096

#include "sys/net/sixlowpan/sixlowip.h"

typedef struct __attribute__ ((packed)) udp_h_t{
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} udp_hdr_t;

uint8_t buffer_udp[BUFFER_SIZE];
char udp_stack_buffer[UDP_STACK_SIZE];

uint16_t udp_csum(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header);
void udp_packet_handler(void);
void printArrayRange_udp(uint8_t *array, uint16_t len);

#endif /* UDP_H_ */
