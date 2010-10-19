#include "sixlowip.h"
#include "sixlownd.h"
#include "sixlowmac.h"
#include <stdio.h>

#define PRINTF(...) printf(__VA_ARGS__) 
#define PRINT6ADDR(addr) PRINTF("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])

static uint8_t option_field_length;
uint8_t ipv6_ext_hdr_len = 0;

#define IP_BUFFER ((struct ipv6_hdr*)&buffer[LL_HEADER_LENGTH])  
#define ICMP_BUFFER ((struct icmpv6_hdr*)&buffer[LLHDR_IPV6HDR_LENGTH])
#define IPH_LLH_BUFFER &buffer[LL_HEADER_LENGTH + IPV6_HEADER_LENGTH]
#define OPT_FIELD_BUFFER ((struct option_hdr*)&buffer[LLHDR_IPV6HDR_LENGTH + option_field_length])

uint8_t rs_count;

/* send router solicitation message - RFC4861 section 4.1 */
void send_rs(void){
      //uint8_t ipv6_ext_hdr_len = 0;
    if(rs_count < MAX_RTR_SOLICITATIONS){
        ICMP_BUFFER->type = ICMP_ROUTER_SOLICITATION;
        ICMP_BUFFER->code = 0;

        IP_BUFFER->version_trafficclass = IPV6_VERSION;
        IP_BUFFER->trafficclass_flowlabel = 0;
        IP_BUFFER->flowlabel = 0;
        IP_BUFFER->nextheader = ICMP_NEXTHEADER;
        IP_BUFFER->hoplimit = NEIGHBOR_DISCOVERY_HOPLIMIT;

        create_all_routers_mcast_addr(&IP_BUFFER->destaddr);
        PRINT6ADDR(&IP_BUFFER->destaddr);
        /* set payload length */
        IP_BUFFER->length = ICMP_HEADER_LENGTH + RS_LENGTH + SLLAO_OPT_LENGTH; 
        set_llao(&buffer[LLHDR_ICMPV6HDR_LENGTH + RS_LENGTH], SLLAO_OPT_LENGTH);
        ICMP_BUFFER->checksum = 0;
        ICMP_BUFFER->checksum = ~chksum_calc(ICMP_NEXTHEADER);
        printf("%x\n",ICMP_BUFFER->checksum);

        rs_count++;
        // sleep 4 sec
        swtimer_usleep(ROUTER_SOLICITATION_INTERVAL * 1000000);
    } 
}

void recv_rs(void){
    option_field_length = RS_LENGTH;
    uint8_t *llao;
    /* get link layer address option from buf */
    if(OPT_FIELD_BUFFER->type == SLLAO_OPT_LENGTH){
        llao = OPT_FIELD_BUFFER;
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

    create_all_nodes_mcast_addr(&IP_BUFFER->destaddr);
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
