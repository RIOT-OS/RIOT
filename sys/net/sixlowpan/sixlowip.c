#include "msg.h"
#include "sixlowip.h"
#include "sixlowmac.h"
#include "sixlownd.h"
#include "sixlowpan.h"
#include <stdio.h>
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"

msg msg_queue[IP_PKT_RECV_BUF_SIZE];
struct ipv6_hdr_t* ipv6_buf;
struct icmpv6_hdr_t* icmp_buf;
uint8_t ipv6_ext_hdr_len;
uint8_t *nextheader;
iface_t iface;

struct ipv6_hdr_t* get_ipv6_buf(void){
    return ((struct ipv6_hdr_t*)&(buffer[LL_HDR_LEN]));
}

struct icmpv6_hdr_t* get_icmpv6_buf(uint8_t ext_len){
    return ((struct icmpv6_hdr_t*)&(buffer[LLHDR_IPV6HDR_LEN + ext_len]));
}


void bootstrapping(uint8_t *addr){
#ifdef SIXLOWPAN_NODE
    /* create link-local address based on eui-64 */
    ipv6_buf = get_ipv6_buf();
    //RADIO.set_address(5);
    //create_link_local_prefix(&ipv6_buf->srcaddr);
    //init_802154_long_addr((uint8_t*)&(ipv6_buf->srcaddr.uint8[8]));  
    //set_eui64(&ipv6_buf->srcaddr);
//    print6addr(&ipv6_buf->srcaddr);
    /* send router solicitation */
    init_rtr_sol();

    output(addr,(uint8_t*)ipv6_buf);
    //send_rtr_adv(&ipv6_buf->destaddr);
#endif
}

void ip_process(void){
    msg m;
    msg_init_queue(msg_queue, IP_PKT_RECV_BUF_SIZE);
    while(1){
        msg_receive(&m);

        ipv6_buf = get_ipv6_buf();
        ipv6_buf = (struct ipv6_hdr_t*) m.content.ptr;

        printf("INFO: packet received, source: ");
        print6addr(&ipv6_buf->srcaddr);

        /* identifiy packet */
        nextheader = &ipv6_buf->nextheader;
        
        switch(*nextheader) {
            case(ICMPV6_NXT_HDR):{
                /* checksum test*/
                if(icmpv6_csum(ICMPV6_NXT_HDR) != 0xffff){
                    printf("ERROR: wrong checksum\n");
                }
                icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
                switch(icmp_buf->type) {
                    case(ICMP_RTR_SOL):{
                        printf("INFO: packet type: icmp router solicitation\n");
                        recv_rtr_sol();
                    }
                    default:
                        break;
                }            
                break;
            }
            default:
                break;
        }
    }   
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

void get_src_ipaddr(ipv6_addr_t *ipaddr){
    int i;
    for(i=0;i<16;i++){
        ipaddr->uint8[i] = iface.ipaddr.uint8[i];
    }
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
