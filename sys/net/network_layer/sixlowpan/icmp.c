/*
 * 6lowpan neighbor discovery
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
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
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vtimer.h"
#include "mutex.h"
#include "net_if.h"
#include "sixlowpan/error.h"

#include "ip.h"
#include "icmp.h"
#include "serialnumber.h"
#include "net_help.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

#define LLHDR_ICMPV6HDR_LEN             (LL_HDR_LEN + IPV6_HDR_LEN + ICMPV6_HDR_LEN)
#define ND_HOPLIMIT                     (0xFF)

/* parameter problem [rfc4443] */
#define PARA_PROB_LEN                   (8)
/* echo request */
#define ECHO_REQ_LEN                    (4)
/* echo reply */
#define ECHO_REPL_LEN                   (4)
/* router solicitation */
#define RTR_SOL_LEN                     (4)
#define RTR_SOL_INTERVAL                (4)
#define RTR_SOL_MAX                     (3)
/* router advertisment */
#define RTR_ADV_MAX                     (3)
#define RTR_ADV_MAX_INTERVAL            (600)
#define RTR_ADV_LEN                     (12)
/* neighbour solicitation */
#define NBR_SOL_LEN                     (20)
/* neighbour advertisement */
#define NBR_ADV_LEN                     (20)
/* stllao option rfc4861 4.6.1 */
#define OPT_STLLAO_MIN_LEN              (8)
#define OPT_STLLAO_MAX_LEN              (16)
/* prefix info option rfc 4.6.2 */
#define OPT_PI_LIST_LEN                 (5) //TODO: initalwert suchen
#define OPT_PI_TYPE                     (3)
#define OPT_PI_LEN                      (4)
#define OPT_PI_HDR_LEN                  (32)
/* mtu option rfc4861 4.6.4 */
#define OPT_MTU_TYPE                    (5)
#define OPT_MTU_LEN                     (1)
#define OPT_MTU_HDR_LEN                 (8)
/* aro - address registration option rfc6775 4.1 */
#define OPT_ARO_TYPE                    (33)
#define OPT_ARO_LEN                     (2)
#define OPT_ARO_HDR_LEN                 (16)
#define OPT_ARO_LTIME                   (300) /* TODO: geeigneten wert finden */
/* 6lowpan context option */
#define OPT_6CO_TYPE                    (34)
#define OPT_6CO_MIN_LEN                 (2)
#define OPT_6CO_MAX_LEN                 (3)
#define OPT_6CO_HDR_LEN                 (8)
#define OPT_6CO_LTIME                   (5)   /* TODO geeigneten Wert finden */
/* authoritative border router option */
#define OPT_ABRO_TYPE                   (35)
#define OPT_ABRO_LEN                    (3)
#define OPT_ABRO_HDR_LEN                (24)
/* authoritive border router cache size */
#define ABR_CACHE_SIZE                  (2)
/* neighbor cache size */
#define NBR_CACHE_SIZE                  (8)
#define NBR_CACHE_LTIME_TEN             (20)
/* default router list size */
#define DEF_RTR_LST_SIZE                    (3) /* geeigneten wert finden */

#define PREFIX_BUF_LEN                  (NET_IF_MAX * OPT_PI_LIST_LEN)

/* extern variables */
uint8_t ipv6_ext_hdr_len = 0;

/* counter */
uint8_t abr_count = 0;
uint8_t nbr_count = 0;
uint8_t def_rtr_count = 0;
uint8_t rtr_sol_count = 0;
uint8_t prefix_info_count = 0;
uint8_t prefix_buf_count = 0;

/* datastructures */
ndp_a6br_cache_t abr_cache[ABR_CACHE_SIZE];
ndp_neighbor_cache_t nbr_cache[NBR_CACHE_SIZE];
ndp_default_router_list_t def_rtr_lst[DEF_RTR_LST_SIZE];
ndp_prefix_info_t prefix_info_buf[PREFIX_BUF_LEN];
uint8_t prefix_buf[sizeof(ipv6_addr_t) * PREFIX_BUF_LEN];

/* pointer */
static uint8_t *llao;

static ipv6_hdr_t *ipv6_buf;
static icmpv6_hdr_t *icmp_buf;
static icmpv6_router_adv_hdr_t *rtr_adv_buf;
static icmpv6_neighbor_sol_hdr_t *nbr_sol_buf;
static icmpv6_neighbor_adv_hdr_t *nbr_adv_buf;
static icmpv6_ndp_opt_hdr_t *opt_buf;
static icmpv6_ndp_opt_stllao_t *opt_stllao_buf;
static icmpv6_ndp_opt_mtu_t *opt_mtu_buf;
static icmpv6_ndp_opt_abro_t *opt_abro_buf;
static icmpv6_ndp_opt_6co_hdr_t *opt_6co_hdr_buf;
static uint8_t *opt_6co_prefix_buf;
static icmpv6_ndp_opt_pi_t *opt_pi_buf;
static icmpv6_ndp_opt_aro_t *opt_aro_buf;

ndp_neighbor_cache_t *nbr_entry;
ndp_default_router_list_t *def_rtr_entry;

/* elements */
//ipv6_addr_t tmpaddr;

uint8_t recvd_cids[NDP_6LOWPAN_CONTEXT_MAX];
uint8_t icmpv6_opt_hdr_len = 0;
uint8_t recvd_cids_len = 0;
ndp_prefix_info_t *recvd_prefixes[PREFIX_BUF_LEN];
uint8_t recvd_pref_len = 0;

void def_rtr_lst_add(ipv6_addr_t *ipaddr, uint32_t rtr_ltime);
void def_rtr_lst_rem(ndp_default_router_list_t *entry);

/**
 * @brief   Set Source link-layer address option according to interface
 *          configuration.
 *
 * @param[out]  sllao   The SLLAO to set.
 * @param[in]   if_id   The interface to get the link-layer address from.
 * @param[in]   type    The value for the type field of the SLLAO.
 * @param[in]   length  The value for the length field of the SLLAO
 */
void icmpv6_ndp_set_sllao(icmpv6_ndp_opt_stllao_t *sllao, int if_id,
                          uint8_t type, uint8_t length);

int min(int a, int b)
{
    if (a < b) {
        return a;
    }
    else {
        return b;
    }
}

static icmpv6_parameter_prob_hdr_t *get_para_prob_buf(uint8_t ext_len)
{
    return ((icmpv6_parameter_prob_hdr_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len]);
}

static icmpv6_echo_request_hdr_t *get_echo_req_buf(uint8_t ext_len)
{
    return ((icmpv6_echo_request_hdr_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len]);
}

static icmpv6_echo_reply_hdr_t *get_echo_repl_buf(uint8_t ext_len)
{
    return ((icmpv6_echo_reply_hdr_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len]);
}

static icmpv6_router_adv_hdr_t *get_rtr_adv_buf(uint8_t ext_len)
{
    return ((icmpv6_router_adv_hdr_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len]);
}

static icmpv6_neighbor_sol_hdr_t *get_nbr_sol_buf(uint8_t ext_len)
{
    return ((icmpv6_neighbor_sol_hdr_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len]);
}

static icmpv6_neighbor_adv_hdr_t *get_nbr_adv_buf(uint8_t ext_len)
{
    return ((icmpv6_neighbor_adv_hdr_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len]);
}

static icmpv6_ndp_opt_hdr_t *get_opt_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((icmpv6_ndp_opt_hdr_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]);
}

static icmpv6_ndp_opt_stllao_t *get_opt_stllao_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((icmpv6_ndp_opt_stllao_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]);
}

static icmpv6_ndp_opt_mtu_t *get_opt_mtu_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((icmpv6_ndp_opt_mtu_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]);
}

static icmpv6_ndp_opt_abro_t *get_opt_abro_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((icmpv6_ndp_opt_abro_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]);
}

static icmpv6_ndp_opt_6co_hdr_t *get_opt_6co_hdr_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((icmpv6_ndp_opt_6co_hdr_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]);
}

static uint8_t *get_opt_6co_prefix_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((uint8_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]);
}

static icmpv6_ndp_opt_pi_t *get_opt_pi_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((icmpv6_ndp_opt_pi_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]);
}

static icmpv6_ndp_opt_aro_t *get_opt_aro_buf(uint8_t ext_len, uint8_t opt_len)
{
    return ((icmpv6_ndp_opt_aro_t *) &buffer[LLHDR_ICMPV6HDR_LEN + ext_len + opt_len]);
}

void icmpv6_send_echo_request(ipv6_addr_t *destaddr, uint16_t id, uint16_t seq, uint8_t *data, size_t data_len)
{
    uint16_t packet_length;

    ipv6_buf = ipv6_get_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmpv6_echo_request_hdr_t *echo_buf = get_echo_req_buf(ipv6_ext_hdr_len);
    uint8_t *echo_data_buf = ((uint8_t *)echo_buf) + sizeof(icmpv6_echo_request_hdr_t);

    icmp_buf->type = ICMPV6_TYPE_ECHO_REQUEST;
    icmp_buf->code = 0;
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = IPV6_PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ipv6_get_default_hop_limit();

    memcpy(&ipv6_buf->destaddr, destaddr, sizeof(ipv6_addr_t));
    ipv6_net_if_get_best_src_addr(&ipv6_buf->srcaddr, &ipv6_buf->destaddr);
    echo_buf->id = HTONS(id);
    echo_buf->seq = HTONS(seq);

    memcpy(echo_data_buf, data, data_len);
    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + ipv6_ext_hdr_len +
                    ECHO_REQ_LEN + data_len;

    ipv6_buf->length = HTONS(packet_length - IPV6_HDR_LEN);

    icmp_buf->checksum = icmpv6_csum(ipv6_buf, icmp_buf);

#ifdef DEBUG_ENABLED
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("INFO: send echo request (id = %04x, seq = %d, data_len = %zu) to: %s\n",
           id, seq, data_len, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                   &ipv6_buf->destaddr));
#endif
    ipv6_send_packet(ipv6_buf);
}

void icmpv6_send_echo_reply(ipv6_addr_t *destaddr, uint16_t id, uint16_t seq, uint8_t *data, size_t data_len)
{
    uint16_t packet_length;

    ipv6_buf = ipv6_get_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmpv6_echo_reply_hdr_t *echo_buf = get_echo_repl_buf(ipv6_ext_hdr_len);
    uint8_t *echo_data_buf = ((uint8_t *)echo_buf) + sizeof(icmpv6_echo_reply_hdr_t);

    icmp_buf->type = ICMPV6_TYPE_ECHO_REPLY;
    icmp_buf->code = 0;
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = IPV6_PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ipv6_get_default_hop_limit();

    memcpy(&ipv6_buf->destaddr, destaddr, sizeof(ipv6_addr_t));
    ipv6_net_if_get_best_src_addr(&ipv6_buf->srcaddr, &ipv6_buf->destaddr);
    echo_buf->id = HTONS(id);
    echo_buf->seq = HTONS(seq);

    memcpy(echo_data_buf, data, data_len);
    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + ipv6_ext_hdr_len +
                    ECHO_REPL_LEN + data_len;

    ipv6_buf->length = HTONS(packet_length - IPV6_HDR_LEN);

    icmp_buf->checksum = icmpv6_csum(ipv6_buf, icmp_buf);

#ifdef DEBUG_ENABLED
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("INFO: send echo reply (id = %04x, seq = %d, data_len = %zu) to: %s\n",
           id, seq, data_len, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                   &ipv6_buf->destaddr));
#endif
    ipv6_send_packet(ipv6_buf);
}

/* send router solicitation message - RFC4861 section 4.1 */
void icmpv6_send_router_sol(uint8_t sllao)
{
    uint16_t packet_length;
    int if_id = 0;  // TODO get this somehow

    ipv6_buf = ipv6_get_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_buf->type = ICMPV6_TYPE_ROUTER_SOL;
    icmp_buf->code = 0;
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = IPV6_PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    ipv6_addr_set_all_routers_addr(&ipv6_buf->destaddr);
    //iface_find_src_ipaddr(&ipv6_buf->srcaddr, NDP_ADDR_STATE_PREFERRED,
    /*                      IPV6_ADDR_TYPE_MULTICAST); */

    ipv6_net_if_get_best_src_addr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));

    icmpv6_opt_hdr_len = RTR_SOL_LEN;

    if (sllao == OPT_SLLAO) {
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

        if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_LONG) {
            icmpv6_ndp_set_sllao(opt_stllao_buf, if_id, NDP_OPT_SLLAO_TYPE, 2);
            packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + ipv6_ext_hdr_len +
                            RTR_SOL_LEN + OPT_STLLAO_MAX_LEN;
        }
        else {
            icmpv6_ndp_set_sllao(opt_stllao_buf, if_id, NDP_OPT_SLLAO_TYPE, 1);
            packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + ipv6_ext_hdr_len +
                            RTR_SOL_LEN + OPT_STLLAO_MIN_LEN;
        }
    }
    else {
        packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + ipv6_ext_hdr_len +
                        RTR_SOL_LEN;
    }

    ipv6_buf->length = HTONS(packet_length - IPV6_HDR_LEN);

    icmp_buf->checksum = icmpv6_csum(ipv6_buf, icmp_buf);

#ifdef DEBUG_ENABLED
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("INFO: send router solicitation to: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_buf->destaddr));
#endif
    ipv6_send_packet(ipv6_buf);
}

void recv_echo_req(void)
{
    ipv6_buf = ipv6_get_buf();
    icmpv6_echo_request_hdr_t *echo_buf = get_echo_req_buf(ipv6_ext_hdr_len);
    uint8_t *echo_data_buf = ((uint8_t *)echo_buf) + sizeof(icmpv6_echo_reply_hdr_t);
    size_t data_len = NTOHS(ipv6_buf->length) - ICMPV6_HDR_LEN - ECHO_REQ_LEN;

#ifdef DEBUG_ENABLED
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("INFO: received echo request from: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_buf->srcaddr));
    printf("\n");
    printf("id = 0x%04x, seq = %d, data_len = %zu\n", NTOHS(echo_buf->id),
           NTOHS(echo_buf->seq), data_len);

    for (size_t i = 0; i < data_len; i++) {
        printf("%02x ", echo_data_buf[i]);

        if (!((i + 1) % 16) || i == data_len - 1) {
            printf("\n");
        }
    }

#endif
    icmpv6_send_echo_reply(&ipv6_buf->srcaddr, NTOHS(echo_buf->id),
                           NTOHS(echo_buf->seq), echo_data_buf, data_len);
}

void recv_echo_repl(void)
{
#ifdef DEBUG_ENABLED
    ipv6_buf = ipv6_get_buf();
    icmpv6_echo_reply_hdr_t *echo_buf = get_echo_repl_buf(ipv6_ext_hdr_len);
    uint8_t *echo_data_buf = ((uint8_t *)echo_buf) + sizeof(icmpv6_echo_reply_hdr_t);
    size_t data_len = NTOHS(ipv6_buf->length) - ICMPV6_HDR_LEN - ECHO_REPL_LEN;
    char addr_str[IPV6_MAX_ADDR_STR_LEN];

    printf("INFO: received echo reply from: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_buf->srcaddr));
    printf("\n");
    printf("id = 0x%04x, seq = %d, data_len = %zu\n", NTOHS(echo_buf->id),
           NTOHS(echo_buf->seq), data_len);

    for (size_t i = 0; i < data_len; i++) {
        printf("%02x ", echo_data_buf[i]);

        if (!((i + 1) % 16) || i == data_len - 1) {
            printf("\n");
        }
    }
#endif
}

void recv_rtr_sol(void)
{
    int if_id = 0;  // TODO, get this somehow
    icmpv6_opt_hdr_len = RTR_SOL_LEN;
    ipv6_buf = ipv6_get_buf();

    /* check if source option is set*/
    if (opt_stllao_buf->type == NDP_OPT_SLLAO_TYPE) {
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);
        llao = (uint8_t *)opt_stllao_buf;
        icmpv6_opt_hdr_len += (opt_stllao_buf->length) << 3;
    }

    if (llao != NULL) {
        uint8_t lladdr_len;
        nbr_entry = ndp_neighbor_cache_search(&ipv6_buf->srcaddr);

        if (opt_stllao_buf->length == 2) {
            lladdr_len = 8;
        }
        else if (opt_stllao_buf->length == 1) {
            lladdr_len = 2;
        }
        else {
            DEBUG("Unknown length for S/TLLAO: %d * 8 Bytes.\n", opt_stllao_buf->length);
            return;
        }

        if (nbr_entry != NULL) {
            /* found neighbor in cache, update values and check addr */
            if (memcmp(&llao[2], &nbr_entry->lladdr, lladdr_len) == 0) {
                nbr_entry->if_id = if_id;
                nbr_entry->isrouter = 0;
            }
            else {
                /* new addr found, update */
                nbr_entry->if_id = if_id;
                memcpy(&nbr_entry->lladdr, &llao[2], lladdr_len);
                nbr_entry->state = NDP_NCE_STATUS_STALE;
                nbr_entry->isrouter = 0;
            }
        }
        else {
            /* nothing found, add neigbor into cache*/
            ndp_neighbor_cache_add(if_id, &ipv6_buf->srcaddr, &llao[2], lladdr_len,
                                   0, NDP_NCE_STATUS_STALE, NDP_NCE_TYPE_TENTATIVE,
                                   NBR_CACHE_LTIME_TEN);
        }
    }

    /* send solicited router advertisment */
    if (abr_count > 0) {
        icmpv6_send_router_adv(&ipv6_buf->srcaddr, 0, 0, OPT_PI, OPT_6CO, OPT_ABRO);
    }
    else {
        icmpv6_send_router_adv(&ipv6_buf->srcaddr, 0, 0, OPT_PI, 0, 0);
    }
}

uint8_t set_opt_6co_flags(uint8_t compression_flag, uint8_t cid)
{
    uint8_t flags;

    if (compression_flag) {
        flags = ICMPV6_NDP_OPT_6CO_FLAG_CCOMPR;
    }
    else {
        flags = 0;
    }

    flags |= cid & ICMPV6_NDP_OPT_6CO_FLAG_CID;
    return flags;
}

void get_opt_6co_flags(uint8_t *compression_flag, uint8_t *cid, uint8_t flags)
{
    compression_flag[0] = flags & ICMPV6_NDP_OPT_6CO_FLAG_CID;
    compression_flag[0] = compression_flag[0] != 0;
    cid[0] = flags & ICMPV6_NDP_OPT_6CO_FLAG_CID;
}

lowpan_context_t *abr_get_context(ndp_a6br_cache_t *abr, uint8_t cid);

void icmpv6_send_router_adv(ipv6_addr_t *addr, uint8_t sllao, uint8_t mtu, uint8_t pi,
                            uint8_t sixco, uint8_t abro)
{
    int if_id = 0;      // TODO: get this somehow
    uint16_t packet_length;
    lowpan_context_t *contexts = NULL;

    ndp_a6br_cache_t *msg_abr = NULL;
    ipv6_buf = ipv6_get_buf();
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);

    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = IPV6_PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    if (addr == NULL) {
        /* not solicited */
        ipv6_addr_set_all_nodes_addr(&ipv6_buf->destaddr);
    }
    else {
        memcpy(&ipv6_buf->destaddr, addr, 16);
    }

    ipv6_net_if_get_best_src_addr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));

    icmp_buf->type = ICMPV6_TYPE_ROUTER_ADV;
    icmp_buf->code = 0;

    //TODO: gethoplimit func, set current ttl

    rtr_adv_buf = get_rtr_adv_buf(ipv6_ext_hdr_len);
    rtr_adv_buf->hoplimit = MULTIHOP_HOPLIMIT;
    /* set M and O flag, last 6 bits are zero */
    rtr_adv_buf->autoconfig_flags = ICMPV6_ROUTER_ADV_FLAG_MANAGED | ICMPV6_ROUTER_ADV_FLAG_OTHER;
    rtr_adv_buf->router_lifetime = HTONS(RTR_ADV_MAX_INTERVAL * RTR_ADV_MAX);
    rtr_adv_buf->reachable_time = 0;
    rtr_adv_buf->retrans_timer = 0;
    icmpv6_opt_hdr_len = RTR_ADV_LEN;

    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + RTR_ADV_LEN;

    if (sllao == OPT_SLLAO) {
        /* set link layer address option */
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

        if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_LONG) {
            icmpv6_ndp_set_sllao(opt_stllao_buf, if_id, NDP_OPT_SLLAO_TYPE, 2);
            icmpv6_opt_hdr_len += OPT_STLLAO_MAX_LEN;
            packet_length += OPT_STLLAO_MAX_LEN;
        }
        else {
            icmpv6_ndp_set_sllao(opt_stllao_buf, if_id, NDP_OPT_SLLAO_TYPE, 1);
            icmpv6_opt_hdr_len += OPT_STLLAO_MIN_LEN;
            packet_length += OPT_STLLAO_MIN_LEN;
        }
    }

    if (mtu == OPT_MTU) {
        /* set MTU options */
        opt_mtu_buf = get_opt_mtu_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);
        opt_mtu_buf->type = OPT_MTU_TYPE;
        opt_mtu_buf->length = OPT_MTU_LEN;
        opt_mtu_buf->reserved = 0;
        opt_mtu_buf->mtu = HTONL(1500);
        icmpv6_opt_hdr_len += OPT_MTU_HDR_LEN;
        packet_length += OPT_MTU_HDR_LEN;
    }

    /* set payload length field */

    if (abro == OPT_ABRO) {
        /* set authoritive border router option */
        if (abr_count > 0) {
            msg_abr = ndp_a6br_cache_get_most_current();
            opt_abro_buf = get_opt_abro_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);
            opt_abro_buf->type = OPT_ABRO_TYPE;
            opt_abro_buf->length = OPT_ABRO_LEN;
            opt_abro_buf->version = HTONS(msg_abr->version);
            opt_abro_buf->reserved = 0;
            memcpy(&(opt_abro_buf->addr), &(msg_abr->abr_addr), sizeof(ipv6_addr_t));
        }
    }

    if (sixco == OPT_6CO) {
        /* set 6lowpan context option */
        int contexts_len = 0;
        mutex_lock(&lowpan_context_mutex);

        if (msg_abr == NULL) {
            contexts = lowpan_context_get();
            contexts_len = lowpan_context_len();
        }
        else {
            lowpan_context_t c_tmp[NDP_6LOWPAN_CONTEXT_MAX];

            contexts_len = 0;

            for (int i = 0; i < NDP_6LOWPAN_CONTEXT_MAX; i++) {
                lowpan_context_t *ctx = abr_get_context(msg_abr, i);

                if (ctx != NULL) {
                    memcpy(&(c_tmp[contexts_len++]), ctx, sizeof(lowpan_context_t));
                }
            }

            contexts = (lowpan_context_t *)malloc(contexts_len * sizeof(lowpan_context_t));
            memcpy(contexts, c_tmp, contexts_len);
        }

        for (int i = 0; i < contexts_len; i++) {
            opt_6co_hdr_buf = get_opt_6co_hdr_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);
            opt_6co_hdr_buf->type = OPT_6CO_TYPE;

            if (contexts[i].length > 64) {
                opt_6co_hdr_buf->length = OPT_6CO_MAX_LEN;
            }
            else {
                opt_6co_hdr_buf->length = OPT_6CO_MIN_LEN;
            }

            opt_6co_hdr_buf->c_length = contexts[i].length;
            opt_6co_hdr_buf->c_flags = set_opt_6co_flags(contexts[i].comp, contexts[i].num);
            opt_6co_hdr_buf->reserved = 0;
            opt_6co_hdr_buf->val_ltime = HTONS(contexts[i].lifetime);

            icmpv6_opt_hdr_len += OPT_6CO_HDR_LEN;
            packet_length += OPT_6CO_HDR_LEN;
            /* attach prefixes */
            opt_6co_prefix_buf = get_opt_6co_prefix_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

            if (opt_6co_hdr_buf->c_length > 64) {
                memset((void *)opt_6co_prefix_buf, 0, 16);
                memcpy((void *)opt_6co_prefix_buf, (void *) &contexts[i].prefix.uint8[0], opt_6co_hdr_buf->c_length / 8);
                icmpv6_opt_hdr_len += 16;
                packet_length += 16;
            }
            else {
                memset((void *)opt_6co_prefix_buf, 0, 8);
                memcpy((void *)opt_6co_prefix_buf, (void *) &contexts[i].prefix.uint8[0], opt_6co_hdr_buf->c_length / 8);
                icmpv6_opt_hdr_len += 8;
                packet_length += 8;
            }

        }

        if (msg_abr != NULL && contexts != NULL) {
            free(contexts);
        }

        mutex_unlock(&lowpan_context_mutex);
    }

    if (pi == OPT_PI) {
        ndp_prefix_info_t *prefix = NULL;

        /* set prefix option */
        while (net_if_iter_addresses(if_id, (net_if_addr_t **) &prefix)) {
            if (prefix->prefix_protocol & NET_IF_L3P_IPV6_PREFIX &&
                prefix->inuse && prefix->advertisable) {
                opt_pi_buf = get_opt_pi_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);
                memset(&opt_pi_buf->addr, 0, sizeof(ipv6_addr_t));
                memcpy(&opt_pi_buf->addr, &prefix->prefix_data, prefix->prefix_len);
                opt_pi_buf->type = OPT_PI_TYPE;
                opt_pi_buf->length = OPT_PI_LEN;
                opt_pi_buf->prefix_length = prefix->prefix_len;
                opt_pi_buf->l_a_reserved1 = prefix->flags;
                opt_pi_buf->val_ltime = HTONL(prefix->valid_lifetime);
                opt_pi_buf->pref_ltime = HTONL(prefix->preferred_lifetime);
                opt_pi_buf->reserved2 = 0;
                packet_length += OPT_PI_HDR_LEN;
                icmpv6_opt_hdr_len += OPT_PI_HDR_LEN;
            }
        }
    }

    ipv6_buf->length = HTONS(packet_length - IPV6_HDR_LEN);

    /* calculate checksum */
    icmp_buf->checksum = icmpv6_csum(ipv6_buf, icmp_buf);

#ifdef DEBUG_ENABLED
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("INFO: send router advertisement to: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_buf->destaddr));
#endif
    ipv6_send_packet(ipv6_buf);
}

void recv_rtr_adv(void)
{
    int if_id = 0;              // TODO: get this somehow
    int8_t trigger_ns = -1;
    int8_t abro_found = 0;
    int16_t abro_version = 0;    /* later replaced, just to supress warnings */
    uint16_t packet_length;
    ipv6_addr_t abro_addr;

    ipv6_buf = ipv6_get_buf();
    packet_length = IPV6_HDR_LEN + NTOHS(ipv6_buf->length);
    icmpv6_opt_hdr_len = RTR_ADV_LEN;
    rtr_adv_buf = get_rtr_adv_buf(ipv6_ext_hdr_len);
    ipv6_addr_t newaddr;
    recvd_cids_len = 0;

    /* update interface reachable time and retrans timer */
    if (rtr_adv_buf->reachable_time != 0) {
        ipv6_net_if_ext_t *iface;

        iface = ipv6_net_if_get_ext(if_id);
        if (iface) {
            iface->adv_reachable_time = NTOHL(rtr_adv_buf->reachable_time);
        }
    }

    if (rtr_adv_buf->retrans_timer != 0) {
        ipv6_net_if_ext_t *iface;

        iface = ipv6_net_if_get_ext(if_id);
        if (iface) {
            iface->adv_retrans_timer = NTOHL(rtr_adv_buf->retrans_timer);
        }
    }

    def_rtr_entry = ndp_default_router_list_search(&ipv6_buf->srcaddr);

    if (rtr_adv_buf->router_lifetime != 0) {
        if (def_rtr_entry != NULL) {
            set_remaining_time(&(def_rtr_entry->inval_time), HTONL(rtr_adv_buf->router_lifetime));
        }
        else {
            def_rtr_lst_add(&(ipv6_buf->srcaddr), HTONL(rtr_adv_buf->router_lifetime));
            trigger_ns = 1;
        }
    }
    else {
        /* remove router from default router list */
        if (def_rtr_entry != NULL) {
            def_rtr_lst_rem(def_rtr_entry);
        }
    }

    mutex_lock(&lowpan_context_mutex);

    /* read options */
    while (packet_length > IPV6_HDR_LEN + ICMPV6_HDR_LEN + icmpv6_opt_hdr_len) {
        opt_buf = get_opt_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

        switch (opt_buf->type) {
            case (NDP_OPT_SLLAO_TYPE): {
                break;
            }

            case (OPT_MTU_TYPE): {
                break;
            }

            /* rfc 4862 section 5.5.3 */
            case (OPT_PI_TYPE): {
                opt_pi_buf = get_opt_pi_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

                /* crazy condition, read 5.5.3a-b-c for further information */
                if (ipv6_addr_is_link_local(&opt_pi_buf->addr) ||
                    (HTONL(opt_pi_buf->pref_ltime) >
                     HTONL(opt_pi_buf->val_ltime))) {
                    break;
                }
                else {
                    /* check if on-link flag is set */
                    if (opt_pi_buf->l_a_reserved1 & ICMPV6_NDP_OPT_PI_FLAG_ON_LINK) {
                        /* TODO: do on-link pi handling */
                    }

                    if (opt_pi_buf->l_a_reserved1 & ICMPV6_NDP_OPT_PI_FLAG_AUTONOM) {
                        ipv6_net_if_hit_t addr_hit;

                        if (!ipv6_net_if_addr_prefix_eq(&addr_hit, &opt_pi_buf->addr)) {
                            /* 5.5.3d */
                            if (opt_pi_buf->val_ltime != 0) {
                                /* iid will also be added here */
                                ipv6_addr_set_by_eui64(&newaddr, if_id,
                                                       &opt_pi_buf->addr);
                                /* add into address list
                                * TODO: duplicate address detection is not
                                *       implementet yet, so all new addresse will
                                *       be added with state PREFFERED */
                                ipv6_net_if_add_addr(if_id, &newaddr,
                                                     NDP_ADDR_STATE_PREFERRED,
                                                     opt_pi_buf->val_ltime,
                                                     opt_pi_buf->pref_ltime, 0);
                                DEBUG("INFO: added address to interface\n");
                                trigger_ns = 1;
                            }
                        }
                        else {
                            /* 5.5.3e */
                            set_remaining_time(&addr_hit.addr->preferred_lifetime,
                                               opt_pi_buf->pref_ltime);

                            /* 7200 = 2hours in seconds */
                            if (HTONL(opt_pi_buf->val_ltime) > 7200 ||
                                HTONL(opt_pi_buf->val_ltime) >
                                get_remaining_time(&addr_hit.addr->valid_lifetime)) {
                                set_remaining_time(&addr_hit.addr->valid_lifetime,
                                                   HTONL(opt_pi_buf->val_ltime));
                            }
                            else {
                                /* reset valid lifetime to 2 hours */
                                set_remaining_time(&addr_hit.addr->valid_lifetime,
                                                   7200);
                            }
                        }
                    }
                }

                ndp_add_prefix_info(if_id, &opt_pi_buf->addr, opt_pi_buf->length,
                                    opt_pi_buf->val_ltime, opt_pi_buf->pref_ltime,
                                    0, opt_pi_buf->l_a_reserved1);

                break;
            }

            case (OPT_6CO_TYPE): {
                uint8_t comp;
                uint8_t num;

                opt_6co_hdr_buf = get_opt_6co_hdr_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

                get_opt_6co_flags(&comp, &num, opt_6co_hdr_buf->c_flags);

                ipv6_addr_t prefix;
                memset(&prefix, 0, 16);

                opt_6co_prefix_buf = get_opt_6co_prefix_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len + OPT_6CO_HDR_LEN);

                memcpy(&prefix, opt_6co_prefix_buf, opt_6co_hdr_buf->c_length);

                lowpan_context_update(
                    num,
                    &prefix,
                    opt_6co_hdr_buf->c_length,
                    comp,
                    HTONS(opt_6co_hdr_buf->val_ltime)
                );
                recvd_cids[recvd_cids_len] = num;
                recvd_cids_len = (recvd_cids_len + 1) % NDP_6LOWPAN_CONTEXT_MAX;
                break;
            }

            case (OPT_ABRO_TYPE): {
                opt_abro_buf = get_opt_abro_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);
                abro_found = 1;
                abro_version = HTONS(opt_abro_buf->version);
                memcpy(&(abro_addr), &(opt_abro_buf->addr), sizeof(ipv6_addr_t));
                break;
            }

            default:
                break;
        }

        /* multiplied with 8 because options length is in units of 8 bytes */
        icmpv6_opt_hdr_len += (opt_buf->length * 8);
    }

    if (abro_found) {
        int i;

        for (i = 0; i < recvd_cids_len; i++) {
            abr_add_context(abro_version, &abro_addr, recvd_cids[i]);
        }
    }

    mutex_unlock(&lowpan_context_mutex);

    if (trigger_ns >= 0) {
        /* send ns - draft-ietf-6lowpan-nd-15#section-5.5.1
         *
         * section-10.2.4
         * "Next the 6LN registers that address with one or more of its
         * default routers by sending a unicast NS message with an ARO
         * containing its tentative global IPv6 address to register
         *
         * if new address was configured, set src to newaddr(gp16) */
        icmpv6_send_neighbor_sol(&newaddr, &(ipv6_buf->srcaddr), &(ipv6_buf->srcaddr), OPT_SLLAO, OPT_ARO);
    }
}

void icmpv6_send_neighbor_sol(ipv6_addr_t *src, ipv6_addr_t *dest, ipv6_addr_t *targ,
                              uint8_t sllao, uint8_t aro)
{
    uint16_t packet_length;
    int if_id = 0;          // TODO: get this somehow
    ipv6_net_if_hit_t hit;

    ipv6_buf = ipv6_get_buf();
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = IPV6_PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    if (dest == NULL) {
        ipv6_addr_set_solicited_node_addr(&(ipv6_buf->destaddr), targ);
    }
    else {
        memcpy(&(ipv6_buf->destaddr.uint8[0]), &(dest->uint8[0]), 16);
    }

    ipv6_ext_hdr_len = 0;
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_buf->type = ICMPV6_TYPE_NEIGHBOR_SOL;
    icmp_buf->code = 0;

    nbr_sol_buf = get_nbr_sol_buf(ipv6_ext_hdr_len);
    nbr_sol_buf->reserved = 0;
    memcpy(&(nbr_sol_buf->target_addr), targ, 16);
    icmpv6_opt_hdr_len = NBR_SOL_LEN;

    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + NBR_SOL_LEN;

    if (!ipv6_net_if_addr_match(&hit, targ)) {
        if (src == NULL) {
            ipv6_net_if_get_best_src_addr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));
        }
        else {
            memcpy(&(ipv6_buf->srcaddr), src, 16);
        }

        if (sllao == OPT_SLLAO) {
            /* set sllao option */
            opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

            if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_LONG) {
                icmpv6_ndp_set_sllao(opt_stllao_buf, if_id, NDP_OPT_SLLAO_TYPE, 2);
                icmpv6_opt_hdr_len += OPT_STLLAO_MAX_LEN;
                packet_length += OPT_STLLAO_MAX_LEN;
            }
            else {
                icmpv6_ndp_set_sllao(opt_stllao_buf, if_id, NDP_OPT_SLLAO_TYPE, 1);
                icmpv6_opt_hdr_len += OPT_STLLAO_MIN_LEN;
                packet_length += OPT_STLLAO_MIN_LEN;
            }
        }
    }

    if (aro == OPT_ARO) {
        /* set aro option */
        opt_aro_buf = get_opt_aro_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);
        opt_aro_buf->type = OPT_ARO_TYPE;
        opt_aro_buf->length = OPT_ARO_LEN;
        opt_aro_buf->status = 0;
        opt_aro_buf->reserved1 = 0;
        opt_aro_buf->reserved2 = 0;

        if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_SHORT) {
            net_if_get_eui64((net_if_eui64_t *) &opt_aro_buf->eui64, if_id, 1);
        }
        else if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_LONG) {
            net_if_get_eui64((net_if_eui64_t *) &opt_aro_buf->eui64, if_id, 0);
        }

        icmpv6_opt_hdr_len += OPT_ARO_HDR_LEN;

        packet_length += OPT_ARO_HDR_LEN;
    }

    ipv6_buf->length = HTONS(packet_length - IPV6_HDR_LEN);

    icmp_buf->checksum = icmpv6_csum(ipv6_buf, icmp_buf);

#ifdef DEBUG_ENABLED
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("INFO: send neighbor solicitation to: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_buf->destaddr));
#endif
    ipv6_send_packet(ipv6_buf);
}

void recv_nbr_sol(void)
{
    int if_id = 0;  // TODO, get this somehow
    ipv6_buf = ipv6_get_buf();
    llao = NULL;
    icmpv6_opt_hdr_len = NBR_SOL_LEN;

    uint8_t send_na = 0;
    uint8_t sllao_set = 0;
    uint16_t packet_length = IPV6_HDR_LEN + NTOHS(ipv6_buf->length);

    /* check whick options are set, we need that because an aro
     * option condition is that a sllao option is set. thus that we don't
     * know which option comes first we need to this here */

    while (packet_length > IPV6_HDR_LEN + ICMPV6_HDR_LEN + icmpv6_opt_hdr_len) {
        opt_buf = get_opt_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

        if (opt_buf->type == NDP_OPT_SLLAO_TYPE) {
            sllao_set = 1;
        }

        icmpv6_opt_hdr_len += (opt_buf->length * 8);
    }

    icmpv6_opt_hdr_len = NBR_SOL_LEN;

    while (packet_length > IPV6_HDR_LEN + ICMPV6_HDR_LEN + icmpv6_opt_hdr_len) {
        opt_buf = get_opt_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

        switch (opt_buf->type) {
            case (NDP_OPT_SLLAO_TYPE): {
                opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len,
                                                    icmpv6_opt_hdr_len);
                llao = (uint8_t *)opt_stllao_buf;

                if (llao != NULL &&
                    !(ipv6_addr_is_unspecified(&ipv6_buf->srcaddr))) {
                    nbr_entry = ndp_neighbor_cache_search(&(ipv6_buf->srcaddr));

                    if (nbr_entry != NULL) {
                        switch (opt_stllao_buf->length) {
                            case (1): {
                                if (memcmp(&llao[2], &(nbr_entry->lladdr), 2) == 0) {
                                    nbr_entry->if_id = if_id;
                                    nbr_entry->isrouter = 0;
                                }
                                else {
                                    nbr_entry->if_id = if_id;
                                    memcpy(&nbr_entry->lladdr, &llao[2], 2);
                                    nbr_entry->state = NDP_NCE_STATUS_STALE;
                                    nbr_entry->isrouter = 0;
                                }

                                break;
                            }

                            case (2): {
                                if (memcmp(&llao[2], &(nbr_entry->lladdr), 8) == 0) {
                                    nbr_entry->if_id = if_id;
                                    nbr_entry->isrouter = 0;
                                }
                                else {
                                    nbr_entry->if_id = if_id;
                                    memcpy(&nbr_entry->lladdr, &llao[2], 8);
                                    nbr_entry->state = NDP_NCE_STATUS_STALE;
                                    nbr_entry->isrouter = 0;
                                }

                                break;
                            }

                            default:
                                break;
                        }
                    }
                    else {
                        switch (opt_stllao_buf->length) {
                            case (1): {
                                ndp_neighbor_cache_add(if_id, &ipv6_buf->srcaddr,
                                                       &llao[2], 2, 0,
                                                       NDP_NCE_STATUS_STALE,
                                                       NDP_NCE_TYPE_TENTATIVE,
                                                       NBR_CACHE_LTIME_TEN);

                                break;
                            }

                            case (2): {
                                ndp_neighbor_cache_add(if_id, &ipv6_buf->srcaddr,
                                                       &llao[2], 8, 0,
                                                       NDP_NCE_STATUS_STALE,
                                                       NDP_NCE_TYPE_TENTATIVE,
                                                       NBR_CACHE_LTIME_TEN);
                                break;
                            }

                            default:
                                break;
                        }
                    }
                }

                break;
            }

            case (OPT_ARO_TYPE): {
                /* check if sllao option is set, and if address src address
                 * isn't unspecified - draft-ietf-6lowpan-nd-15#section-6.5 */
                if (!(ipv6_addr_is_unspecified(&ipv6_buf->srcaddr)) &&
                    sllao_set == 1) {
                    uint8_t aro_state = NDP_OPT_ARO_STATE_SUCCESS;
                    opt_aro_buf = get_opt_aro_buf(ipv6_ext_hdr_len,
                                                  icmpv6_opt_hdr_len);

                    if ((opt_aro_buf->length == 2) &&
                        (opt_aro_buf->status == 0)) {
                        /* check neighbor cache for duplicates */
                        nbr_entry = ndp_neighbor_cache_search(&(ipv6_buf->srcaddr));

                        if (nbr_entry == NULL) {
                            /* create neighbor cache */
                            aro_state = ndp_neighbor_cache_add(if_id, &ipv6_buf->srcaddr,
                                                               &(opt_aro_buf->eui64), 8, 0,
                                                               NDP_NCE_STATUS_STALE, NDP_NCE_TYPE_TENTATIVE,
                                                               opt_aro_buf->reg_ltime);
                        }
                        else {
                            if (memcmp(&(nbr_entry->addr.uint16[4]),
                                       &(opt_aro_buf->eui64.uint16[0]), 8) == 0) {
                                /* update neighbor cache entry */
                                if (opt_aro_buf->reg_ltime == 0) {
                                    /* delete neighbor cache entry */
                                    ndp_neighbor_cache_remove(&nbr_entry->addr);
                                }
                                else {
                                    set_remaining_time(&(nbr_entry->ltime), (uint32_t)opt_aro_buf->reg_ltime);
                                    nbr_entry->state = NDP_NCE_STATUS_STALE;
                                    nbr_entry->isrouter = 0;
                                    memcpy(&(nbr_entry->addr.uint8[0]),
                                           &(ipv6_buf->srcaddr.uint8[0]), 16);
                                }

                                aro_state = NDP_OPT_ARO_STATE_SUCCESS;
                            }
                            else {
                                /* duplicate found */
                                aro_state = NDP_OPT_ARO_STATE_DUP_ADDR;
                            }
                        }
                    }

                    (void) aro_state;
                }

                break;
            }

            default:
                break;
        }

        icmpv6_opt_hdr_len += (opt_buf->length * 8);
    }

    ipv6_net_if_hit_t alist_targ, alist_dest;

    nbr_sol_buf = get_nbr_sol_buf(ipv6_ext_hdr_len);

    if (ipv6_net_if_addr_match(&alist_targ, &nbr_sol_buf->target_addr) != NULL) {
        ipv6_net_if_addr_match(&alist_dest, &ipv6_buf->destaddr);

        if ((memcmp(alist_targ.addr->addr_data, alist_dest.addr->addr_data, 16) == 0) ||
            ipv6_addr_is_solicited_node(&ipv6_buf->destaddr)) {
            memcpy(&(ipv6_buf->destaddr.uint8[0]),
                   &(ipv6_buf->srcaddr.uint8[0]), sizeof(ipv6_addr_t));
            memcpy(&(ipv6_buf->srcaddr.uint8[0]),
                   &(nbr_sol_buf->target_addr.uint8[0]), sizeof(ipv6_addr_t));
            send_na = 1;
        }
    }

    if (send_na) {
        /* solicited na */
        uint8_t flags = (ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE | ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED);
        icmpv6_send_neighbor_adv(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr),
                                 alist_targ.addr->addr_data, flags, 0, OPT_ARO);
    }
}

void icmpv6_send_neighbor_adv(ipv6_addr_t *src, ipv6_addr_t *dst, ipv6_addr_t *tgt,
                              uint8_t rso, uint8_t sllao, uint8_t aro)
{
    int if_id = 0;              // TODO: get this somehow
    uint16_t packet_length;

    ipv6_buf = ipv6_get_buf();
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = IPV6_PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    ipv6_ext_hdr_len = 0;
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_buf->type = ICMPV6_TYPE_NEIGHBOR_ADV;
    icmp_buf->code = 0;

    if (&ipv6_buf->destaddr != dst) {
        memcpy(&(ipv6_buf->destaddr.uint8[0]), &(dst->uint8[0]), 16);
    }

    if (&ipv6_buf->srcaddr != src) {
        memcpy(&(ipv6_buf->srcaddr.uint8[0]), &(src->uint8[0]), 16);
    }

    nbr_adv_buf = get_nbr_adv_buf(ipv6_ext_hdr_len);
    nbr_adv_buf->rso = rso;

    memset(&(nbr_adv_buf->reserved[0]), 0, 3);
    memcpy(&(nbr_adv_buf->target_addr.uint8[0]), &(tgt->uint8[0]), 16);

    packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + NBR_ADV_LEN;

    if (sllao == OPT_SLLAO) {
        /* set sllao option */
        opt_stllao_buf = get_opt_stllao_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

        if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_LONG) {
            icmpv6_ndp_set_sllao(opt_stllao_buf, if_id, NDP_OPT_SLLAO_TYPE, 2);
            icmpv6_opt_hdr_len += OPT_STLLAO_MAX_LEN;
            packet_length += OPT_STLLAO_MAX_LEN;
        }
        else {
            icmpv6_ndp_set_sllao(opt_stllao_buf, if_id, NDP_OPT_SLLAO_TYPE, 1);
            icmpv6_opt_hdr_len += OPT_STLLAO_MIN_LEN;
            packet_length += OPT_STLLAO_MIN_LEN;
        }
    }

    if (aro == OPT_ARO) {
        /* set aro option */
        opt_aro_buf = get_opt_aro_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);
        opt_aro_buf->type = OPT_ARO_TYPE;
        opt_aro_buf->length = OPT_ARO_LEN;
        opt_aro_buf->status = 0;    /* TODO */
        opt_aro_buf->reserved1 = 0;
        opt_aro_buf->reserved2 = 0;

        if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_SHORT) {
            net_if_get_eui64((net_if_eui64_t *) &opt_aro_buf->eui64, if_id, 1);
        }
        else if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_LONG) {
            net_if_get_eui64((net_if_eui64_t *) &opt_aro_buf->eui64, if_id, 0);
        }

        icmpv6_opt_hdr_len += OPT_ARO_HDR_LEN;

        packet_length += OPT_ARO_HDR_LEN;
    }

    ipv6_buf->length = HTONS(packet_length - IPV6_HDR_LEN);

    icmp_buf->checksum = icmpv6_csum(ipv6_buf, icmp_buf);

#ifdef DEBUG_ENABLED
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("INFO: send neighbor advertisement to: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_buf->destaddr));
#endif
    ipv6_send_packet(ipv6_buf);
}

void recv_nbr_adv(void)
{
    int if_id = 0;  // TODO, get this somehow
    ipv6_buf = ipv6_get_buf();
    uint16_t packet_length = IPV6_HDR_LEN + NTOHS(ipv6_buf->length);
    icmpv6_opt_hdr_len = NBR_ADV_LEN;
    llao = NULL;
    nbr_entry = NULL;
    int8_t new_ll = -1;
    nbr_adv_buf = get_nbr_adv_buf(ipv6_ext_hdr_len);

    /* check if options are present */
    while (packet_length > IPV6_HDR_LEN + ICMPV6_HDR_LEN + icmpv6_opt_hdr_len) {
        opt_buf = get_opt_buf(ipv6_ext_hdr_len, icmpv6_opt_hdr_len);

        switch (opt_buf->type) {
            case (NDP_OPT_TLLAO_TYPE): {
                llao = (uint8_t *)get_opt_stllao_buf(ipv6_ext_hdr_len,
                                                     icmpv6_opt_hdr_len);
                break;
            }

            case (OPT_ARO_TYPE): {
                break;
            }
        }

        icmpv6_opt_hdr_len += (opt_buf->length * 8);
    }

    ipv6_net_if_hit_t hit;

    if (ipv6_net_if_addr_match(&hit, &nbr_adv_buf->target_addr) == NULL) {
        nbr_entry = ndp_neighbor_cache_search(&nbr_adv_buf->target_addr);

        if (nbr_entry != NULL) {
            if (llao != 0) {
                new_ll = memcmp(&llao[2], &(nbr_entry->lladdr),
                                nbr_entry->lladdr_len);
                ((icmpv6_ndp_opt_stllao_t *)llao)->length = nbr_entry->lladdr_len / 8 + 1;
            }

            if (nbr_entry->state == NDP_NCE_STATUS_INCOMPLETE) {
                if (llao == NULL) {
                    return;
                }

                if (((icmpv6_ndp_opt_stllao_t *)llao)->length == 2) {
                    nbr_entry->if_id = if_id;
                    nbr_entry->lladdr_len = 8;
                    memcpy(&nbr_entry->lladdr, &llao[2], nbr_entry->lladdr_len);
                }
                else if (((icmpv6_ndp_opt_stllao_t *)llao)->length == 1) {
                    nbr_entry->if_id = if_id;
                    nbr_entry->lladdr_len = 2;
                    memcpy(&nbr_entry->lladdr, &llao[2], nbr_entry->lladdr_len);
                }
                else {
                    return;
                }

                if (nbr_adv_buf->rso & ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED) {
                    nbr_entry->state = NDP_NCE_STATUS_REACHABLE;
                    /* TODO: set rechability */
                }
                else {
                    nbr_entry->state = NDP_NCE_STATUS_STALE;
                }

                nbr_entry->isrouter = nbr_adv_buf->rso & ICMPV6_NEIGHBOR_ADV_FLAG_ROUTER;
            }
            else {
                if (new_ll && !(nbr_adv_buf->rso & ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE)) {
                    if (nbr_entry->state == NDP_NCE_STATUS_REACHABLE) {
                        nbr_entry->state = NDP_NCE_STATUS_STALE;
                    }

                    return;
                }
                else {
                    if ((nbr_adv_buf->rso & ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE) ||
                        (!(nbr_adv_buf->rso & ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE) && llao != 0 &&
                         !new_ll)) {
                        if (llao != 0) {
                            if (((icmpv6_ndp_opt_stllao_t *)llao)->length == 2) {
                                nbr_entry->if_id = if_id;
                                nbr_entry->lladdr_len = 8;
                            }
                            else if (((icmpv6_ndp_opt_stllao_t *)llao)->length == 1) {
                                nbr_entry->if_id = if_id;
                                nbr_entry->lladdr_len = 2;
                            }
                            else {
                                return;
                            }

                            memcpy(&nbr_entry->lladdr, &llao[2], nbr_entry->lladdr_len);
                        }

                        if (nbr_adv_buf->rso & ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED) {
                            nbr_entry->state = NDP_NCE_STATUS_REACHABLE;
                            /* TODO: set rechablility */
                        }
                        else {
                            if (llao != 0 && new_ll) {
                                nbr_entry->state = NDP_NCE_STATUS_STALE;
                            }
                        }
                    }
                }
            }
        }
    }
}

/* link-layer address option - RFC4861 section 4.6.1/ RFC4944 8. */
void icmpv6_ndp_set_sllao(icmpv6_ndp_opt_stllao_t *sllao, int if_id,
                          uint8_t type, uint8_t length)
{
    sllao->type = type;
    sllao->length = length;

    uint8_t *llao = (uint8_t *)sllao;

    /* get link layer address */
    switch (length) {
        case (1): {
            uint16_t addr = net_if_get_hardware_address(if_id);

            if (addr != 0) {
                addr = HTONS(addr);
                memcpy(&llao[2], &addr, 2);
                memset(&llao[4], 0, 4);
                break;
            }
            else {
                goto SET_SLLAO_DEFAULT;
            }
        }

        case (2): {
            net_if_eui64_t addr;

            if (net_if_get_eui64(&addr, if_id, 0)) {
                memcpy(&llao[2], &addr, 8);
                memset(&llao[10], 0, 6);
                break;
            }
            else {
                goto SET_SLLAO_DEFAULT;
            }
        }

    SET_SLLAO_DEFAULT:

        default: {
            printf("ERROR: llao not set\n");
            break;
        }
    }
}

uint16_t icmpv6_csum(ipv6_hdr_t *ipv6_buf, icmpv6_hdr_t *icmpv6_buf)
{
    uint16_t sum;
    uint16_t len = NTOHS(ipv6_buf->length);

    icmpv6_buf->checksum = 0;
    sum = len + IPV6_PROTO_NUM_ICMPV6;

    sum = csum(sum, (uint8_t *)&ipv6_buf->srcaddr, 2 * sizeof(ipv6_addr_t));
    sum = csum(sum, (uint8_t *)icmpv6_buf, len);

    return (sum == 0) ? 0 : ~HTONS(sum);
}


void icmpv6_send_parameter_prob(ipv6_addr_t *src, ipv6_addr_t *dest,
                                uint8_t code, uint32_t pointer,
                                uint8_t *packet, uint8_t packet_len)
{
    uint16_t packet_length = IPV6_HDR_LEN + ICMPV6_HDR_LEN + PARA_PROB_LEN;
    icmpv6_parameter_prob_hdr_t *para_prob_buf;

    memcpy(&(ipv6_buf[packet_length]), packet, min(IPV6_MTU - packet_length, packet_len));

    ipv6_buf = ipv6_get_buf();
    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = IPV6_PROTO_NUM_ICMPV6;
    ipv6_buf->hoplimit = ND_HOPLIMIT;

    ipv6_ext_hdr_len = 0;
    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_buf->type = ICMPV6_TYPE_PARAMETER_PROB;
    icmp_buf->code = code;

    memcpy(&(ipv6_buf->destaddr.uint8[0]), &(dest->uint8[0]), 16);
    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &(src->uint8[0]), 16);

    para_prob_buf = get_para_prob_buf(ipv6_ext_hdr_len);

    para_prob_buf->pointer = pointer;

    packet_length += min(IPV6_MTU - packet_length, packet_len);

    ipv6_buf->length = HTONS(packet_length - IPV6_HDR_LEN);

    icmp_buf->checksum = icmpv6_csum(ipv6_buf, icmp_buf);

#ifdef DEBUG_ENABLED
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("INFO: send parameter problem to: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_buf->destaddr));
#endif
    ipv6_send_packet(ipv6_buf);
}

//------------------------------------------------------------------------------
/* neighbor cache functions */

ndp_neighbor_cache_t *ndp_neighbor_cache_search(ipv6_addr_t *ipaddr)
{
    int i;

    for (i = 0; i < NBR_CACHE_SIZE; i++) {
        if (memcmp(&(nbr_cache[i].addr.uint8[0]), &(ipaddr->uint8[0]), 16) == 0) {
            return &nbr_cache[i];
        }
    }

    return NULL;
}

ndp_neighbor_cache_t *ndp_get_ll_address(ipv6_addr_t *ipaddr)
{
    ndp_neighbor_cache_t *nce = ndp_neighbor_cache_search(ipaddr);

    if (nce == NULL || nce->type == NDP_NCE_TYPE_GC ||
        nce->state == NDP_NCE_STATUS_INCOMPLETE) {
        // TODO: send neighbor solicitation, wait, and recheck cache
        return NULL;
    }

    return nce;
}

int ndp_addr_is_on_link(ipv6_addr_t *dest_addr)
{
    ndp_prefix_info_t *pi;
    ndp_neighbor_cache_t *nce;
    int if_id = -1;

    if ((nce = ndp_neighbor_cache_search(dest_addr))) {
        DEBUG("INFO: %s is in nbr cache\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, dest_addr));
        return 1;
    }

    while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
        if ((pi = ndp_prefix_info_search(if_id, dest_addr, 128))) {
            return (pi->flags & ICMPV6_NDP_OPT_PI_FLAG_ON_LINK) != 0;
        }
    }

    /* TODO Other cases (http://tools.ietf.org/html/rfc4861#page-6):
     *  * neighboring router specifies address as target of
     *    redircect message
     *  * neighbor discovery message is received from address.
     */

    return 0;
}

uint8_t ndp_neighbor_cache_add(int if_id, const ipv6_addr_t *ipaddr,
                               const void *lladdr, uint8_t lladdr_len,
                               uint8_t isrouter, ndp_nce_state_t state,
                               ndp_nce_type_t type, uint16_t ltime)
{
    (void) ltime;

    if (nbr_count == NBR_CACHE_SIZE) {
        printf("ERROR: neighbor cache full\n");
        return NDP_OPT_ARO_STATE_NBR_CACHE_FULL;
    }

    nbr_cache[nbr_count].if_id = if_id;
    memcpy(&(nbr_cache[nbr_count].addr), ipaddr, 16);
    memcpy(&(nbr_cache[nbr_count].lladdr), lladdr, lladdr_len);
    nbr_cache[nbr_count].lladdr_len = lladdr_len;
    nbr_cache[nbr_count].isrouter = isrouter;
    nbr_cache[nbr_count].state = state;
    nbr_cache[nbr_count].type = type;

    //vtimer_set_wakeup(&(nbr_cache[nbr_count].ltime), t,
    /*                  nd_nbr_cache_rem_pid); */

    nbr_count++;

    return NDP_OPT_ARO_STATE_SUCCESS;
}

void nbr_cache_auto_rem(void)
{
    int i;

    for (i = 0; i < NBR_CACHE_SIZE; i++) {
        if (get_remaining_time(&(nbr_cache[i].ltime)) == 0 &&
            nbr_cache[i].type == NDP_NCE_TYPE_TENTATIVE) {
            memmove(&(nbr_cache[i]), &(nbr_cache[nbr_count]),
                    sizeof(ndp_neighbor_cache_t));
            memset(&(nbr_cache[nbr_count]), 0, sizeof(ndp_neighbor_cache_t));
            nbr_count--;
        }
    }
}

uint8_t ndp_neighbor_cache_remove(const ipv6_addr_t *ipaddr)
{
    int i;
    uint8_t removed = 0;

    for (i = 0; i < NBR_CACHE_SIZE; i++) {
        if (memcmp(&(nbr_cache[i].addr.uint8[0]), &(ipaddr->uint8[0]), 16) == 0) {
            memmove(&(nbr_cache[i]), &(nbr_cache[nbr_count]),
                    sizeof(ndp_neighbor_cache_t));
            memset(&(nbr_cache[nbr_count]), 0, sizeof(ndp_neighbor_cache_t));
            nbr_count--;
            removed = 1;
        }
    }

    return removed;
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
ndp_a6br_cache_t *ndp_a6br_cache_get_most_current(void)
{
    ndp_a6br_cache_t *abr = NULL;
    int i;
    int version = abr_cache[0].version;

    for (i = 0; i < abr_count; i++) {
        if (serial_comp16(version, abr_cache[i].version) == GREATER) {
            abr = &(abr_cache[i]);
            version = abr_cache[i].version;
        }
    }

    return abr;
}

ndp_a6br_cache_t *ndp_a6br_cache_get_oldest(void)
{
    ndp_a6br_cache_t *abr = NULL;
    int i;
    int version = abr_cache[0].version;

    for (i = 0; i < abr_count; i++) {
        if (serial_comp16(version, abr_cache[i].version) == LESS) {
            abr = &(abr_cache[i]);
            version = abr_cache[i].version;
        }
    }

    return abr;
}

ndp_a6br_cache_t *abr_get_version(uint16_t version, ipv6_addr_t *abr_addr)
{
    int i = 0;

    for (i = 0; i < ABR_CACHE_SIZE; i++) {
        if (abr_cache[i].version == version &&
            memcmp(&(abr_cache[i].abr_addr.uint8[0]),
                   &(abr_addr->uint8[0]), 16
                  ) == 0) {
            return &(abr_cache[i]);
        }
    }

    return NULL;
}

lowpan_context_t *abr_get_context(ndp_a6br_cache_t *abr, uint8_t cid)
{
    if (abr->cids[cid] != cid) {
        return NULL;
    }

    return lowpan_context_num_lookup(abr->cids[cid]);
}

ndp_a6br_cache_t *abr_add_context(uint16_t version, ipv6_addr_t *abr_addr,
                                  uint8_t cid)
{
    ndp_a6br_cache_t *abr = abr_get_version(version, abr_addr);

    if (abr == NULL) {
        if (abr_count == ABR_CACHE_SIZE) {
            abr = ndp_a6br_cache_get_oldest();
        }
        else {
            abr = &(abr_cache[abr_count++]);
        }

        abr->version = version;
        memcpy(&(abr->abr_addr), abr_addr, sizeof(ipv6_addr_t));
        memset(abr->cids, 0xFF, NDP_6LOWPAN_CONTEXT_MAX);
    }

    abr->cids[cid] = cid;

    return abr;
}

void abr_remove_context(uint8_t cid)
{
    int i;

    for (i = 0; i < abr_count; i++) {
        abr_cache[i].cids[cid] = 0xFF;
    }
}

//------------------------------------------------------------------------------
/* default router list functions */

ndp_default_router_list_t *ndp_default_router_list_search(ipv6_addr_t *ipaddr)
{
    int i;

    for (i = 0; i < DEF_RTR_LST_SIZE; i++) {
        if (memcmp(&def_rtr_lst[i].addr.uint8[0],
                   &(ipaddr->uint8[0]), 16) == 0) {
            return &def_rtr_lst[i];
        }
    }

    return NULL;
}

void def_rtr_lst_add(ipv6_addr_t *ipaddr, uint32_t rtr_ltime)
{
    if (def_rtr_count == DEF_RTR_LST_SIZE) {
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

void def_rtr_lst_rem(ndp_default_router_list_t *entry)
{
    int i;

    for (i = 0; i < DEF_RTR_LST_SIZE; i++) {
        if (&def_rtr_lst[i] == entry) {
            /* search the to deleted item, then memmove the last item to its
             * position, and decrement array count */
            memmove(entry, &def_rtr_lst[def_rtr_count], sizeof(ndp_default_router_list_t));
            memset(&def_rtr_lst[def_rtr_count], 0, sizeof(ndp_default_router_list_t));
            def_rtr_count--;
        }
    }
}

//------------------------------------------------------------------------------
/* prefix information functions */

int ndp_add_prefix_info(int if_id, const ipv6_addr_t *prefix,
                        uint8_t prefix_len, uint32_t valid_lifetime,
                        uint32_t preferred_lifetime, uint8_t advertisable,
                        uint8_t flags)
{
    ndp_prefix_info_t *prefix_info = ndp_prefix_info_match(if_id, prefix,
                                     prefix_len);

    if (prefix_info) {
        prefix_info->inuse = 1;
        prefix_info->advertisable = advertisable;
        prefix_info->flags = 0xc0 & flags;
        prefix_info->valid_lifetime = HTONL(valid_lifetime);
        prefix_info->preferred_lifetime = HTONL(preferred_lifetime);

        return SIXLOWERROR_SUCCESS;
    }

    if ((prefix_info_count >= PREFIX_BUF_LEN) ||
            (prefix_buf_count >= sizeof(prefix_buf))) {
        return SIXLOWERROR_ARRAYFULL;
    }

    if (prefix_len > 128) {
        prefix_len = 128;
    }

    prefix_info = &prefix_info_buf[prefix_info_count];

    if (prefix_len > 0) {
        memcpy(&prefix_buf[prefix_buf_count], prefix, (prefix_len / 8) + 1);
    }

    prefix_info->prefix_data = (ipv6_addr_t *) &prefix_buf[prefix_buf_count];
    prefix_buf_count += prefix_len;

    prefix_info->prefix_len = prefix_len;
    prefix_info->prefix_protocol = NET_IF_L3P_IPV6_PREFIX;
    prefix_info->inuse = 1;
    prefix_info->advertisable = advertisable;
    prefix_info->flags = 0xc0 & flags;
    prefix_info->valid_lifetime = HTONL(valid_lifetime);
    prefix_info->preferred_lifetime = HTONL(preferred_lifetime);

    if (!net_if_add_address(if_id, (net_if_addr_t *) prefix_info)) {
        return SIXLOWERROR_VALUE;
    }

    prefix_info_count++;

    return SIXLOWERROR_SUCCESS;
}

ndp_prefix_info_t *ndp_prefix_info_search(int if_id, const ipv6_addr_t *addr,
        uint8_t up_to)
{
    uint8_t best_match = 0;
    ndp_prefix_info_t *prefix = NULL, *tmp = NULL;

    if (up_to > 128) {
        up_to = 128;
    }

    while (net_if_iter_addresses(if_id, (net_if_addr_t **) &prefix)) {
        if (prefix->prefix_protocol & NET_IF_L3P_IPV6_PREFIX) {
            uint8_t match = 0, len = min(up_to, prefix->prefix_len);
            uint8_t byte_len = (len / 8);

            for (int i = 0; i < byte_len; i++) {
                if (addr->uint8[i] != prefix->prefix_data->uint8[i]) {
                    break;
                }

                match += 8;
            }

            if (byte_len < 16) {
                for (int i = len % 8; i > 0; i--) {
                    if (addr->uint8[byte_len] >> i !=
                        prefix->prefix_data->uint8[byte_len] >> i) {
                        break;
                    }

                    match += 1;
                }
            }

            if (prefix->prefix_len == 0 && match > best_match) {
                tmp = prefix;
                best_match = match;
            }
        }
    }

    return tmp;
}

ndp_prefix_info_t *ndp_prefix_info_match(int if_id, const ipv6_addr_t *prefix,
        uint8_t prefix_len)
{
    ndp_prefix_info_t *res = ndp_prefix_info_search(if_id, prefix,
                             prefix_len);

    if (res != NULL && res->prefix_len == prefix_len) {
        return res;
    }
    else {
        return NULL;
    }
}
