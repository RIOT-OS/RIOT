//#include "sixlowpan.h"
#include "sixlowip.h"
#include <stdio.h>
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"


struct ipv6_hdr_t* ipv6_buf;

struct ipv6_hdr_t* get_ipv6_buf(void){
    return ((struct ipv6_hdr_t*)&(buffer[LL_HDR_LEN]));
}

struct icmpv6_hdr_t* get_icmpv6_buf(uint8_t ext_len){
    return ((struct icmpv6_hdr_t*)&(buffer[LLHDR_IPV6HDR_LEN + ext_len]));
}


void bootstrapping(void){
#ifdef SIXLOWPAN_NODE
    /* create link-local address based on eui-64 */
    ipv6_buf = get_ipv6_buf();
    RADIO.set_address(5);
    create_link_local_prefix(&ipv6_buf->srcaddr);
    set_eui64(&ipv6_buf->srcaddr);
    print6addr(&ipv6_buf->srcaddr);
    /* send router solicitation */
    send_rtr_sol();
    //send_rtr_adv(&ipv6_buf->destaddr);
#endif
}

void set_eui64(ipv6_addr_t *ipaddr){
    uint16_t radio_driver_addr = RADIO.get_address();

    ipaddr->uint8[8] = ((uint8_t) (OUI >> 16)) | 0x2 ;
    ipaddr->uint8[9] = (uint8_t) (OUI >> 8);
    ipaddr->uint8[10] = (uint8_t) OUI;
    ipaddr->uint8[11] = 0xFF;
    ipaddr->uint8[12] = 0xFE;
    ipaddr->uint8[13] = R8BIT;
    ipaddr->uint8[14] = (uint8_t) (radio_driver_addr >> 8);
    ipaddr->uint8[15] = (uint8_t) radio_driver_addr;
}

ieee_802154_long_t* get_eui(ipv6_addr_t *ipaddr){
    return ((ieee_802154_long_t *) &(ipaddr->uint8[8]));
}

void create_all_routers_mcast_addr(ipv6_addr_t *ipaddr){
    ipaddr->uint16[0] = HTONS(0xff02);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0002);
}

void create_all_nodes_mcast_addr(ipv6_addr_t *ipaddr){
    ipaddr->uint16[0] = HTONS(0xff02);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0001);
}

void create_link_local_prefix(ipv6_addr_t *ipaddr){
    ipaddr->uint16[0] = HTONS(0xfe80);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
}

void print6addr(ipv6_addr_t *ipaddr){
    printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
        ((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1], ((uint8_t *)ipaddr)[2],
        ((uint8_t *)ipaddr)[3], ((uint8_t *)ipaddr)[4], ((uint8_t *)ipaddr)[5], 
        ((uint8_t *)ipaddr)[6], ((uint8_t *)ipaddr)[7], ((uint8_t *)ipaddr)[8],
        ((uint8_t *)ipaddr)[9], ((uint8_t *)ipaddr)[10], ((uint8_t *)ipaddr)[11], 
        ((uint8_t *)ipaddr)[12], ((uint8_t *)ipaddr)[13], ((uint8_t *)ipaddr)[14], 
        ((uint8_t *)ipaddr)[15]);      
}
