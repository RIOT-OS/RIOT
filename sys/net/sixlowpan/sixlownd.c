#include "sixlowip.h"
#include "sixlownd.h"
#include "sixlowmac.h"
#include <stdio.h>
#include <string.h>

uint8_t rtr_sol_count;
uint8_t option_field_length;
uint8_t ipv6_ext_hdr_len = 0;
uint16_t packet_length;

#define ipv6_buf ((struct ipv6_hdr*)&buffer[LL_HDR_LEN])  
#define icmp_buf ((struct icmpv6_hdr*)&buffer[LLHDR_IPV6HDR_LEN])
/* fields after icmp header*/
#define rtr_adv_buf ((struct rtr_adv*)&buffer[LLHDR_ICMPV6HDR_LEN])
#define iph_llh_buf &buffer[LL_HDR_LEN + IPV6_HDR_LEN]
/* neighbour discovery options rfc4861 4.6 */
#define opt_stllao_buf ((struct opt_stllao*)&buffer[LLHDR_IPV6HDR_LEN + option_field_length])
#define opt_mtu_buf ((struct opt_mtu*)&buffer[LLHDR_IPV6HDR_LEN + option_field_length])
#define opt_pi_buf ((struct opt_pi*)&buffer[LLHDR_IPV6HDR_LEN + option_field_length])

/* send router solicitation message - RFC4861 section 4.1 */
void send_rtr_sol(void){
      //uint8_t ipv6_ext_hdr_len = 0;
    if(rtr_sol_count < RTR_SOL_MAX){
        packet_length = 0;

        icmp_buf->type = ICMP_RTR_SOL;
        icmp_buf->code = 0;

        ipv6_buf->version_trafficclass = IPV6_VER;
        ipv6_buf->trafficclass_flowlabel = 0;
        ipv6_buf->flowlabel = 0;
        ipv6_buf->nextheader = ICMPV6_NXT_HDR;
        ipv6_buf->hoplimit = ND_HOPLIMIT;

        create_all_routers_mcast_addr(&ipv6_buf->destaddr);

        print6addr(&ipv6_buf->destaddr);

        /* set payload length */
        ipv6_buf->length = ICMPV6_HDR_LEN + RTR_SOL_LEN + OPT_STLLAO_LEN; 
        set_llao(&buffer[LLHDR_ICMPV6HDR_LEN + RTR_SOL_LEN], OPT_SLLAO_TYPE);
        icmp_buf->checksum = 0;
        icmp_buf->checksum = ~chksum_calc(ICMPV6_NXT_HDR);
        printf("%x\n",icmp_buf->checksum);

        rtr_sol_count++;
        // sleep 4 sec
        swtimer_usleep(RTR_SOL_INTERVAL * 1000000);
    } 
}

void recv_rtr_sol(void){
    option_field_length = RTR_SOL_LEN;
    uint8_t *llao;
    /* get link layer address option from buf */
    if(opt_stllao_buf->type == OPT_STLLAO_LEN){
        llao = opt_stllao_buf;
    }

    if(llao != NULL){
        // TODO: Neighbor lookup        
    }

    send_rtr_adv();        
}

void send_rtr_adv(ipv6_addr *addr){
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = ICMPV6_NXT_HDR;
    ipv6_buf->hoplimit = ND_HOPLIMIT;
    // not solicited
    create_all_nodes_mcast_addr(&ipv6_buf->destaddr);
    
    icmp_buf->type = ICMP_RTR_ADV;
    icmp_buf->code = 0;
    
    //TODO: gethoplimit func, set current ttl 
    rtr_adv_buf->hoplimit = MULTIHOP_HOPLIMIT;
    // set M and O flag, last 6 bits are zero
    rtr_adv_buf->autoconfig_flags = (RTR_ADV_M_FLAG << 7) | (RTR_ADV_O_FLAG << 6);
    rtr_adv_buf->router_lifetime = RTR_ADV_MAX_INTERVAL * RTR_ADV_MAX;
    rtr_adv_buf->reachable_time = 0;
    rtr_adv_buf->retrans_timer = 0;

    set_llao((uint8_t *)opt_stllao_buf,OPT_STLLAO_LEN);

    /* set MTU options */
    opt_mtu_buf->type = OPT_MTU_TYPE;
    opt_mtu_buf->length = OPT_MTU_LEN;
    opt_mtu_buf->reserved = 0;
    // 1500 octets mtu
    opt_mtu_buf->mtu = HTONL(1500);

    /* set packet length */
    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + 
                    RTR_ADV_LEN + OPT_STLLAO_LEN + OPT_MTU_HDR_LEN;
    /* set payload length field */

    /* set prefix option */
//    for(){
//    
//    }

    ipv6_buf->length = packet_length - IPV6_HDR_LEN;

    /* calculate checksum */
    icmp_buf->checksum = 0;
    icmp_buf->checksum = ~chksum_calc(ICMPV6_NXT_HDR);
}

/* link-layer address option - RFC4861 section 4.6.1/ RFC4944 8. */
void set_llao(uint8_t *llao, uint8_t type){
    llao[0] = type;
    if(OPT_STLLAO_LEN == 16){
        // 802.15.4 long address
        llao[1] = 2;
    } else
        // 16-bit short address
        llao[1] = 1;
    // get link layer address
    link_layer_addr *mac = get_eui(&ipv6_buf->srcaddr);
    //get_link_layer_addr_from_ipaddr(&ipv6_buf->srcaddr,&mac);    

    memcpy(&llao[2], mac, SIXLOWPAN_IPV6_LL_ADDR_LEN);
    // padding (all zeros) - 48bit 
    memset(&llao[2 + SIXLOWPAN_IPV6_LL_ADDR_LEN], 0, 
            OPT_STLLAO_LEN - SIXLOWPAN_IPV6_LL_ADDR_LEN - 2); 
}

/* pseudo-header checksum calculation - RFC4443 Section 2.3*/
uint16_t chksum_calc(uint8_t type){
    uint16_t length = ipv6_buf->length;
    uint16_t sum = length + type;
    
    uint8_t *addrptr = (uint8_t)&ipv6_buf->srcaddr;
    uint16_t addr_fields_length = (2 * sizeof(ipv6_addr));
    while(addr_fields_length > 1){
        sum += (addrptr[0] << 8) + addrptr[1];
        addrptr += 2;
        addr_fields_length -= 2;
    }    
    if(addr_fields_length > 1){
        sum += (addrptr[0] << 8) + 0;
    }

    uint8_t *bufptr = iph_llh_buf;
    while(length > 1){
        sum += (bufptr[0] << 8) + bufptr[1];
        bufptr += 2;
        length -= 2;
    } 
    if(length){
        sum += (bufptr[0] << 8) + 0;
    }

    return sum; 
}
