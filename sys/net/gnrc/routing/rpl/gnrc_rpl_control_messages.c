/*
 * Copyright (C) 2013 - 2014  INRIA.
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
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
 * @author  Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include "net/af.h"
#include "net/icmpv6.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc.h"
#include "net/eui64.h"

#include "net/gnrc/rpl.h"

#ifdef MODULE_GNRC_RPL_P2P
#include "net/gnrc/rpl/p2p_structs.h"
#include "net/gnrc/rpl/p2p_dodag.h"
#include "net/gnrc/rpl/p2p.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

#define GNRC_RPL_GROUNDED_SHIFT             (7)
#define GNRC_RPL_MOP_SHIFT                  (3)
#define GNRC_RPL_OPT_DODAG_CONF_LEN         (14)
#define GNRC_RPL_OPT_PREFIX_INFO_LEN        (30)
#define GNRC_RPL_OPT_TARGET_LEN             (18)
#define GNRC_RPL_OPT_TRANSIT_E_FLAG_SHIFT   (7)
#define GNRC_RPL_OPT_TRANSIT_E_FLAG         (1 << GNRC_RPL_OPT_TRANSIT_E_FLAG_SHIFT)
#define GNRC_RPL_OPT_TRANSIT_INFO_LEN       (4)
#define GNRC_RPL_SHIFTED_MOP_MASK           (0x7)
#define GNRC_RPL_PRF_MASK                   (0x7)
#define GNRC_RPL_PREFIX_AUTO_ADDRESS_BIT    (1 << 6)
#define GNRC_RPL_DAO_D_BIT                  (1 << 6)
#define GNRC_RPL_DAO_K_BIT                  (1 << 7)
#define GNRC_RPL_DAO_ACK_D_BIT              (1 << 7)

void gnrc_rpl_send(gnrc_pktsnip_t *pkt, kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *dst,
                   ipv6_addr_t *dodag_id)
{
    (void)dodag_id;
    gnrc_pktsnip_t *hdr;
    if (iface == KERNEL_PID_UNDEF) {
        if ((iface = gnrc_ipv6_netif_find_by_addr(NULL, &ipv6_addr_all_rpl_nodes))
            == KERNEL_PID_UNDEF) {
            DEBUG("RPL: no suitable interface found for this destination address\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }

    if (src == NULL) {
        src = gnrc_ipv6_netif_match_prefix(iface, &ipv6_addr_link_local_prefix);

        if (src == NULL) {
            DEBUG("RPL: no suitable src address found\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }

    if (dst == NULL) {
        dst = (ipv6_addr_t *) &ipv6_addr_all_rpl_nodes;
    }

    hdr = gnrc_ipv6_hdr_build(pkt, src, dst);

    if (hdr == NULL) {
        DEBUG("RPL: Send - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;

    hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    ((gnrc_netif_hdr_t *)hdr->data)->if_pid = iface;
    LL_PREPEND(pkt, hdr);

    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("RPL: cannot send packet: no subscribers found.\n");
        gnrc_pktbuf_release(pkt);
    }
}

gnrc_pktsnip_t *_dio_dodag_conf_build(gnrc_pktsnip_t *pkt, gnrc_rpl_dodag_t *dodag)
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

#ifndef GNRC_RPL_WITHOUT_PIO
gnrc_pktsnip_t *_dio_prefix_info_build(gnrc_pktsnip_t *pkt, gnrc_rpl_dodag_t *dodag)
{
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
    prefix_info->valid_lifetime = dodag->netif_addr->valid;
    prefix_info->pref_lifetime = dodag->netif_addr->preferred;
    prefix_info->prefix_len = dodag->netif_addr->prefix_len;
    prefix_info->reserved = 0;

    memset(&prefix_info->prefix, 0, sizeof(prefix_info->prefix));
    ipv6_addr_init_prefix(&prefix_info->prefix, &dodag->dodag_id, dodag->netif_addr->prefix_len);
    return opt_snip;
}
#endif

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

#ifndef GNRC_RPL_WITHOUT_PIO
    if (dodag->dio_opts & GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO) {
        if ((pkt = _dio_prefix_info_build(pkt, dodag)) == NULL) {
            return;
        }
    }
#endif

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

    gnrc_rpl_send(pkt, dodag->iface, NULL, destination, &dodag->dodag_id);
}

void gnrc_rpl_send_DIS(gnrc_rpl_instance_t *inst, ipv6_addr_t *destination)
{
    gnrc_pktsnip_t *pkt;
    icmpv6_hdr_t *icmp;
    gnrc_rpl_dis_t *dis;

    /* TODO: Currently the DIS is too small so that wireshark complains about an incorrect
     * ethernet frame check sequence. In order to prevent this, 4 PAD1 options are added.
     * This will be addressed in follow-up PRs */
    uint8_t padding[] = {
            0x01, 0x02, 0x00, 0x00
    };

    int size = sizeof(icmpv6_hdr_t) + sizeof(gnrc_rpl_dis_t) + sizeof(padding);

    if ((pkt = gnrc_icmpv6_build(NULL, ICMPV6_RPL_CTRL, GNRC_RPL_ICMPV6_CODE_DIS, size)) == NULL) {
        DEBUG("RPL: Send DIS - no space left in packet buffer\n");
        return;
    }

    icmp = (icmpv6_hdr_t *)pkt->data;
    dis = (gnrc_rpl_dis_t *)(icmp + 1);
    dis->flags = 0;
    dis->reserved = 0;

    /* TODO add padding may be removed if packet size grows */
    memcpy((dis + 1), padding, sizeof(padding));

    gnrc_rpl_send(pkt, KERNEL_PID_UNDEF, NULL, destination, (inst? &(inst->dodag.dodag_id) : NULL));
}

static bool _gnrc_rpl_check_DIS_validity(gnrc_rpl_dis_t *dis, uint16_t len)
{
    uint16_t expected_len = sizeof(*dis) + sizeof(icmpv6_hdr_t);

    if (expected_len <= len) {
        return true;
    }

    DEBUG("RPL: wrong DIS len: %d, expected: %d\n", len, expected_len);

    return false;
}

void gnrc_rpl_recv_DIS(gnrc_rpl_dis_t *dis, kernel_pid_t iface, ipv6_addr_t *src,
                       ipv6_addr_t *dst, uint16_t len)
{
    /* TODO handle Solicited Information Option */
    (void)iface;

    if (!_gnrc_rpl_check_DIS_validity(dis, len)) {
        return;
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
                gnrc_rpl_instances[i].dodag.dio_opts |= GNRC_RPL_REQ_DIO_OPT_DODAG_CONF;
                gnrc_rpl_send_DIO(&gnrc_rpl_instances[i], src);
            }
        }
    }
}

static bool _gnrc_rpl_check_options_validity(int msg_type, gnrc_rpl_instance_t *inst,
                                                gnrc_rpl_opt_t *opt, uint16_t len)
{
    uint16_t expected_len = 0;

    while(expected_len < len) {
        switch(opt->type) {
            case (GNRC_RPL_OPT_PAD1):
                expected_len += 1;
                opt = (gnrc_rpl_opt_t *) (((uint8_t *) opt) + 1);
                continue;

            case (GNRC_RPL_OPT_DODAG_CONF):
                if (msg_type != GNRC_RPL_ICMPV6_CODE_DIO) {
                    DEBUG("RPL: DODAG CONF DIO option not expected\n");
                    return false;
                }

                if (opt->length != GNRC_RPL_OPT_DODAG_CONF_LEN) {
                    DEBUG("RPL: wrong DIO option (DODAG CONF) len: %d, expected: %d\n",
                            opt->length, GNRC_RPL_OPT_DODAG_CONF_LEN);
                    return false;
                }
                break;

            case (GNRC_RPL_OPT_PREFIX_INFO):
                if (msg_type != GNRC_RPL_ICMPV6_CODE_DIO) {
                    DEBUG("RPL: PREFIX INFO DIO option not expected\n");
                    return false;
                }

                if (opt->length != GNRC_RPL_OPT_PREFIX_INFO_LEN) {
                    DEBUG("RPL: wrong DIO option (PREFIX INFO) len: %d, expected: %d\n",
                            opt->length, GNRC_RPL_OPT_PREFIX_INFO_LEN);
                    return false;
                }
                break;

            case (GNRC_RPL_OPT_TARGET):
                if (msg_type != GNRC_RPL_ICMPV6_CODE_DAO) {
                    DEBUG("RPL: RPL TARGET DAO option not expected\n");
                    return false;
                }

                if (opt->length > GNRC_RPL_OPT_TARGET_LEN) {
                    DEBUG("RPL: wrong DAO option (RPL TARGET) len: %d, expected (max): %d\n",
                            opt->length, GNRC_RPL_OPT_TARGET_LEN);
                    return false;
                }
                break;

            case (GNRC_RPL_OPT_TRANSIT):
                if (msg_type != GNRC_RPL_ICMPV6_CODE_DAO) {
                    DEBUG("RPL: RPL TRANSIT INFO DAO option not expected\n");
                    return false;
                }

                uint8_t parent_addr = 0;
                if (inst->mop == GNRC_RPL_MOP_NON_STORING_MODE) {
                    parent_addr = sizeof(ipv6_addr_t);
                }

                if (opt->length != (GNRC_RPL_OPT_TRANSIT_INFO_LEN + parent_addr)) {
                    DEBUG("RPL: wrong DAO option (TRANSIT INFO) len: %d, expected: %d\n",
                            opt->length, (GNRC_RPL_OPT_TRANSIT_INFO_LEN + parent_addr));
                    return false;
                }
                break;

            default:
                break;

        }
        expected_len += opt->length + sizeof(gnrc_rpl_opt_t);
        opt = (gnrc_rpl_opt_t *) (((uint8_t *) (opt + 1)) + opt->length);
    }

    if (expected_len == len) {
        return true;
    }

    DEBUG("RPL: wrong options len: %d, expected: %d\n", len, expected_len);

    return false;
}

/** @todo allow target prefixes in target options to be of variable length */
bool _parse_options(int msg_type, gnrc_rpl_instance_t *inst, gnrc_rpl_opt_t *opt, uint16_t len,
                    ipv6_addr_t *src, uint32_t *included_opts)
{
    uint16_t l = 0;
    gnrc_rpl_opt_target_t *first_target = NULL;
    gnrc_rpl_dodag_t *dodag = &inst->dodag;
    eui64_t iid;
    *included_opts = 0;
    ipv6_addr_t *me;

    if (!_gnrc_rpl_check_options_validity(msg_type, inst, opt, len)) {
        return false;
    }

    while(l < len) {
        switch(opt->type) {
            case (GNRC_RPL_OPT_PAD1):
                DEBUG("RPL: PAD1 option parsed\n");
                *included_opts |= ((uint32_t) 1) << GNRC_RPL_OPT_PAD1;
                l += 1;
                opt = (gnrc_rpl_opt_t *) (((uint8_t *) opt) + 1);
                continue;

            case (GNRC_RPL_OPT_PADN):
                DEBUG("RPL: PADN option parsed\n");
                *included_opts |= ((uint32_t) 1) << GNRC_RPL_OPT_PADN;
                break;

            case (GNRC_RPL_OPT_DODAG_CONF):
                DEBUG("RPL: DODAG CONF DIO option parsed\n");
                *included_opts |= ((uint32_t) 1) << GNRC_RPL_OPT_DODAG_CONF;
                dodag->dio_opts |= GNRC_RPL_REQ_DIO_OPT_DODAG_CONF;
                gnrc_rpl_opt_dodag_conf_t *dc = (gnrc_rpl_opt_dodag_conf_t *) opt;
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
                *included_opts |= ((uint32_t) 1) << GNRC_RPL_OPT_PREFIX_INFO;
#ifndef GNRC_RPL_WITHOUT_PIO
                dodag->dio_opts |= GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO;
#endif
                gnrc_rpl_opt_prefix_info_t *pi = (gnrc_rpl_opt_prefix_info_t *) opt;
                /* check for the auto address-configuration flag */
                if ((gnrc_netapi_get(dodag->iface, NETOPT_IPV6_IID, 0, &iid, sizeof(eui64_t)) < 0)
                     && !(pi->LAR_flags & GNRC_RPL_PREFIX_AUTO_ADDRESS_BIT)) {
                    break;
                }
                ipv6_addr_set_aiid(&pi->prefix, iid.uint8);
                me = gnrc_ipv6_netif_add_addr(dodag->iface, &pi->prefix, pi->prefix_len, 0);
                if (me) {
                    dodag->netif_addr = gnrc_ipv6_netif_addr_get(me);
                }

                break;

            case (GNRC_RPL_OPT_TARGET):
                DEBUG("RPL: RPL TARGET DAO option parsed\n");
                *included_opts |= ((uint32_t) 1) << GNRC_RPL_OPT_TARGET;

                gnrc_rpl_opt_target_t *target = (gnrc_rpl_opt_target_t *) opt;
                if (first_target == NULL) {
                    first_target = target;
                }

                uint32_t fib_dst_flags = 0;

                if (target->prefix_length < IPV6_ADDR_BIT_LEN) {
                    fib_dst_flags = ((uint32_t)(target->prefix_length) << FIB_FLAG_NET_PREFIX_SHIFT);
                }

                DEBUG("RPL: adding fib entry %s/%d 0x%" PRIx32 "\n",
                      ipv6_addr_to_str(addr_str, &(target->target), sizeof(addr_str)),
                      target->prefix_length,
                      fib_dst_flags);

                fib_add_entry(&gnrc_ipv6_fib_table, dodag->iface, target->target.u8,
                              sizeof(ipv6_addr_t), fib_dst_flags, src->u8,
                              sizeof(ipv6_addr_t), FIB_FLAG_RPL_ROUTE,
                              (dodag->default_lifetime * dodag->lifetime_unit) *
                              SEC_IN_MS);
                break;

            case (GNRC_RPL_OPT_TRANSIT):
                DEBUG("RPL: RPL TRANSIT INFO DAO option parsed\n");
                *included_opts |= ((uint32_t) 1) << GNRC_RPL_OPT_TRANSIT;
                gnrc_rpl_opt_transit_t *transit = (gnrc_rpl_opt_transit_t *) opt;
                if (first_target == NULL) {
                    DEBUG("RPL: Encountered a RPL TRANSIT DAO option without "
                          "a preceding RPL TARGET DAO option\n");
                    break;
                }

                do {
                    DEBUG("RPL: updating fib entry %s/%d\n",
                          ipv6_addr_to_str(addr_str, &(first_target->target), sizeof(addr_str)),
                          first_target->prefix_length);

                    fib_update_entry(&gnrc_ipv6_fib_table,
                                     first_target->target.u8,
                                     sizeof(ipv6_addr_t), src->u8,
                                     sizeof(ipv6_addr_t),
                                     ((transit->e_flags & GNRC_RPL_OPT_TRANSIT_E_FLAG) ?
                                      0x0 : FIB_FLAG_RPL_ROUTE),
                                     (transit->path_lifetime *
                                      dodag->lifetime_unit * SEC_IN_MS));
                    first_target = (gnrc_rpl_opt_target_t *) (((uint8_t *) (first_target)) +
                                   sizeof(gnrc_rpl_opt_t) + first_target->length);
                }
                while (first_target->type == GNRC_RPL_OPT_TARGET);

                first_target = NULL;
                break;

#ifdef MODULE_GNRC_RPL_P2P
            case (GNRC_RPL_P2P_OPT_RDO):
                gnrc_rpl_p2p_rdo_parse((gnrc_rpl_p2p_opt_rdo_t *) opt, gnrc_rpl_p2p_ext_get(dodag));
                break;
#endif
        }
        l += opt->length + sizeof(gnrc_rpl_opt_t);
        opt = (gnrc_rpl_opt_t *) (((uint8_t *) (opt + 1)) + opt->length);
    }
    return true;
}

static bool _gnrc_rpl_check_DIO_validity(gnrc_rpl_dio_t *dio, uint16_t len)
{
    uint16_t expected_len = sizeof(*dio) + sizeof(icmpv6_hdr_t);

    if (expected_len <= len) {
        return true;
    }

    DEBUG("RPL: wrong DIO len: %d, expected: %d\n", len, expected_len);

    return false;
}

void gnrc_rpl_recv_DIO(gnrc_rpl_dio_t *dio, kernel_pid_t iface, ipv6_addr_t *src, uint16_t len)
{
    gnrc_rpl_instance_t *inst = NULL;
    gnrc_rpl_dodag_t *dodag = NULL;

    if (!_gnrc_rpl_check_DIO_validity(dio, len)) {
        return;
    }

    len -= (sizeof(gnrc_rpl_dio_t) + sizeof(icmpv6_hdr_t));

    if (gnrc_rpl_instance_add(dio->instance_id, &inst)) {
        /* new instance and DODAG */

        if (byteorder_ntohs(dio->rank) == GNRC_RPL_INFINITE_RANK) {
            DEBUG("RPL: ignore INFINITE_RANK DIO when we are not yet part of this DODAG\n");
            gnrc_rpl_instance_remove(inst);
            return;
        }

        inst->mop = (dio->g_mop_prf >> GNRC_RPL_MOP_SHIFT) & GNRC_RPL_SHIFTED_MOP_MASK;
        inst->of = gnrc_rpl_get_of_for_ocp(GNRC_RPL_DEFAULT_OCP);

        if (iface == KERNEL_PID_UNDEF) {
            iface = gnrc_ipv6_netif_find_by_addr(NULL, &ipv6_addr_all_rpl_nodes);
            assert(iface != KERNEL_PID_UNDEF);
        }

        gnrc_rpl_dodag_init(inst, &dio->dodag_id, iface, NULL);

        dodag = &inst->dodag;

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
        if(!_parse_options(GNRC_RPL_ICMPV6_CODE_DIO, inst, (gnrc_rpl_opt_t *)(dio + 1), len,
                           src, &included_opts)) {
            DEBUG("RPL: Error encountered during DIO option parsing - remove DODAG\n");
            gnrc_rpl_instance_remove(inst);
            return;
        }

        if (!(included_opts & (((uint32_t) 1) << GNRC_RPL_OPT_DODAG_CONF))) {
#ifndef GNRC_RPL_DODAG_CONF_OPTIONAL_ON_JOIN
            DEBUG("RPL: DIO without DODAG_CONF option - remove DODAG and request new DIO\n");
            gnrc_rpl_instance_remove(inst);
            gnrc_rpl_send_DIS(NULL, src);
            return;
#else
            DEBUG("RPL: DIO without DODAG_CONF option - use default trickle parameters\n");
            gnrc_rpl_send_DIS(NULL, src);
#endif
        }

        /* if there was no netif_addr created manually or by a PIO, then leave this DODAG */
        if (!dodag->netif_addr) {
            ipv6_addr_t *configured_addr;

            if (!(configured_addr = gnrc_ipv6_netif_match_prefix(dodag->iface, &dodag->dodag_id))) {
                DEBUG("RPL: no IPv6 address configured to match the given dodag id: %s\n",
                      ipv6_addr_to_str(addr_str, &(dodag->dodag_id), sizeof(addr_str)));
                gnrc_rpl_instance_remove(inst);
                return;
            }

            if (!(dodag->netif_addr = gnrc_ipv6_netif_addr_get(configured_addr))) {
                DEBUG("RPL: no netif address found for %s\n",
                       ipv6_addr_to_str(addr_str, configured_addr, sizeof(addr_str)));
                gnrc_rpl_instance_remove(inst);
                return;
            }
        }

        gnrc_rpl_delay_dao(dodag);
        trickle_start(gnrc_rpl_pid, &dodag->trickle, GNRC_RPL_MSG_TYPE_TRICKLE_INTERVAL,
                      GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min),
                      dodag->dio_interval_doubl, dodag->dio_redun);

        gnrc_rpl_parent_update(dodag, parent);
        return;
    }
    else if (inst == NULL) {
        DEBUG("RPL: Could not allocate a new instance.\n");
        return;
    }
    else {
        /* instance exists already */
        /* ignore dodags with other dodag_id's for now */
        /* TODO: choose DODAG with better rank */

        dodag = &inst->dodag;

        if (memcmp(&dodag->dodag_id, &dio->dodag_id, sizeof(ipv6_addr_t)) != 0) {
            DEBUG("RPL: DIO received from another DODAG, but same instance - ignore\n");
            return;
        }
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
    /* cppcheck-suppress nullPointer */
    else if (parent != NULL) {
        trickle_increment_counter(&dodag->trickle);
    }

    /* gnrc_rpl_parent_add_by_addr should have set this already */
    assert(parent != NULL);

    parent->rank = byteorder_ntohs(dio->rank);

    gnrc_rpl_parent_update(dodag, parent);

    /* sender of incoming DIO is not a parent of mine (anymore) and has an INFINITE rank
       and I have a rank != INFINITE_RANK */
    if (parent->state == 0) {
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
        if(!_parse_options(GNRC_RPL_ICMPV6_CODE_DIO, inst, (gnrc_rpl_opt_t *)(dio + 1), len,
                           src, &included_opts)) {
            DEBUG("RPL: Error encountered during DIO option parsing - remove DODAG\n");
            gnrc_rpl_instance_remove(inst);
            return;
        }
    }
}

gnrc_pktsnip_t *_dao_target_build(gnrc_pktsnip_t *pkt, ipv6_addr_t *addr, uint8_t prefix_length)
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

gnrc_pktsnip_t *_dao_transit_build(gnrc_pktsnip_t *pkt, uint8_t lifetime, bool external)
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

    gnrc_pktsnip_t *pkt = NULL, **ptr = NULL,  *tmp = NULL, *tr_int = NULL;
    gnrc_rpl_dao_t *dao;
    bool ext_processed = false, int_processed = false;

    /* find my address */
    ipv6_addr_t *me = NULL;
    gnrc_ipv6_netif_find_by_prefix(&me, &dodag->dodag_id);
    if (me == NULL) {
        DEBUG("RPL: no address configured\n");
        return;
    }

    mutex_lock(&(gnrc_ipv6_fib_table.mtx_access));

    /* add external and RPL FIB entries */
    for (size_t i = 0; i < gnrc_ipv6_fib_table.size; ++i) {
        ipv6_addr_t *addr;
        fib_entry_t *fentry = &gnrc_ipv6_fib_table.data.entries[i];
        if (fentry->lifetime != 0) {
            if (!(fentry->next_hop_flags & FIB_FLAG_RPL_ROUTE)) {
                ptr = &tmp;
                if (!ext_processed) {
                    DEBUG("RPL: Send DAO - building external transit\n");
                    if ((tmp = _dao_transit_build(NULL, lifetime, true)) == NULL) {
                        DEBUG("RPL: Send DAO - no space left in packet buffer\n");
                        mutex_unlock(&(gnrc_ipv6_fib_table.mtx_access));
                        return;
                    }
                    ext_processed = true;
                }
            }
            else {
                ptr = &pkt;
                if (!int_processed) {
                    DEBUG("RPL: Send DAO - building internal transit\n");
                    if ((tr_int = pkt = _dao_transit_build(NULL, lifetime, false)) == NULL) {
                        DEBUG("RPL: Send DAO - no space left in packet buffer\n");
                        mutex_unlock(&(gnrc_ipv6_fib_table.mtx_access));
                        return;
                    }
                    int_processed = true;
                }
            }
            addr = (ipv6_addr_t *) fentry->global->address;
            if (ipv6_addr_is_global(addr)) {
                size_t prefix_length = (fentry->global_flags >> FIB_FLAG_NET_PREFIX_SHIFT);

                DEBUG("RPL: Send DAO - building target %s/%d\n",
                      ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
                      (int) prefix_length);

                if ((*ptr = _dao_target_build(*ptr, addr, (uint8_t) prefix_length)) == NULL) {
                    DEBUG("RPL: Send DAO - no space left in packet buffer\n");
                    mutex_unlock(&(gnrc_ipv6_fib_table.mtx_access));
                    return;
                }
            }
        }
    }

    mutex_unlock(&(gnrc_ipv6_fib_table.mtx_access));

    if (tr_int) {
        tr_int->next = tmp;
    }
    else {
        pkt = tmp;
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

    gnrc_rpl_send(pkt, dodag->iface, NULL, destination, &dodag->dodag_id);

    GNRC_RPL_COUNTER_INCREMENT(dodag->dao_seq);
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

    if ((pkt = gnrc_icmpv6_build(NULL, ICMPV6_RPL_CTRL, GNRC_RPL_ICMPV6_CODE_DAO_ACK, size)) == NULL) {
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

    gnrc_rpl_send(pkt, dodag->iface, NULL, destination, &dodag->dodag_id);
}

static bool _gnrc_rpl_check_DAO_validity(gnrc_rpl_dao_t *dao, uint16_t len)
{
    uint16_t expected_len = sizeof(*dao) + sizeof(icmpv6_hdr_t);

    if ((dao->k_d_flags & GNRC_RPL_DAO_D_BIT)) {
        expected_len += sizeof(ipv6_addr_t);
    }

    if (expected_len <= len) {
        return true;
    }

    DEBUG("RPL: wrong DAO len: %d, expected: %d\n", len, expected_len);

    return false;
}

void gnrc_rpl_recv_DAO(gnrc_rpl_dao_t *dao, kernel_pid_t iface, ipv6_addr_t *src, uint16_t len)
{
    (void)iface;

    gnrc_rpl_instance_t *inst = NULL;
    gnrc_rpl_dodag_t *dodag = NULL;

    if (!_gnrc_rpl_check_DAO_validity(dao, len)) {
        return;
    }

    gnrc_rpl_opt_t *opts = (gnrc_rpl_opt_t *) (dao + 1);

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
                        (ipv6_addr_t *)(dao + 1), sizeof(addr_str)));
            return;
        }
        opts = (gnrc_rpl_opt_t *)(((uint8_t *) opts) + sizeof(ipv6_addr_t));
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
    if(!_parse_options(GNRC_RPL_ICMPV6_CODE_DAO, inst, opts, len, src, &included_opts)) {
        DEBUG("RPL: Error encountered during DAO option parsing - ignore DAO\n");
        return;
    }

    /* send a DAO-ACK if K flag is set */
    if (dao->k_d_flags & GNRC_RPL_DAO_K_BIT) {
        gnrc_rpl_send_DAO_ACK(inst, src, dao->dao_sequence);
    }

    gnrc_rpl_delay_dao(dodag);
}

static bool _gnrc_rpl_check_DAO_ACK_validity(gnrc_rpl_dao_ack_t *dao_ack, uint16_t len)
{
    uint16_t expected_len = sizeof(*dao_ack) + sizeof(icmpv6_hdr_t);

    if ((dao_ack->d_reserved & GNRC_RPL_DAO_ACK_D_BIT)) {
        expected_len += sizeof(ipv6_addr_t);
    }

    if (expected_len == len) {
        return true;
    }

    DEBUG("RPL: wrong DAO-ACK len: %d, expected: %d\n", len, expected_len);

    return false;
}

void gnrc_rpl_recv_DAO_ACK(gnrc_rpl_dao_ack_t *dao_ack, kernel_pid_t iface, uint16_t len)
{
    (void)iface;

    gnrc_rpl_instance_t *inst = NULL;
    gnrc_rpl_dodag_t *dodag = NULL;

    if (!_gnrc_rpl_check_DAO_ACK_validity(dao_ack, len)) {
        return;
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
                  (ipv6_addr_t *)(dao_ack + 1), sizeof(addr_str)));
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
