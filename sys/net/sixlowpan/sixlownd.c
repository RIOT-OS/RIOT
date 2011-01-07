#include "sixlowip.h"
#include "sixlownd.h"
#include "sixlowmac.h"
#include <stdio.h>
#include <string.h>

uint8_t rtr_sol_count = 0;
uint8_t opt_hdr_len = 0;
uint8_t ipv6_ext_hdr_len = 0;
uint16_t packet_length;

static struct rtr_adv_t* get_rtr_adv_buf(uint8_t ext_len){
    return ((struct rtr_adv_t*)&(buffer[LLHDR_ICMPV6HDR_LEN + ext_len]));
}

static struct nbr_sol_t* get_nbr_sol_buf(uint8_t ext_len){
    return ((struct nbr_sol_t*)&(buffer[LLHDR_ICMPV6HDR_LEN + ext_len]));
}

static struct opt_stllao_t* get_opt_stllao_buf(uint8_t ext_len, uint8_t opt_len){
    return ((struct opt_stllao_t*)&(buffer[LLHDR_ICMPV6HDR_LEN + 
                                           ext_len + opt_len]));
}

static struct opt_mtu_t* get_opt_mtu_buf(uint8_t ext_len, uint8_t opt_len){
    return ((struct opt_mtu_t*)&(buffer[LLHDR_ICMPV6HDR_LEN + 
                                        ext_len + opt_len]));
}

static struct opt_pi_t* get_opt_pi_buf(uint8_t ext_len, uint8_t opt_len){
    return ((struct opt_pi_t*)&(buffer[LLHDR_ICMPV6HDR_LEN + 
                                       ext_len + opt_len]));
}

static struct opt_aro_t* get_opt_aro_buf(uint8_t ext_len, uint8_t opt_len){
    return ((struct opt_aro_t*)&(buffer[LLHDR_ICMPV6HDR_LEN + 
                                        ext_len + opt_len]));
}

/* data stuctures  */
// TODO: prefix list size initialwert herausfinden
pfx_elem_t plist[OPT_PI_LIST_LEN];

/* pointer */
static pfx_elem_t *pelem;
static uint8_t *llao;

static struct ipv6_hdr_t *ipv6_buf;
static struct icmpv6_hdr_t *icmp_buf;
static struct rtr_adv_t *rtr_adv_buf;
static struct nbr_sol_t *nbr_sol_buf;
static struct opt_stllao_t *opt_stllao_buf;
static struct opt_mtu_t *opt_mtu_buf;
static struct opt_pi_t *opt_pi_buf;
static struct opt_aro_t *opt_aro_buf;


/* send router solicitation message - RFC4861 section 4.1 */
void init_rtr_sol(void){
    ipv6_buf = get_ipv6_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len); 
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
        
//        print6addr(&ipv6_buf->destaddr);
        
        opt_hdr_len = RTR_SOL_LEN;
        ipv6_buf->length = ICMPV6_HDR_LEN + RTR_SOL_LEN + OPT_STLLAO_LEN; 
        
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, opt_hdr_len);
        set_llao((uint8_t*)opt_stllao_buf, OPT_SLLAO_TYPE);

        packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + RTR_SOL_LEN + OPT_STLLAO_LEN;

        icmp_buf->checksum = 0;
        
        icmp_buf->checksum = ~icmpv6_csum(ICMPV6_NXT_HDR);
//        printf("%x\n",icmp_buf->checksum);

        rtr_sol_count++;
        // sleep 4 sec
        //swtimer_usleep(RTR_SOL_INTERVAL * 1000000);
    } 
}

void recv_rtr_sol(void){
    opt_hdr_len = RTR_SOL_LEN;
    /* get link layer address option from buf */
    if(opt_stllao_buf->type == OPT_STLLAO_LEN){
        llao = (uint8_t*)opt_stllao_buf;
    }

    if(llao != NULL){
        // TODO: Neighbor lookup        
    }

    //send_rtr_adv();        
}

void init_rtr_adv(ipv6_addr_t *addr){
    ipv6_buf = get_ipv6_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);    

    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = ICMPV6_NXT_HDR;
    ipv6_buf->hoplimit = ND_HOPLIMIT;
    /* not solicited */
    create_all_nodes_mcast_addr(&ipv6_buf->destaddr);
    
    icmp_buf->type = ICMP_RTR_ADV;
    icmp_buf->code = 0;
    
    //TODO: gethoplimit func, set current ttl
    
    rtr_adv_buf = get_rtr_adv_buf(ipv6_ext_hdr_len);
    rtr_adv_buf->hoplimit = MULTIHOP_HOPLIMIT;
    /* set M and O flag, last 6 bits are zero */
    rtr_adv_buf->autoconfig_flags = (RTR_ADV_M_FLAG << 7) | (RTR_ADV_O_FLAG << 6);
    rtr_adv_buf->router_lifetime = RTR_ADV_MAX_INTERVAL * RTR_ADV_MAX;
    rtr_adv_buf->reachable_time = 0;
    rtr_adv_buf->retrans_timer = 0;
    opt_hdr_len = RTR_ADV_LEN;
    
    /* set link layer address option */
    opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, opt_hdr_len);
    set_llao((uint8_t *)opt_stllao_buf,OPT_STLLAO_LEN);
    opt_hdr_len += OPT_STLLAO_LEN;

    /* set MTU options */
    opt_mtu_buf = get_opt_mtu_buf(ipv6_ext_hdr_len, opt_hdr_len);
    opt_mtu_buf->type = OPT_MTU_TYPE;
    opt_mtu_buf->length = OPT_MTU_LEN;
    opt_mtu_buf->reserved = 0;
    /* 1500 octets mtu */
    opt_mtu_buf->mtu = HTONL(1500);
    opt_hdr_len += OPT_MTU_HDR_LEN;

    /* set packet length */
    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + 
                    RTR_ADV_LEN + OPT_STLLAO_LEN + OPT_MTU_HDR_LEN;
    /* set payload length field */

    /* set prefix option */
    opt_pi_buf = get_opt_pi_buf(ipv6_ext_hdr_len, opt_hdr_len);
    for(int i=0;i<OPT_PI_LIST_LEN; i++){
          if(plist[i].inuse && plist[i].adv){
                opt_pi_buf->addr = plist[i].addr;
                opt_pi_buf->type = OPT_PI_TYPE;
                opt_pi_buf->length = OPT_PI_LEN;
                opt_pi_buf->prefix_length = plist[i].length;
                opt_pi_buf->l_a_reserved1 = plist[i].l_a_reserved1;
                opt_pi_buf->val_ltime = HTONL(pelem->val_ltime);
                opt_pi_buf->pref_ltime = HTONL(pelem->val_ltime);
                opt_pi_buf->reserved2 = 0;
                packet_length += OPT_PI_HDR_LEN;
                opt_hdr_len += OPT_PI_HDR_LEN;
          }
    }

    ipv6_buf->length = packet_length - IPV6_HDR_LEN;

    /* calculate checksum */
    icmp_buf->checksum = 0;
    icmp_buf->checksum = ~icmpv6_csum(ICMPV6_NXT_HDR);
    //printf("%x\n",icmp_buf->checksum);
}

void init_nbr_sol(ipv6_addr_t *src, ipv6_addr_t *dest, ipv6_addr_t *targ){
    ipv6_ext_hdr_len = 0; 
    ipv6_buf = get_ipv6_buf();
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = ICMPV6_NXT_HDR;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    if(src == NULL){
        // TODO: src auswaehlen
    } else{
        ipv6_buf->srcaddr = *src;
    } 
    if(dest == NULL){
        // TODO: solicited knoten erstellen
    } else{
        ipv6_buf->destaddr = *dest;
    }
    
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_buf->type = ICMP_NBR_SOL;
    icmp_buf->code = 0;
    
    nbr_sol_buf = get_nbr_sol_buf(ipv6_ext_hdr_len);    
    nbr_sol_buf->reserved = 0;
    nbr_sol_buf->tgtaddr = *targ;
    opt_hdr_len = NBR_SOL_LEN;
    
    /* set sllao option */
    opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, opt_hdr_len);
    set_llao((uint8_t*)opt_stllao_buf, OPT_SLLAO_TYPE);
    opt_hdr_len += OPT_STLLAO_LEN;
    
    /* set aro option */
    opt_aro_buf = get_opt_aro_buf(ipv6_ext_hdr_len, opt_hdr_len);
    opt_aro_buf->type = OPT_ARO_TYPE;
    opt_aro_buf->length = OPT_ARO_LEN;
    opt_aro_buf->status = 0;
    opt_aro_buf->reserved1 = 0;
    opt_aro_buf->reserved2 = 0;
    opt_aro_buf->eui64 = *get_eui(src);
    opt_hdr_len += OPT_ARO_HDR_LEN; 
    
    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + NBR_SOL_LEN + 
                    OPT_STLLAO_LEN + OPT_ARO_HDR_LEN;
    ipv6_buf->length = packet_length - IPV6_HDR_LEN;
}

void pfx_add(ipv6_addr_t *addr, uint8_t size, uint32_t val_ltime, 
             uint32_t pref_ltime, uint8_t adv_opt, uint8_t l_a_reserved1){
    if(pfx_chk_list(addr,size) == 1){
        pelem->inuse = 1;
        pelem->length = size;
        pelem->adv = adv_opt;
        pelem->l_a_reserved1 = l_a_reserved1;
        pelem->val_ltime = val_ltime;
        pelem->pref_ltime = pref_ltime;
        pelem->addr = *addr; 
    }   
}

uint8_t pfx_chk_list(ipv6_addr_t *addr, uint8_t size){
    pelem = NULL;
    for(int i=0 ; i < size ; i++){
        if(plist[i].inuse){
            if(pfx_cmp(&plist[i].addr, addr)){
                *pelem = plist[i];
                return 0;
            }
        } else {
            *pelem = plist[i];
        }
    }
    if(pelem){
        return 1;
    } 
    return 2;
}

uint8_t pfx_cmp(ipv6_addr_t *addr1, ipv6_addr_t *addr2){
    if(memcmp(addr1, addr2, 8) == 0){
        return 1;
    }
    return 0;
}

/* link-layer address option - RFC4861 section 4.6.1/ RFC4944 8. */
void set_llao(uint8_t *llao, uint8_t type){
    llao[0] = type;
    if(OPT_STLLAO_LEN == 16){
        // 802.15.4 long address
        llao[1] = 2;
    } else{
        // 16-bit short address
        llao[1] = 1;
    }
    // get link layer address
    ieee_802154_long_t *mac = get_eui(&ipv6_buf->srcaddr);
    //get_ieee_802154_long_t_from_ipaddr(&ipv6_buf->srcaddr,&mac);    

    memcpy(&llao[2], mac, SIXLOWPAN_IPV6_LL_ADDR_LEN);
    // padding (all zeros) - 48bit 
    memset(&llao[2 + SIXLOWPAN_IPV6_LL_ADDR_LEN], 0, 
            OPT_STLLAO_LEN - SIXLOWPAN_IPV6_LL_ADDR_LEN - 2); 
}


uint16_t csum(uint16_t sum, uint8_t *buf, uint16_t len){
    int count;
    uint16_t carry; 
   
    count = len >> 1;
    if(count){
        if(count){
            carry = 0;
            do {
                uint16_t t = (*buf << 8) + *(buf+1);
                count--;
                buf += 2;
                sum += carry;
                sum += t;
                carry = (t > sum);
            } while(count);
            sum += carry;
        }
    }
    if(len & 1){
        uint16_t u = (*buf << 8);
        sum += (*buf << 8);
        if(sum < u){
            sum++;
        }
    }

    return sum;
}


uint16_t icmpv6_csum(uint8_t proto){
    ipv6_buf = get_ipv6_buf();
    uint16_t sum;
    uint16_t len = ipv6_buf->length;
    sum = len + proto;
    sum = csum(sum, (uint8_t *)&ipv6_buf->srcaddr, 2 * sizeof(ipv6_addr_t));
    sum = csum(sum,(uint8_t*)get_icmpv6_buf(0),len);
    
    return (sum == 0) ? 0xffff : HTONS(sum);
}


