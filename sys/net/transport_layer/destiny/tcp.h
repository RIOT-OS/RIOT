/**
 * Destiny TCP header
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup destiny
 * @{
 * @file
 * @brief   TCP data structs and prototypes
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef TCP_H_
#define TCP_H_

#include "ipv6.h"
#include "destiny/types.h"

#define TCP_EOO_OPTION          (0x00)        /* End of option list */
#define TCP_NOP_OPTION          (0x01)        /* No operation */
#define TCP_MSS_OPTION          (0x02)        /* Maximum segment size */
#define TCP_WSF_OPTION          (0x03)        /* Window scale factor */
#define TCP_TS_OPTION           (0x08)        /* Timestamp */

enum tcp_flags {
    TCP_FIN                    = (1 << 0),
    TCP_SYN                    = (1 << 1),
    TCP_RST                    = (1 << 2),
    TCP_PSH                    = (1 << 3),
    TCP_ACK                    = (1 << 4),
    TCP_URG                    = (1 << 5),
    TCP_ECE                    = (1 << 6),
    TCP_CWR                    = (1 << 7),
    TCP_SYN_ACK                = (TCP_SYN | TCP_ACK),
    TCP_URG_PSH                = (TCP_URG | TCP_PSH),
    TCP_FIN_ACK                = (TCP_FIN | TCP_ACK)
};

enum tcp_states {
    TCP_CLOSED          = 0,
    TCP_LISTEN          = 1,
    TCP_SYN_SENT        = 2,
    TCP_SYN_RCVD        = 3,
    TCP_ESTABLISHED     = 4,
    TCP_FIN_WAIT_1      = 5,
    TCP_FIN_WAIT_2      = 6,
    TCP_CLOSE_WAIT      = 7,
    TCP_CLOSING         = 8,
    TCP_LAST_ACK        = 9,
    TCP_TIME_WAIT       = 10,
    TCP_UNKNOWN         = 11
};

enum tcp_codes {
    UNDEFINED           = 0,
    PACKET_OK           = 1,
    CLOSE_CONN          = 2,
    SEQ_NO_TOO_SMALL    = 3,
    ACK_NO_TOO_SMALL    = 4,
    ACK_NO_TOO_BIG      = 5
};

#define REMOVE_RESERVED         (0xFC)

#define IS_TCP_ACK(a)           (((a) & TCP_ACK)      == TCP_ACK) /* Test for ACK flag only, ignore URG und PSH flag */
#define IS_TCP_RST(a)           (((a) & TCP_RST)      == TCP_RST)
#define IS_TCP_SYN(a)           (((a) & TCP_SYN)      == TCP_SYN)
#define IS_TCP_SYN_ACK(a)       (((a) & TCP_SYN_ACK)  == TCP_SYN_ACK)
#define IS_TCP_FIN(a)           (((a) & TCP_FIN)      == TCP_FIN)
#define IS_TCP_FIN_ACK(a)       (((a) & TCP_FIN_ACK)  == TCP_FIN_ACK)

#define SET_TCP_ACK(a)          (a) = TCP_ACK
#define SET_TCP_RST(a)          (a) = TCP_RST
#define SET_TCP_SYN(a)          (a) = TCP_SYN
#define SET_TCP_SYN_ACK(a)      (a) = TCP_SYN_ACK
#define SET_TCP_FIN(a)          (a) = TCP_FIN
#define SET_TCP_FIN_ACK(a)      (a) = TCP_FIN_ACK

#define TCP_STACK_SIZE          (KERNEL_CONF_STACKSIZE_MAIN)

typedef struct __attribute__((packed)) tcp_mms_o_t {
    uint8_t     kind;
    uint8_t     len;
    uint16_t    mss;
} tcp_mss_option_t;

#ifdef TCP_HC
extern mutex_t             global_context_counter_mutex;
extern uint8_t             global_context_counter;
#endif

extern mutex_t             global_sequence_counter_mutex;
extern uint32_t            global_sequence_counter;

void *tcp_packet_handler(void *);
uint16_t tcp_csum(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header);
void printTCPHeader(tcp_hdr_t *tcp_header);
void printArrayRange_tcp(uint8_t *udp_header, uint16_t len);

/**
 * @}
 */

#endif /* TCP_H_ */
