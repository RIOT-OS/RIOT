/* 6LoWPAN IP header file */

#ifndef SIXLOWIP_H
#define SIXLOWIP_H

#include <stdint.h>

#define MSBA2_SENSOR_NODE 1

/* set maximum transmission unit */
#ifdef MSBA2_SENSOR_NODE
#define MTU                         0x3A
#else 
#define MTU                         0x0
#endif                         

#define IPV6_VERSION                0x60
#define ICMP_NEXTHEADER             0x3A
#define NEIGHBOR_DISCOVERY_HOPLIMIT 0xFF

#define ICMP_HEADER_LENGTH          0x4
#define IPV6_HEADER_LENGTH          0x28            

/* link layer protocol control information length*/
#ifdef MSBA2_SENSOR_NODE
#define LL_HEADER_LENGTH            0x4
#else
#define LL_HEADER_LENGTH            0x0    
#endif

#define SIXLOWPAN_IPV6_LL_ADDR_LEN  8            

/* size of global buffer */
#define BUFFER_SIZE (LL_HEADER_LENGTH + MTU)

#define MSBA2_OUI                   0x005BA2    // 24bit OUI 
#define R8BIT                       0xA2        // random 8bit

#ifdef MSBA2_SENSOR_NODE
#define OUI                         0x005BA2
#else
#define OUI                         0x0
#endif

#define RADIO_CONF                  radio_cc1100
#define RADIO                       RADIO_CONF

#define MUTLIHOP_HOPLIMIT           64

extern uint8_t ipv6_ext_hdr_len;

#define LLHDR_IPV6HDR_LENGTH (LL_HEADER_LENGTH + IPV6_HEADER_LENGTH + ipv6_ext_hdr_len)
#define LLHDR_ICMPV6HDR_LENGTH (LL_HEADER_LENGTH + IPV6_HEADER_LENGTH + ipv6_ext_hdr_len + ICMP_HEADER_LENGTH)
#define IPV6HDR_ICMPV6HDR_LENGTH (IPV6_HEADER_LENGTH + ipv6_ext_hdr_len + ICMP_HEADER_LENGTH)
/* global buffer*/
uint8_t buffer[BUFFER_SIZE];
/* packet length*/
extern uint16_t packet_length;

/* ipv6 extension header length */

typedef union ipv6_addr{
    uint8_t uint8[16];
    uint16_t uint16[8];
} ipv6_addr;

struct icmpv6_hdr{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
};

struct ipv6_hdr{
    uint8_t version_trafficclass;
    uint8_t trafficclass_flowlabel;
    uint16_t flowlabel;
    uint16_t length;
    uint8_t nextheader;
    uint8_t hoplimit;
    ipv6_addr srcaddr;
    ipv6_addr destaddr;
};

typedef struct link_layer_addr{
    uint8_t uint8[8];  
} link_layer_addr;

#define HTONS(a) (uint16_t)((((uint16_t) (a)) << 8) | (((uint16_t) (a)) >> 8))
#define HTONL(a) ((((uint32_t) (a) & 0xff000000) >> 24) | \
                  (((uint32_t) (a) & 0x00ff0000) >> 8)  | \
                  (((uint32_t) (a) & 0x0000ff00) << 8)  | \
                  (((uint32_t) (a) & 0x000000ff) << 24)) 

void create_link_local_prefix(ipv6_addr *ipaddr);

void create_all_routers_mcast_addr(ipv6_addr *ipaddr);

void set_eui64(ipv6_addr *ipaddr);

link_layer_addr* get_eui(ipv6_addr *ipaddr);

void bootstrapping(void);

#endif /* SIXLOWIP_H*/
