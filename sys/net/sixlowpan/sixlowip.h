/* 6LoWPAN IP header file */

#ifndef SIXLOWIP_H
#define SIXLOWIP_H

#include <stdint.h>
#include <vtimer.h>

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

#define MULTIHOP_HOPLIMIT           64

#define IP_PKT_RECV_BUF_SIZE        64

/* extern variables */
extern uint8_t ipv6_ext_hdr_len;
extern uint8_t opt_hdr_len;
extern uint16_t packet_length;
extern uint8_t packet_dispatch;
extern uint8_t iface_addr_list_count;

/* base header lengths */
#define LL_HDR_LEN                  0x4
#define ICMPV6_HDR_LEN              0x4
#define IPV6_HDR_LEN                0x28 
#define LLHDR_IPV6HDR_LEN           (LL_HDR_LEN + IPV6_HDR_LEN)
#define LLHDR_ICMPV6HDR_LEN         (LL_HDR_LEN + IPV6_HDR_LEN + ICMPV6_HDR_LEN)
#define IPV6HDR_ICMPV6HDR_LEN       (IPV6_HDR_LEN + ipv6_ext_hdr_len + ICMPV6_HDR_LEN)

#define IFACE_ADDR_LIST_LEN         5 // maybe to much
/* rfc 4862 section 2. address states */
#define ADDR_STATE_TENTATIVE        0
#define ADDR_STATE_PREFERRED        1
#define ADDR_STATE_DEPRECATED       2
/* addresses with this state are always permitted */ 
#define ADDR_STATE_ANY              4
/* how the address is configured */
#define ADDR_CONFIGURED_AUTO        1
#define ADDR_CONFIGURED_MANUAL      2
/* address types */
#define ADDR_TYPE_NONE              0
#define ADDR_TYPE_UNICAST           1
#define ADDR_TYPE_MULTICAST         2
#define ADDR_TYPE_ANYCAST           3
#define ADDR_TYPE_SOL_NODE_MCAST    4
#define ADDR_TYPE_LINK_LOCAL        5
#define ADDR_TYPE_GLOBAL            6
/* dispatch types */
#define DISPATCH_TYPE_IPV6          0x41
#define DISPATCH_TYPE_LOWPAN_HC1    0x42
/* compression types */
#define COMPRESSION_TYPE_NONE       

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

typedef struct __attribute__ ((packed)) addr_list_t {
    uint8_t state;
    vtimer_t val_ltime;
    vtimer_t pref_ltime;
    uint8_t type;
    ipv6_addr_t addr;
} addr_list_t;

typedef struct __attribute__ ((packed)) iface_t {
    ieee_802154_short_t saddr;
    ieee_802154_long_t laddr;
    addr_list_t addr_list[IFACE_ADDR_LIST_LEN];
    uint8_t adv_cur_hop_limit;
    uint32_t adv_reachable_time;
    uint32_t adv_retrans_timer;
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
void iface_find_src_ipaddr(ipv6_addr_t *ipaddr, uint8_t state, 
                           uint8_t dest_addr_type);
uint8_t prefix_link_local_check(ipv6_addr_t *addr);
void iface_addr_list_add(ipv6_addr_t* addr, uint8_t state, uint32_t val_ltime, 
                         uint32_t pref_ltime, uint8_t type);
addr_list_t * iface_addr_list_prefix_equals(ipv6_addr_t *addr);
void setup_addr_with_prefix(ipv6_addr_t *inout, ipv6_addr_t *prefix);
uint32_t get_remaining_time(vtimer_t *t);
void set_remaining_time(vtimer_t *t, uint32_t time);
void create_prefix(ipv6_addr_t *inout, uint16_t prefix);
#endif /* SIXLOWIP_H*/
