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

uint8_t * get_payload_buf(uint8_t ext_len){
    return &(buffer[LLHDR_IPV6HDR_LEN + ext_len]);
}

void sixlowpan_bootstrapping(void){
    
    init_rtr_sol(OPT_SLLAO);
}

void sixlowpan_send(ipv6_addr_t *addr, uint8_t *payload, uint16_t p_len){
    ipv6_buf = get_ipv6_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    packet_length = 0;

    icmp_buf->type = ICMP_RTR_SOL;
    icmp_buf->code = 0;
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = PROTO_NUM_NONE;
    ipv6_buf->hoplimit = MULTIHOP_HOPLIMIT;
    ipv6_buf->length = p_len;    

    memcpy(&(ipv6_buf->destaddr), addr, 16);
    ipv6_get_saddr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));

    uint8_t *p_ptr = get_payload_buf(ipv6_ext_hdr_len); 
    //*p_ptr = payload;

    memcpy(p_ptr,payload,p_len);

    packet_length = IPV6_HDR_LEN + p_len;

    output((ieee_802154_long_t*)&(ipv6_buf->destaddr.uint16[4]),(uint8_t*)ipv6_buf); 
}

void ipv6_process(void){
    msg m;
    msg_init_queue(msg_queue, IP_PKT_RECV_BUF_SIZE);

    while(1){
        msg_receive(&m);

        ipv6_buf = get_ipv6_buf();
        ipv6_buf = (struct ipv6_hdr_t*) m.content.ptr;
        
        //printf("INFO: packet received, source: ");
        //ipv6_print_addr(&ipv6_buf->srcaddr);
        /* identifiy packet */
        nextheader = &ipv6_buf->nextheader;
        
        switch(*nextheader) {
            case(PROTO_NUM_ICMPV6):{
                /* checksum test*/
                if(icmpv6_csum(PROTO_NUM_ICMPV6) != 0xffff){
                    printf("ERROR: wrong checksum\n");
                }
                icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
                switch(icmp_buf->type) {
                    case(ICMP_RTR_SOL):{
                        printf("INFO: packet type: icmp router solicitation\n");
                        /* processing router solicitation */
                        recv_rtr_sol();    
                        /* init solicited router advertisment*/
                        break;
                    }
                    case(ICMP_RTR_ADV):{
                        printf("INFO: packet type: icmp router advertisment\n");
                        /* processing router advertisment */
                        recv_rtr_adv();
                        /* init neighbor solicitation */
                        break;
                    }
                    case(ICMP_NBR_SOL):{
                        printf("INFO: packet type: icmp neighbor solicitation\n");
                        recv_nbr_sol();
                    }
                    default:
                        break;
                }            
                break;
            }
            case(PROTO_NUM_NONE):{
                //printf("Packet with no Header following the IPv6 Header received\n");
                uint8_t *ptr = get_payload_buf(ipv6_ext_hdr_len);
            }
            default:
                break;
        }
    }   
}

void ipv6_iface_add_addr(ipv6_addr_t *addr, uint8_t state, uint32_t val_ltime,
                         uint32_t pref_ltime, uint8_t type){
    if(iface_addr_list_count < IFACE_ADDR_LIST_LEN){
        memcpy(&(iface.addr_list[iface_addr_list_count].addr.uint8[0]), 
               &(addr->uint8[0]), 16);
        iface.addr_list[iface_addr_list_count].state = state;
        timex_t vtime;
        vtime.seconds = val_ltime;
        timex_t prlt;
        prlt.seconds = pref_ltime;
        vtimer_set_sec(&(iface.addr_list[iface_addr_list_count].val_ltime), vtime);
        vtimer_set_sec(&(iface.addr_list[iface_addr_list_count].pref_ltime), prlt);
        iface.addr_list[iface_addr_list_count].type = type;
        iface_addr_list_count++;
    }
}

addr_list_t * ipv6_iface_addr_match(ipv6_addr_t *addr){
    int i;
    for(i = 0; i < IFACE_ADDR_LIST_LEN; i++){
        if(memcmp(&(iface.addr_list[i].addr.uint8[0]),
                  &(addr->uint8[0]),16) == 0){
            return &(iface.addr_list[i]);
        }
    }
    return NULL;
}

addr_list_t * ipv6_iface_addr_prefix_eq(ipv6_addr_t *addr){
    int i;
    for(i = 0; i < IFACE_ADDR_LIST_LEN; i++){
        if(memcmp(&(iface.addr_list[i].addr.uint8[0]),
                  &(addr->uint8[0]), 8) == 0){
            return &(iface.addr_list[i]);
        }
    }
    return NULL;
}

void ipv6_iface_print_addrs(void){
    for(int i = 0; i < iface_addr_list_count; i++){
        ipv6_print_addr(&(iface.addr_list[i].addr));
    } 
}

void ipv6_init_addr_prefix(ipv6_addr_t *inout, ipv6_addr_t *prefix){
    inout->uint16[0] = prefix->uint16[0];
    inout->uint16[1] = prefix->uint16[1];
    inout->uint16[2] = prefix->uint16[2];
    inout->uint16[3] = prefix->uint16[3];

    memcpy(&(inout->uint8[8]),&(iface.laddr.uint8[0]), 8); 
}

void ipv6_set_prefix(ipv6_addr_t *inout, ipv6_addr_t *prefix){
    inout->uint16[0] = prefix->uint16[0];
    inout->uint16[1] = prefix->uint16[1];
    inout->uint16[2] = prefix->uint16[2];
    inout->uint16[3] = prefix->uint16[3];
    inout->uint16[4] = 0;
    inout->uint16[5] = 0;
    inout->uint16[6] = 0;
    inout->uint16[7] = 0;
}

void ipv6_set_all_rtrs_mcast_addr(ipv6_addr_t *ipaddr){
    ipaddr->uint16[0] = HTONS(0xff02);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0002);
}

void ipv6_set_all_nds_mcast_addr(ipv6_addr_t *ipaddr){
    ipaddr->uint16[0] = HTONS(0xff02);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0001);
}

void ipv6_get_saddr(ipv6_addr_t *src, ipv6_addr_t *dst){
    /* try to find best match if dest is not mcast or link local */
    int8_t itmp = -1;
    uint8_t tmp = 0; 
    uint8_t bmatch = 0;    

    if(!(ipv6_prefix_ll_match(dst)) && !(ipv6_prefix_mcast_match(dst))){
        for(int i = 0; i < IFACE_ADDR_LIST_LEN; i++){
            if(iface.addr_list[i].state == ADDR_STATE_PREFERRED){
                if(!(ipv6_prefix_ll_match(&(iface.addr_list[i].addr)))){
                    tmp = ipv6_get_addr_match(dst, &(iface.addr_list[i].addr));
                    if(tmp >= bmatch){
                        bmatch = tmp;
                        itmp = i;
                    }
                }
            }
        }
    } else {
        for(int j=0; j < IFACE_ADDR_LIST_LEN; j++){
            if((iface.addr_list[j].state == ADDR_STATE_PREFERRED) &&
                ipv6_prefix_ll_match(&(iface.addr_list[j].addr))){
                itmp = j;    
            } 
        }
    }

    if(itmp == -1){
        memset(src, 0, 16);
    } else {
        memcpy(src, &(iface.addr_list[itmp].addr), 16);
    }
}

uint8_t ipv6_get_addr_match(ipv6_addr_t *src, ipv6_addr_t *dst){
    uint8_t val, xor;
    for(int i = 0; i < 16; i++){
        /* if bytes are equal add 8 */
        if(src->uint8[i] == dst->uint8[i]){
            val += 8; 
        } else {
            xor = src->uint8[i] ^ dst->uint8[i];
            /* while bits from byte equal add 1 */
            for(int j = 0; j < 8; j++){
                if((xor & 0x80) == 0){
                    val++;
                    xor = xor << 1;
                } else {
                    break;
                }
            }
        }
    }
    return val;
}

void ipv6_set_ll_prefix(ipv6_addr_t *ipaddr){
    ipaddr->uint16[0] = HTONS(0xfe80);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
}

void ipv6_init_address(ipv6_addr_t *addr, uint16_t addr0, uint16_t addr1,
                       uint16_t addr2, uint16_t addr3, uint16_t addr4,
                       uint16_t addr5, uint16_t addr6, uint16_t addr7){
    addr->uint16[0] = HTONS(addr0);
    addr->uint16[1] = HTONS(addr1);
    addr->uint16[2] = HTONS(addr2);
    addr->uint16[3] = HTONS(addr3);
    addr->uint16[4] = HTONS(addr4);
    addr->uint16[5] = HTONS(addr5);
    addr->uint16[6] = HTONS(addr6);
    addr->uint16[7] = HTONS(addr7);
}

uint8_t ipv6_prefix_ll_match(ipv6_addr_t *addr){
    if(addr->uint8[0] == 0xfe && addr->uint8[1] == 0x80){
        return 1;
    }
    return 0;
}

uint8_t ipv6_prefix_mcast_match(ipv6_addr_t *addr){
    if(addr->uint8[0] == 0xff && addr->uint8[1] == 0x02){
        return 1;
    }
    return 0;    
}

uint8_t ipv6_addr_unspec_match(ipv6_addr_t *addr){
    if((addr->uint16[0] == 0) && (addr->uint16[1] == 0) && 
       (addr->uint16[2] == 0) && (addr->uint16[3] == 0) &&
       (addr->uint16[4] == 0) && (addr->uint16[5] == 0) &&
       (addr->uint16[6] == 0) && (addr->uint16[7] == 0)){
        return 1;
    }
    return 0;
}

uint8_t ipv6_addr_sol_node_mcast_match(ipv6_addr_t *addr){
    /* note: cool if-condition*/
    if((addr->uint8[0] == 0xFF) && (addr->uint8[1] == 0x02) &&
       (addr->uint16[1] == 0x00) && (addr->uint16[2] == 0x00) &&
       (addr->uint16[3] == 0x00) && (addr->uint16[4] == 0x00) &&
       (addr->uint8[10] == 0x00) && (addr->uint8[11] == 0x01) &&
       (addr->uint8[12] == 0xFF)){
        return 1;
    }
    return 0;
}

void ipv6_set_sol_node_mcast_addr(ipv6_addr_t *addr_in, ipv6_addr_t *addr_out){
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

void ipv6_print_addr(ipv6_addr_t *ipaddr){
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
