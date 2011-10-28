/*
 * tcp.h
 *
 *  Created on: 29.09.2011
 *      Author: Oliver
 */

#ifndef TCP_H_
#define TCP_H_

#define TCP_HDR_LEN 			20

enum tcp_flags
	{
	TCP_ACK			= 0x08,
	TCP_URG_PSH		= 0x14,
	TCP_RST			= 0x20,
	TCP_SYN			= 0x40,
	TCP_SYN_ACK		= 0x48,
	TCP_FIN			= 0x80,
	TCP_FIN_ACK		= 0x88
	};

enum tcp_states
	{
	CLOSED      	= 0,
	LISTEN      	= 1,
	SYN_SENT    	= 2,
	SYN_RCVD    	= 3,
	ESTABLISHED 	= 4,
	FIN_WAIT_1  	= 5,
	FIN_WAIT_2  	= 6,
	CLOSE_WAIT  	= 7,
	CLOSING     	= 8,
	LAST_ACK    	= 9,
	TIME_WAIT   	= 10,
	UNKNOWN			= 11
	};

#define IS_TCP_ACK(a) 			(((a & 0xC0) & ~TCP_URG_PSH) | TCP_ACK)			// Test for ACK flag only, iognore URG und PSH flag
#define IS_TCP_RST(a) 			(((a & 0xC0) & ~TCP_URG_PSH) | TCP_RST)
#define IS_TCP_SYN(a) 			(((a & 0xC0) & ~TCP_URG_PSH) | TCP_SYN)
#define IS_TCP_SYN_ACK(a) 		(((a & 0xC0) & ~TCP_URG_PSH) | TCP_SYN_ACK)
#define IS_TCP_FIN(a) 			(((a & 0xC0) & ~TCP_URG_PSH) | TCP_FIN)
#define IS_TCP_FIN_ACK(a) 		(((a & 0xC0) & ~TCP_URG_PSH) | TCP_FIN_ACK)

#define SET_TCP_ACK(a)			a = ((a & 0x00) | TCP_ACK)
#define SET_TCP_RST(a) 			a = ((a & 0x00) | TCP_RST)
#define SET_TCP_SYN(a) 			a = ((a & 0x00) | TCP_SYN)
#define SET_TCP_SYN_ACK(a) 		a = ((a & 0x00) | TCP_SYN_ACK)
#define SET_TCP_FIN(a) 			a = ((a & 0x00) | TCP_FIN)
#define SET_TCP_FIN_ACK(a) 		a = ((a & 0x00) | TCP_FIN_ACK)

// TODO: Probably stack size too high
#define TCP_STACK_SIZE 			2048

#include "sys/net/sixlowpan/sixlowip.h"

typedef struct __attribute__ ((packed)) tcp_socket_status_t
	{
	uint32_t	ack_nr;
	uint32_t 	seq_nr;
	uint16_t 	window;
	uint8_t		mss;
	uint8_t 	state;
	} tcp_socket_status_t;

typedef struct __attribute__ ((packed)) tcp_hdr_t
	{
    uint16_t 	src_port;
    uint16_t 	dst_port;
    uint32_t 	seq_nr;
    uint32_t 	ack_nr;
    uint8_t 	dataOffset_reserved;
    uint8_t 	reserved_flags;
    uint16_t 	window;
    uint16_t	checksum;
    uint16_t	urg_pointer;
	} tcp_hdr_t;


uint8_t buffer_tcp[BUFFER_SIZE];
char tcp_stack_buffer[TCP_STACK_SIZE];

void tcp_packet_handler (void);
uint16_t tcp_csum(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header);

#endif /* TCP_H_ */
