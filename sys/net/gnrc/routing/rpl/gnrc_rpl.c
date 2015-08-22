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

#include "net/gnrc/rpl.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG && defined(MODULE_IPV6_ADDR)
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static char _stack[GNRC_RPL_STACK_SIZE];
kernel_pid_t gnrc_rpl_pid = KERNEL_PID_UNDEF;
static timex_t _lt_time;
static vtimer_t _lt_timer;
static msg_t _msg_q[GNRC_RPL_MSG_QUEUE_SIZE];
static gnrc_netreg_entry_t _me_reg;

gnrc_rpl_instance_t gnrc_rpl_instances[GNRC_RPL_INSTANCES_NUMOF];
gnrc_rpl_dodag_t gnrc_rpl_dodags[GNRC_RPL_DODAGS_NUMOF];
gnrc_rpl_parent_t gnrc_rpl_parents[GNRC_RPL_PARENTS_NUMOF];

static void _update_lifetime(void);
static void _dao_handle_send(gnrc_rpl_dodag_t *dodag);
static void _receive(gnrc_pktsnip_t *pkt);
static void *_event_loop(void *args);

kernel_pid_t gnrc_rpl_init(kernel_pid_t if_pid)
{
    /* check if RPL was initialized before */
    if (gnrc_rpl_pid == KERNEL_PID_UNDEF) {
        /* start the event loop */
        gnrc_rpl_pid = thread_create(_stack, sizeof(_stack), GNRC_RPL_PRIO, CREATE_STACKTEST,
                _event_loop, NULL, "RPL");

        if (gnrc_rpl_pid == KERNEL_PID_UNDEF) {
            DEBUG("RPL: could not start the event loop\n");
            return KERNEL_PID_UNDEF;
        }

        _me_reg.demux_ctx = ICMPV6_RPL_CTRL;
        _me_reg.pid = gnrc_rpl_pid;
        /* register interest in all ICMPv6 packets */
        gnrc_netreg_register(GNRC_NETTYPE_ICMPV6, &_me_reg);

        gnrc_rpl_of_manager_init();
        _lt_time = timex_set(GNRC_RPL_LIFETIME_UPDATE_STEP, 0);
        vtimer_set_msg(&_lt_timer, _lt_time, gnrc_rpl_pid, GNRC_RPL_MSG_TYPE_LIFETIME_UPDATE, NULL);
    }

    /* register all_RPL_nodes multicast address */
    ipv6_addr_t all_RPL_nodes = GNRC_RPL_ALL_NODES_ADDR;
    gnrc_ipv6_netif_add_addr(if_pid, &all_RPL_nodes, IPV6_ADDR_BIT_LEN, 0);

    gnrc_rpl_send_DIS(NULL, &all_RPL_nodes);
    return gnrc_rpl_pid;
}

gnrc_rpl_dodag_t *gnrc_rpl_root_init(uint8_t instance_id, ipv6_addr_t *dodag_id)
{
    gnrc_rpl_dodag_t *dodag = gnrc_rpl_root_dodag_init(instance_id, dodag_id, GNRC_RPL_DEFAULT_MOP);

    if (!dodag) {
        return NULL;
    }

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

    trickle_start(gnrc_rpl_pid, &dodag->trickle, GNRC_RPL_MSG_TYPE_TRICKLE_INTERVAL,
                  GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min),
                  dodag->dio_interval_doubl, dodag->dio_redun);

    return dodag;
}

static void _receive(gnrc_pktsnip_t *icmpv6)
{
    gnrc_pktsnip_t *ipv6 = NULL;
    ipv6_hdr_t *ipv6_hdr = NULL;
    icmpv6_hdr_t *icmpv6_hdr = NULL;

    LL_SEARCH_SCALAR(icmpv6, ipv6, type, GNRC_NETTYPE_IPV6);

    assert(ipv6 != NULL);

    ipv6_hdr = (ipv6_hdr_t *)ipv6->data;

    icmpv6_hdr = (icmpv6_hdr_t *)icmpv6->data;
    switch (icmpv6_hdr->code) {
        case GNRC_RPL_ICMPV6_CODE_DIS:
            DEBUG("RPL: DIS received\n");
            gnrc_rpl_recv_DIS((gnrc_rpl_dis_t *)(icmpv6_hdr + 1), &ipv6_hdr->src, &ipv6_hdr->dst,
                    byteorder_ntohs(ipv6_hdr->len));
            break;
        case GNRC_RPL_ICMPV6_CODE_DIO:
            DEBUG("RPL: DIO received\n");
            gnrc_rpl_recv_DIO((gnrc_rpl_dio_t *)(icmpv6_hdr + 1), &ipv6_hdr->src,
                    byteorder_ntohs(ipv6_hdr->len));
            break;
        case GNRC_RPL_ICMPV6_CODE_DAO:
            DEBUG("RPL: DAO received\n");
            gnrc_rpl_recv_DAO((gnrc_rpl_dao_t *)(icmpv6_hdr + 1), &ipv6_hdr->src,
                    byteorder_ntohs(ipv6_hdr->len));
            break;
        case GNRC_RPL_ICMPV6_CODE_DAO_ACK:
            DEBUG("RPL: DAO-ACK received\n");
            gnrc_rpl_recv_DAO_ACK((gnrc_rpl_dao_ack_t *)(icmpv6_hdr + 1));
            break;
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
    gnrc_rpl_dodag_t *dodag;
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
                trickle = (trickle_t *) msg.content.ptr;
                if (trickle && (trickle->callback.func != NULL)) {
                    trickle_interval(trickle);
                }
                break;
            case GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK:
                DEBUG("RPL: GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK received\n");
                trickle = (trickle_t *) msg.content.ptr;
                if (trickle && (trickle->callback.func != NULL)) {
                    trickle_callback(trickle);
                }
                break;
            case GNRC_RPL_MSG_TYPE_DAO_HANDLE:
                DEBUG("RPL: GNRC_RPL_MSG_TYPE_DAO_HANDLE received\n");
                dodag = (gnrc_rpl_dodag_t *) msg.content.ptr;
                if (dodag && (dodag->state != 0)) {
                    _dao_handle_send(dodag);
                }
                break;
            case GNRC_RPL_MSG_TYPE_CLEANUP_HANDLE:
                DEBUG("RPL: GNRC_RPL_MSG_TYPE_CLEANUP received\n");
                dodag = (gnrc_rpl_dodag_t *) msg.content.ptr;
                if (dodag && (dodag->state != 0) && (dodag->parents == NULL)
                    && (dodag->my_rank == GNRC_RPL_INFINITE_RANK)) {
                    /* no parents - delete this DODAG */
                    gnrc_rpl_dodag_remove(dodag);
                }
                break;
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("RPL: GNRC_NETAPI_MSG_TYPE_RCV received\n");
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
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
    gnrc_rpl_parent_t *parent;
    for (uint8_t i = 0; i < GNRC_RPL_PARENTS_NUMOF; ++i) {
        parent = &gnrc_rpl_parents[i];
        if (parent->state != 0) {
            if ((signed)(parent->lifetime.seconds - now.seconds) <= GNRC_RPL_LIFETIME_UPDATE_STEP) {
                gnrc_rpl_dodag_t *dodag = parent->dodag;
                gnrc_rpl_parent_remove(parent);
                gnrc_rpl_parent_update(dodag, NULL);
                continue;
            }
            else if (((signed)(parent->lifetime.seconds - now.seconds) <=
                        GNRC_RPL_LIFETIME_UPDATE_STEP * 2)) {
                gnrc_rpl_send_DIS(parent->dodag, &parent->addr);
            }
        }
    }
    vtimer_remove(&_lt_timer);
    vtimer_set_msg(&_lt_timer, _lt_time, gnrc_rpl_pid, GNRC_RPL_MSG_TYPE_LIFETIME_UPDATE, NULL);
}

void gnrc_rpl_delay_dao(gnrc_rpl_dodag_t *dodag)
{
    dodag->dao_time = timex_set(GNRC_RPL_DEFAULT_DAO_DELAY, 0);
    dodag->dao_counter = 0;
    dodag->dao_ack_received = false;
    vtimer_remove(&dodag->dao_timer);
    vtimer_set_msg(&dodag->dao_timer, dodag->dao_time, gnrc_rpl_pid, GNRC_RPL_MSG_TYPE_DAO_HANDLE, dodag);
}

void gnrc_rpl_long_delay_dao(gnrc_rpl_dodag_t *dodag)
{
    dodag->dao_time = timex_set(GNRC_RPL_REGULAR_DAO_INTERVAL, 0);
    dodag->dao_counter = 0;
    dodag->dao_ack_received = false;
    vtimer_remove(&dodag->dao_timer);
    vtimer_set_msg(&dodag->dao_timer, dodag->dao_time, gnrc_rpl_pid, GNRC_RPL_MSG_TYPE_DAO_HANDLE, dodag);
}

void _dao_handle_send(gnrc_rpl_dodag_t *dodag)
{
    if ((dodag->dao_ack_received == false) && (dodag->dao_counter < GNRC_RPL_DAO_SEND_RETRIES)) {
        dodag->dao_counter++;
        gnrc_rpl_send_DAO(dodag, NULL, dodag->default_lifetime);
        dodag->dao_time = timex_set(GNRC_RPL_DEFAULT_WAIT_FOR_DAO_ACK, 0);
        vtimer_remove(&dodag->dao_timer);
        vtimer_set_msg(&dodag->dao_timer, dodag->dao_time,
                       gnrc_rpl_pid, GNRC_RPL_MSG_TYPE_DAO_HANDLE, dodag);
    }
    else if (dodag->dao_ack_received == false) {
        gnrc_rpl_long_delay_dao(dodag);
    }
}

/**
 * @}
 */
