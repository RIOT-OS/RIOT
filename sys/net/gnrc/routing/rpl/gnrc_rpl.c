/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include "net/icmpv6.h"
#include "net/ipv6.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc.h"
#include "mutex.h"

#include "net/gnrc/rpl.h"
#ifdef MODULE_GNRC_RPL_P2P
#include "net/gnrc/rpl/p2p.h"
#include "net/gnrc/rpl/p2p_dodag.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

static char _stack[GNRC_RPL_STACK_SIZE];
kernel_pid_t gnrc_rpl_pid = KERNEL_PID_UNDEF;
const ipv6_addr_t ipv6_addr_all_rpl_nodes = GNRC_RPL_ALL_NODES_ADDR;
static uint32_t _lt_time = GNRC_RPL_LIFETIME_UPDATE_STEP * SEC_IN_USEC;
static xtimer_t _lt_timer;
static msg_t _lt_msg = { .type = GNRC_RPL_MSG_TYPE_LIFETIME_UPDATE };
static msg_t _msg_q[GNRC_RPL_MSG_QUEUE_SIZE];
static gnrc_netreg_entry_t _me_reg;
static mutex_t _inst_id_mutex = MUTEX_INIT;
static uint8_t _instance_id;

gnrc_rpl_instance_t gnrc_rpl_instances[GNRC_RPL_INSTANCES_NUMOF];
gnrc_rpl_parent_t gnrc_rpl_parents[GNRC_RPL_PARENTS_NUMOF];

#ifdef MODULE_NETSTATS_RPL
netstats_rpl_t gnrc_rpl_netstats;
#endif

static void _update_lifetime(void);
static void _dao_handle_send(gnrc_rpl_dodag_t *dodag);
static void _receive(gnrc_pktsnip_t *pkt);
static void *_event_loop(void *args);

kernel_pid_t gnrc_rpl_init(kernel_pid_t if_pid)
{
    /* check if RPL was initialized before */
    if (gnrc_rpl_pid == KERNEL_PID_UNDEF) {
        _instance_id = 0;
        /* start the event loop */
        gnrc_rpl_pid = thread_create(_stack, sizeof(_stack), GNRC_RPL_PRIO,
                                     THREAD_CREATE_STACKTEST,
                                     _event_loop, NULL, "RPL");

        if (gnrc_rpl_pid == KERNEL_PID_UNDEF) {
            DEBUG("RPL: could not start the event loop\n");
            return KERNEL_PID_UNDEF;
        }

        _me_reg.demux_ctx = ICMPV6_RPL_CTRL;
        _me_reg.target.pid = gnrc_rpl_pid;
        /* register interest in all ICMPv6 packets */
        gnrc_netreg_register(GNRC_NETTYPE_ICMPV6, &_me_reg);

        gnrc_rpl_of_manager_init();
        xtimer_set_msg(&_lt_timer, _lt_time, &_lt_msg, gnrc_rpl_pid);

#ifdef MODULE_NETSTATS_RPL
        memset(&gnrc_rpl_netstats, 0, sizeof(gnrc_rpl_netstats));
#endif
    }

    /* register all_RPL_nodes multicast address */
    gnrc_ipv6_netif_add_addr(if_pid, &ipv6_addr_all_rpl_nodes, IPV6_ADDR_BIT_LEN, 0);

    gnrc_rpl_send_DIS(NULL, (ipv6_addr_t *) &ipv6_addr_all_rpl_nodes);
    return gnrc_rpl_pid;
}

gnrc_rpl_instance_t *gnrc_rpl_root_init(uint8_t instance_id, ipv6_addr_t *dodag_id,
                                        bool gen_inst_id, bool local_inst_id)
{
    if (gen_inst_id) {
        instance_id = gnrc_rpl_gen_instance_id(local_inst_id);
    }

    gnrc_rpl_dodag_t *dodag = NULL;
    gnrc_rpl_instance_t *inst = gnrc_rpl_root_instance_init(instance_id, dodag_id,
                                                            GNRC_RPL_DEFAULT_MOP);

    if (!inst) {
        return NULL;
    }

    dodag = &inst->dodag;

    dodag->dtsn = 1;
    dodag->prf = 0;
    dodag->dio_interval_doubl = GNRC_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS;
    dodag->dio_min = GNRC_RPL_DEFAULT_DIO_INTERVAL_MIN;
    dodag->dio_redun = GNRC_RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT;
    dodag->default_lifetime = GNRC_RPL_DEFAULT_LIFETIME;
    dodag->lifetime_unit = GNRC_RPL_LIFETIME_UNIT;
    dodag->version = GNRC_RPL_COUNTER_INIT;
    dodag->grounded = GNRC_RPL_GROUNDED;
    dodag->node_status = GNRC_RPL_ROOT_NODE;
    dodag->my_rank = GNRC_RPL_ROOT_RANK;
    dodag->dio_opts |= GNRC_RPL_REQ_DIO_OPT_DODAG_CONF;
#ifndef GNRC_RPL_WITHOUT_PIO
    dodag->dio_opts |= GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO;
#endif

    trickle_start(gnrc_rpl_pid, &dodag->trickle, GNRC_RPL_MSG_TYPE_TRICKLE_INTERVAL,
                  GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min),
                  dodag->dio_interval_doubl, dodag->dio_redun);

    return inst;
}

static void _receive(gnrc_pktsnip_t *icmpv6)
{
    gnrc_pktsnip_t *ipv6, *netif;
    ipv6_hdr_t *ipv6_hdr;
    icmpv6_hdr_t *icmpv6_hdr;
    kernel_pid_t iface = KERNEL_PID_UNDEF;

    assert(icmpv6 != NULL);

    ipv6 = gnrc_pktsnip_search_type(icmpv6, GNRC_NETTYPE_IPV6);
    netif = gnrc_pktsnip_search_type(icmpv6, GNRC_NETTYPE_NETIF);

    assert(ipv6 != NULL);

    if (netif) {
        iface = ((gnrc_netif_hdr_t *)netif->data)->if_pid;
    }

    ipv6_hdr = (ipv6_hdr_t *)ipv6->data;

    icmpv6_hdr = (icmpv6_hdr_t *)icmpv6->data;
    switch (icmpv6_hdr->code) {
        case GNRC_RPL_ICMPV6_CODE_DIS:
            DEBUG("RPL: DIS received\n");
            gnrc_rpl_recv_DIS((gnrc_rpl_dis_t *)(icmpv6_hdr + 1), iface, &ipv6_hdr->src,
                              &ipv6_hdr->dst, byteorder_ntohs(ipv6_hdr->len));
            break;
        case GNRC_RPL_ICMPV6_CODE_DIO:
            DEBUG("RPL: DIO received\n");
            gnrc_rpl_recv_DIO((gnrc_rpl_dio_t *)(icmpv6_hdr + 1), iface, &ipv6_hdr->src,
                              &ipv6_hdr->dst, byteorder_ntohs(ipv6_hdr->len));
            break;
        case GNRC_RPL_ICMPV6_CODE_DAO:
            DEBUG("RPL: DAO received\n");
            gnrc_rpl_recv_DAO((gnrc_rpl_dao_t *)(icmpv6_hdr + 1), iface, &ipv6_hdr->src,
                              &ipv6_hdr->dst, byteorder_ntohs(ipv6_hdr->len));
            break;
        case GNRC_RPL_ICMPV6_CODE_DAO_ACK:
            DEBUG("RPL: DAO-ACK received\n");
            gnrc_rpl_recv_DAO_ACK((gnrc_rpl_dao_ack_t *)(icmpv6_hdr + 1), iface, &ipv6_hdr->src,
                                  &ipv6_hdr->dst, byteorder_ntohs(ipv6_hdr->len));
            break;
#ifdef MODULE_GNRC_RPL_P2P
        case GNRC_RPL_P2P_ICMPV6_CODE_DRO:
            DEBUG("RPL: P2P DRO received\n");
            gnrc_pktsnip_t *icmpv6_snip = gnrc_pktbuf_add(NULL, NULL, icmpv6->size,
                                                          GNRC_NETTYPE_ICMPV6);
            if (icmpv6_snip == NULL) {
                DEBUG("RPL-P2P: cannot copy ICMPv6 packet\n");
                break;
            }

            memcpy(icmpv6_snip->data, icmpv6->data, icmpv6->size);

            gnrc_rpl_p2p_recv_DRO(icmpv6_snip, &ipv6_hdr->src);
            break;
        case GNRC_RPL_P2P_ICMPV6_CODE_DRO_ACK:
            DEBUG("RPL: P2P DRO-ACK received\n");
            break;
#endif
        default:
            DEBUG("RPL: Unknown ICMPV6 code received\n");
            break;
    }

    gnrc_pktbuf_release(icmpv6);
}

static void *_event_loop(void *args)
{
    msg_t msg, reply;

    (void)args;
    msg_init_queue(_msg_q, GNRC_RPL_MSG_QUEUE_SIZE);

    /* preinitialize ACK */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    trickle_t *trickle;
    /* start event loop */
    while (1) {
        DEBUG("RPL: waiting for incoming message.\n");
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_RPL_MSG_TYPE_LIFETIME_UPDATE:
                DEBUG("RPL: GNRC_RPL_MSG_TYPE_LIFETIME_UPDATE received\n");
                _update_lifetime();
                break;
            case GNRC_RPL_MSG_TYPE_TRICKLE_INTERVAL:
                DEBUG("RPL: GNRC_RPL_MSG_TYPE_TRICKLE_INTERVAL received\n");
                trickle = msg.content.ptr;
                if (trickle && (trickle->callback.func != NULL)) {
                    trickle_interval(trickle);
                }
                break;
            case GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK:
                DEBUG("RPL: GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK received\n");
                trickle = msg.content.ptr;
                if (trickle && (trickle->callback.func != NULL)) {
                    trickle_callback(trickle);
                }
                break;
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("RPL: GNRC_NETAPI_MSG_TYPE_RCV received\n");
                _receive(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("RPL: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;
            default:
                break;
        }
    }

    return NULL;
}

void _update_lifetime(void)
{
    uint32_t now = xtimer_now();
    uint16_t now_sec = now / SEC_IN_USEC;

    gnrc_rpl_parent_t *parent;
    gnrc_rpl_instance_t *inst;

    for (uint8_t i = 0; i < GNRC_RPL_PARENTS_NUMOF; ++i) {
        parent = &gnrc_rpl_parents[i];
        if (parent->state != 0) {
            if ((int32_t)(parent->lifetime - now_sec) <= GNRC_RPL_LIFETIME_UPDATE_STEP) {
                gnrc_rpl_dodag_t *dodag = parent->dodag;
                gnrc_rpl_parent_remove(parent);
                gnrc_rpl_parent_update(dodag, NULL);
                continue;
            }
            else if ((int32_t)(parent->lifetime - now_sec) <= (GNRC_RPL_LIFETIME_UPDATE_STEP * 2)) {
                gnrc_rpl_send_DIS(parent->dodag->instance, &parent->addr);
            }
        }
    }

    for (int i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        inst = &gnrc_rpl_instances[i];
        if (inst->state != 0) {
            if ((inst->cleanup > 0) && (inst->dodag.parents == NULL) &&
                (inst->dodag.my_rank == GNRC_RPL_INFINITE_RANK)) {
                inst->cleanup -= GNRC_RPL_LIFETIME_UPDATE_STEP;
                if (inst->cleanup <= 0) {
                    /* no parents - delete this instance and DODAG */
                    gnrc_rpl_instance_remove(inst);
                    continue;
                }
            }

            if (inst->dodag.dao_time > GNRC_RPL_LIFETIME_UPDATE_STEP) {
                inst->dodag.dao_time -= GNRC_RPL_LIFETIME_UPDATE_STEP;
            }
            else {
                _dao_handle_send(&inst->dodag);
            }
        }
    }

#ifdef MODULE_GNRC_RPL_P2P
    gnrc_rpl_p2p_update();
#endif

    xtimer_set_msg(&_lt_timer, _lt_time, &_lt_msg, gnrc_rpl_pid);
}

void gnrc_rpl_delay_dao(gnrc_rpl_dodag_t *dodag)
{
    dodag->dao_time = GNRC_RPL_DEFAULT_DAO_DELAY;
    dodag->dao_counter = 0;
    dodag->dao_ack_received = false;
}

void gnrc_rpl_long_delay_dao(gnrc_rpl_dodag_t *dodag)
{
    dodag->dao_time = GNRC_RPL_REGULAR_DAO_INTERVAL;
    dodag->dao_counter = 0;
    dodag->dao_ack_received = false;
}

void _dao_handle_send(gnrc_rpl_dodag_t *dodag)
{
#ifdef MODULE_GNRC_RPL_P2P
    if (dodag->instance->mop == GNRC_RPL_P2P_MOP) {
        return;
    }
#endif
    if ((dodag->dao_ack_received == false) && (dodag->dao_counter < GNRC_RPL_DAO_SEND_RETRIES)) {
        dodag->dao_counter++;
        gnrc_rpl_send_DAO(dodag->instance, NULL, dodag->default_lifetime);
        dodag->dao_time = GNRC_RPL_DEFAULT_WAIT_FOR_DAO_ACK;
    }
    else if (dodag->dao_ack_received == false) {
        gnrc_rpl_long_delay_dao(dodag);
    }
}

uint8_t gnrc_rpl_gen_instance_id(bool local)
{
    mutex_lock(&_inst_id_mutex);
    uint8_t instance_id = GNRC_RPL_DEFAULT_INSTANCE;

    if (local) {
        instance_id = ((_instance_id++) | GNRC_RPL_INSTANCE_ID_MSB);
        mutex_unlock(&_inst_id_mutex);
        return instance_id;
    }

    instance_id = ((_instance_id++) & GNRC_RPL_GLOBAL_INSTANCE_MASK);
    mutex_unlock(&_inst_id_mutex);
    return instance_id;
}

/**
 * @}
 */
