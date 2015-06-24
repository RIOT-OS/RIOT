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
#include "net/ng_netapi.h"
#include "net/ng_netif.h"
#include "net/ng_netconf.h"
#include "net/ng_netreg.h"
#include "net/ng_pktbuf.h"
#include "timex.h"
#include "thread.h"
#include "vtimer.h"

#include "net/ng_nettest.h"

static ng_nettest_opt_cbs_t _opt_cbs[NETCONF_OPT_NUMOF];
static mutex_t _mutex = MUTEX_INIT;
static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _stack[NG_NETTEST_STACK_SIZE];

static void *_event_loop(void *arg);

void ng_nettest_register_get(ng_netconf_opt_t opt, ng_nettest_opt_cb_t cb)
{
    mutex_lock(&_mutex);
    _opt_cbs[opt].get = cb;
    mutex_unlock(&_mutex);
}

void ng_nettest_register_set(ng_netconf_opt_t opt, ng_nettest_opt_cb_t cb)
{
    mutex_lock(&_mutex);
    _opt_cbs[opt].set = cb;
    mutex_unlock(&_mutex);
}

static ng_nettest_res_t _pkt_test(uint16_t cmd_type, kernel_pid_t pid, ng_pktsnip_t *in,
                                  unsigned int exp_pkts, kernel_pid_t exp_senders[],
                                  ng_pktsnip_t *exp_out[])
{
    msg_t msg;
    timex_t t = { 0, NG_NETTEST_TIMEOUT };
    ng_nettest_res_t res = NG_NETTEST_SUCCESS;

    msg.type = cmd_type;
    msg.content.ptr = (char *)in;

    msg_send(&msg, pid);
    timex_normalize(&t);

    for (unsigned int i = 0; i < exp_pkts; i++) {
        ng_pktsnip_t *out, *exp = exp_out[i];

        if (vtimer_msg_receive_timeout(&msg, t) < 0) {
            return NG_NETTEST_TIMED_OUT;
        }

        if (msg.type != cmd_type) {
            return NG_NETTEST_WRONG_MSG;
        }

        if (msg.sender_pid != exp_senders[i]) {
            return NG_NETTEST_WRONG_SENDER;
        }

        out = (ng_pktsnip_t *)msg.content.ptr;

        if (out == NULL) {
            return NG_NETTEST_FAIL;
        }

        while (out && exp) {
            if ((out->users != exp->users) ||
                (out->size != exp->size) ||
                (out->type != exp->type) ||
                (memcmp(out->data, exp->data, out->size) != 0)) {
                return NG_NETTEST_FAIL;
            }

            out = out->next;
            exp = exp->next;
        }

        ng_pktbuf_release((ng_pktsnip_t *)msg.content.ptr);
    }

    return res;
}

ng_nettest_res_t ng_nettest_send(kernel_pid_t pid, ng_pktsnip_t *in,
                                 unsigned int exp_pkts, kernel_pid_t exp_senders[],
                                 ng_pktsnip_t *exp_out[], ng_nettype_t exp_type,
                                 uint32_t exp_demux_ctx)
{
    ng_netreg_entry_t reg_entry = { NULL, exp_demux_ctx, thread_getpid() };
    ng_nettest_res_t res;

    ng_netreg_register(exp_type, &reg_entry);

    res = _pkt_test(NG_NETAPI_MSG_TYPE_SND, pid, in, exp_pkts, exp_senders,
                    exp_out);

    ng_netreg_unregister(exp_type, &reg_entry);

    return res;
}

ng_nettest_res_t ng_nettest_send_iface(kernel_pid_t pid, ng_pktsnip_t *in,
                                       unsigned int exp_pkts,
                                       kernel_pid_t exp_senders[],
                                       ng_pktsnip_t *exp_out[])
{
    ng_nettest_res_t res;

    ng_netif_add(thread_getpid());

    res = _pkt_test(NG_NETAPI_MSG_TYPE_SND, pid, in, exp_pkts, exp_senders,
                    exp_out);

    ng_netif_remove(thread_getpid());

    return res;
}

ng_nettest_res_t ng_nettest_receive(kernel_pid_t pid, ng_pktsnip_t *in,
                                    unsigned int exp_pkts, kernel_pid_t exp_senders[],
                                    ng_pktsnip_t *exp_out[], ng_nettype_t exp_type,
                                    uint32_t exp_demux_ctx)
{
    ng_netreg_entry_t reg_entry = { NULL, exp_demux_ctx, thread_getpid() };
    ng_nettest_res_t res;

    ng_netreg_register(exp_type, &reg_entry);

    res = _pkt_test(NG_NETAPI_MSG_TYPE_RCV, pid, in, exp_pkts, exp_senders,
                    exp_out);

    ng_netreg_unregister(exp_type, &reg_entry);

    return res;
}

ng_nettest_res_t ng_nettest_get(kernel_pid_t pid, ng_netconf_opt_t opt,
                                uint16_t context, void *data, size_t data_len,
                                void *exp_data, int exp_res)
{
    if ((exp_res != ng_netapi_get(pid, opt, context, data, data_len)) ||
        ((exp_res > 0) && (memcpy(exp_data, data, exp_res)))) {
        return NG_NETTEST_FAIL;
    }

    return NG_NETTEST_SUCCESS;
}

ng_nettest_res_t ng_nettest_set(kernel_pid_t pid, ng_netconf_opt_t opt,
                                uint16_t context, void *data, size_t data_len,
                                int exp_res)
{
    if (exp_res != ng_netapi_get(pid, opt, context, data, data_len)) {
        return NG_NETTEST_FAIL;
    }

    return NG_NETTEST_SUCCESS;
}

int ng_nettest_init(void)
{
    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, sizeof(_stack), NG_NETTEST_PRIO,
                             CREATE_STACKTEST, _event_loop, NULL, "nettest");
    }

    return _pid;
}

void ng_nettest_reset(void)
{
    for (int i = 0; i < NETCONF_OPT_NUMOF; i++) {
        _opt_cbs[i].get = NULL;
        _opt_cbs[i].set = NULL;
    }
}

static inline uint32_t _get_set_opt(ng_nettest_opt_cb_t cb, uint16_t context,
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
    msg_t reply, msg_queue[NG_NETTEST_MSG_QUEUE_SIZE];

    (void)arg;
    msg_init_queue(msg_queue, NG_NETTEST_MSG_QUEUE_SIZE);
    reply.type = NG_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_t msg;
        ng_netapi_opt_t *opt;

        msg_receive(&msg);

        switch (msg.type) {
            case NG_NETAPI_MSG_TYPE_GET:
                opt = (ng_netapi_opt_t *)msg.content.ptr;
                reply.content.value = _get_set_opt(_opt_cbs[opt->opt].get,
                                                   opt->context, opt->data,
                                                   opt->data_len);
                break;

            case NG_NETAPI_MSG_TYPE_SET:
                opt = (ng_netapi_opt_t *)msg.content.ptr;
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
