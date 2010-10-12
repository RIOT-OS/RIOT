/* 6LoWPAN/IPv6 Neighbor Discovery implementation*/

#include "sixlowip.h"
#include <stdio.h>

/* send router solicitation message - RFC4861 section 4.1 */
void send_rs(icmp_hdr *icmphdr, ip_hdr *iphdr){
    create_all_routers_multicast_addr();

    icmphdr->type = ICMP_ROUTER_SOLICITATION;
    icmphdr->code = 0;
    icmphdr->checksum = calc_icmpv6_checksum();

    iphdr->version_trafficclass = IPV6_VERSION;
    iphdr->trafficclass_flowlabel = 0;
    iphdr->flowlabel = 0;
    iphdr->nextheader = ICMP_NEXTHEADER;
    iphdr->hoplimit = NEIGHBOR_DISCOVERY_HOPLIMIT;
     
}

void recv_rs(void){

}

void calc_icmpv6_checksum(void){

}
