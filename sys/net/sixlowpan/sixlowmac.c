/* 6LoWPAN MAC - layer 2 implementations */

#include "sixlowmac.h"
#include "sixlownd.h"
#include "sixlowip.h"
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"

#define IP_BUFFER ((struct ipv6_hdr*)&buffer[LL_HEADER_LENGTH])

#define PRINTF(...) printf(__VA_ARGS__) 
#define PRINT6ADDR(addr) PRINTF("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])


#include <stdio.h>

void bootstrapping(void){
#ifdef SIXLOWPAN_NODE
    /* create link-local address based on eui-64 */
    RADIO.set_address(5);
    create_link_local_prefix(&IP_BUFFER->srcaddr);
    set_eui64(&IP_BUFFER->srcaddr);
    PRINT6ADDR(&IP_BUFFER->srcaddr);
    /* send router solicitation */
    send_rs();
#endif  
}


void create_link_local_prefix(ipv6_addr *ipaddr){
    ipaddr->uint16[0] = HTONS(0xfe80);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
}

/* cc1100 - eui-64 init fuction */
void set_eui64(ipv6_addr *ipaddr){
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

void create_all_routers_mcast_addr(ipv6_addr *ipaddr){
    ipaddr->uint16[0] = HTONS(0xff02);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0002);
}

void create_all_nodes_mcast_addr(ipv6_addr *ipaddr){
    ipaddr->uint16[0] = HTONS(0xff02);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0001);
}


void get_link_layer_addr_from_ipaddr(ipv6_addr *ipaddr, link_layer_addr *lla){
    lla->uint8[0] = ipaddr->uint8[8];
    lla->uint8[1] = ipaddr->uint8[9];
    lla->uint8[2] = ipaddr->uint8[10];
    lla->uint8[3] = ipaddr->uint8[11];
    lla->uint8[4] = ipaddr->uint8[12];
    lla->uint8[5] = ipaddr->uint8[13];
    lla->uint8[6] = ipaddr->uint8[14];
    lla->uint8[7] = ipaddr->uint8[15];
}
