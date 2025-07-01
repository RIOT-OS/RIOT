/*
 * Copyright (C) 2018       HAW Hamburg
 * Copyright (C) 2015–2017  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 * Copyright (C) 2013–2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 *
 * @file
 *
 * @author Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#include <assert.h>
#include <string.h>
#include "kernel_defines.h"
#if IS_USED(MODULE_ZTIMER_MSEC)
#include "ztimer.h"
#else
#include "xtimer.h"
#endif

#include "net/af.h"
#include "net/icmpv6.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc.h"
#include "net/eui64.h"
#include "gnrc_rpl_internal/globals.h"

#ifdef MODULE_NETSTATS_RPL
#include "gnrc_rpl_internal/netstats.h"
#endif

#include "net/gnrc/rpl.h"
#include "gnrc_rpl_internal/validation.h"

#ifdef MODULE_GNRC_RPL_P2P
#include "net/gnrc/rpl/p2p_structs.h"
#include "net/gnrc/rpl/p2p_dodag.h"
#include "net/gnrc/rpl/p2p.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

#define GNRC_RPL_GROUNDED_SHIFT             (7)
#define GNRC_RPL_MOP_SHIFT                  (3)
#define GNRC_RPL_OPT_TRANSIT_E_FLAG_SHIFT   (7)
#define GNRC_RPL_OPT_TRANSIT_E_FLAG         (1 << GNRC_RPL_OPT_TRANSIT_E_FLAG_SHIFT)
#define GNRC_RPL_SHIFTED_MOP_MASK           (0x7)
#define GNRC_RPL_PRF_MASK                   (0x7)
#define GNRC_RPL_PREFIX_AUTO_ADDRESS_BIT    (1 << 6)

/**
 * @brief   Checks validity of DIO control messages
 *
 * @param[in]   dio         The DIO control message
 * @param[in]   len         Length of the DIO control message
 *
 * @return  true, if @p dio is valid
 * @return  false, otherwise
 */
static inline bool gnrc_rpl_validation_DIO(gnrc_rpl_dio_t *dio, uint16_t len)
{
    uint16_t expected_len = sizeof(*dio) + sizeof(icmpv6_hdr_t);

    if (expected_len <= len) {
        return true;
    }

    DEBUG("RPL: wrong DIO len: %d, expected: %d\n", len, expected_len);

    return false;
}

/**
 * @brief   Checks validity of DIS control messages
 *
 * @param[in]   dis     The DIS control message
 * @param[in]   len     Length of the DIS control message
 *
 * @return  true, if @p dis is valid
 * @return  false, otherwise
 */
static inline bool gnrc_rpl_validation_DIS(gnrc_rpl_dis_t *dis, uint16_t len)
{
    uint16_t expected_len = sizeof(*dis) + sizeof(icmpv6_hdr_t);

    if (expected_len <= len) {
        return true;
    }

    DEBUG("RPL: wrong DIS len: %d, expected: %d\n", len, expected_len);

    return false;
}

/**
 * @brief   Checks validity of DAO control messages
 *
 * @param[in]   dao         The DAO control message
 * @param[in]   len         Length of the DAO control message
 *
 * @return  true, if @p dao is valid
 * @return  false, otherwise
 */
static inline bool gnrc_rpl_validation_DAO(gnrc_rpl_dao_t *dao, uint16_t len)
{
    uint16_t expected_len = sizeof(*dao) + sizeof(icmpv6_hdr_t);

    if (expected_len <= len) {
        if ((dao->k_d_flags & GNRC_RPL_DAO_D_BIT)) {
            expected_len += sizeof(ipv6_addr_t);
        }

        if (expected_len <= len) {
            return true;
        }
    }

    DEBUG("RPL: wrong DAO len: %d, expected: %d\n", len, expected_len);

    return false;
}

/**
 * @brief   Checks validity of DAO-ACK control messages
 *
 * @param[in]   dao_ack     The DAO-ACK control message
 * @param[in]   len         Length of the DAO-ACK control message
 * @param[in]   dst         Pointer to the destination address of the IPv6 packet.
 *
 * @return  true, if @p dao_ack is valid
 * @return  false, otherwise
 */
static inline bool gnrc_rpl_validation_DAO_ACK(gnrc_rpl_dao_ack_t *dao_ack,
                                               uint16_t len,
                                               ipv6_addr_t *dst)
{
    uint16_t expected_len = sizeof(*dao_ack) + sizeof(icmpv6_hdr_t);

    if (ipv6_addr_is_multicast(dst)) {
        DEBUG("RPL: received DAO-ACK on multicast address\n");
        return false;
    }

    if (expected_len <= len) {
        if ((dao_ack->d_reserved & GNRC_RPL_DAO_ACK_D_BIT)) {
            expected_len += sizeof(ipv6_addr_t);
        }

        if (expected_len == len) {
            return true;
        }
    }

    DEBUG("RPL: wrong DAO-ACK len: %d, expected: %d\n", len, expected_len);

    return false;
}

static gnrc_netif_t *_find_interface_with_rpl_mcast(void)
{
    gnrc_netif_t *netif = NULL;

    while ((netif = gnrc_netif_iter(netif))) {
        for (unsigned i = 0; i < GNRC_NETIF_IPV6_GROUPS_NUMOF; i++) {
            if (ipv6_addr_equal(&netif->ipv6.groups[i], &ipv6_addr_all_rpl_nodes)) {
                return netif;
            }
        }
    }
    return NULL;
}

void gnrc_rpl_send(gnrc_pktsnip_t *pkt, kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *dst,
                   ipv6_addr_t *dodag_id)
{
    gnrc_netif_t *netif;

    (void)dodag_id;
    gnrc_pktsnip_t *hdr;

    if (iface == KERNEL_PID_UNDEF) {
        netif = _find_interface_with_rpl_mcast();

        if (netif == NULL) {
            DEBUG("RPL: no suitable interface found for this destination address\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }
    else {
        netif = gnrc_netif_get_by_pid(iface);
    }

    if (dst == NULL) {
        dst = (ipv6_addr_t *)&ipv6_addr_all_rpl_nodes;
    }

    if (src == NULL) {
        src = gnrc_netif_ipv6_addr_best_src(netif, dst, true);

        if (src == NULL) {
            DEBUG("RPL: no suitable src address found\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }

    hdr = gnrc_ipv6_hdr_build(pkt, src, dst);

    if (hdr == NULL) {
        DEBUG("RPL: Send - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;

    hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (hdr == NULL) {
        DEBUG("RPL: Send - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    gnrc_netif_hdr_set_netif(hdr->data, netif);
    pkt = gnrc_pkt_prepend(pkt, hdr);

    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("RPL: cannot send packet: no subscribers found.\n");
        gnrc_pktbuf_release(pkt);
    }
}

static gnrc_pktsnip_t *_dio_dodag_conf_build(gnrc_pktsnip_t *pkt, gnrc_rpl_dodag_t *dodag)
{
    gnrc_rpl_opt_dodag_conf_t *dodag_conf;
    gnrc_pktsnip_t *opt_snip;

    if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_opt_dodag_conf_t),
                                    GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: BUILD DODAG CONF - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    dodag_conf = opt_snip->data;
    dodag_conf->type = GNRC_RPL_OPT_DODAG_CONF;
    dodag_conf->length = GNRC_RPL_OPT_DODAG_CONF_LEN;
    dodag_conf->flags_a_pcs = 0;
    dodag_conf->dio_int_doubl = dodag->dio_interval_doubl;
    dodag_conf->dio_int_min = dodag->dio_min;
    dodag_conf->dio_redun = dodag->dio_redun;
    dodag_conf->max_rank_inc = byteorder_htons(dodag->instance->max_rank_inc);
    dodag_conf->min_hop_rank_inc = byteorder_htons(dodag->instance->min_hop_rank_inc);
    dodag_conf->ocp = byteorder_htons(dodag->instance->of->ocp);
    dodag_conf->reserved = 0;
    dodag_conf->default_lifetime = dodag->default_lifetime;
    dodag_conf->lifetime_unit = byteorder_htons(dodag->lifetime_unit);
    return opt_snip;
}

static gnrc_pktsnip_t *_dis_solicited_opt_build(gnrc_pktsnip_t *pkt,
                                                gnrc_rpl_internal_opt_dis_solicited_t *opt)
{
    gnrc_pktsnip_t *opt_snip;
    size_t snip_size = sizeof(gnrc_rpl_opt_dis_solicited_t);

    if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, snip_size,
                                    GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: BUILD SOLICITED OPT - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }

    gnrc_rpl_opt_dis_solicited_t *solicited_information;

    solicited_information = opt_snip->data;

    solicited_information->type = GNRC_RPL_OPT_SOLICITED_INFO;
    solicited_information->length = GNRC_RPL_DIS_SOLICITED_INFO_LENGTH;
    solicited_information->instance_id = opt->instance_id;

    solicited_information->VID_flags = opt->VID_flags;
    solicited_information->dodag_id = opt->dodag_id;
    solicited_information->version_number = opt->version_number;

    return opt_snip;
}

static bool _get_pl_entry(unsigned iface, ipv6_addr_t *pfx,
                          unsigned pfx_len, gnrc_ipv6_nib_pl_t *ple)
{
    void *state = NULL;

    while (gnrc_ipv6_nib_pl_iter(iface, &state, ple)) {
        if (ipv6_addr_match_prefix(&ple->pfx, pfx) >= pfx_len) {
            return true;
        }
    }
    return false;
}

static gnrc_pktsnip_t *_dio_prefix_info_build(gnrc_pktsnip_t *pkt, gnrc_rpl_dodag_t *dodag)
{
    gnrc_ipv6_nib_pl_t ple;
    gnrc_rpl_opt_prefix_info_t *prefix_info;
    gnrc_pktsnip_t *opt_snip;

    if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_opt_prefix_info_t),
                                    GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: BUILD PREFIX INFO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    prefix_info = opt_snip->data;
    prefix_info->type = GNRC_RPL_OPT_PREFIX_INFO;
    prefix_info->length = GNRC_RPL_OPT_PREFIX_INFO_LEN;
    /* auto-address configuration */
    prefix_info->LAR_flags = GNRC_RPL_PREFIX_AUTO_ADDRESS_BIT;
    prefix_info->prefix_len = 64;
    if (_get_pl_entry(dodag->iface, &dodag->dodag_id, prefix_info->prefix_len,
                      &ple)) {
#if IS_USED(MODULE_ZTIMER_MSEC)
        uint32_t now = (uint32_t)ztimer_now(ZTIMER_MSEC);
#else
        uint32_t now = (xtimer_now_usec64() / US_PER_MS) & UINT32_MAX;
#endif
        uint32_t valid_ltime = (ple.valid_until < UINT32_MAX) ?
                               (ple.valid_until - now) / MS_PER_SEC : UINT32_MAX;
        uint32_t pref_ltime = (ple.pref_until < UINT32_MAX) ?
                              (ple.pref_until - now) / MS_PER_SEC : UINT32_MAX;

        prefix_info->valid_lifetime = byteorder_htonl(valid_ltime);
        prefix_info->pref_lifetime = byteorder_htonl(pref_ltime);
    }
    else {
        DEBUG("RPL: Prefix of DODAG-ID not in prefix list\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    prefix_info->reserved = 0;

    memset(&prefix_info->prefix, 0, sizeof(prefix_info->prefix));
    ipv6_addr_init_prefix(&prefix_info->prefix, &dodag->dodag_id,
                          prefix_info->prefix_len);
    return opt_snip;
}

void gnrc_rpl_send_DIO(gnrc_rpl_instance_t *inst, ipv6_addr_t *destination)
{
    if (inst == NULL) {
        DEBUG("RPL: Error - trying to send DIO without being part of a dodag.\n");
        return;
    }

    gnrc_rpl_dodag_t *dodag = &inst->dodag;
    gnrc_pktsnip_t *pkt = NULL, *tmp;
    gnrc_rpl_dio_t *dio;

#ifdef MODULE_GNRC_RPL_P2P
    gnrc_rpl_p2p_ext_t *p2p_ext = gnrc_rpl_p2p_ext_get(dodag);
    if (dodag->instance->mop == GNRC_RPL_P2P_MOP) {
        if (!p2p_ext->for_me) {
            if ((pkt = gnrc_rpl_p2p_rdo_build(pkt, p2p_ext)) == NULL) {
                return;
            }
        }
        dodag->dio_opts &= ~GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO;
    }
#endif

    if (!IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_PIO) &&
        dodag->dio_opts & GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO) {
        if ((pkt = _dio_prefix_info_build(pkt, dodag)) == NULL) {
            return;
        }
    }

    if (dodag->dio_opts & GNRC_RPL_REQ_DIO_OPT_DODAG_CONF) {
        if ((pkt = _dio_dodag_conf_build(pkt, dodag)) == NULL) {
            return;
        }
        dodag->dio_opts &= ~GNRC_RPL_REQ_DIO_OPT_DODAG_CONF;
    }

    if ((tmp = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_dio_t), GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: Send DIO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp;
    dio = pkt->data;
    dio->instance_id = inst->id;
    dio->version_number = dodag->version;
    /* a leaf node announces an INFINITE_RANK */
    dio->rank = ((dodag->node_status == GNRC_RPL_LEAF_NODE) ?
                 byteorder_htons(GNRC_RPL_INFINITE_RANK) : byteorder_htons(dodag->my_rank));
    dio->g_mop_prf = (dodag->grounded << GNRC_RPL_GROUNDED_SHIFT) |
                     (inst->mop << GNRC_RPL_MOP_SHIFT) | dodag->prf;
    dio->dtsn = dodag->dtsn;
    dio->flags = 0;
    dio->reserved = 0;
    dio->dodag_id = dodag->dodag_id;

    if ((tmp = gnrc_icmpv6_build(pkt, ICMPV6_RPL_CTRL, GNRC_RPL_ICMPV6_CODE_DIO,
                                 sizeof(icmpv6_hdr_t))) == NULL) {
        DEBUG("RPL: Send DIO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp;

#ifdef MODULE_NETSTATS_RPL
    gnrc_rpl_netstats_tx_DIO(&gnrc_rpl_netstats, gnrc_pkt_len(pkt),
                             (destination && !ipv6_addr_is_multicast(destination)));
#endif

    gnrc_rpl_send(pkt, dodag->iface, NULL, destination, &dodag->dodag_id);
}

void gnrc_rpl_send_DIS(gnrc_rpl_instance_t *inst, ipv6_addr_t *destination,
                       gnrc_rpl_internal_opt_t **options, size_t num_opts)
{
    gnrc_pktsnip_t *pkt = NULL, *tmp;
    gnrc_rpl_dis_t *dis;

    /* No options provided to be attached to the DIS, so we PadN 2 bytes */
    if (options == NULL || num_opts == 0) {
        assert(!options);
        gnrc_pktsnip_t *opt_snip;
        size_t snip_size = 0;
        /* The DIS is too small so that wireshark complains about an incorrect
         * ethernet frame check sequence.
         * To trick it we PadN 2 additional bytes, i.e. 4 bytes in sum. */
        uint8_t padding[] = {
            GNRC_RPL_OPT_PADN,  /* Option Type */
            0x02,               /* Number of extra padding bytes */
            0x00, 0x00
        };

        snip_size = sizeof(padding);
        if ((opt_snip = gnrc_pktbuf_add(NULL, NULL, snip_size,
                                        GNRC_NETTYPE_UNDEF)) == NULL) {
            DEBUG("RPL: BUILD PadN OPT - no space left in packet buffer\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        memcpy(opt_snip->data, padding, snip_size);
        pkt = opt_snip;
    }
    else {
        assert(options);
        for (size_t i = 0; i < num_opts; ++i) {
            if (options[i]->type == GNRC_RPL_OPT_SOLICITED_INFO) {
                if ((pkt = _dis_solicited_opt_build(pkt,
                                                    (gnrc_rpl_internal_opt_dis_solicited_t *)options
                                                    [i])) == NULL) {
                    return;
                }
            }
        }
    }
    if ((tmp = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_dis_t), GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: Send DIS - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp;
    dis = (gnrc_rpl_dis_t *)pkt->data;
    dis->flags = 0;
    dis->reserved = 0;

    if ((tmp = gnrc_icmpv6_build(pkt, ICMPV6_RPL_CTRL, GNRC_RPL_ICMPV6_CODE_DIS,
                                 sizeof(icmpv6_hdr_t))) == NULL) {
        DEBUG("RPL: Send DIS - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp;
#ifdef MODULE_NETSTATS_RPL
    gnrc_rpl_netstats_tx_DIS(&gnrc_rpl_netstats, gnrc_pkt_len(pkt),
                             (destination && !ipv6_addr_is_multicast(destination)));
#endif

    gnrc_rpl_send(pkt, KERNEL_PID_UNDEF, NULL, destination, (inst? &(inst->dodag.dodag_id) : NULL));
}

static inline uint32_t _sec_to_ms(uint32_t sec)
{
    if (sec == UINT32_MAX) {
        /* infinite stays infinite */
        return UINT32_MAX;
    }
    else if (sec > ((UINT32_MAX - 1) / MS_PER_SEC)) {
        /* truncate long intervals to largest possible value */
        return UINT32_MAX - 1;
    }
    else {
        return sec * MS_PER_SEC;
    }
}

/** @todo allow target prefixes in target options to be of variable length */
static bool _parse_options(int msg_type, gnrc_rpl_instance_t *inst, gnrc_rpl_opt_t *opt,
                           uint16_t len,
                           ipv6_addr_t *src, uint32_t *included_opts)
{
    uint16_t len_parsed = 0;
    gnrc_rpl_opt_target_t *first_target = NULL;
    gnrc_rpl_dodag_t *dodag = &inst->dodag;
    eui64_t iid;

    *included_opts = 0;

    if (!IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_VALIDATION)) {
        if (!gnrc_rpl_validation_options(msg_type, inst, opt, len)) {
            return false;
        }
    }

    while (len_parsed < len) {
        switch (opt->type) {
        case (GNRC_RPL_OPT_PAD1):
            DEBUG("RPL: PAD1 option parsed\n");
            *included_opts |= ((uint32_t)1) << GNRC_RPL_OPT_PAD1;
            len_parsed += 1;
            opt = (gnrc_rpl_opt_t *)(((uint8_t *)opt) + 1);
            continue;

        case (GNRC_RPL_OPT_PADN):
            DEBUG("RPL: PADN option parsed\n");
            *included_opts |= ((uint32_t)1) << GNRC_RPL_OPT_PADN;
            break;

        case (GNRC_RPL_OPT_DODAG_CONF):
            DEBUG("RPL: DODAG CONF DIO option parsed\n");
            *included_opts |= ((uint32_t)1) << GNRC_RPL_OPT_DODAG_CONF;
            dodag->dio_opts |= GNRC_RPL_REQ_DIO_OPT_DODAG_CONF;
            gnrc_rpl_opt_dodag_conf_t *dc = (gnrc_rpl_opt_dodag_conf_t *)opt;
            gnrc_rpl_of_t *of = gnrc_rpl_get_of_for_ocp(byteorder_ntohs(dc->ocp));
            if (of != NULL) {
                inst->of = of;
            }
            else {
                DEBUG("RPL: Unsupported OCP 0x%02x\n", byteorder_ntohs(dc->ocp));
                inst->of = gnrc_rpl_get_of_for_ocp(GNRC_RPL_DEFAULT_OCP);
            }
            dodag->dio_interval_doubl = dc->dio_int_doubl;
            dodag->dio_min = dc->dio_int_min;
            dodag->dio_redun = dc->dio_redun;
            inst->max_rank_inc = byteorder_ntohs(dc->max_rank_inc);
            inst->min_hop_rank_inc = byteorder_ntohs(dc->min_hop_rank_inc);
            dodag->default_lifetime = dc->default_lifetime;
            dodag->lifetime_unit = byteorder_ntohs(dc->lifetime_unit);
            dodag->trickle.Imin = (1 << dodag->dio_min);
            dodag->trickle.Imax = dodag->dio_interval_doubl;
            dodag->trickle.k = dodag->dio_redun;
            break;

        case (GNRC_RPL_OPT_PREFIX_INFO):
            DEBUG("RPL: Prefix Information DIO option parsed\n");
            *included_opts |= ((uint32_t)1) << GNRC_RPL_OPT_PREFIX_INFO;

            if (!IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_PIO)) {
                dodag->dio_opts |= GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO;
            }

            gnrc_rpl_opt_prefix_info_t *pi = (gnrc_rpl_opt_prefix_info_t *)opt;
            /* check for the auto address-configuration flag */
            gnrc_netif_t *netif = gnrc_netif_get_by_pid(dodag->iface);

            assert(netif != NULL);
            if ((gnrc_netif_ipv6_get_iid(netif, &iid) < 0)
                && !(pi->LAR_flags & GNRC_RPL_PREFIX_AUTO_ADDRESS_BIT)) {
                break;
            }
            ipv6_addr_set_aiid(&pi->prefix, iid.uint8);
            /* TODO: find a way to do this with DAD (i.e. state != VALID) */
            gnrc_netif_ipv6_addr_add_internal(netif, &pi->prefix, pi->prefix_len,
                                              GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);
            /* set lifetimes */
            gnrc_ipv6_nib_pl_set(netif->pid, &pi->prefix, pi->prefix_len,
                                 _sec_to_ms(byteorder_ntohl(pi->valid_lifetime)),
                                 _sec_to_ms(byteorder_ntohl(pi->pref_lifetime)));

            break;
        case (GNRC_RPL_OPT_SOLICITED_INFO):
            DEBUG("RPL: RPL SOLICITED INFO option parsed\n");
            *included_opts |= ((uint32_t)1) << GNRC_RPL_OPT_SOLICITED_INFO;
            gnrc_rpl_opt_dis_solicited_t *sol = (gnrc_rpl_opt_dis_solicited_t *)opt;

            /* check expected length */
            if (sol->length != GNRC_RPL_DIS_SOLICITED_INFO_LENGTH) {
                DEBUG("RPL: RPL SOLICITED INFO option, unexpected length: %d\n", sol->length);
                return false;
            }

            /* check the DODAG Version */
            if ((sol->VID_flags & GNRC_RPL_DIS_SOLICITED_INFO_FLAG_V)
                && (sol->version_number != inst->dodag.version)) {
                DEBUG("RPL: RPL SOLICITED INFO option, ignore DIS cause: DODAG Version mismatch\n");
                return false;
            }

            /* check the Instance ID */
            if ((sol->VID_flags & GNRC_RPL_DIS_SOLICITED_INFO_FLAG_I)
                && (sol->instance_id != inst->id)) {
                DEBUG("RPL: RPL SOLICITED INFO option, ignore DIS cause: InstanceID mismatch\n");
                return false;
            }

            /* check the DODAG ID */
            if (sol->VID_flags & GNRC_RPL_DIS_SOLICITED_INFO_FLAG_D) {
                if (memcmp(&sol->dodag_id, &inst->dodag.dodag_id, sizeof(ipv6_addr_t)) != 0) {
                    DEBUG("RPL: RPL SOLICITED INFO option, ignore DIS cause: DODAGID mismatch\n");
                    return false;
                }
            }
            break;
        case (GNRC_RPL_OPT_TARGET):
            DEBUG("RPL: RPL TARGET DAO option parsed\n");
            *included_opts |= ((uint32_t)1) << GNRC_RPL_OPT_TARGET;

            gnrc_rpl_opt_target_t *target = (gnrc_rpl_opt_target_t *)opt;
            if (first_target == NULL) {
                first_target = target;
            }

            DEBUG("RPL: adding FT entry %s/%d\n",
                  ipv6_addr_to_str(addr_str, &(target->target), (unsigned)sizeof(addr_str)),
                  target->prefix_length);

            gnrc_ipv6_nib_ft_del(&(target->target), target->prefix_length);
            gnrc_ipv6_nib_ft_add(&(target->target), target->prefix_length, src,
                                 dodag->iface,
                                 dodag->default_lifetime * dodag->lifetime_unit);
            break;

        case (GNRC_RPL_OPT_TRANSIT):
            DEBUG("RPL: RPL TRANSIT INFO DAO option parsed\n");
            *included_opts |= ((uint32_t)1) << GNRC_RPL_OPT_TRANSIT;
            gnrc_rpl_opt_transit_t *transit = (gnrc_rpl_opt_transit_t *)opt;
            if (first_target == NULL) {
                DEBUG("RPL: Encountered a RPL TRANSIT DAO option without "
                      "a preceding RPL TARGET DAO option\n");
                break;
            }

            do {
                DEBUG("RPL: updating FT entry %s/%d\n",
                      ipv6_addr_to_str(addr_str, &(first_target->target), sizeof(addr_str)),
                      first_target->prefix_length);

                gnrc_ipv6_nib_ft_del(&(first_target->target),
                                     first_target->prefix_length);
                gnrc_ipv6_nib_ft_add(&(first_target->target),
                                     first_target->prefix_length, src,
                                     dodag->iface,
                                     transit->path_lifetime * dodag->lifetime_unit);

                first_target = (gnrc_rpl_opt_target_t *)(((uint8_t *)(first_target)) +
                                                         sizeof(gnrc_rpl_opt_t) +
                                                         first_target->length);
            }while (first_target->type == GNRC_RPL_OPT_TARGET);

            first_target = NULL;
            break;

#ifdef MODULE_GNRC_RPL_P2P
        case (GNRC_RPL_P2P_OPT_RDO):
            gnrc_rpl_p2p_rdo_parse((gnrc_rpl_p2p_opt_rdo_t *)opt, gnrc_rpl_p2p_ext_get(dodag));
            break;
#endif
        }
        len_parsed += opt->length + sizeof(gnrc_rpl_opt_t);
        opt = (gnrc_rpl_opt_t *)(((uint8_t *)(opt + 1)) + opt->length);
    }
    return true;
}

void gnrc_rpl_recv_DIS(gnrc_rpl_dis_t *dis, kernel_pid_t iface, ipv6_addr_t *src,
                       ipv6_addr_t *dst, uint16_t len)
{
    (void)iface;

#ifdef MODULE_NETSTATS_RPL
    gnrc_rpl_netstats_rx_DIS(&gnrc_rpl_netstats, len, (dst && !ipv6_addr_is_multicast(dst)));
#endif

    if (!IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_VALIDATION)) {
        if (!gnrc_rpl_validation_DIS(dis, len)) {
            return;
        }
    }

    if (ipv6_addr_is_multicast(dst)) {
        for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
            if ((gnrc_rpl_instances[i].state != 0)
                /* a leaf node should only react to unicast DIS */
                && (gnrc_rpl_instances[i].dodag.node_status != GNRC_RPL_LEAF_NODE)) {
#ifdef MODULE_GNRC_RPL_P2P
                if (gnrc_rpl_instances[i].mop == GNRC_RPL_P2P_MOP) {
                    DEBUG("RPL: Not responding to DIS for P2P-RPL DODAG\n");
                    continue;
                }
#endif
                trickle_reset_timer(&(gnrc_rpl_instances[i].dodag.trickle));
            }
        }
    }
    else {
        for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
            if (gnrc_rpl_instances[i].state != 0) {

                uint32_t included_opts = 0;
                size_t opt_len = len - sizeof(gnrc_rpl_dis_t) - sizeof(icmpv6_hdr_t);
                if (!_parse_options(GNRC_RPL_ICMPV6_CODE_DIS, &gnrc_rpl_instances[i],
                                    (gnrc_rpl_opt_t *)(dis + 1), opt_len, src, &included_opts)) {
                    DEBUG("RPL: DIS option parsing error - skip processing the DIS\n");
                    continue;
                }
                gnrc_rpl_instances[i].dodag.dio_opts |= GNRC_RPL_REQ_DIO_OPT_DODAG_CONF;
                gnrc_rpl_send_DIO(&gnrc_rpl_instances[i], src);
            }
        }
    }
}

/**
 * @brief   Handles a received DIO message for a new DODAG.
 *
 * @param[in] inst      The @p RPL instance of the DODAG.
 * @param[in] dio       The received @p DIO packet.
 * @param[in] iface     The interface that the DIO was received on.
 * @param[in] src       The address of the sender.
 * @param[in] len       The length of the whole DIO packet.
 */
void _recv_DIO_for_new_dodag(gnrc_rpl_instance_t *inst, gnrc_rpl_dio_t *dio, kernel_pid_t iface,
                             ipv6_addr_t *src, uint16_t len)
{
    gnrc_netif_t *netif;

    if (byteorder_ntohs(dio->rank) == GNRC_RPL_INFINITE_RANK) {
        DEBUG("RPL: ignore INFINITE_RANK DIO when we are not yet part of this DODAG\n");
        gnrc_rpl_instance_remove(inst);
        return;
    }

    inst->mop = (dio->g_mop_prf >> GNRC_RPL_MOP_SHIFT) & GNRC_RPL_SHIFTED_MOP_MASK;
    inst->of = gnrc_rpl_get_of_for_ocp(GNRC_RPL_DEFAULT_OCP);

    if (iface == KERNEL_PID_UNDEF) {
        netif = _find_interface_with_rpl_mcast();
    }
    else {
        netif = gnrc_netif_get_by_pid(iface);
    }
    assert(netif != NULL);

    gnrc_rpl_dodag_init(inst, &dio->dodag_id, netif->pid);

    gnrc_rpl_dodag_t *dodag = &inst->dodag;

    DEBUG("RPL: Joined DODAG (%s).\n",
          ipv6_addr_to_str(addr_str, &dio->dodag_id, sizeof(addr_str)));

    gnrc_rpl_parent_t *parent = NULL;

    if (!gnrc_rpl_parent_add_by_addr(dodag, src, &parent) && (parent == NULL)) {
        DEBUG("RPL: Could not allocate new parent.\n");
        gnrc_rpl_instance_remove(inst);
        return;
    }

    dodag->version = dio->version_number;
    dodag->grounded = dio->g_mop_prf >> GNRC_RPL_GROUNDED_SHIFT;
    dodag->prf = dio->g_mop_prf & GNRC_RPL_PRF_MASK;

    parent->rank = byteorder_ntohs(dio->rank);

    uint32_t included_opts = 0;
    if (!_parse_options(GNRC_RPL_ICMPV6_CODE_DIO, inst, (gnrc_rpl_opt_t *)(dio + 1), len,
                        src, &included_opts)) {
        DEBUG("RPL: Error encountered during DIO option parsing - remove DODAG\n");
        gnrc_rpl_instance_remove(inst);
        return;
    }

    if (!(included_opts & (((uint32_t)1) << GNRC_RPL_OPT_DODAG_CONF))) {
        if (!IS_ACTIVE(CONFIG_GNRC_RPL_DODAG_CONF_OPTIONAL_ON_JOIN)) {
            DEBUG("RPL: DIO without DODAG_CONF option - remove DODAG and request new DIO\n");
            gnrc_rpl_instance_remove(inst);
            gnrc_rpl_send_DIS(NULL, src, NULL, 0);
            return;
        }
        else {
            DEBUG("RPL: DIO without DODAG_CONF option - use default trickle parameters\n");
            gnrc_rpl_send_DIS(NULL, src, NULL, 0);
        }
    }

    /* if there was no address created manually or by a PIO on the interface,
     * leave this DODAG */
    if (gnrc_netif_ipv6_addr_match(netif, &dodag->dodag_id) < 0) {
        DEBUG("RPL: no IPv6 address configured on interface %i to match the "
              "given dodag id: %s\n", netif->pid,
              ipv6_addr_to_str(addr_str, &(dodag->dodag_id), sizeof(addr_str)));
        gnrc_rpl_instance_remove(inst);
        return;
    }

    gnrc_rpl_delay_dao(dodag);
    trickle_start(gnrc_rpl_pid, &dodag->trickle, GNRC_RPL_MSG_TYPE_TRICKLE_MSG,
                  (1 << dodag->dio_min), dodag->dio_interval_doubl,
                  dodag->dio_redun);

    gnrc_rpl_parent_update(dodag, parent);
}

/**
 * @brief   Handles a received DIO message for an existing DODAG.
 *
 * @param[in] inst      The @p RPL instance of the DODAG.
 * @param[in] dio       The received @p DIO packet.
 * @param[in] src       The address of the sender.
 * @param[in] len       The length of the DIO packet.
 */
static void _recv_DIO_for_existing_dodag(gnrc_rpl_instance_t *inst, gnrc_rpl_dio_t *dio,
                                         ipv6_addr_t *src, uint16_t len)
{
    gnrc_rpl_dodag_t *dodag = &inst->dodag;

    /* ignore dodags with other dodag_id's for now */
    /* TODO: choose DODAG with better rank */
    if (memcmp(&dodag->dodag_id, &dio->dodag_id, sizeof(ipv6_addr_t)) != 0) {
        DEBUG("RPL: DIO received from another DODAG, but same instance - ignore\n");
        return;
    }

    if (inst->mop != ((dio->g_mop_prf >> GNRC_RPL_MOP_SHIFT) & GNRC_RPL_SHIFTED_MOP_MASK)) {
        DEBUG("RPL: invalid MOP for this instance.\n");
        return;
    }

#ifdef MODULE_GNRC_RPL_P2P
    gnrc_rpl_p2p_ext_t *p2p_ext = gnrc_rpl_p2p_ext_get(dodag);
    if ((dodag->instance->mop == GNRC_RPL_P2P_MOP) && (p2p_ext->lifetime_sec <= 0)) {
        return;
    }
#endif

    if (GNRC_RPL_COUNTER_GREATER_THAN(dio->version_number, dodag->version)) {
        if (dodag->node_status == GNRC_RPL_ROOT_NODE) {
            dodag->version = GNRC_RPL_COUNTER_INCREMENT(dio->version_number);
            trickle_reset_timer(&dodag->trickle);
        }
        else {
            dodag->version = dio->version_number;
            gnrc_rpl_local_repair(dodag);
        }
    }
    else if (GNRC_RPL_COUNTER_GREATER_THAN(dodag->version, dio->version_number)) {
        trickle_reset_timer(&dodag->trickle);
        return;
    }

    if (dodag->node_status == GNRC_RPL_ROOT_NODE) {
        if (byteorder_ntohs(dio->rank) != GNRC_RPL_INFINITE_RANK) {
            trickle_increment_counter(&dodag->trickle);
        }
        else {
            trickle_reset_timer(&dodag->trickle);
        }
        return;
    }

    gnrc_rpl_parent_t *parent = NULL;

    if (!gnrc_rpl_parent_add_by_addr(dodag, src, &parent) && (parent == NULL)) {
        DEBUG("RPL: Could not allocate new parent.\n");
        return;
    }
    else if (parent != NULL) {
        trickle_increment_counter(&dodag->trickle);
    }

    /* gnrc_rpl_parent_add_by_addr should have set this already */
    assert(parent != NULL);

    parent->rank = byteorder_ntohs(dio->rank);

    gnrc_rpl_parent_update(dodag, parent);

    /* sender of incoming DIO is not a parent of mine (anymore) and has an INFINITE rank
       and I have a rank != INFINITE_RANK */
    if (parent->state == GNRC_RPL_PARENT_UNUSED) {
        if ((byteorder_ntohs(dio->rank) == GNRC_RPL_INFINITE_RANK)
            && (dodag->my_rank != GNRC_RPL_INFINITE_RANK)) {
            trickle_reset_timer(&dodag->trickle);
            return;
        }
    }
    /* incoming DIO is from pref. parent */
    else if (parent == dodag->parents) {
        if (parent->dtsn != dio->dtsn) {
            gnrc_rpl_delay_dao(dodag);
        }
        parent->dtsn = dio->dtsn;
        dodag->grounded = dio->g_mop_prf >> GNRC_RPL_GROUNDED_SHIFT;
        dodag->prf = dio->g_mop_prf & GNRC_RPL_PRF_MASK;
        uint32_t included_opts = 0;
        if (!_parse_options(GNRC_RPL_ICMPV6_CODE_DIO, inst, (gnrc_rpl_opt_t *)(dio + 1), len,
                            src, &included_opts)) {
            DEBUG("RPL: Error encountered during DIO option parsing - remove DODAG\n");
            gnrc_rpl_instance_remove(inst);
            return;
        }
    }
}

void gnrc_rpl_recv_DIO(gnrc_rpl_dio_t *dio, kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *dst,
                       uint16_t len)
{
    (void)dst;
    gnrc_rpl_instance_t *inst = NULL;

#ifdef MODULE_NETSTATS_RPL
    gnrc_rpl_netstats_rx_DIO(&gnrc_rpl_netstats, len, (dst && !ipv6_addr_is_multicast(dst)));
#endif

    if (!IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_VALIDATION)) {
        if (!gnrc_rpl_validation_DIO(dio, len)) {
            return;
        }
    }

    len -= (sizeof(gnrc_rpl_dio_t) + sizeof(icmpv6_hdr_t));

    if (gnrc_rpl_instance_add(dio->instance_id, &inst)) {
        _recv_DIO_for_new_dodag(inst, dio, iface, src, len);
    }
    else if (inst == NULL) {
        DEBUG("RPL: Could not allocate a new instance.\n");
    }
    else {
        _recv_DIO_for_existing_dodag(inst, dio, src, len);
    }
}

static gnrc_pktsnip_t *_dao_target_build(gnrc_pktsnip_t *pkt, ipv6_addr_t *addr,
                                         uint8_t prefix_length)
{
    gnrc_rpl_opt_target_t *target;
    gnrc_pktsnip_t *opt_snip;

    if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_opt_target_t),
                                    GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: Send DAO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    target = opt_snip->data;
    target->type = GNRC_RPL_OPT_TARGET;
    target->length = sizeof(target->flags) + sizeof(target->prefix_length) + sizeof(target->target);
    target->flags = 0;
    target->prefix_length = prefix_length;
    target->target = *addr;
    return opt_snip;
}

static gnrc_pktsnip_t *_dao_transit_build(gnrc_pktsnip_t *pkt, uint8_t lifetime, bool external)
{
    gnrc_rpl_opt_transit_t *transit;
    gnrc_pktsnip_t *opt_snip;

    if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_opt_transit_t),
                                    GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: Send DAO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    transit = opt_snip->data;
    transit->type = GNRC_RPL_OPT_TRANSIT;
    transit->length = sizeof(transit->e_flags) + sizeof(transit->path_control) +
                      sizeof(transit->path_sequence) + sizeof(transit->path_lifetime);
    transit->e_flags = (external) << GNRC_RPL_OPT_TRANSIT_E_FLAG_SHIFT;
    transit->path_control = 0;
    transit->path_sequence = 0;
    transit->path_lifetime = lifetime;
    return opt_snip;
}

void gnrc_rpl_send_DAO(gnrc_rpl_instance_t *inst, ipv6_addr_t *destination, uint8_t lifetime)
{
    gnrc_rpl_dodag_t *dodag;

    if (inst == NULL) {
        DEBUG("RPL: Error - trying to send DAO without being part of a dodag.\n");
        return;
    }

    dodag = &inst->dodag;

    if (dodag->node_status == GNRC_RPL_ROOT_NODE) {
        return;
    }

#ifdef MODULE_GNRC_RPL_P2P
    if (dodag->instance->mop == GNRC_RPL_P2P_MOP) {
        return;
    }
#endif

    if (destination == NULL) {
        if (dodag->parents == NULL) {
            DEBUG("RPL: dodag has no preferred parent\n");
            return;
        }

        destination = &(dodag->parents->addr);
    }

    gnrc_pktsnip_t *pkt = NULL, *tmp = NULL;
    gnrc_rpl_dao_t *dao;

    /* find my address */
    ipv6_addr_t *me = NULL;
    gnrc_netif_t *netif = gnrc_netif_get_by_prefix(&dodag->dodag_id);
    int idx;

    if (netif == NULL) {
        DEBUG("RPL: no address configured\n");
        return;
    }
    idx = gnrc_netif_ipv6_addr_match(netif, &dodag->dodag_id);
    if (idx < 0) {
        DEBUG("RPL: no address matching DODAG ID found\n");
        return;
    }
    me = &netif->ipv6.addrs[idx];

    /* add external and RPL FT entries */
    /* TODO: nib: dropped support for external transit options for now */
    void *ft_state = NULL;
    gnrc_ipv6_nib_ft_t fte;
    while (gnrc_ipv6_nib_ft_iter(NULL, dodag->iface, &ft_state, &fte)) {
        DEBUG("RPL: Send DAO - building transit option\n");

        if ((pkt = _dao_transit_build(pkt, lifetime, false)) == NULL) {
            DEBUG("RPL: Send DAO - no space left in packet buffer\n");
            return;
        }
        if (ipv6_addr_is_global(&fte.dst) &&
            !ipv6_addr_is_unspecified(&fte.next_hop)) {
            DEBUG("RPL: Send DAO - building target %s/%d\n",
                  ipv6_addr_to_str(addr_str, &fte.dst, sizeof(addr_str)), fte.dst_len);

            if ((pkt = _dao_target_build(pkt, &fte.dst, fte.dst_len)) == NULL) {
                DEBUG("RPL: Send DAO - no space left in packet buffer\n");
                return;
            }
        }
    }

    /* add own address */
    DEBUG("RPL: Send DAO - building target %s/128\n",
          ipv6_addr_to_str(addr_str, me, sizeof(addr_str)));
    if ((pkt = _dao_target_build(pkt, me, IPV6_ADDR_BIT_LEN)) == NULL) {
        DEBUG("RPL: Send DAO - no space left in packet buffer\n");
        return;
    }

    bool local_instance = (inst->id & GNRC_RPL_INSTANCE_ID_MSB) ? true : false;

    if (local_instance) {
        if ((tmp = gnrc_pktbuf_add(pkt, &dodag->dodag_id, sizeof(ipv6_addr_t),
                                   GNRC_NETTYPE_UNDEF)) == NULL) {
            DEBUG("RPL: Send DAO - no space left in packet buffer\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = tmp;
    }

    if ((tmp = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_dao_t), GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: Send DAO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp;
    dao = pkt->data;
    dao->instance_id = inst->id;
    if (local_instance) {
        /* set the D flag to indicate that a DODAG id is present */
        dao->k_d_flags = GNRC_RPL_DAO_D_BIT;
    }
    else {
        dao->k_d_flags = 0;
    }

    /* set the K flag to indicate that ACKs are required */
    dao->k_d_flags |= GNRC_RPL_DAO_K_BIT;
    dao->dao_sequence = dodag->dao_seq;
    dao->reserved = 0;

    if ((tmp = gnrc_icmpv6_build(pkt, ICMPV6_RPL_CTRL, GNRC_RPL_ICMPV6_CODE_DAO,
                                 sizeof(icmpv6_hdr_t))) == NULL) {
        DEBUG("RPL: Send DAO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp;

#ifdef MODULE_NETSTATS_RPL
    gnrc_rpl_netstats_tx_DAO(&gnrc_rpl_netstats, gnrc_pkt_len(pkt),
                             (destination && !ipv6_addr_is_multicast(destination)));
#endif

    gnrc_rpl_send(pkt, dodag->iface, NULL, destination, &dodag->dodag_id);

    dodag->dao_seq = GNRC_RPL_COUNTER_INCREMENT(dodag->dao_seq);
}

void gnrc_rpl_send_DAO_ACK(gnrc_rpl_instance_t *inst, ipv6_addr_t *destination, uint8_t seq)
{
    gnrc_rpl_dodag_t *dodag = NULL;

    if (inst == NULL) {
        DEBUG("RPL: Error - trying to send DAO-ACK without being part of a dodag.\n");
        return;
    }

    dodag = &inst->dodag;

    gnrc_pktsnip_t *pkt;
    icmpv6_hdr_t *icmp;
    gnrc_rpl_dao_ack_t *dao_ack;
    int size = sizeof(icmpv6_hdr_t) + sizeof(gnrc_rpl_dao_ack_t);
    bool local_instance = (inst->id & GNRC_RPL_INSTANCE_ID_MSB) ? true : false;

    if (local_instance) {
        size += sizeof(ipv6_addr_t);
    }

    if (!(pkt = gnrc_icmpv6_build(NULL, ICMPV6_RPL_CTRL, GNRC_RPL_ICMPV6_CODE_DAO_ACK, size))) {
        DEBUG("RPL: Send DAOACK - no space left in packet buffer\n");
        return;
    }

    icmp = (icmpv6_hdr_t *)pkt->data;
    dao_ack = (gnrc_rpl_dao_ack_t *)(icmp + 1);

    dao_ack->instance_id = inst->id;
    if (local_instance) {
        /* set the D flag to indicate that a DODAG id is present */
        dao_ack->d_reserved = GNRC_RPL_DAO_ACK_D_BIT;
        memcpy((dao_ack + 1), &dodag->dodag_id, sizeof(ipv6_addr_t));
    }
    else {
        dao_ack->d_reserved = 0;
    }

    dao_ack->dao_sequence = seq;
    dao_ack->status = 0;

#ifdef MODULE_NETSTATS_RPL
    gnrc_rpl_netstats_tx_DAO_ACK(&gnrc_rpl_netstats, gnrc_pkt_len(pkt),
                                 (destination && !ipv6_addr_is_multicast(destination)));
#endif

    gnrc_rpl_send(pkt, dodag->iface, NULL, destination, &dodag->dodag_id);
}

void gnrc_rpl_recv_DAO(gnrc_rpl_dao_t *dao, kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *dst,
                       uint16_t len)
{
    (void)iface;
    (void)dst;

#ifdef MODULE_NETSTATS_RPL
    gnrc_rpl_netstats_rx_DAO(&gnrc_rpl_netstats, len, (dst && !ipv6_addr_is_multicast(dst)));
#endif

    gnrc_rpl_instance_t *inst = NULL;
    gnrc_rpl_dodag_t *dodag = NULL;

    if (!IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_VALIDATION)) {
        if (!gnrc_rpl_validation_DAO(dao, len)) {
            return;
        }
    }

    gnrc_rpl_opt_t *opts = (gnrc_rpl_opt_t *)(dao + 1);

    if ((inst = gnrc_rpl_instance_get(dao->instance_id)) == NULL) {
        DEBUG("RPL: DAO with unknown instance id (%d) received\n", dao->instance_id);
        return;
    }

    dodag = &inst->dodag;

    len -= (sizeof(gnrc_rpl_dao_t) + sizeof(icmpv6_hdr_t));

    /* check if the D flag is set before accessing the DODAG id */
    if ((dao->k_d_flags & GNRC_RPL_DAO_D_BIT)) {
        if (memcmp(&dodag->dodag_id, (ipv6_addr_t *)(dao + 1), sizeof(ipv6_addr_t)) != 0) {
            DEBUG("RPL: DAO with unknown DODAG id (%s)\n", ipv6_addr_to_str(addr_str,
                                                                            (ipv6_addr_t *)(dao +
                                                                                            1),
                                                                            sizeof(addr_str)));
            return;
        }
        opts = (gnrc_rpl_opt_t *)(((uint8_t *)opts) + sizeof(ipv6_addr_t));
        len -= sizeof(ipv6_addr_t);
    }

    /* a leaf node should not parse DAOs */
    if (dodag->node_status == GNRC_RPL_LEAF_NODE) {
        return;
    }

#ifdef MODULE_GNRC_RPL_P2P
    if (dodag->instance->mop == GNRC_RPL_P2P_MOP) {
        return;
    }
#endif

    uint32_t included_opts = 0;
    if (!_parse_options(GNRC_RPL_ICMPV6_CODE_DAO, inst, opts, len, src, &included_opts)) {
        DEBUG("RPL: Error encountered during DAO option parsing - ignore DAO\n");
        return;
    }

    /* send a DAO-ACK if K flag is set */
    if (dao->k_d_flags & GNRC_RPL_DAO_K_BIT) {
        gnrc_rpl_send_DAO_ACK(inst, src, dao->dao_sequence);
    }

    gnrc_rpl_delay_dao(dodag);
}

void gnrc_rpl_recv_DAO_ACK(gnrc_rpl_dao_ack_t *dao_ack, kernel_pid_t iface, ipv6_addr_t *src,
                           ipv6_addr_t *dst, uint16_t len)
{
    (void)iface;
    (void)src;
    (void)dst;
    (void)len;

    gnrc_rpl_instance_t *inst = NULL;
    gnrc_rpl_dodag_t *dodag = NULL;

#ifdef MODULE_NETSTATS_RPL
    gnrc_rpl_netstats_rx_DAO_ACK(&gnrc_rpl_netstats, len, (dst && !ipv6_addr_is_multicast(dst)));
#endif

    if (!IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_VALIDATION)) {
        if (!dst || !gnrc_rpl_validation_DAO_ACK(dao_ack, len, dst)) {
            return;
        }
    }

    if ((inst = gnrc_rpl_instance_get(dao_ack->instance_id)) == NULL) {
        DEBUG("RPL: DAO-ACK with unknown instance id (%d) received\n", dao_ack->instance_id);
        return;
    }

    dodag = &inst->dodag;

    /* check if the D flag is set before accessing the DODAG id */
    if ((dao_ack->d_reserved & GNRC_RPL_DAO_ACK_D_BIT)) {
        if (memcmp(&dodag->dodag_id, (ipv6_addr_t *)(dao_ack + 1), sizeof(ipv6_addr_t)) != 0) {
            DEBUG("RPL: DAO-ACK with unknown DODAG id (%s)\n", ipv6_addr_to_str(addr_str,
                                                                                (ipv6_addr_t *)(
                                                                                    dao_ack + 1),
                                                                                sizeof(addr_str)));
            return;
        }
    }

    if ((dao_ack->status != 0) && (dao_ack->dao_sequence != dodag->dao_seq)) {
        DEBUG("RPL: DAO-ACK sequence (%d) does not match expected sequence (%d)\n",
              dao_ack->dao_sequence, dodag->dao_seq);
        return;
    }

    dodag->dao_ack_received = true;
    gnrc_rpl_long_delay_dao(dodag);
}

/**
 * @}
 */
