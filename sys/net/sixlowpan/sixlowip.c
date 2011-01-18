#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include "msg.h"
#include "sixlowip.h"
#include "sixlowmac.h"
#include "sixlownd.h"
#include "sixlowpan.h"

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
    //ipv6_buf = get_ipv6_buf();
    //RADIO.set_address(5);
    //create_link_local_prefix(&ipv6_buf->srcaddr);
    //init_802154_long_addr((uint8_t*)&(ipv6_buf->srcaddr.uint8[8]));  
    //set_eui64(&ipv6_buf->srcaddr);
//    print6addr(&ipv6_buf->srcaddr);
    /* send router solicitation */
    
    init_rtr_sol(OPT_SLLAO);
    //init_rtr_adv(NULL, 0, 0, 0, 0, 0);

    //init_rtr_adv(&ipv6_buf->srcaddr, 0, 0, OPT_PI, 0, 0);

    ipv6_buf = get_ipv6_buf();
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
                        /* processing router solicitation */
                        recv_rtr_sol();    
                        /* init solicited router advertisment*/
                        init_rtr_adv(&ipv6_buf->srcaddr, 0, 0, OPT_PI, 0, 0);
                        /* send solicited router advertisment */
                        uint8_t addr = 0;
                        output(&addr,(uint8_t*)ipv6_buf);

                        break;
                    }
                    case(ICMP_RTR_ADV):{
                        printf("INFO: packet type: icmp router advertisment\n");
                        /* processing router advertisment */
                        recv_rtr_adv();
                        /* init neighbor solicitation */
                        //init_nbr_sol();
                        break;
                    }
                    case(ICMP_NBR_SOL):{
                        printf("INFO: packet type: icmp neighbor solicitation\n");
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

void iface_addr_list_add(ipv6_addr_t *addr, uint8_t state, uint32_t val_ltime,
                         uint32_t pref_ltime, uint8_t type){
    if(iface_addr_list_count < IFACE_ADDR_LIST_LEN){
        memcpy(&(iface.addr_list[iface_addr_list_count].addr.uint8[0]), 
               &(addr->uint8[0]), 16);
        iface.addr_list[iface_addr_list_count].state = state;
        iface.addr_list[iface_addr_list_count].val_ltime.absolute.seconds = val_ltime;
        timex_t vtime, ptime;
        vtime.seconds = val_ltime;
        ptime.seconds = pref_ltime;
        vtimer_set_wakeup(&(iface.addr_list[iface_addr_list_count].val_ltime),vtime,NULL);
        //iface.addr_list[iface_addr_list_count].pref_ltime.absolute.seconds = pref_ltime;
        vtimer_set_wakeup(&(iface.addr_list[iface_addr_list_count].pref_ltime), ptime, NULL);
        iface.addr_list[iface_addr_list_count].type = type;
        iface_addr_list_count++;
    }
}

addr_list_t * iface_addr_list_search(ipv6_addr_t *addr){
    int i;
    for(i = 0; i < IFACE_ADDR_LIST_LEN; i++){
        if(memcmp(&(iface.addr_list[i].addr.uint8[0]),
                  &(addr->uint8[0]),16) == 0){
            return &(iface.addr_list[i]);
        }
    }
}

addr_list_t * iface_addr_list_prefix_equals(ipv6_addr_t *addr){
    int i;
    for(i = 0; i < IFACE_ADDR_LIST_LEN; i++){
        if(memcmp(&(iface.addr_list[i].addr.uint8[0]),
                  &(addr->uint8[0]), 8) == 0){
            return &(iface.addr_list[i]);
        }
    }
    return NULL;
}

void setup_addr_with_prefix(ipv6_addr_t *inout, ipv6_addr_t *prefix){
    inout->uint16[0] = prefix->uint16[0];
    inout->uint16[1] = prefix->uint16[1];
    inout->uint16[2] = prefix->uint16[2];
    inout->uint16[3] = prefix->uint16[3];

    memcpy(&(inout->uint8[8]),&(iface.laddr.uint8[0]), 8); 
}

ieee_802154_long_t* get_eui(ipv6_addr_t *ipaddr){
    return ((ieee_802154_long_t *) &(ipaddr->uint8[8]));
}

void create_prefix(ipv6_addr_t *inout, uint16_t prefix){
    inout->uint16[0] = prefix;
    inout->uint16[1] = 0;
    inout->uint16[2] = 0;
    inout->uint16[3] = 0;
    inout->uint16[4] = 0;
    inout->uint16[5] = 0;
    inout->uint16[6] = 0;
    inout->uint16[7] = 0;
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

void iface_find_src_ipaddr(ipv6_addr_t *ipaddr, uint8_t state, 
                           uint8_t dest_addr_type){
    int i;
    int found = 0;

    switch(dest_addr_type) {
        case(ADDR_TYPE_MULTICAST):{
            printf("a\n");
            for(i = 0; i < IFACE_ADDR_LIST_LEN; i++){
                if(prefix_link_local_check(&(iface.addr_list[i].addr))){
                    printf("b\n");
                    if(iface.addr_list[i].state == state){
                        printf("c\n");
                        memcpy(&(ipaddr->uint8[0]),
                               &(iface.addr_list[i].addr.uint8[0]),16);
                        found = 1;
                    }
                }
            }
            break;
        }
        case(ADDR_TYPE_LINK_LOCAL):{
            for(i = 0; i < IFACE_ADDR_LIST_LEN; i++){
                if(prefix_link_local_check(&(iface.addr_list[i].addr))){
                    if(iface.addr_list[i].state == state){
                        memcpy(&(ipaddr->uint8[0]),
                               &(iface.addr_list[i].addr.uint8[0]),16);
                        found = 1;
                    }
                }
            }
            break;
        }
        case(ADDR_TYPE_GLOBAL):{
            for(i = 0; i < IFACE_ADDR_LIST_LEN; i++){
                if(!(prefix_link_local_check(&(iface.addr_list[i].addr)))){
                    if(iface.addr_list[i].state == state){
                        memcpy(&(ipaddr->uint8[0]),
                               &(iface.addr_list[i].addr.uint8[0]),16);
                        found = 1;
                    }
                }
            }
            break;            
        }
        default:
            break;
    }
    
    if(found == 0){
        memset(ipaddr, 0, 16);
    }
}

void create_link_local_prefix(ipv6_addr_t *ipaddr){
    ipaddr->uint16[0] = HTONS(0xfe80);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
}

uint8_t prefix_link_local_check(ipv6_addr_t *addr){
    if(addr->uint8[0] == 0xfe && addr->uint8[1] == 0x80){
        return 1;
    }
    return 0;
}

void create_solicited_node_mcast_addr(ipv6_addr_t *addr_in, ipv6_addr_t *addr_out){
    /* copy only the last 24-bit of the ip-address that is beeing resolved */
    addr_out->uint16[0] = HTONS(0xff02);
    addr_out->uint16[1] = 0;
    addr_out->uint16[2] = 0;
    addr_out->uint16[3] = 0;
    addr_out->uint16[4] = 0;
    addr_out->uint16[5] = HTONS(0x0001);
    addr_out->uint8[12] = 0xff;
    addr_out->uint8[13] = addr_in->uint8[13];
    addr_out->uint16[7] = addr_in->uint16[7];
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

uint32_t get_remaining_time(vtimer_t *t){
    timex_t time;
    time = vtimer_remaining(t);
    
    uint32_t seconds = time.nanoseconds / 1000000;
    
    return seconds;
}

void set_remaining_time(vtimer_t *t, uint32_t time){
    t->absolute.nanoseconds = time * 1000000;
}
