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

#include "net/icmpv6.h"
#include "net/ng_icmpv6.h"
#include "net/ng_rpl.h"
#include "inet_ntop.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG && defined(MODULE_IPV6_ADDR)
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

#define NG_RPL_GROUNDED_SHIFT           (7)
#define NG_RPL_MOP_SHIFT                (3)
#define NG_RPL_OPT_DODAG_CONF_LEN       (14)
#define NG_RPL_SHIFTED_MOP_MASK         (0x7)
#define NG_RPL_PRF_MASK                 (0x7)

void _ng_rpl_send(ng_pktsnip_t *pkt, ipv6_addr_t *src, ipv6_addr_t *dst,
        ipv6_addr_t *dodag_id)
{
    ng_pktsnip_t *hdr;
    ipv6_addr_t all_RPL_nodes = NG_IPV6_ADDR_ALL_RPL_NODES, ll_addr;
    kernel_pid_t iface = ng_ipv6_netif_find_by_addr(NULL, &all_RPL_nodes);
    if (iface == KERNEL_PID_UNDEF) {
        DEBUG("RPL: no suitable interface found for this destination address\n");
        ng_pktbuf_release(pkt);
        return;
    }

    if (src == NULL) {
        ipv6_addr_t *tmp = NULL;
        if (dodag_id != NULL) {
            tmp = ng_ipv6_netif_match_prefix(iface, dodag_id);
        }
        else if (dodag_id == NULL) {
            tmp = ng_ipv6_netif_find_best_src_addr(iface, &all_RPL_nodes);
        }

        if (tmp == NULL) {
            DEBUG("RPL: no suitable src address found\n");
            ng_pktbuf_release(pkt);
            return;
        }

        memcpy(&ll_addr, tmp, sizeof(ll_addr));
        ipv6_addr_set_link_local_prefix(&ll_addr);
        src = &ll_addr;
    }

    if (dst == NULL) {
        dst = &all_RPL_nodes;
    }

    hdr = ng_ipv6_hdr_build(pkt, (uint8_t *)src, sizeof(ipv6_addr_t), (uint8_t *)dst,
                            sizeof(ipv6_addr_t));

    if (hdr == NULL) {
        DEBUG("RPL: Send - no space left in packet buffer\n");
        ng_pktbuf_release(pkt);
        return;
    }

    if (!ng_netapi_dispatch_send(NG_NETTYPE_IPV6, NG_NETREG_DEMUX_CTX_ALL,hdr)) {
        DEBUG("RPL: cannot send packet: no subscribers found.\n");
        ng_pktbuf_release(hdr);
    }

}

void ng_rpl_send_DIO(ng_rpl_dodag_t *dodag, ipv6_addr_t *destination)
{
    if (dodag == NULL) {
        DEBUG("RPL: Error - trying to send DIO without being part of a dodag.\n");
        return;
    }

    ng_pktsnip_t *pkt;
    icmpv6_hdr_t *icmp;
    ng_rpl_dio_t *dio;
    uint8_t *pos;
    int size = sizeof(icmpv6_hdr_t) + sizeof(ng_rpl_dio_t);

    if ((dodag->dodag_conf_counter % 3) == 0) {
        size += sizeof(ng_rpl_opt_dodag_conf_t);
    }

    if ((pkt = ng_icmpv6_build(NULL, ICMPV6_RPL_CTRL, NG_RPL_ICMPV6_CODE_DIO, size)) == NULL) {
        DEBUG("RPL: Send DIO - no space left in packet buffer\n");
        return;
    }

    icmp = (icmpv6_hdr_t *)pkt->data;
    dio = (ng_rpl_dio_t *)(icmp + 1);
    pos = (uint8_t *) dio;
    dio->instance_id = dodag->instance->id;
    dio->version_number = dodag->version;
    dio->rank = byteorder_htons(dodag->my_rank);
    dio->g_mop_prf = (dodag->grounded << NG_RPL_GROUNDED_SHIFT) |
        (dodag->instance->mop << NG_RPL_MOP_SHIFT) | dodag->prf;
    dio->dtsn = dodag->dtsn;
    dio->flags = 0;
    dio->reserved = 0;
    dio->dodag_id = dodag->dodag_id;

    pos += sizeof(*dio);

    if ((dodag->dodag_conf_counter % 3) == 0) {
        ng_rpl_opt_dodag_conf_t *dodag_conf;
        dodag_conf = (ng_rpl_opt_dodag_conf_t *) pos;
        dodag_conf->type = NG_RPL_OPT_DODAG_CONF;
        dodag_conf->length = NG_RPL_OPT_DODAG_CONF_LEN;
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
    }

    dodag->dodag_conf_counter++;
    _ng_rpl_send(pkt, NULL, destination, &dodag->dodag_id);
}

void ng_rpl_send_DIS(ng_rpl_dodag_t *dodag, ipv6_addr_t *destination)
{
    (void) dodag;
    ng_pktsnip_t *pkt;
    icmpv6_hdr_t *icmp;
    ng_rpl_dis_t *dis;
    /* TODO: Currently the DIS is too small so that wireshark complains about an incorrect
     * ethernet frame check sequence. In order to prevent this, 4 PAD1 options are added.
     * This will be addressed in follow-up PRs */
    int size = sizeof(icmpv6_hdr_t) + sizeof(ng_rpl_dis_t) + 4;

    if ((pkt = ng_icmpv6_build(NULL, ICMPV6_RPL_CTRL, NG_RPL_ICMPV6_CODE_DIS, size)) == NULL) {
        DEBUG("RPL: Send DIS - no space left in packet buffer\n");
        return;
    }

    icmp = (icmpv6_hdr_t *)pkt->data;
    dis = (ng_rpl_dis_t *)(icmp + 1);
    dis->flags = 0;
    dis->reserved = 0;
    /* TODO: see above TODO */
    memset((dis + 1), 0, 4);

    _ng_rpl_send(pkt, NULL, destination, (dodag ? &dodag->dodag_id : NULL));
}

void ng_rpl_recv_DIS(ng_rpl_dis_t *dis, ipv6_addr_t *src, ipv6_addr_t *dst, uint16_t len)
{
    /* TODO handle Solicited Information Option */
    (void) dis;
    (void) len;

    if (ipv6_addr_is_multicast(dst)) {
        for (uint8_t i = 0; i < NG_RPL_DODAGS_NUMOF; ++i) {
            if (ng_rpl_dodags[i].state != 0) {
                trickle_reset_timer(&ng_rpl_dodags[i].trickle);
            }
        }
    }
    else {
        for (uint8_t i = 0; i < NG_RPL_DODAGS_NUMOF; ++i) {
            if (ng_rpl_dodags[i].state != 0) {
                ng_rpl_dodags[i].dodag_conf_counter = 0;
                ng_rpl_send_DIO(&ng_rpl_dodags[i], src);
            }
        }
    }
}

/** @todo allow target prefixes in target options to be of variable length */
bool _parse_options(int msg_type, ng_rpl_dodag_t *dodag, ng_rpl_opt_t *opt, uint16_t len,
                    ipv6_addr_t *src)
{
    uint16_t l = 0;
    ng_rpl_opt_target_t *first_target = NULL;
    while(l < len) {
        if ((opt->type != NG_RPL_OPT_PAD1) && (len < opt->length + sizeof(ng_rpl_opt_t) + l)) {
            /* return false to delete the dodag,
             * because former options may also contain errors */
            return false;
        }

        switch(opt->type) {
            case (NG_RPL_OPT_PAD1): {
                DEBUG("RPL: PAD1 option parsed\n");
                l += 1;
                opt = (ng_rpl_opt_t *) (((uint8_t *) opt) + 1);
                continue;
            }
            case (NG_RPL_OPT_PADN): {
                DEBUG("RPL: PADN option parsed\n");
                break;
            }
            case (NG_RPL_OPT_DODAG_CONF): {
                if (msg_type != NG_RPL_ICMPV6_CODE_DIO) {
                    DEBUG("RPL: Ignore DODAG CONF DIO option\n");
                    return true;
                }
                DEBUG("RPL: DODAG CONF DIO option parsed\n");
                ng_rpl_opt_dodag_conf_t *dc = (ng_rpl_opt_dodag_conf_t *) opt;
                ng_rpl_of_t *of = ng_rpl_get_of_for_ocp(byteorder_ntohs(dc->ocp));
                if (of != NULL) {
                    dodag->instance->of = of;
                }
                else {
                    DEBUG("RPL: Unsupported OCP 0x%02x\n", byteorder_ntohs(dc->ocp));
                    dodag->instance->of = ng_rpl_get_of_for_ocp(NG_RPL_DEFAULT_OCP);
                }
                dodag->dio_interval_doubl = dc->dio_int_doubl;
                dodag->dio_min = dc->dio_int_min;
                dodag->dio_redun = dc->dio_redun;
                dodag->instance->max_rank_inc = byteorder_ntohs(dc->max_rank_inc);
                dodag->instance->min_hop_rank_inc = byteorder_ntohs(dc->min_hop_rank_inc);
                dodag->default_lifetime = dc->default_lifetime;
                dodag->lifetime_unit = byteorder_ntohs(dc->lifetime_unit);
                dodag->trickle.Imin = (1 << dodag->dio_min);
                dodag->trickle.Imax = dodag->dio_interval_doubl;
                dodag->trickle.k = dodag->dio_redun;
                break;
            }
            case (NG_RPL_OPT_TARGET): {
                if (msg_type != NG_RPL_ICMPV6_CODE_DAO) {
                    DEBUG("RPL: Ignore RPL TARGET DAO option\n");
                    return true;
                }
                DEBUG("RPL: RPL TARGET DAO option parsed\n");
                kernel_pid_t if_id = ng_ipv6_netif_find_by_prefix(NULL, &dodag->dodag_id);
                if (if_id == KERNEL_PID_UNDEF) {
                    DEBUG("RPL: no interface found for the configured DODAG id\n");
                    return false;
                }

                ng_rpl_opt_target_t *target = (ng_rpl_opt_target_t *) opt;
                if (first_target == NULL) {
                    first_target = target;
                }

                fib_add_entry(if_id, target->target.u8, sizeof(ipv6_addr_t), AF_INET6, src->u8,
                        sizeof(ipv6_addr_t), AF_INET6,
                        (dodag->default_lifetime * dodag->lifetime_unit) * SEC_IN_MS);
                break;
            }
            case (NG_RPL_OPT_TRANSIT): {
                if (msg_type != NG_RPL_ICMPV6_CODE_DAO) {
                    DEBUG("RPL: Ignore RPL TRANSIT INFO DAO option\n");
                    return true;
                }
                DEBUG("RPL: RPL TRANSIT INFO DAO option parsed\n");
                ng_rpl_opt_transit_t *transit = (ng_rpl_opt_transit_t *) opt;
                if (first_target == NULL) {
                    DEBUG("RPL: Encountered a RPL TRANSIT DAO option without \
a preceding RPL TARGET DAO option\n");
                    break;
                }

                do {
                    fib_update_entry(first_target->target.u8, sizeof(ipv6_addr_t),
                            src->u8, sizeof(ipv6_addr_t), AF_INET6,
                            (transit->path_lifetime * dodag->lifetime_unit * SEC_IN_MS));
                    first_target = (ng_rpl_opt_target_t *) (((uint8_t *) (first_target)) +
                        sizeof(ng_rpl_opt_t) + first_target->length);
                }
                while (first_target->type == NG_RPL_OPT_TARGET);

                first_target = NULL;
                break;
            }
        }
        l += opt->length + sizeof(ng_rpl_opt_t);
        opt = (ng_rpl_opt_t *) (((uint8_t *) (opt + 1)) + opt->length);
    }
    return true;
}

void ng_rpl_recv_DIO(ng_rpl_dio_t *dio, ipv6_addr_t *src, uint16_t len)
{
    ng_rpl_instance_t *inst = NULL;
    ng_rpl_dodag_t *dodag = NULL;

    len -= (sizeof(ng_rpl_dio_t) + sizeof(icmpv6_hdr_t));

    if (ng_rpl_instance_add(dio->instance_id, &inst)) {
        inst->mop = (dio->g_mop_prf >> NG_RPL_MOP_SHIFT) & NG_RPL_SHIFTED_MOP_MASK;
        inst->of = ng_rpl_get_of_for_ocp(NG_RPL_DEFAULT_OCP);
    }
    else if (inst == NULL) {
        DEBUG("RPL: Could not allocate a new instance.\n");
        return;
    }

    if ((byteorder_ntohs(dio->rank) == NG_RPL_INFINITE_RANK) &&
            (ng_rpl_dodag_get(inst, &dio->dodag_id) == NULL)) {
        DEBUG("RPL: ignore INFINITE_RANK DIO when we are not part of this DODAG\n");
        ng_rpl_instance_remove(inst);
        return;
    }

    if (ng_rpl_dodag_add(inst, &dio->dodag_id, &dodag)) {
        DEBUG("RPL: Joined DODAG (%s).\n",
                ipv6_addr_to_str(addr_str, &dio->dodag_id, sizeof(addr_str)));

        ng_rpl_parent_t *parent = NULL;

        if (!ng_rpl_parent_add_by_addr(dodag, src, &parent) && (parent == NULL)) {
            DEBUG("RPL: Could not allocate new parent.\n");
            ng_rpl_dodag_remove(dodag);
            return;
        }

        dodag->version = dio->version_number;

        parent->rank = byteorder_ntohs(dio->rank);

        if(!_parse_options(NG_RPL_ICMPV6_CODE_DIO, dodag, (ng_rpl_opt_t *)(dio + 1), len, NULL)) {
            ng_rpl_dodag_remove(dodag);
            return;
        }

        ng_rpl_delay_dao(dodag);
        trickle_start(ng_rpl_pid, &dodag->trickle, NG_RPL_MSG_TYPE_TRICKLE_INTERVAL,
                      NG_RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min),
                      dodag->dio_interval_doubl, dodag->dio_redun);

        ng_rpl_parent_update(dodag, parent);
        return;
    }
    else if (dodag == NULL) {
        DEBUG("RPL: Could not allocate a new DODAG.\n");
        if (inst->dodags == NULL) {
            ng_rpl_instance_remove(inst);
        }
        return;
    }

    if (dodag->instance->mop !=
            ((dio->g_mop_prf >> NG_RPL_MOP_SHIFT) & NG_RPL_SHIFTED_MOP_MASK)) {
        DEBUG("RPL: invalid MOP for this instance.\n");
        return;
    }

    if (NG_RPL_COUNTER_GREATER_THAN(dio->version_number, dodag->version)) {
        if (dodag->node_status == NG_RPL_ROOT_NODE) {
            dodag->version = NG_RPL_COUNTER_INCREMENT(dio->version_number);
            trickle_reset_timer(&dodag->trickle);
        }
        else {
            dodag->version = dio->version_number;
            ng_rpl_local_repair(dodag);
        }
    }
    else if (NG_RPL_COUNTER_GREATER_THAN(dodag->version, dio->version_number)) {
        trickle_reset_timer(&dodag->trickle);
        return;
    }

    if (dodag->node_status == NG_RPL_ROOT_NODE) {
        if (byteorder_ntohs(dio->rank) != NG_RPL_INFINITE_RANK) {
            trickle_increment_counter(&dodag->trickle);
        }
        return;
    }

    dodag->grounded = dio->g_mop_prf >> NG_RPL_GROUNDED_SHIFT;
    dodag->prf = dio->g_mop_prf & NG_RPL_PRF_MASK;

    ng_rpl_parent_t *parent = NULL;

    if (!ng_rpl_parent_add_by_addr(dodag, src, &parent) && (parent == NULL)) {
        DEBUG("RPL: Could not allocate new parent.\n");
        if (dodag->parents == NULL) {
            ng_rpl_dodag_remove(dodag);
        }
        return;
    }
    /* cppcheck-suppress nullPointer */
    else if (parent != NULL) {
        trickle_increment_counter(&dodag->trickle);
    }

    /* ng_rpl_parent_add_by_addr should have set this already */
    assert(parent != NULL);

    parent->rank = byteorder_ntohs(dio->rank);

    if(!_parse_options(NG_RPL_ICMPV6_CODE_DIO, dodag, (ng_rpl_opt_t *)(dio + 1), len, NULL)) {
        ng_rpl_dodag_remove(dodag);
        return;
    }

    ng_rpl_parent_update(dodag, parent);

    if (parent->state != 0) {
        if (dodag->parents && (parent == dodag->parents) && (parent->dtsn != dio->dtsn)) {
            ng_rpl_delay_dao(dodag);
        }
        parent->dtsn = dio->dtsn;
    }
}

void _dao_fill_target(ng_rpl_opt_target_t *target, ipv6_addr_t *addr)
{
    target->type = NG_RPL_OPT_TARGET;
    target->length = sizeof(target->flags) + sizeof(target->prefix_length) + sizeof(target->target);
    target->flags = 0;
    target->prefix_length = 128;
    target->target = *addr;
}

void ng_rpl_send_DAO(ng_rpl_dodag_t *dodag, ipv6_addr_t *destination, uint8_t lifetime)
{
    size_t dst_size = NG_RPL_PARENTS_NUMOF;
    fib_destination_set_entry_t fib_dest_set[NG_RPL_PARENTS_NUMOF];

    if (dodag == NULL) {
        DEBUG("RPL: Error - trying to send DAO without being part of a dodag.\n");
        return;
    }

    if (dodag->node_status == NG_RPL_ROOT_NODE) {
        return;
    }

    if (destination == NULL) {
        if (dodag->parents == NULL) {
            DEBUG("RPL: dodag has no preferred parent\n");
            return;
        }

        destination = &(dodag->parents->addr);
    }

    ng_pktsnip_t *pkt;
    icmpv6_hdr_t *icmp;
    ng_rpl_dao_t *dao;
    ng_rpl_opt_target_t *target;
    ng_rpl_opt_transit_t *transit;

    /* find my address */
    ipv6_addr_t *me = NULL;
    ng_ipv6_netif_find_by_prefix(&me, &dodag->dodag_id);
    if (me == NULL) {
        DEBUG("RPL: no address configured\n");
        return;
    }

    ng_ipv6_netif_addr_t *me_netif = ng_ipv6_netif_addr_get(me);
    if (me_netif == NULL) {
        DEBUG("RPL: no netif address found for %s\n", ipv6_addr_to_str(addr_str, me,
                    sizeof(addr_str)));
        return;
    }

    /* find prefix for my address */
    ipv6_addr_t prefix;
    ipv6_addr_init_prefix(&prefix, me, me_netif->prefix_len);
    fib_get_destination_set(prefix.u8, sizeof(ipv6_addr_t), fib_dest_set, &dst_size);

    int size = sizeof(icmpv6_hdr_t) + sizeof(ng_rpl_dao_t) +
        (sizeof(ng_rpl_opt_target_t) * (dst_size + 1)) + sizeof(ng_rpl_opt_transit_t);

    if ((pkt = ng_icmpv6_build(NULL, ICMPV6_RPL_CTRL, NG_RPL_ICMPV6_CODE_DAO, size)) == NULL) {
        DEBUG("RPL: Send DAO - no space left in packet buffer\n");
        return;
    }

    icmp = (icmpv6_hdr_t *)pkt->data;
    dao = (ng_rpl_dao_t *)(icmp + 1);

    dao->instance_id = dodag->instance->id;
    /* set the D flag to indicate that a DODAG id is present */
    /* set the K flag to indicate that a ACKs are required */
    dao->k_d_flags = ((1 << 6) | (1 << 7));
    dao->dao_sequence = dodag->dao_seq;
    dao->dodag_id = dodag->dodag_id;
    dao->reserved = 0;

    /* add own address */
    target = (ng_rpl_opt_target_t *) (dao + 1);
    _dao_fill_target(target, me);
    /* add children */
    for (size_t i = 0; i < dst_size; ++i) {
        target = (target + 1);
        _dao_fill_target(target, ((ipv6_addr_t *) fib_dest_set[i].dest));
    }

    transit = (ng_rpl_opt_transit_t *) (target + 1);
    transit->type = NG_RPL_OPT_TRANSIT;
    transit->length = sizeof(transit->e_flags) + sizeof(transit->path_control) +
        sizeof(transit->path_sequence) + sizeof(transit->path_lifetime);
    transit->e_flags = 0;
    transit->path_control = 0;
    transit->path_sequence = 0;
    transit->path_lifetime = lifetime;

    _ng_rpl_send(pkt, NULL, destination, &dodag->dodag_id);

    NG_RPL_COUNTER_INCREMENT(dodag->dao_seq);
}

void ng_rpl_send_DAO_ACK(ng_rpl_dodag_t *dodag, ipv6_addr_t *destination, uint8_t seq)
{
    if (dodag == NULL) {
        DEBUG("RPL: Error - trying to send DAO-ACK without being part of a dodag.\n");
        return;
    }

    ng_pktsnip_t *pkt;
    icmpv6_hdr_t *icmp;
    ng_rpl_dao_ack_t *dao_ack;
    int size = sizeof(icmpv6_hdr_t) + sizeof(ng_rpl_dao_ack_t);

    if ((pkt = ng_icmpv6_build(NULL, ICMPV6_RPL_CTRL, NG_RPL_ICMPV6_CODE_DAO_ACK, size)) == NULL) {
        DEBUG("RPL: Send DAOACK - no space left in packet buffer\n");
        return;
    }

    icmp = (icmpv6_hdr_t *)pkt->data;
    dao_ack = (ng_rpl_dao_ack_t *)(icmp + 1);

    dao_ack->instance_id = dodag->instance->id;
    /* set the D flag to indicate that a DODAG id is present */
    dao_ack->d_reserved = (1 << 7);
    dao_ack->dao_sequence = seq;
    dao_ack->status = 0;
    dao_ack->dodag_id = dodag->dodag_id;

    _ng_rpl_send(pkt, NULL, destination, &dodag->dodag_id);
}

void ng_rpl_recv_DAO(ng_rpl_dao_t *dao, ipv6_addr_t *src, uint16_t len)
{
    ng_rpl_instance_t *inst = NULL;
    ng_rpl_dodag_t *dodag = NULL;
    if ((inst = ng_rpl_instance_get(dao->instance_id)) == NULL) {
        DEBUG("RPL: DAO with unknown instance id (%d) received\n", dao->instance_id);
        return;
    }

    /* check if the D flag is set before accessing the DODAG id */
    if (!(dao->k_d_flags & (1 << 6))) {
        DEBUG("RPL: DAO with D flag unset - global instances not supported\n");
        return;
    }

    if ((dodag = ng_rpl_dodag_get(inst, &dao->dodag_id)) == NULL) {
        DEBUG("RPL: DAO with unknown DODAG id (%s)\n", ipv6_addr_to_str(addr_str,
                    &dao->dodag_id, sizeof(addr_str)));
        return;
    }

    len -= (sizeof(ng_rpl_dao_t) + sizeof(icmpv6_hdr_t));
    if(!_parse_options(NG_RPL_ICMPV6_CODE_DAO, dodag, (ng_rpl_opt_t *) (dao + 1), len, src)) {
        ng_rpl_dodag_remove(dodag);
        return;
    }

    /* send a DAO-ACK if K flag is set */
    if (dao->k_d_flags & (1 << 7)) {
        ng_rpl_send_DAO_ACK(dodag, src, dao->dao_sequence);
    }

    ng_rpl_delay_dao(dodag);
}

void ng_rpl_recv_DAO_ACK(ng_rpl_dao_ack_t *dao_ack)
{
    ng_rpl_instance_t *inst = NULL;
    ng_rpl_dodag_t *dodag = NULL;
    if ((inst = ng_rpl_instance_get(dao_ack->instance_id)) == NULL) {
        DEBUG("RPL: DAO-ACK with unknown instance id (%d) received\n", dao_ack->instance_id);
        return;
    }

    /* check if the D flag is set before accessing the DODAG id */
    if (!(dao_ack->d_reserved & (1 << 7))) {
        DEBUG("RPL: DAO-ACK with D flag unset - global instances not supported\n");
        return;
    }

    if ((dodag = ng_rpl_dodag_get(inst, &dao_ack->dodag_id)) == NULL) {
        DEBUG("RPL: DAO-ACK with unknown DODAG id (%s)\n", ipv6_addr_to_str(addr_str,
                    &dao_ack->dodag_id, sizeof(addr_str)));
        return;
    }

    if ((dao_ack->status != 0) && (dao_ack->dao_sequence != dodag->dao_seq)) {
        DEBUG("RPL: DAO-ACK sequence (%d) does not match expected sequence (%d)\n",
                dao_ack->dao_sequence, dodag->dao_seq);
        return;
    }

    dodag->dao_ack_received = true;
    ng_rpl_long_delay_dao(dodag);
}

/**
 * @}
 */
