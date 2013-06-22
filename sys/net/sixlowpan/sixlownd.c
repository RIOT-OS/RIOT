/*
 * 6lowpan neighbor discovery 
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlownd.c
 * @brief   6lowpan neighbor discovery functions 
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#include "sixlowip.h"
#include "sixlownd.h"
#include "sixlowmac.h"
#include "sixlowpan.h"
#include "sixlowerror.h"
#include "serialnumber.h"
#include "sys/net/net_help/net_help.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <vtimer.h>
#include <mutex.h>

#define ENABLE_DEBUG

/* extern variables */
uint8_t opt_hdr_len = 0;
uint8_t ipv6_ext_hdr_len = 0;
uint16_t packet_length;

/* counter */
uint8_t abr_count = 0;
uint8_t nbr_count = 0;
uint8_t def_rtr_count = 0;
uint8_t rtr_sol_count = 0;
uint8_t prefix_count = 0;

/* global interface*/
iface_t iface;

/* datastructures */
abr_cache_t abr_cache[ABR_CACHE_SIZE];
nbr_cache_t nbr_cache[NBR_CACHE_SIZE];
def_rtr_lst_t def_rtr_lst[DEF_RTR_LST_SIZE];
plist_t plist[OPT_PI_LIST_LEN];

/* pointer */
static uint8_t *llao;
addr_list_t *addr_list_ptr;

static struct ipv6_hdr_t *ipv6_buf;
static struct icmpv6_hdr_t *icmp_buf;
static struct rtr_adv_t *rtr_adv_buf;
static struct nbr_sol_t *nbr_sol_buf;
static struct nbr_adv_t *nbr_adv_buf;
static struct opt_buf_t *opt_buf;
static struct opt_stllao_t *opt_stllao_buf;
static struct opt_mtu_t *opt_mtu_buf;
static struct opt_abro_t *opt_abro_buf;
static struct opt_6co_hdr_t *opt_6co_hdr_buf;
static uint8_t *opt_6co_prefix_buf;
static struct opt_pi_t *opt_pi_buf;
static struct opt_aro_t *opt_aro_buf;

nbr_cache_t *nbr_entry;
def_rtr_lst_t *def_rtr_entry;

/* elements */
//ipv6_addr_t tmpaddr;

uint8_t recvd_cids[LOWPAN_CONTEXT_MAX];
uint8_t recvd_cids_len = 0;
plist_t *recvd_prefixes[OPT_PI_LIST_LEN];
uint8_t recvd_pref_len = 0;

static abr_cache_t *abr_get_most_current(void);
static abr_cache_t *abr_get_oldest(void);

int min(int a, int b)
{
    if(a < b) {
        return a;
    }
    else {
        return b;
    }
}

static struct para_prob_t *get_para_prob_buf(uint8_t ext_len)
{
    return ((struct para_prob_t *) & (buffer[LLHDR_ICMPV6HDR_LEN + ext_len]));
}

static struct rtr_adv_t *get_rtr_adv_buf(uint8_t ext_len)
{
    return ((struct rtr_adv_t *) & (buffer[LLHDR_ICMPV6HDR_LEN + ext_len]));
}

static struct nbr_sol_t *get_nbr_sol_buf(uint8_t ext_len)
{
    return ((struct nbr_sol_t *) & (buffer[LLHDR_ICMPV6HDR_LEN + ext_len]));
}

static struct nbr_adv_t *get_nbr_adv_buf(uint8_t ext_len)
{
    return ((struct nbr_adv_t *) & (buffer[LLHDR_ICMPV6HDR_LEN + ext_len]));
}

static struct opt_buf_t *get_opt_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((struct opt_buf_t *) & (buffer[LLHDR_ICMPV6HDR_LEN +
                                           ext_len + opt_len]));
}

static struct opt_stllao_t *get_opt_stllao_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((struct opt_stllao_t *)&(buffer[LLHDR_ICMPV6HDR_LEN +
                                       ext_len + opt_len]));
}

static struct opt_mtu_t *get_opt_mtu_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((struct opt_mtu_t *) & (buffer[LLHDR_ICMPV6HDR_LEN +
                                           ext_len + opt_len]));
}

static struct opt_abro_t *get_opt_abro_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((struct opt_abro_t *)&(buffer[LLHDR_ICMPV6HDR_LEN +
                                            ext_len + opt_len]));
}

static struct opt_6co_hdr_t *get_opt_6co_hdr_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((struct opt_6co_hdr_t *)&(buffer[LLHDR_ICMPV6HDR_LEN +
                                       ext_len + opt_len]));
}

static uint8_t *get_opt_6co_prefix_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((uint8_t *)&(buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]));
}

static struct opt_pi_t *get_opt_pi_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((struct opt_pi_t *)&(buffer[LLHDR_ICMPV6HDR_LEN + ext_len +
                                 opt_len]));
}

static struct opt_aro_t *get_opt_aro_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((struct opt_aro_t *)&(buffer[LLHDR_ICMPV6HDR_LEN + ext_len +
                                  opt_len]));
}

/* send router solicitation message - RFC4861 section 4.1 */
void init_rtr_sol(uint8_t sllao)
{
    ipv6_buf = get_ipv6_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);

    packet_length = 0;

    icmp_buf->type = ICMP_RTR_SOL;
    icmp_buf->code = 0;
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    ipv6_set_all_rtrs_mcast_addr(&ipv6_buf->destaddr);
    //iface_find_src_ipaddr(&ipv6_buf->srcaddr, ADDR_STATE_PREFERRED,
    /*                      ADDR_TYPE_MULTICAST); */

    ipv6_get_saddr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));

    opt_hdr_len = RTR_SOL_LEN;
    ipv6_buf->length = ICMPV6_HDR_LEN + RTR_SOL_LEN + OPT_STLLAO_MAX_LEN;

    if(sllao == OPT_SLLAO) {
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, opt_hdr_len);
        set_llao(opt_stllao_buf, OPT_SLLAO_TYPE, 2);
        packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + ipv6_ext_hdr_len +
                        RTR_SOL_LEN + OPT_STLLAO_MAX_LEN;
    }
    else {
        packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + ipv6_ext_hdr_len +
                        RTR_SOL_LEN;
    }

    icmp_buf->checksum = 0;
    icmp_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);

#ifdef ENABLE_DEBUG
    printf("INFO: send router solicitation to: ");
    ipv6_print_addr(&ipv6_buf->destaddr);
#endif
    lowpan_init((ieee_802154_long_t *)&(ipv6_buf->destaddr.uint16[4]),
                (uint8_t *)ipv6_buf);
}

void recv_rtr_sol(void)
{
    opt_hdr_len = RTR_SOL_LEN;
    ipv6_buf = get_ipv6_buf();

    /* check if source option is set*/
    if(opt_stllao_buf->type == OPT_SLLAO_TYPE) {
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, opt_hdr_len);
        llao = (uint8_t *)opt_stllao_buf;
        opt_hdr_len += (opt_stllao_buf->length) << 3;
    }

    if(llao != NULL) {
        nbr_entry = nbr_cache_search(&ipv6_buf->srcaddr);

        if(nbr_entry != NULL) {
            /* found neighbor in cache, update values and check long addr */
            if(memcmp(&llao[2], &nbr_entry->laddr, 8) == 0) {
                nbr_entry->isrouter = 0;
            }
            else {
                /* new long addr found, update */
                memcpy(&nbr_entry->laddr, &llao[2], 8);
                nbr_entry->state = NBR_STATUS_STALE;
                nbr_entry->isrouter = 0;
            }
        }
        else {
            /* nothing found, add neigbor into cache*/
            nbr_cache_add(&ipv6_buf->srcaddr, (ieee_802154_long_t *)&llao[2],
                          0, NBR_STATUS_STALE, NBR_CACHE_TYPE_TEN,
                          NBR_CACHE_LTIME_TEN, NULL);
        }
    }

    /* send solicited router advertisment */
    if(abr_count > 0) {
        init_rtr_adv(&ipv6_buf->srcaddr, 0, 0, OPT_PI, OPT_6CO, OPT_ABRO);
    }
    else {
        init_rtr_adv(&ipv6_buf->srcaddr, 0, 0, OPT_PI, 0, 0);
    }

#ifdef ENABLE_DEBUG
    printf("INFO: send router advertisment to: ");
    ipv6_print_addr(&ipv6_buf->destaddr);
#endif
    lowpan_init((ieee_802154_long_t *)&(ipv6_buf->destaddr.uint16[4]),
                (uint8_t *)ipv6_buf);

}

uint8_t set_opt_6co_flags(uint8_t compression_flag, uint8_t cid)
{
    uint8_t flags;

    if(compression_flag) {
        flags = OPT_6CO_FLAG_C;
    }
    else {
        flags = 0;
    }

    flags |= cid & OPT_6CO_FLAG_CID;
    return flags;
}

void get_opt_6co_flags(uint8_t *compression_flag, uint8_t *cid, uint8_t flags)
{
    compression_flag[0] = flags & OPT_6CO_FLAG_CID;
    compression_flag[0] = compression_flag[0] != 0;
    cid[0] = flags & OPT_6CO_FLAG_CID;
}

lowpan_context_t *abr_get_context(abr_cache_t *abr, uint8_t cid);

void init_rtr_adv(ipv6_addr_t *addr, uint8_t sllao, uint8_t mtu, uint8_t pi, 
                  uint8_t sixco, uint8_t abro)
{
    lowpan_context_t *contexts = NULL;

    abr_cache_t *msg_abr = NULL;
    ipv6_buf = get_ipv6_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);

    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    if(addr == NULL) {
        /* not solicited */
        ipv6_set_all_nds_mcast_addr(&ipv6_buf->destaddr);
    }
    else {
        memcpy(&ipv6_buf->destaddr, addr, 16);
    }

    ipv6_get_saddr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));

    icmp_buf->type = ICMP_RTR_ADV;
    icmp_buf->code = 0;

    //TODO: gethoplimit func, set current ttl

    rtr_adv_buf = get_rtr_adv_buf(ipv6_ext_hdr_len);
    rtr_adv_buf->hoplimit = MULTIHOP_HOPLIMIT;
    /* set M and O flag, last 6 bits are zero */
    rtr_adv_buf->autoconfig_flags = (RTR_ADV_M_FLAG << 7) | (RTR_ADV_O_FLAG << 6);
    rtr_adv_buf->router_lifetime = HTONS(RTR_ADV_MAX_INTERVAL * RTR_ADV_MAX);
    rtr_adv_buf->reachable_time = 0;
    rtr_adv_buf->retrans_timer = 0;
    opt_hdr_len = RTR_ADV_LEN;

    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + RTR_ADV_LEN;

    if(sllao == OPT_SLLAO) {
        /* set link layer address option */
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, opt_hdr_len);
        set_llao(opt_stllao_buf, OPT_SLLAO_TYPE, 2);
        opt_hdr_len += OPT_STLLAO_MAX_LEN;
        packet_length += OPT_STLLAO_MAX_LEN;
    }

    if(mtu == OPT_MTU) {
        /* set MTU options */
        opt_mtu_buf = get_opt_mtu_buf(ipv6_ext_hdr_len, opt_hdr_len);
        opt_mtu_buf->type = OPT_MTU_TYPE;
        opt_mtu_buf->length = OPT_MTU_LEN;
        opt_mtu_buf->reserved = 0;
        opt_mtu_buf->mtu = HTONL(1500);
        opt_hdr_len += OPT_MTU_HDR_LEN;
        packet_length += OPT_MTU_HDR_LEN;
    }

    /* set payload length field */

    if(abro == OPT_ABRO) {
        /* set authoritive border router option */
        if(abr_count > 0) {
            msg_abr = abr_get_most_current();
            opt_abro_buf = get_opt_abro_buf(ipv6_ext_hdr_len, opt_hdr_len);
            opt_abro_buf->type = OPT_ABRO_TYPE;
            opt_abro_buf->length = OPT_ABRO_LEN;
            opt_abro_buf->version = HTONS(msg_abr->version);
            opt_abro_buf->reserved = 0;
            memcpy(&(opt_abro_buf->addr), &(msg_abr->abr_addr), sizeof(ipv6_addr_t));
        }
    }

    if(sixco == OPT_6CO) {
        /* set 6lowpan context option */
        int contexts_len = 0;
        mutex_lock(&lowpan_context_mutex);

        if(msg_abr == NULL) {
            contexts = lowpan_context_get();
            contexts_len = lowpan_context_len();
        }
        else {
            lowpan_context_t c_tmp[LOWPAN_CONTEXT_MAX];

            contexts_len = 0;

            for(int i = 0; i < LOWPAN_CONTEXT_MAX; i++) {
                lowpan_context_t *ctx = abr_get_context(msg_abr, i);

                if(ctx != NULL) {
                    memcpy(&(c_tmp[contexts_len++]), ctx, sizeof(lowpan_context_t));
                }
            }

            contexts = (lowpan_context_t *)calloc(contexts_len, sizeof(lowpan_context_t));
            memcpy(contexts, c_tmp, contexts_len);
        }

        for(int i = 0; i < contexts_len; i++) {
            opt_6co_hdr_buf = get_opt_6co_hdr_buf(ipv6_ext_hdr_len, opt_hdr_len);
            opt_6co_hdr_buf->type = OPT_6CO_TYPE;

            if(contexts[i].length > 64) {
                opt_6co_hdr_buf->length = OPT_6CO_MAX_LEN;
            }
            else {
                opt_6co_hdr_buf->length = OPT_6CO_MIN_LEN;
            }

            opt_6co_hdr_buf->c_length = contexts[i].length;
            opt_6co_hdr_buf->c_flags = set_opt_6co_flags(contexts[i].comp, contexts[i].num);
            opt_6co_hdr_buf->reserved = 0;
            opt_6co_hdr_buf->val_ltime = HTONS(contexts[i].lifetime);

            opt_hdr_len += OPT_6CO_HDR_LEN;
            packet_length += OPT_6CO_HDR_LEN;
            /* attach prefixes */
            opt_6co_prefix_buf = get_opt_6co_prefix_buf(ipv6_ext_hdr_len, opt_hdr_len);

            if(opt_6co_hdr_buf->c_length > 64) {
                memset((void *)opt_6co_prefix_buf, 0, 16);
                memcpy((void *)opt_6co_prefix_buf, (void *) & (contexts[i].prefix.uint8[0]), opt_6co_hdr_buf->c_length / 8);
                opt_hdr_len += 16;
                packet_length += 16;
            }
            else {
                memset((void *)opt_6co_prefix_buf, 0, 8);
                memcpy((void *)opt_6co_prefix_buf, (void *) & (contexts[i].prefix.uint8[0]), opt_6co_hdr_buf->c_length / 8);
                opt_hdr_len += 8;
                packet_length += 8;
            }

        }

        if(msg_abr != NULL && contexts != NULL) {
            free(contexts);
        }

        mutex_unlock(&lowpan_context_mutex, 0);
    }

    if(pi == OPT_PI) {
        /* set prefix option */
        for(int i = 0; i < OPT_PI_LIST_LEN; i++) {
            if(plist[i].inuse && plist[i].adv) {
                opt_pi_buf = get_opt_pi_buf(ipv6_ext_hdr_len, opt_hdr_len);
                memcpy(&(opt_pi_buf->addr.uint8[0]), &(plist[i].addr.uint8[0]), 16);
                opt_pi_buf->type = OPT_PI_TYPE;
                opt_pi_buf->length = OPT_PI_LEN;
                opt_pi_buf->prefix_length = plist[i].length;
                opt_pi_buf->l_a_reserved1 = plist[i].l_a_reserved1;
                opt_pi_buf->val_ltime = HTONL(plist[i].val_ltime);
                opt_pi_buf->pref_ltime = HTONL(plist[i].pref_ltime);
                opt_pi_buf->reserved2 = 0;
                packet_length += OPT_PI_HDR_LEN;
                opt_hdr_len += OPT_PI_HDR_LEN;
            }
        }
    }

    ipv6_buf->length = packet_length - IPV6_HDR_LEN;

    /* calculate checksum */
    icmp_buf->checksum = 0;
    icmp_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);
}

void recv_rtr_adv(void)
{
    int8_t trigger_ns = -1;
    int8_t abro_found = 0;
    int16_t abro_version = 0;    /* later replaced, just to supress warnings */
    ipv6_addr_t abro_addr;

    ipv6_buf = get_ipv6_buf();
    opt_hdr_len = RTR_ADV_LEN;
    rtr_adv_buf = get_rtr_adv_buf(ipv6_ext_hdr_len);
    ipv6_addr_t newaddr;
    recvd_cids_len = 0;

    /* update interface reachable time and retrans timer */
    if(rtr_adv_buf->reachable_time != 0) {
        iface.adv_reachable_time = HTONL(rtr_adv_buf->reachable_time);
    }

    if(rtr_adv_buf->retrans_timer != 0) {
        iface.adv_retrans_timer = HTONL(rtr_adv_buf->retrans_timer);
    }

    def_rtr_entry = def_rtr_lst_search(&ipv6_buf->srcaddr);

    if(rtr_adv_buf->router_lifetime != 0) {
        if(def_rtr_entry != NULL) {
            set_remaining_time(&(def_rtr_entry->inval_time), HTONL(rtr_adv_buf->router_lifetime));
        }
        else {
            def_rtr_lst_add(&(ipv6_buf->srcaddr), HTONL(rtr_adv_buf->router_lifetime));
            trigger_ns = 1;
        }
    }
    else {
        /* remove router from default router list */
        if(def_rtr_entry != NULL) {
            def_rtr_lst_rem(def_rtr_entry);
        }
    }

    mutex_lock(&lowpan_context_mutex);

    /* read options */
    while(packet_length > IPV6HDR_ICMPV6HDR_LEN + opt_hdr_len) {
        opt_buf = get_opt_buf(ipv6_ext_hdr_len, opt_hdr_len);

        switch(opt_buf->type) {
            case(OPT_SLLAO_TYPE): {
                break;
            }

            case(OPT_MTU_TYPE): {
                break;
            }

            /* rfc 4862 section 5.5.3 */
            case(OPT_PI_TYPE): {
                opt_pi_buf = get_opt_pi_buf(ipv6_ext_hdr_len, opt_hdr_len);

                /* crazy condition, read 5.5.3a-b-c for further information */
                if(ipv6_prefix_ll_match(&opt_pi_buf->addr) ||
                   (HTONL(opt_pi_buf->pref_ltime) >
                    HTONL(opt_pi_buf->val_ltime))) {
                    break;
                }
                else {
                    /* check if on-link flag is set */
                    if(opt_pi_buf->l_a_reserved1 & OPT_PI_FLAG_L) {
                        /* TODO: do on-link pi handling */
                    }

                    if(opt_pi_buf->l_a_reserved1 & OPT_PI_FLAG_A) {
                        addr_list_ptr = ipv6_iface_addr_prefix_eq(&opt_pi_buf->addr);

                        if(addr_list_ptr == NULL) {
                            /* 5.5.3d */
                            if(opt_pi_buf->val_ltime != 0) {
                                /* iid will also be added here */
                                ipv6_init_addr_prefix(&newaddr, &opt_pi_buf->addr);
                                /* add into address list
                                * TODO: duplicate address detection is not
                                *       implementet yet, so all new addresse will
                                *       be added with state PREFFERED */
                                ipv6_iface_add_addr(&newaddr,
                                                    ADDR_STATE_PREFERRED,
                                                    opt_pi_buf->val_ltime,
                                                    opt_pi_buf->pref_ltime,
                                                    ADDR_CONFIGURED_AUTO);
                                printf("INFO: added address to interface\n");
                                trigger_ns = 1;
                            }
                        }
                        else {
                            /* 5.5.3e */
                            set_remaining_time(&(addr_list_ptr->pref_ltime), opt_pi_buf->pref_ltime);

                            /* 7200 = 2hours in seconds */
                            if(HTONL(opt_pi_buf->val_ltime) > 7200 ||
                               HTONL(opt_pi_buf->val_ltime) >
                               get_remaining_time(&(addr_list_ptr->val_ltime))) {
                                set_remaining_time(&(addr_list_ptr->val_ltime), HTONL(opt_pi_buf->val_ltime));
                            }
                            else {
                                /* reset valid lifetime to 2 hours */
                                set_remaining_time(&(addr_list_ptr->val_ltime), 7200);
                            }
                        }
                    }
                }

                /* TODO: save found prefixes */
                break;
            }

            case(OPT_6CO_TYPE): {
                uint8_t comp;
                uint8_t num;

                opt_6co_hdr_buf = get_opt_6co_hdr_buf(ipv6_ext_hdr_len, opt_hdr_len);

                get_opt_6co_flags(&comp, &num, opt_6co_hdr_buf->c_flags);

                ipv6_addr_t prefix;
                memset(&prefix, 0, 16);

                opt_6co_prefix_buf = get_opt_6co_prefix_buf(ipv6_ext_hdr_len, opt_hdr_len + OPT_6CO_HDR_LEN);

                memcpy(&prefix, opt_6co_prefix_buf, opt_6co_hdr_buf->c_length);

                lowpan_context_update(
                    num,
                    &prefix,
                    opt_6co_hdr_buf->c_length,
                    comp,
                    HTONS(opt_6co_hdr_buf->val_ltime)
                );
                recvd_cids[recvd_cids_len] = num;
                recvd_cids_len = (recvd_cids_len + 1) % LOWPAN_CONTEXT_MAX;
                break;
            }

            case(OPT_ABRO_TYPE): {
                opt_abro_buf = get_opt_abro_buf(ipv6_ext_hdr_len, opt_hdr_len);
                abro_found = 1;
                abro_version = HTONS(opt_abro_buf->version);
                memcpy(&(abro_addr), &(opt_abro_buf->addr), sizeof(ipv6_addr_t));
                break;
            }

            default:
                break;
        }

        /* multiplied with 8 because options length is in units of 8 bytes */
        opt_hdr_len += (opt_buf->length * 8);
    }

    if(abro_found) {
        int i;

        for(i = 0; i < recvd_cids_len; i++) {
            abr_add_context(abro_version, &abro_addr, recvd_cids[i]);
        }
    }

    mutex_unlock(&lowpan_context_mutex, 0);

    if(trigger_ns >= 0) {
        /* send ns - draft-ietf-6lowpan-nd-15#section-5.5.1
         *
         * section-10.2.4
         * "Next the 6LN registers that address with one or more of its
         * default routers by sending a unicast NS message with an ARO
         * containing its tentative global IPv6 address to register
         *
         * if new address was configured, set src to newaddr(gp16) */
        init_nbr_sol(&newaddr, &(ipv6_buf->srcaddr), &(ipv6_buf->srcaddr), OPT_SLLAO, OPT_ARO);
#ifdef ENABLE_DEBUG
        printf("INFO: send neighbor solicitation to: ");
        ipv6_print_addr(&(ipv6_buf->destaddr));
#endif
        lowpan_init((ieee_802154_long_t *) & (ipv6_buf->destaddr.uint16[4]), (uint8_t *)ipv6_buf);
    }
}

void init_nbr_sol(ipv6_addr_t *src, ipv6_addr_t *dest, ipv6_addr_t *targ,
                  uint8_t sllao, uint8_t aro)
{
    ipv6_buf = get_ipv6_buf();
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    if(dest == NULL) {
        ipv6_set_sol_node_mcast_addr(targ, &(ipv6_buf->destaddr));
    }
    else {
        memcpy(&(ipv6_buf->destaddr.uint8[0]), &(dest->uint8[0]), 16);
    }

    ipv6_ext_hdr_len = 0;
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_buf->type = ICMP_NBR_SOL;
    icmp_buf->code = 0;

    nbr_sol_buf = get_nbr_sol_buf(ipv6_ext_hdr_len);
    nbr_sol_buf->reserved = 0;
    memcpy(&(nbr_sol_buf->tgtaddr), targ, 16);
    opt_hdr_len = NBR_SOL_LEN;

    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + NBR_SOL_LEN;

    if(ipv6_iface_addr_match(targ) == NULL) {
        if(src == NULL) {
            ipv6_get_saddr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));
        }
        else {
            memcpy(&(ipv6_buf->srcaddr), src, 16);
        }

        if(sllao == OPT_SLLAO) {
            /* set sllao option */
            opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, opt_hdr_len);
            set_llao(opt_stllao_buf, OPT_SLLAO_TYPE, 1);
            opt_hdr_len += OPT_STLLAO_MIN_LEN;

            packet_length += OPT_STLLAO_MIN_LEN;
        }
    }

    if(aro == OPT_ARO) {
        /* set aro option */
        opt_aro_buf = get_opt_aro_buf(ipv6_ext_hdr_len, opt_hdr_len);
        opt_aro_buf->type = OPT_ARO_TYPE;
        opt_aro_buf->length = OPT_ARO_LEN;
        opt_aro_buf->status = 0;
        opt_aro_buf->reserved1 = 0;
        opt_aro_buf->reserved2 = 0;
        memcpy(&(opt_aro_buf->eui64), mac_get_eui(src), 8);
        opt_hdr_len += OPT_ARO_HDR_LEN;

        packet_length += OPT_ARO_HDR_LEN;
    }

    ipv6_buf->length = packet_length - IPV6_HDR_LEN;

    icmp_buf->checksum = 0;
    icmp_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);
}

void recv_nbr_sol(void)
{
    ipv6_buf = get_ipv6_buf();
    llao = NULL;
    opt_hdr_len = NBR_SOL_LEN;

    uint8_t send_na = 0;
    uint8_t sllao_set = 0;
    uint8_t aro_state = OPT_ARO_STATE_SUCCESS;

    /* check whick options are set, we need that because an aro
     * option condition is that a sllao option is set. thus that we don't
     * know which option comes first we need to this here */

    while(packet_length > IPV6HDR_ICMPV6HDR_LEN + opt_hdr_len) {
        opt_buf = get_opt_buf(ipv6_ext_hdr_len, opt_hdr_len);

        if(opt_buf->type == OPT_SLLAO_TYPE) {
            sllao_set = 1;
        }

        opt_hdr_len += (opt_buf->length * 8);
    }

    opt_hdr_len = NBR_SOL_LEN;

    while(packet_length > IPV6HDR_ICMPV6HDR_LEN + opt_hdr_len) {
        opt_buf = get_opt_buf(ipv6_ext_hdr_len, opt_hdr_len);

        switch(opt_buf->type) {
            case(OPT_SLLAO_TYPE): {
                opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len,
                                                    opt_hdr_len);
                llao = (uint8_t *)opt_stllao_buf;

                if(llao != NULL &&
                   !(ipv6_addr_unspec_match(&ipv6_buf->srcaddr))) {
                    nbr_entry = nbr_cache_search(&(ipv6_buf->srcaddr));

                    if(nbr_entry != NULL) {
                        switch(opt_stllao_buf->length) {
                            case(1): {
                                if(memcmp(&llao[2], &(nbr_entry->saddr), 2) == 0) {
                                    nbr_entry->isrouter = 0;
                                }
                                else {
                                    memcpy(&nbr_entry->saddr, &llao[2], 2);
                                    nbr_entry->state = NBR_STATUS_STALE;
                                    nbr_entry->isrouter = 0;
                                }

                                break;
                            }

                            case(2): {
                                if(memcmp(&llao[2], &(nbr_entry->laddr), 8) == 0) {
                                    nbr_entry->isrouter = 0;
                                }
                                else {
                                    memcpy(&nbr_entry->laddr, &llao[2], 8);
                                    nbr_entry->state = NBR_STATUS_STALE;
                                    nbr_entry->isrouter = 0;
                                }

                                break;
                            }

                            default:
                                break;
                        }
                    }
                    else {
                        switch(opt_stllao_buf->length) {
                            case(1): {
                                nbr_cache_add(&ipv6_buf->srcaddr,
                                              NULL , 0, NBR_STATUS_STALE,
                                              NBR_CACHE_TYPE_TEN,
                                              NBR_CACHE_LTIME_TEN,
                                              (ieee_802154_short_t *)&llao[2]);

                                break;
                            }

                            case(2): {
                                nbr_cache_add(&ipv6_buf->srcaddr,
                                              (ieee_802154_long_t *)&llao[2], 0,
                                              NBR_STATUS_STALE,
                                              NBR_CACHE_TYPE_TEN,
                                              NBR_CACHE_LTIME_TEN, NULL);
                                break;
                            }

                            default:
                                break;
                        }
                    }
                }

                break;
            }

            case(OPT_ARO_TYPE): {
                /* check if sllao option is set, and if address src address
                 * isn't unspecified - draft-ietf-6lowpan-nd-15#section-6.5 */
                if(!(ipv6_addr_unspec_match(&ipv6_buf->srcaddr)) &&
                   sllao_set == 1) {
                    opt_aro_buf = get_opt_aro_buf(ipv6_ext_hdr_len,
                                                  opt_hdr_len);

                    if((opt_aro_buf->length == 2) &&
                       (opt_aro_buf->status == 0)) {
                        /* check neighbor cache for duplicates */
                        nbr_entry = nbr_cache_search(&(ipv6_buf->srcaddr));

                        if(nbr_entry == NULL) {
                            /* create neighbor cache */
                            aro_state = nbr_cache_add(&ipv6_buf->srcaddr,
                                                      &(opt_aro_buf->eui64), 0,
                                                      NBR_STATUS_STALE, NBR_CACHE_TYPE_TEN,
                                                      opt_aro_buf->reg_ltime, NULL);
                        }
                        else {
                            if(memcmp(&(nbr_entry->addr.uint16[4]),
                                      &(opt_aro_buf->eui64.uint16[0]), 8) == 0) {
                                /* update neighbor cache entry */
                                if(opt_aro_buf->reg_ltime == 0) {
                                    /* delete neighbor cache entry */
                                    nbr_cache_rem(&nbr_entry->addr);
                                }
                                else {
                                    set_remaining_time(&(nbr_entry->ltime), (uint32_t)opt_aro_buf->reg_ltime);
                                    nbr_entry->state = NBR_STATUS_STALE;
                                    nbr_entry->isrouter = 0;
                                    memcpy(&(nbr_entry->addr.uint8[0]),
                                           &(ipv6_buf->srcaddr.uint8[0]), 16);
                                }

                                aro_state = OPT_ARO_STATE_SUCCESS;
                            }
                            else {
                                /* duplicate found */
                                aro_state = OPT_ARO_STATE_DUP_ADDR;
                            }
                        }
                    }
                }

                break;
            }

            default:
                break;
        }

        opt_hdr_len += (opt_buf->length * 8);
    }

    addr_list_t *alist_targ, *alist_dest;

    nbr_sol_buf = get_nbr_sol_buf(ipv6_ext_hdr_len);
    alist_targ = ipv6_iface_addr_match(&(nbr_sol_buf->tgtaddr));

    if(alist_targ != NULL) {
        alist_dest = ipv6_iface_addr_match(&(ipv6_buf->destaddr));

        if((memcmp(&(alist_targ->addr), &(alist_dest->addr), 16) == 0) ||
           ipv6_addr_sol_node_mcast_match(&ipv6_buf->destaddr)) {
            memcpy(&(ipv6_buf->destaddr.uint8[0]),
                   &(ipv6_buf->srcaddr.uint8[0]), 16);
            memcpy(&(ipv6_buf->srcaddr.uint8[0]),
                   &(nbr_sol_buf->tgtaddr.uint8[0]), 16);
            send_na = 1;
        }
    }

    if(send_na) {
        /* solicited na */
        uint8_t flags = (NBR_ADV_FLAG_O | NBR_ADV_FLAG_S);
        init_nbr_adv(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr),
                     &(alist_targ->addr), flags, 0, OPT_ARO, aro_state);
#ifdef ENABLE_DEBUG
        printf("INFO: send neighbor advertisment to: ");
        ipv6_print_addr(&ipv6_buf->destaddr);
#endif
        lowpan_init((ieee_802154_long_t *) & (ipv6_buf->destaddr.uint16[4]), (uint8_t *)ipv6_buf);
    }
}

void init_nbr_adv(ipv6_addr_t *src, ipv6_addr_t *dst, ipv6_addr_t *tgt,
                  uint8_t rso, uint8_t sllao, uint8_t aro, uint8_t aro_state)
{
    ipv6_buf = get_ipv6_buf();
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    ipv6_ext_hdr_len = 0;
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_buf->type = ICMP_NBR_ADV;
    icmp_buf->code = 0;

    memcpy(&(ipv6_buf->destaddr.uint8[0]), &(dst->uint8[0]), 16);
    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &(src->uint8[0]), 16);

    nbr_adv_buf = get_nbr_adv_buf(ipv6_ext_hdr_len);
    nbr_adv_buf->rso = rso;

    memset(&(nbr_adv_buf->reserved[0]), 0, 3);
    memcpy(&(nbr_adv_buf->tgtaddr.uint8[0]), &(tgt->uint8[0]), 16);

    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + NBR_ADV_LEN;

    if(sllao == OPT_SLLAO) {
        /* set sllao option */
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, opt_hdr_len);
        set_llao(opt_stllao_buf, OPT_SLLAO_TYPE, 1);
        opt_hdr_len += OPT_STLLAO_MIN_LEN;

        packet_length += OPT_STLLAO_MIN_LEN;
    }

    if(aro == OPT_ARO) {
        /* set aro option */
        opt_aro_buf = get_opt_aro_buf(ipv6_ext_hdr_len, opt_hdr_len);
        opt_aro_buf->type = OPT_ARO_TYPE;
        opt_aro_buf->length = OPT_ARO_LEN;
        opt_aro_buf->status = 0;    /* TODO */
        opt_aro_buf->reserved1 = 0;
        opt_aro_buf->reserved2 = 0;
        memcpy(&(opt_aro_buf->eui64), mac_get_eui(dst), 8);
        opt_hdr_len += OPT_ARO_HDR_LEN;

        packet_length += OPT_ARO_HDR_LEN;
    }

    ipv6_buf->length = packet_length - IPV6_HDR_LEN;

    icmp_buf->checksum = 0;
    icmp_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);
}

void recv_nbr_adv(void)
{
    opt_hdr_len = NBR_ADV_LEN;
    llao = NULL;
    nbr_entry = NULL;
    int8_t new_ll = -1;
    nbr_adv_buf = get_nbr_adv_buf(ipv6_ext_hdr_len);

    /* check if options are present */
    while(packet_length > IPV6HDR_ICMPV6HDR_LEN + opt_hdr_len) {
        opt_buf = get_opt_buf(ipv6_ext_hdr_len, opt_hdr_len);

        switch(opt_buf->type) {
            case(OPT_TLLAO_TYPE): {
                llao = (uint8_t *)get_opt_stllao_buf(ipv6_ext_hdr_len,
                                                     opt_hdr_len);
                break;
            }

            case(OPT_ARO_TYPE): {
                break;
            }
        }

        opt_hdr_len += (opt_buf->length * 8);
    }

    addr_list_t *addr;
    addr = ipv6_iface_addr_match(&nbr_adv_buf->tgtaddr);

    if(addr == NULL) {
        nbr_entry = nbr_cache_search(&nbr_adv_buf->tgtaddr);

        if(nbr_entry != NULL) {
            if(llao != 0) {
                /* TODO: untersheiden zwischen short und long stllao option */
                new_ll = memcmp(&llao[2], &(nbr_entry->laddr), 8);
            }

            if(nbr_entry->state == NBR_STATUS_INCOMPLETE) {
                if(llao == NULL) {
                    return;
                }

                /* TODO: untersheiden zwischen short und long stllao option */
                memcpy(&nbr_entry->laddr, &llao[2], 8);

                if(nbr_adv_buf->rso & NBR_ADV_FLAG_S) {
                    nbr_entry->state = NBR_STATUS_REACHABLE;
                    /* TODO: set rechability */
                }
                else {
                    nbr_entry->state = NBR_STATUS_STALE;
                }

                nbr_entry->isrouter = nbr_adv_buf->rso & NBR_ADV_FLAG_R;
            }
            else {
                if(new_ll && !(nbr_adv_buf->rso & NBR_ADV_FLAG_O)) {
                    if(nbr_entry->state == NBR_STATUS_REACHABLE) {
                        nbr_entry->state = NBR_STATUS_STALE;
                    }

                    return;
                }
                else {
                    if((nbr_adv_buf->rso & NBR_ADV_FLAG_O) ||
                       (!(nbr_adv_buf->rso & NBR_ADV_FLAG_O) && llao != 0 &&
                        !new_ll)) {
                        if(llao != 0) {
                            memcpy(&nbr_entry->laddr, &llao[2], 8);
                        }

                        if(nbr_adv_buf->rso & NBR_ADV_FLAG_S) {
                            nbr_entry->state = NBR_STATUS_REACHABLE;
                            /* TODO: set rechablility */
                        }
                        else {
                            if(llao != 0 && new_ll) {
                                nbr_entry->state = NBR_STATUS_STALE;
                            }
                        }
                    }
                }
            }
        }
    }
}

/* link-layer address option - RFC4861 section 4.6.1/ RFC4944 8. */
void set_llao(opt_stllao_t *sllao, uint8_t type, uint8_t length)
{
    sllao->type = type;
    sllao->length = length;

    uint8_t *llao = (uint8_t *)sllao;

    /* get link layer address */
    switch(length) {
        case(1): {
            memcpy(&llao[2], &(iface.saddr), 2);
            memset(&llao[4], 0, 4);
            break;
        }

        case(2): {
            memcpy(&llao[2], &(iface.laddr), 8);
            memset(&llao[10], 0, 6);
            break;
        }

        default: {
            printf("ERROR: llao not set\n");
            break;
        }
    }
}

uint16_t icmpv6_csum(uint8_t proto)
{
    ipv6_buf = get_ipv6_buf();
    uint16_t sum;
    uint16_t len = ipv6_buf->length;
    sum = len + proto;

    sum = csum(sum, (uint8_t *)&ipv6_buf->srcaddr, 2 * sizeof(ipv6_addr_t));
    sum = csum(sum, (uint8_t *)get_icmpv6_buf(0), len);

    return (sum == 0) ? 0xffff : HTONS(sum);
}


void init_para_prob(ipv6_addr_t *src, ipv6_addr_t *dest, uint8_t code, uint32_t pointer, uint8_t *packet, uint8_t packet_len)
{
    struct para_prob_t *para_prob_buf;


    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + PARA_PROB_LEN;

    memcpy(&(ipv6_buf[packet_length]), packet, min(MTU - packet_length, packet_len));

    ipv6_buf = get_ipv6_buf();
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    ipv6_ext_hdr_len = 0;
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_buf->type = ICMP_PARA_PROB;
    icmp_buf->code = code;

    memcpy(&(ipv6_buf->destaddr.uint8[0]), &(dest->uint8[0]), 16);
    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &(src->uint8[0]), 16);

    para_prob_buf = get_para_prob_buf(ipv6_ext_hdr_len);

    para_prob_buf->pointer = pointer;

    packet_length += min(MTU - packet_length, packet_len);

    ipv6_buf->length = packet_length - IPV6_HDR_LEN;

    icmp_buf->checksum = 0;
    icmp_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);
}

//------------------------------------------------------------------------------
/* neighbor cache functions */

nbr_cache_t *nbr_cache_search(ipv6_addr_t *ipaddr)
{
    int i;

    for(i = 0; i < NBR_CACHE_SIZE; i++) {
        if(memcmp(&(nbr_cache[i].addr.uint8[0]), &(ipaddr->uint8[0]), 16) == 0) {
            return &nbr_cache[i];
        }
    }

    return NULL;
}

uint8_t nbr_cache_add(ipv6_addr_t *ipaddr, ieee_802154_long_t *laddr,
                      uint8_t isrouter, uint8_t state, uint8_t type,
                      uint16_t ltime, ieee_802154_short_t *saddr)
{
    if(nbr_count == NBR_CACHE_SIZE) {
        printf("ERROR: neighbor cache full\n");
        return OPT_ARO_STATE_NBR_CACHE_FULL;
    }

    memcpy(&(nbr_cache[nbr_count].addr), ipaddr, 16);
    memcpy(&(nbr_cache[nbr_count].laddr), laddr, 8);
    nbr_cache[nbr_count].isrouter = isrouter;
    nbr_cache[nbr_count].state = state;
    nbr_cache[nbr_count].type = type;

    //vtimer_set_wakeup(&(nbr_cache[nbr_count].ltime), t,
    /*                  nd_nbr_cache_rem_pid); */

    nbr_count++;

    return OPT_ARO_STATE_SUCCESS;
}

void nbr_cache_auto_rem(void)
{
    int i;

    for(i = 0; i < NBR_CACHE_SIZE; i++) {
        if(get_remaining_time(&(nbr_cache[i].ltime)) == 0 &&
           nbr_cache[i].type == NBR_CACHE_TYPE_TEN) {
            memmove(&(nbr_cache[i]), &(nbr_cache[nbr_count]),
                    sizeof(nbr_cache_t));
            memset(&(nbr_cache[nbr_count]), 0, sizeof(nbr_cache_t));
            nbr_count--;
        }
    }
}

void nbr_cache_rem(ipv6_addr_t *addr)
{
    int i;

    for(i = 0; i < NBR_CACHE_SIZE; i++) {
        if(memcmp(&(nbr_cache[i].addr.uint8[0]), &(addr->uint8[0]), 16) == 0) {
            memmove(&(nbr_cache[i]), &(nbr_cache[nbr_count]),
                    sizeof(nbr_cache_t));
            memset(&(nbr_cache[nbr_count]), 0, sizeof(nbr_cache_t));
            nbr_count--;
        }
    }
}

//------------------------------------------------------------------------------
/* authoritive border router list functions */
/**
 * @brief Finds the most current (by version number) authoritive border
 *        router information.
 * @pre  assumes that abro versions are centrally managed
 * @return The most current authoritive border router information, NULL
 *         if no such information is given.
 */
static abr_cache_t *abr_get_most_current(void)
{
    abr_cache_t *abr = NULL;
    int i;
    int version = abr_cache[0].version;

    for(i = 0; i < abr_count; i++) {
        if(serial_comp16(version, abr_cache[i].version) == GREATER) {
            abr = &(abr_cache[i]);
            version = abr_cache[i].version;
        }
    }

    return abr;
}

static abr_cache_t *abr_get_oldest(void)
{
    abr_cache_t *abr = NULL;
    int i;
    int version = abr_cache[0].version;

    for(i = 0; i < abr_count; i++) {
        if(serial_comp16(version, abr_cache[i].version) == LESS) {
            abr = &(abr_cache[i]);
            version = abr_cache[i].version;
        }
    }

    return abr;
}

abr_cache_t *abr_get_version(uint16_t version, ipv6_addr_t *abr_addr)
{
    int i = 0;

    for(i = 0; i < ABR_CACHE_SIZE; i++) {
        if(abr_cache[i].version == version &&
           memcmp(&(abr_cache[i].abr_addr.uint8[0]),
                  &(abr_addr->uint8[0]), 16
                 ) == 0) {
            return &(abr_cache[i]);
        }
    }

    return NULL;
}

lowpan_context_t *abr_get_context(abr_cache_t *abr, uint8_t cid)
{
    if(abr->cids[cid] != cid) {
        return NULL;
    }

    return lowpan_context_num_lookup(abr->cids[cid]);
}

abr_cache_t *abr_add_context(uint16_t version, ipv6_addr_t *abr_addr,
                             uint8_t cid)
{
    abr_cache_t *abr = abr_get_version(version, abr_addr);

    if(abr == NULL) {
        if(abr_count == ABR_CACHE_SIZE) {
            abr = abr_get_oldest();
        }
        else {
            abr = &(abr_cache[abr_count++]);
        }

        abr->version = version;
        memcpy(&(abr->abr_addr), abr_addr, sizeof(ipv6_addr_t));
        memset(abr->cids, 0xFF, LOWPAN_CONTEXT_MAX);
    }

    abr->cids[cid] = cid;

    return abr;
}

void abr_remove_context(uint8_t cid)
{
    int i;

    for(i = 0; i < abr_count; i++) {
        abr_cache[i].cids[cid] = 0xFF;
    }
}

//------------------------------------------------------------------------------
/* default router list functions */

def_rtr_lst_t *def_rtr_lst_search(ipv6_addr_t *ipaddr)
{
    int i;

    for(i = 0; i < DEF_RTR_LST_SIZE; i++) {
        if(memcmp(&def_rtr_lst[i].addr.uint8[0],
                  &(ipaddr->uint8[0]), 16) == 0) {
            return &def_rtr_lst[i];
        }
    }

    return NULL;
}

void def_rtr_lst_add(ipv6_addr_t *ipaddr, uint32_t rtr_ltime)
{
    if(def_rtr_count == DEF_RTR_LST_SIZE) {
        DEBUG("ERROR: default router list full\n");
    }
    else {
        memcpy(&(def_rtr_lst[def_rtr_count].addr), ipaddr, 16);
        timex_t rltime = {rtr_ltime, 0};
        timex_t now;
        vtimer_now(&now);

        def_rtr_lst[def_rtr_count].inval_time = timex_add(now, rltime);

        def_rtr_count++;
    }
}

void def_rtr_lst_rem(def_rtr_lst_t *entry)
{
    int i;

    for(i = 0; i < DEF_RTR_LST_SIZE; i++) {
        if(&def_rtr_lst[i] == entry) {
            /* search the to deleted item, then memmove the last item to its
             * position, and decrement array count */
            memmove(entry, &def_rtr_lst[def_rtr_count], sizeof(def_rtr_lst_t));
            memset(&def_rtr_lst[def_rtr_count], 0, sizeof(def_rtr_lst_t));
            def_rtr_count--;
        }
    }
}

//------------------------------------------------------------------------------
/* prefix list functions */

int8_t plist_add(ipv6_addr_t *addr, uint8_t size, uint32_t val_ltime,
                 uint32_t pref_ltime, uint8_t adv_opt, uint8_t l_a_reserved1)
{
    if(prefix_count == OPT_PI_LIST_LEN) {
        return SIXLOWERROR_ARRAYFULL;
    }
    else {
        plist[prefix_count].inuse = 1;
        plist[prefix_count].length = size;
        plist[prefix_count].adv = adv_opt;
        plist[prefix_count].l_a_reserved1 = l_a_reserved1;
        plist[prefix_count].val_ltime = HTONL(val_ltime);
        plist[prefix_count].pref_ltime = HTONL(pref_ltime);
        memcpy(&(plist[prefix_count].addr.uint8[0]), &(addr->uint8[0]), 16);

        return SUCCESS;
    }
}
