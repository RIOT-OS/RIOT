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
#include "net/ng_rpl.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG && defined(MODULE_IPV6_ADDR)
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static char _stack[NG_RPL_STACK_SIZE];
kernel_pid_t ng_rpl_pid = KERNEL_PID_UNDEF;
static timex_t _lt_time;
static vtimer_t _lt_timer;
static msg_t _msg_q[NG_RPL_MSG_QUEUE_SIZE];
static ng_netreg_entry_t _me_reg;

ng_rpl_instance_t ng_rpl_instances[NG_RPL_INSTANCES_NUMOF];
ng_rpl_dodag_t ng_rpl_dodags[NG_RPL_DODAGS_NUMOF];
ng_rpl_parent_t ng_rpl_parents[NG_RPL_PARENTS_NUMOF];

static void _update_lifetime(void);
static void _dao_handle_send(ng_rpl_dodag_t *dodag);
static void _receive(ng_pktsnip_t *pkt);
static void *_event_loop(void *args);
static ng_rpl_dodag_t *_root_dodag_init(uint8_t instance_id, ipv6_addr_t *dodag_id, uint8_t mop);

kernel_pid_t ng_rpl_init(kernel_pid_t if_pid)
{
    /* check if RPL was initialized before */
    if (ng_rpl_pid == KERNEL_PID_UNDEF) {
        /* start the event loop */
        ng_rpl_pid = thread_create(_stack, sizeof(_stack), NG_RPL_PRIO, CREATE_STACKTEST,
                _event_loop, NULL, "RPL");

        if (ng_rpl_pid == KERNEL_PID_UNDEF) {
            DEBUG("RPL: could not start the event loop\n");
            return KERNEL_PID_UNDEF;
        }

        _me_reg.demux_ctx = ICMPV6_RPL_CTRL;
        _me_reg.pid = ng_rpl_pid;
        /* register interest in all ICMPv6 packets */
        ng_netreg_register(NG_NETTYPE_ICMPV6, &_me_reg);

        ng_rpl_of_manager_init();
        _lt_time = timex_set(NG_RPL_LIFETIME_UPDATE_STEP, 0);
        vtimer_set_msg(&_lt_timer, _lt_time, ng_rpl_pid, NG_RPL_MSG_TYPE_LIFETIME_UPDATE, NULL);
    }

    /* register all_RPL_nodes multicast address */
    ipv6_addr_t all_RPL_nodes = NG_IPV6_ADDR_ALL_RPL_NODES;
    ng_ipv6_netif_add_addr(if_pid, &all_RPL_nodes, IPV6_ADDR_BIT_LEN, 0);

    ng_rpl_send_DIS(NULL, &all_RPL_nodes);
    return ng_rpl_pid;
}

ng_rpl_dodag_t *ng_rpl_root_init(uint8_t instance_id, ipv6_addr_t *dodag_id)
{
    ng_rpl_dodag_t *dodag = _root_dodag_init(instance_id, dodag_id, NG_RPL_DEFAULT_MOP);

    if (!dodag) {
        return NULL;
    }

    dodag->dtsn = 1;
    dodag->prf = 0;
    dodag->dio_interval_doubl = NG_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS;
    dodag->dio_min = NG_RPL_DEFAULT_DIO_INTERVAL_MIN;
    dodag->dio_redun = NG_RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT;
    dodag->default_lifetime = NG_RPL_DEFAULT_LIFETIME;
    dodag->lifetime_unit = NG_RPL_LIFETIME_UNIT;
    dodag->version = NG_RPL_COUNTER_INIT;
    dodag->grounded = NG_RPL_GROUNDED;
    dodag->node_status = NG_RPL_ROOT_NODE;
    dodag->my_rank = NG_RPL_ROOT_RANK;

    trickle_start(ng_rpl_pid, &dodag->trickle, NG_RPL_MSG_TYPE_TRICKLE_INTERVAL,
                  NG_RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min),
                  dodag->dio_interval_doubl, dodag->dio_redun);

    return dodag;
}

static ng_rpl_dodag_t *_root_dodag_init(uint8_t instance_id, ipv6_addr_t *dodag_id, uint8_t mop)
{
    if (ng_rpl_pid == KERNEL_PID_UNDEF) {
        DEBUG("RPL: RPL thread not started\n");
        return NULL;
    }

    ipv6_addr_t *configured_addr;
    ng_rpl_instance_t *inst = NULL;
    ng_rpl_dodag_t *dodag = NULL;

    if (instance_id == 0) {
        DEBUG("RPL: instance id (%d) must be a positive number greater than zero\n", instance_id);
        return NULL;
    }

    if (ng_ipv6_netif_find_by_addr(&configured_addr, dodag_id) == KERNEL_PID_UNDEF) {
        DEBUG("RPL: no IPv6 address configured to match the given dodag id: %s\n",
              ipv6_addr_to_str(addr_str, dodag_id, sizeof(addr_str)));
        return NULL;
    }

    if (ng_rpl_instance_add(instance_id, &inst)) {
        inst->of = (ng_rpl_of_t *) ng_rpl_get_of_for_ocp(NG_RPL_DEFAULT_OCP);
        inst->mop = mop;
        inst->min_hop_rank_inc = NG_RPL_DEFAULT_MIN_HOP_RANK_INCREASE;
        inst->max_rank_inc = NG_RPL_DEFAULT_MAX_RANK_INCREASE;
    }
    else if (inst == NULL) {
        DEBUG("RPL: could not allocate memory for a new instance with id %d", instance_id);
        return NULL;
    }
    else if (inst->mop != mop) {
        DEBUG("RPL: instance (%d) exists with another MOP", instance_id);
        return NULL;
    }

    if (!ng_rpl_dodag_add(inst, dodag_id, &dodag)) {
        DEBUG("RPL: DODAG with id %s exists or no memory left for a new DODAG",
                ipv6_addr_to_str(addr_str, dodag_id, sizeof(addr_str)));
        return NULL;
    }

    return dodag;
}

static void _receive(ng_pktsnip_t *icmpv6)
{
    ng_pktsnip_t *ipv6 = NULL;
    ipv6_hdr_t *ipv6_hdr = NULL;
    icmpv6_hdr_t *icmpv6_hdr = NULL;

    LL_SEARCH_SCALAR(icmpv6, ipv6, type, NG_NETTYPE_IPV6);

    assert(ipv6 != NULL);

    ipv6_hdr = (ipv6_hdr_t *)ipv6->data;

    icmpv6_hdr = (icmpv6_hdr_t *)icmpv6->data;
    switch (icmpv6_hdr->code) {
        case NG_RPL_ICMPV6_CODE_DIS:
            DEBUG("RPL: DIS received\n");
            ng_rpl_recv_DIS((ng_rpl_dis_t *)(icmpv6_hdr + 1), &ipv6_hdr->src, &ipv6_hdr->dst,
                    byteorder_ntohs(ipv6_hdr->len));
            break;
        case NG_RPL_ICMPV6_CODE_DIO:
            DEBUG("RPL: DIO received\n");
            ng_rpl_recv_DIO((ng_rpl_dio_t *)(icmpv6_hdr + 1), &ipv6_hdr->src,
                    byteorder_ntohs(ipv6_hdr->len));
            break;
        case NG_RPL_ICMPV6_CODE_DAO:
            DEBUG("RPL: DAO received\n");
            ng_rpl_recv_DAO((ng_rpl_dao_t *)(icmpv6_hdr + 1), &ipv6_hdr->src,
                    byteorder_ntohs(ipv6_hdr->len));
            break;
        case NG_RPL_ICMPV6_CODE_DAO_ACK:
            DEBUG("RPL: DAO-ACK received\n");
            ng_rpl_recv_DAO_ACK((ng_rpl_dao_ack_t *)(icmpv6_hdr + 1));
            break;
        default:
            DEBUG("RPL: Unknown ICMPV6 code received\n");
            break;
    }

    ng_pktbuf_release(icmpv6);
}

static void *_event_loop(void *args)
{
    msg_t msg, reply;

    (void)args;
    msg_init_queue(_msg_q, NG_RPL_MSG_QUEUE_SIZE);

    /* preinitialize ACK */
    reply.type = NG_NETAPI_MSG_TYPE_ACK;

    trickle_t *trickle;
    ng_rpl_dodag_t *dodag;
    /* start event loop */
    while (1) {
        DEBUG("RPL: waiting for incoming message.\n");
        msg_receive(&msg);

        switch (msg.type) {
            case NG_RPL_MSG_TYPE_LIFETIME_UPDATE:
                DEBUG("RPL: NG_RPL_MSG_TYPE_LIFETIME_UPDATE received\n");
                _update_lifetime();
                break;
            case NG_RPL_MSG_TYPE_TRICKLE_INTERVAL:
                DEBUG("RPL: NG_RPL_MSG_TYPE_TRICKLE_INTERVAL received\n");
                trickle = (trickle_t *) msg.content.ptr;
                if (trickle && (trickle->callback.func != NULL)) {
                    trickle_interval(trickle);
                }
                break;
            case NG_RPL_MSG_TYPE_TRICKLE_CALLBACK:
                DEBUG("RPL: NG_RPL_MSG_TYPE_TRICKLE_CALLBACK received\n");
                trickle = (trickle_t *) msg.content.ptr;
                if (trickle && (trickle->callback.func != NULL)) {
                    trickle_callback(trickle);
                }
                break;
            case NG_RPL_MSG_TYPE_DAO_HANDLE:
                DEBUG("RPL: NG_RPL_MSG_TYPE_DAO_HANDLE received\n");
                dodag = (ng_rpl_dodag_t *) msg.content.ptr;
                if (dodag && (dodag->state != 0)) {
                    _dao_handle_send(dodag);
                }
                break;
            case NG_RPL_MSG_TYPE_CLEANUP_HANDLE:
                DEBUG("RPL: NG_RPL_MSG_TYPE_CLEANUP received\n");
                dodag = (ng_rpl_dodag_t *) msg.content.ptr;
                if (dodag && (dodag->state != 0) && (dodag->parents == NULL)) {
                    /* no parents - delete this DODAG */
                    ng_rpl_dodag_remove(dodag);
                }
                break;
            case NG_NETAPI_MSG_TYPE_RCV:
                DEBUG("RPL: NG_NETAPI_MSG_TYPE_RCV received\n");
                _receive((ng_pktsnip_t *)msg.content.ptr);
                break;
            case NG_NETAPI_MSG_TYPE_SND:
            case NG_NETAPI_MSG_TYPE_GET:
            case NG_NETAPI_MSG_TYPE_SET:
                DEBUG("RPL: reply to unsupported recv/get/set\n");
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
    timex_t now;
    vtimer_now(&now);
    ng_rpl_parent_t *parent;
    for (uint8_t i = 0; i < NG_RPL_PARENTS_NUMOF; ++i) {
        parent = &ng_rpl_parents[i];
        if (parent->state != 0) {
            if ((signed)(parent->lifetime.seconds - now.seconds) <= NG_RPL_LIFETIME_UPDATE_STEP) {
                ng_rpl_dodag_t *dodag = parent->dodag;
                ng_rpl_parent_remove(parent);
                ng_rpl_parent_update(dodag, NULL);
                continue;
            }
            else if (((signed)(parent->lifetime.seconds - now.seconds) <=
                        NG_RPL_LIFETIME_UPDATE_STEP * 2)) {
                ng_rpl_send_DIS(parent->dodag, &parent->addr);
            }
        }
    }
    vtimer_remove(&_lt_timer);
    vtimer_set_msg(&_lt_timer, _lt_time, ng_rpl_pid, NG_RPL_MSG_TYPE_LIFETIME_UPDATE, NULL);
}

void ng_rpl_delay_dao(ng_rpl_dodag_t *dodag)
{
    dodag->dao_time = timex_set(NG_RPL_DEFAULT_DAO_DELAY, 0);
    dodag->dao_counter = 0;
    dodag->dao_ack_received = false;
    vtimer_remove(&dodag->dao_timer);
    vtimer_set_msg(&dodag->dao_timer, dodag->dao_time, ng_rpl_pid, NG_RPL_MSG_TYPE_DAO_HANDLE, dodag);
}

void ng_rpl_long_delay_dao(ng_rpl_dodag_t *dodag)
{
    dodag->dao_time = timex_set(NG_RPL_REGULAR_DAO_INTERVAL, 0);
    dodag->dao_counter = 0;
    dodag->dao_ack_received = false;
    vtimer_remove(&dodag->dao_timer);
    vtimer_set_msg(&dodag->dao_timer, dodag->dao_time, ng_rpl_pid, NG_RPL_MSG_TYPE_DAO_HANDLE, dodag);
}

void _dao_handle_send(ng_rpl_dodag_t *dodag)
{
    if ((dodag->dao_ack_received == false) && (dodag->dao_counter < NG_RPL_DAO_SEND_RETRIES)) {
        dodag->dao_counter++;
        ng_rpl_send_DAO(dodag, NULL, dodag->default_lifetime);
        dodag->dao_time = timex_set(NG_RPL_DEFAULT_WAIT_FOR_DAO_ACK, 0);
        vtimer_remove(&dodag->dao_timer);
        vtimer_set_msg(&dodag->dao_timer, dodag->dao_time,
                       ng_rpl_pid, NG_RPL_MSG_TYPE_DAO_HANDLE, dodag);
    }
    else if (dodag->dao_ack_received == false) {
        ng_rpl_long_delay_dao(dodag);
    }
}

/**
 * @}
 */
