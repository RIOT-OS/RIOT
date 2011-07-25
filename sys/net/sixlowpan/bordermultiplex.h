#ifndef BORDERMULTIPLEX_H
#define BORDERMULTIPLEX_H

#include <stdint.h>
#include <stdlib.h>

#include "sixlowip.h"

/* packet types of uart-packets */
#define BORDER_PACKET_RAW_TYPE    0
#define BORDER_PACKET_CONF_TYPE   2
#define BORDER_PACKET_L3_TYPE     3

/* configuration types */
#define BORDER_CONF_CONTEXT       2
#define BORDER_CONF_IPADDR        3

/* ethertypes for L3 packets */
#define BORDER_ETHERTYPE_IPV6     0x86DD

typedef struct __attribute__ ((packed)) border_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
} border_packet_t;

typedef struct __attribute__ ((packed)) border_l3_header_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint16_t ethertype;
} border_l3_header_t;

typedef struct __attribute__ ((packed)) border_conf_header_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
} border_conf_header_t;

typedef struct __attribute__ ((packed)) border_addr_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
    ipv6_addr_t addr;
} border_addr_packet_t;

typedef struct __attribute__ ((packed)) border_context_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
    struct border_context_t {
        uint8_t cid;
        ipv6_addr_t prefix;
        uint8_t length;
        uint8_t comp;
        uint16_t lifetime;
    } context;
} border_context_packet_t;

#define BORDER_BUFFER_SIZE (sizeof (border_l3_header_t) + MTU)

void demultiplex(border_packet_t *packet, int len);
void multiplex_send_ipv6_over_uart(struct ipv6_hdr_t *packet);
void multiplex_send_addr_over_uart(ipv6_addr_t *addr);

int readpacket(uint8_t *packet_buf, size_t size);
int writepacket(uint8_t *packet_buf, size_t size);

#endif /* BORDERMULTIPLEX_H*/
