/* 6LoWPAN IP header file */

#ifndef SIXLOWIP_H
#define SIXLOWIP_H

#include <stdint.h>

/* set maximum transmission unit */
#define MTU                         1280
/* IPv6 field values */ 
#define IPV6_VER                    0x60
#define ICMPV6_NXT_HDR              0x3A
#define ND_HOPLIMIT                 0xFF
#define SIXLOWPAN_IPV6_LL_ADDR_LEN  8            
/* size of global buffer */
#define BUFFER_SIZE (LL_HDR_LEN + MTU)
/* board specific configurations*/
#define MSBA2_OUI                   0x005BA2    // 24bit OUI 
#define R8BIT                       0xA2        // random 8bit
#define OUI                         0x005BA2
/* radio driver */
#define RADIO_CONF                  radio_cc1100
#define RADIO                       RADIO_CONF

#define MULTIHOP_HOPLIMIT           64

#define IP_PKT_RECV_BUF_SIZE        64

/* extern variables */
extern uint8_t ipv6_ext_hdr_len;
extern uint8_t opt_hdr_len;
extern uint16_t packet_length;

/* base header lengths */
#define LL_HDR_LEN                  0x4
#define ICMPV6_HDR_LEN              0x4
#define IPV6_HDR_LEN                0x28 
#define LLHDR_IPV6HDR_LEN           (LL_HDR_LEN + IPV6_HDR_LEN)
#define LLHDR_ICMPV6HDR_LEN         (LL_HDR_LEN + IPV6_HDR_LEN + ICMPV6_HDR_LEN)
#define IPV6HDR_ICMPV6HDR_LEN       (IPV6_HDR_LEN + ipv6_ext_hdr_len + ICMPV6_HDR_LEN)

/* buffer */
uint8_t buffer[BUFFER_SIZE];


/* ipv6 extension header length */

typedef union __attribute__ ((packed)) ipv6_addr_t{
    uint8_t uint8[16];
    uint16_t uint16[8];
    uint32_t uint32[4];
} ipv6_addr_t;

struct __attribute__ ((packed)) icmpv6_hdr_t{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
};

struct __attribute__ ((packed)) ipv6_hdr_t{
    uint8_t version_trafficclass;
    uint8_t trafficclass_flowlabel;
    uint16_t flowlabel;
    uint16_t length;
    uint8_t nextheader;
    uint8_t hoplimit;
    ipv6_addr_t srcaddr;
    ipv6_addr_t destaddr;
};

/* link layer addressing */
typedef union __attribute__ ((packed)) ieee_802154_long_t {
    uint8_t uint8[8];
    uint16_t uint16[4]; 
} ieee_802154_long_t;

typedef union __attribute__ ((packed)) ieee_802154_short_t {
    uint8_t uint8[2];
    uint16_t uint16[1];
} ieee_802154_short_t;

typedef struct __attribute__ ((packed)) iface_t {
    ieee_802154_short_t saddr;
    ieee_802154_long_t laddr;
    ipv6_addr_t ipaddr;
} iface_t;

extern iface_t iface;

//#define HTONS(a) (uint16_t)((((uint16_t) (a)) << 8) | (((uint16_t) (a)) >> 8))
#define HTONS(a) ((((uint16_t) (a) >> 8) & 0xff) | ((((uint16_t) (a)) & 0xff) << 8))  
#define HTONL(a) ((((uint32_t) (a) & 0xff000000) >> 24) | \
                  (((uint32_t) (a) & 0x00ff0000) >> 8)  | \
                  (((uint32_t) (a) & 0x0000ff00) << 8)  | \
                  (((uint32_t) (a) & 0x000000ff) << 24)) 

/* function prototypes */
struct icmpv6_hdr_t* get_icmpv6_buf(uint8_t ext_len);
struct ipv6_hdr_t* get_ipv6_buf(void);

void create_link_local_prefix(ipv6_addr_t *ipaddr);
void create_all_routers_mcast_addr(ipv6_addr_t *ipaddr);
void create_all_nodes_mcast_addr(ipv6_addr_t *ipaddr);
void set_eui64(ipv6_addr_t *ipaddr);
ieee_802154_long_t* get_eui(ipv6_addr_t *ipaddr);
void bootstrapping(uint8_t *addr);
void print6addr(ipv6_addr_t *ipaddr);
void ip_process(void);

#endif /* SIXLOWIP_H*/
