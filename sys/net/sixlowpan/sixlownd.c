/* 6LoWPAN/IPv6 Neighbor Discovery implementation*/

#include "sixlowip.h"
#include "sixlownd.h"
#include <stdio.h>

#define (struct *ipv6_hdr)&buffer[LL_HEADER_LENGTH]  

/* send router solicitation message - RFC4861 section 4.1 */
void send_rs(icmpv6_hdr *icmphdr, ipv6_hdr *iphdr){
    create_all_routers_multicast_addr(iphdr->destaddr);

    icmphdr->type = ICMP_ROUTER_SOLICITATION;
    icmphdr->code = 0;
    icmphdr->checksum = calc_icmpv6_checksum();

    iphdr->version_trafficclass = IPV6_VERSION;
    iphdr->trafficclass_flowlabel = 0;
    iphdr->flowlabel = 0;
    iphdr->nextheader = ICMP_NEXTHEADER;
    iphdr->hoplimit = NEIGHBOR_DISCOVERY_HOPLIMIT;

    /* icmp header len + rs header len + slla option len*/
    iphdr->length = ICMP_HEADER_LEN + RS_LEN + LLOA_OPT_LEN; 
    set_llao(); 
}

void set_llao(void){
    
}

void recv_rs(void){

}

void calc_icmpv6_checksum(void){

}
