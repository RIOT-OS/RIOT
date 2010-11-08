#include "sixlowip.h"
#include "sixlownd.h"
#include "sixlowmac.h"
#include <stdio.h>

#define PRINTF(...) printf(__VA_ARGS__) 
#define PRINT6ADDR(addr) PRINTF("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])

static uint8_t option_field_length;
uint8_t ipv6_ext_hdr_len = 0;
uint16_t packet_length;

#define IP_BUFFER ((struct ipv6_hdr*)&buffer[LL_HEADER_LENGTH])  
#define ICMP_BUFFER ((struct icmpv6_hdr*)&buffer[LLHDR_IPV6HDR_LENGTH])
/* fields after icmp header*/
#define RA_BUFFER ((struct router_advertisement*)&buffer[LLHDR_ICMPV6HDR_LENGTH])
#define IPH_LLH_BUFFER &buffer[LL_HEADER_LENGTH + IPV6_HEADER_LENGTH]
#define OPT_ST_FIELD_BUFFER ((struct option_src_trgt_lla*)&buffer[LLHDR_IPV6HDR_LENGTH + option_field_length])
#define OPT_MTU_BUFFER ((struct option_mtu*)&buffer[LLHDR_IPV6HDR_LENGTH + option_field_length])

uint8_t rs_count;

option_prefix_info *prefix; /** prefix list pointer */

/* send router solicitation message - RFC4861 section 4.1 */
void send_rs(void){
      //uint8_t ipv6_ext_hdr_len = 0;
    if(rs_count < MAX_RTR_SOL){
        packet_length = 0;

        ICMP_BUFFER->type = ICMP_RTR_SOL;
        ICMP_BUFFER->code = 0;

        IP_BUFFER->version_trafficclass = IPV6_VERSION;
        IP_BUFFER->trafficclass_flowlabel = 0;
        IP_BUFFER->flowlabel = 0;
        IP_BUFFER->nextheader = ICMP_NEXTHEADER;
        IP_BUFFER->hoplimit = NEIGHBOR_DISCOVERY_HOPLIMIT;

        create_all_routers_mcast_addr(&IP_BUFFER->destaddr);
        PRINT6ADDR(&IP_BUFFER->destaddr);
        /* set payload length */
        IP_BUFFER->length = ICMP_HEADER_LENGTH + RTR_SOL_LENGTH + SLLAO_OPT_LENGTH; 
        set_llao(&buffer[LLHDR_ICMPV6HDR_LENGTH + RTR_SOL_LENGTH], SLLAO_OPT_LENGTH);
        ICMP_BUFFER->checksum = 0;
        ICMP_BUFFER->checksum = ~chksum_calc(ICMP_NEXTHEADER);
        printf("%x\n",ICMP_BUFFER->checksum);

        rs_count++;
        // sleep 4 sec
        swtimer_usleep(RTR_SOL_INTERVAL * 1000000);
    } 
}

void recv_rs(void){
    option_field_length = RTR_SOL_LENGTH;
    uint8_t *llao;
    /* get link layer address option from buf */
    if(OPT_ST_FIELD_BUFFER->type == SLLAO_OPT_LENGTH){
        llao = OPT_ST_FIELD_BUFFER;
    }

    if(llao != NULL){
        // TODO: Neighbor lookup        
    }

    send_ra();        
}

void send_ra(ipv6_addr *addr){
    IP_BUFFER->version_trafficclass = IPV6_VERSION;
    IP_BUFFER->trafficclass_flowlabel = 0;
    IP_BUFFER->flowlabel = 0;
    IP_BUFFER->nextheader = ICMP_NEXTHEADER;
    IP_BUFFER->hoplimit = NEIGHBOR_DISCOVERY_HOPLIMIT;
    // not solicited
    create_all_nodes_mcast_addr(&IP_BUFFER->destaddr);
    
    ICMP_BUFFER->type = ICMP_RTR_ADV;
    ICMP_BUFFER->code = 0;
    
    // set current ttl 
    RA_BUFFER->hoplimit = MULTIHOP_HOPLIMIT;
    // set M and O flag, last 6 bits are zero
    RA_BUFFER->autoconfig_flags = (ND_M_FLAG << 7) | (ND_O_FLAG << 6);
    RA_BUFFER->router_lifetime = MAX_RTR_ADV_INTERVAL * MAX_RTR_ADV;
    RA_BUFFER->reachable_time = 0;
    RA_BUFFER->retrans_timer = 0;

    set_llao((uint8_t *)OPT_ST_FIELD_BUFFER,SLLAO_OPT_LENGTH);

    /* set MTU options */
    OPT_MTU_BUFFER->type = MTU_OPTION_TYPE;
    OPT_MTU_BUFFER->length = MTU_OPTION_LENGTH;
    OPT_MTU_BUFFER->reserved = 0;
    // 1500 octets mtu
    OPT_MTU_BUFFER->mtu = HTONL(1500);

    /* set packet length */
    packet_length = IPV6_HEADER_LENGTH + ICMP_HEADER_LENGTH + 
                    RTR_ADV_LENGTH + SLLAO_OPT_LEN + MTU_OPTION_HDR_LENGTH;
    /* set payload length field */

    /* set prefix option */
    for(){
    
    }

    IP_BUFFER->length = packet_length - IPV6_HEADER_LENGTH;

    /* calculate checksum */
    ICMP_BUFFER->checksum = 0;
    ICMP_BUFFER->checksum = ~chksum_calc(ICMP_NEXTHEADER);
}

/* link-layer address option - RFC4861 section 4.6.1/ RFC4944 8. */
void set_llao(uint8_t *llao, uint8_t type){
    llao[ND_OPT_TYPE] = type;
    if(SLLAO_OPT_LENGTH == 16){
        // 802.15.4 long address
        llao[ND_OPT_LENGTH] = 2;
    } else
        // 16-bit short address
        llao[ND_OPT_LENGTH] = 1;
    // get link layer address
    link_layer_addr *mac = get_eui(&IP_BUFFER->srcaddr);
    //get_link_layer_addr_from_ipaddr(&IP_BUFFER->srcaddr,&mac);    

    memcpy(&llao[ND_OPT_DATA], mac, SIXLOWPAN_IPV6_LL_ADDR_LEN);
    // padding (all zeros) - 48bit 
    memset(&llao[ND_OPT_DATA + SIXLOWPAN_IPV6_LL_ADDR_LEN], 0, 
            SLLAO_OPT_LENGTH - SIXLOWPAN_IPV6_LL_ADDR_LEN - 2); 
}

/* pseudo-header checksum calculation - RFC4443 Section 2.3*/
uint16_t chksum_calc(uint8_t type){
    uint16_t length = IP_BUFFER->length;
    uint16_t sum = length + type;
    
    uint8_t *addrptr = (uint8_t)&IP_BUFFER->srcaddr;
    uint16_t addr_fields_length = (2 * sizeof(ipv6_addr));
    while(addr_fields_length > 1){
        sum += (addrptr[0] << 8) + addrptr[1];
        addrptr += 2;
        addr_fields_length -= 2;
    }    
    if(addr_fields_length > 1){
        sum += (addrptr[0] << 8) + 0;
    }

    uint8_t *bufptr = IPH_LLH_BUFFER;
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
