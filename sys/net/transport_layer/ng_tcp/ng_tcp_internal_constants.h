/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* Events used to control fsm */
typedef enum {
    CALL_OPEN,
    CALL_SEND,
    CALL_RECV,
    CALL_CLOSE,
    CALL_ABORT,
    RCVD_PKT,
    TIME_TIMEOUT,
    TIME_RETRANSMIT,
    TIME_USERTIMEOUT
} ng_tcp_event_t;

/* Offset without Options(in 32-Bit words) */
 #define OFFSET_BASE 0x5
 #define OFFSET_MAX  0xF

 /* Makros to deal with Control Bits */
#define MSK_FIN      0x0001
#define MSK_SYN      0x0002
#define MSK_RST      0x0004
#define MSK_PSH      0x0008
#define MSK_ACK      0x0010
#define MSK_URG      0x0020
#define MSK_SYN_ACK  0x0012
#define MSK_RST_ACK  0x0014
#define MSK_CTL      0x003F
#define MSK_OFFSET   0xF000

#define GET_OFFSET( x ) ((( x ) & MSK_OFFSET) >> 24)

/* Default Window Size */
#define DEFAULT_WINDOW 250

/* PORT 0 is reserved, according to rfc 1700(https://www.ietf.org/rfc/rfc1700.txt) */
/* Here used to indicate, that tcb internal port is currently not assigned */
#define PORT_UNSPEC 0

/* Range of Port Numbers, that are not controlled by IANA. */
#define BEG_REG_PORTS 1024
#define END_REG_PORTS 65535

/* Array Size to pick a seed for the PRNG */
/* Randomness by uninitialized memory garbage */
#define SEED_LEN 100

/* TCP Option Defines */
#define OPT_KIND_EOL   00      /* End of List */
#define OPT_KIND_NOP   01      /* No Operatrion */
#define OPT_KIND_MSS   02      /* Maximum Segment Size */

#define OPT_LENGTH_MSS 04      /* MSS Option Size is 4 byte */

/* Complete Option Macros */
#define OPT_MSS( x ) ((OPT_KIND_MSS << 24) | (OPT_LENGTH_MSS << 16) | ( x ))

/* Value Extraction Macros */
#define OPT_GET_KIND( x )   (((x) & 0xFF000000) >> 24)
#define OPT_GET_LENGTH( x ) (((x) & 0x00FF0000) >> 16)
#define OPT_GET_VAL_2B( x ) (((x) & 0x0000FFFF))