/**
 * Destiny TCP header
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup destiny 
 * @{
 * @file    tcp.c
 * @brief   TCP data structs and prototypes
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#ifndef TCP_H_
#define TCP_H_

#define TCP_HDR_LEN 			20

#define TCP_EOO_OPTION			0x00		// End of option list
#define TCP_NOP_OPTION			0x01		// No operation
#define TCP_MSS_OPTION			0x02		// Maximum segment size
#define TCP_WSF_OPTION			0x03		// Window scale factor
#define TCP_TS_OPTION			0x08		// Timestamp

enum tcp_flags {
    TCP_ACK			= 0x08,
    TCP_URG_PSH		= 0x14,
    TCP_RST			= 0x20,
    TCP_SYN			= 0x40,
    TCP_SYN_ACK		= 0x48,
    TCP_FIN			= 0x80,
    TCP_FIN_ACK		= 0x88
};

enum tcp_states {
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

enum tcp_codes {
    UNDEFINED			= 0,
    PACKET_OK			= 1,
    CLOSE_CONN			= 2,
    SEQ_NO_TOO_SMALL	= 3,
    ACK_NO_TOO_SMALL 	= 4,
    ACK_NO_TOO_BIG		= 5
};

#define REMOVE_RESERVED 		0xFC

#define IS_TCP_ACK(a) 			((a & TCP_ACK) 		== TCP_ACK)	// Test for ACK flag only, ignore URG und PSH flag
#define IS_TCP_RST(a) 			((a & TCP_RST) 		== TCP_RST)
#define IS_TCP_SYN(a) 			((a & TCP_SYN) 		== TCP_SYN)
#define IS_TCP_SYN_ACK(a) 		((a & TCP_SYN_ACK)	== TCP_SYN_ACK)
#define IS_TCP_FIN(a) 			((a & TCP_FIN) 		== TCP_FIN)
#define IS_TCP_FIN_ACK(a) 		((a & TCP_FIN_ACK) 	== TCP_FIN_ACK)

#define SET_TCP_ACK(a)			a = ((a & 0x00) | TCP_ACK)
#define SET_TCP_RST(a) 			a = ((a & 0x00) | TCP_RST)
#define SET_TCP_SYN(a) 			a = ((a & 0x00) | TCP_SYN)
#define SET_TCP_SYN_ACK(a) 		a = ((a & 0x00) | TCP_SYN_ACK)
#define SET_TCP_FIN(a) 			a = ((a & 0x00) | TCP_FIN)
#define SET_TCP_FIN_ACK(a) 		a = ((a & 0x00) | TCP_FIN_ACK)

#define TCP_STACK_SIZE 			1024

#include "../sixlowpan/sixlowip.h"

typedef struct __attribute__((packed)) tcp_mms_o_t {
    uint8_t		kind;
    uint8_t		len;
    uint16_t	mss;
} tcp_mss_option_t;

typedef struct __attribute__((packed)) tcp_h_t {
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


// uint8_t buffer_tcp[BUFFER_SIZE];
char tcp_stack_buffer[TCP_STACK_SIZE];

#ifdef TCP_HC
mutex_t				global_context_counter_mutex;
uint8_t				global_context_counter;
#endif

mutex_t				global_sequence_clunter_mutex;
uint32_t			global_sequence_counter;

void tcp_packet_handler(void);
uint16_t tcp_csum(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header);
void printTCPHeader(tcp_hdr_t *tcp_header);
void printArrayRange_tcp(uint8_t *udp_header, uint16_t len);

#endif /* TCP_H_ */
