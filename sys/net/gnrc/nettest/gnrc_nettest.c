/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <errno.h>
#include <string.h>

#include "msg.h"
#include "mutex.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/netopt.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "timex.h"
#include "thread.h"
#include "vtimer.h"

#include "net/gnrc/nettest.h"

static gnrc_nettest_opt_cbs_t _opt_cbs[NETOPT_NUMOF];
static mutex_t _mutex = MUTEX_INIT;
static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _stack[GNRC_NETTEST_STACK_SIZE];

static void *_event_loop(void *arg);

void gnrc_nettest_register_get(netopt_t opt, gnrc_nettest_opt_cb_t cb)
{
    mutex_lock(&_mutex);
    _opt_cbs[opt].get = cb;
    mutex_unlock(&_mutex);
}

void gnrc_nettest_register_set(netopt_t opt, gnrc_nettest_opt_cb_t cb)
{
    mutex_lock(&_mutex);
    _opt_cbs[opt].set = cb;
    mutex_unlock(&_mutex);
}

static gnrc_nettest_res_t _pkt_test(uint16_t cmd_type, kernel_pid_t pid,
                                    gnrc_pktsnip_t *in, unsigned int exp_pkts,
                                    const kernel_pid_t *exp_senders,
                                    const gnrc_pktsnip_t **exp_out)
{
    msg_t msg;
    timex_t t = { 0, GNRC_NETTEST_TIMEOUT };
    gnrc_nettest_res_t res = GNRC_NETTEST_SUCCESS;

    msg.type = cmd_type;
    msg.content.ptr = (char *)in;

    msg_send(&msg, pid);
    timex_normalize(&t);

    if (exp_pkts == 0) {
        thread_yield();
    }

    for (unsigned int i = 0; i < exp_pkts; i++) {
        gnrc_pktsnip_t *out;
        const gnrc_pktsnip_t *exp = exp_out[i];

        if (vtimer_msg_receive_timeout(&msg, t) < 0) {
            return GNRC_NETTEST_TIMED_OUT;
        }

        if (msg.type != cmd_type) {
            return GNRC_NETTEST_WRONG_MSG;
        }

        if (msg.sender_pid != exp_senders[i]) {
            return GNRC_NETTEST_WRONG_SENDER;
        }

        out = (gnrc_pktsnip_t *)msg.content.ptr;

        if (out == NULL) {
            return GNRC_NETTEST_FAIL;
        }

        while (out && exp) {
            if ((out->users != exp->users) ||
                (out->size != exp->size) ||
                (out->type != exp->type) ||
                (memcmp(out->data, exp->data, out->size) != 0)) {
                return GNRC_NETTEST_FAIL;
            }

            out = out->next;
            exp = exp->next;
        }

        gnrc_pktbuf_release((gnrc_pktsnip_t *)msg.content.ptr);
    }

    return res;
}

gnrc_nettest_res_t gnrc_nettest_send(kernel_pid_t pid, gnrc_pktsnip_t *in,
                                     unsigned int exp_pkts,
                                     const kernel_pid_t *exp_senders,
                                     const gnrc_pktsnip_t **exp_out,
                                     gnrc_nettype_t exp_type, uint32_t exp_demux_ctx)
{
    gnrc_netreg_entry_t reg_entry = { NULL, exp_demux_ctx, thread_getpid() };
    gnrc_nettest_res_t res;

    gnrc_netreg_register(exp_type, &reg_entry);

    res = _pkt_test(GNRC_NETAPI_MSG_TYPE_SND, pid, in, exp_pkts, exp_senders,
                    exp_out);

    gnrc_netreg_unregister(exp_type, &reg_entry);

    return res;
}

gnrc_nettest_res_t gnrc_nettest_send_iface(kernel_pid_t pid, gnrc_pktsnip_t *in,
                                           unsigned int exp_pkts,
                                           const kernel_pid_t *exp_senders,
                                           const gnrc_pktsnip_t **exp_out)
{
    gnrc_nettest_res_t res;

    gnrc_netif_add(thread_getpid());

    res = _pkt_test(GNRC_NETAPI_MSG_TYPE_SND, pid, in, exp_pkts, exp_senders,
                    exp_out);

    gnrc_netif_remove(thread_getpid());

    return res;
}

gnrc_nettest_res_t gnrc_nettest_receive(kernel_pid_t pid, gnrc_pktsnip_t *in,
                                        unsigned int exp_pkts,
                                        const kernel_pid_t *exp_senders,
                                        const gnrc_pktsnip_t **exp_out,
                                        gnrc_nettype_t exp_type, uint32_t exp_demux_ctx)
{
    gnrc_netreg_entry_t reg_entry = { NULL, exp_demux_ctx, thread_getpid() };
    gnrc_nettest_res_t res;

    gnrc_netreg_register(exp_type, &reg_entry);

    res = _pkt_test(GNRC_NETAPI_MSG_TYPE_RCV, pid, in, exp_pkts, exp_senders,
                    exp_out);

    gnrc_netreg_unregister(exp_type, &reg_entry);

    return res;
}

gnrc_nettest_res_t gnrc_nettest_get(kernel_pid_t pid, netopt_t opt,
                                    uint16_t context, void *data, size_t data_len,
                                    void *exp_data, int exp_res)
{
    if ((exp_res != gnrc_netapi_get(pid, opt, context, data, data_len)) ||
        ((exp_res > 0) && (memcpy(exp_data, data, exp_res)))) {
        return GNRC_NETTEST_FAIL;
    }

    return GNRC_NETTEST_SUCCESS;
}

gnrc_nettest_res_t gnrc_nettest_set(kernel_pid_t pid, netopt_t opt,
                                    uint16_t context, void *data, size_t data_len,
                                    int exp_res)
{
    if (exp_res != gnrc_netapi_get(pid, opt, context, data, data_len)) {
        return GNRC_NETTEST_FAIL;
    }

    return GNRC_NETTEST_SUCCESS;
}

int gnrc_nettest_init(void)
{
    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, sizeof(_stack), GNRC_NETTEST_PRIO,
                             CREATE_STACKTEST, _event_loop, NULL, "nettest");
    }

    return _pid;
}

void gnrc_nettest_reset(void)
{
    for (int i = 0; i < NETOPT_NUMOF; i++) {
        _opt_cbs[i].get = NULL;
        _opt_cbs[i].set = NULL;
    }
}

static inline uint32_t _get_set_opt(gnrc_nettest_opt_cb_t cb, uint16_t context,
                                    void *data, uint16_t data_len)
{
    int res;

    mutex_lock(&_mutex);
    if (cb != NULL) {
        res = cb(context, data, data_len);
    }
    else {
        res = -ENOTSUP;
    }
    mutex_unlock(&_mutex);
    return (uint32_t)res;
}

static void *_event_loop(void *arg)
{
    msg_t reply, msg_queue[GNRC_NETTEST_MSG_QUEUE_SIZE];

    (void)arg;
    msg_init_queue(msg_queue, GNRC_NETTEST_MSG_QUEUE_SIZE);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_t msg;
        gnrc_netapi_opt_t *opt;

        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_GET:
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                reply.content.value = _get_set_opt(_opt_cbs[opt->opt].get,
                                                   opt->context, opt->data,
                                                   opt->data_len);
                break;

            case GNRC_NETAPI_MSG_TYPE_SET:
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                reply.content.value = _get_set_opt(_opt_cbs[opt->opt].set,
                                                   opt->context, opt->data,
                                                   opt->data_len);
                break;
        }

        msg_reply(&msg, &reply);
    }

    return NULL;
}

/** @} */
