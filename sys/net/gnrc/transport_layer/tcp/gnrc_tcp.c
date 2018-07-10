/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 *
 * @file
 * @brief       GNRC TCP API implementation
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 * @}
 */

#include <errno.h>
#include <utlist.h>
#include <string.h>
#include "net/af.h"
#include "net/gnrc.h"
#include "net/gnrc/tcp.h"
#include "internal/common.h"
#include "internal/fsm.h"
#include "internal/pkt.h"
#include "internal/option.h"
#include "internal/eventloop.h"
#include "internal/rcvbuf.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Allocate memory for GNRC TCP thread stack.
 */
#if ENABLE_DEBUG
static char _stack[TCP_EVENTLOOP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[TCP_EVENTLOOP_STACK_SIZE];
#endif

/**
 * @brief TCPs eventloop pid, declared externally.
 */
kernel_pid_t gnrc_tcp_pid = KERNEL_PID_UNDEF;

/**
 * @brief Head of liked TCB list.
 */
gnrc_tcp_tcb_t *_list_tcb_head;

/**
 * @brief Mutex for TCB list synchronization.
 */
mutex_t _list_tcb_lock;

/**
 * @brief Helper struct, holding all argument data for_cb_mbox_put_msg.
 */
typedef struct _cb_arg {
    uint32_t msg_type;   /**< Message Type to Put into mbox behind mbox_ptr */
    mbox_t *mbox_ptr;    /**< Pointer to mbox */
} cb_arg_t;

/**
 * @brief Callback for xtimer, puts a message in a mbox.
 *
 * @param[in] arg   Ptr to cb_arg_t. Must not be NULL or anything else.
 */
static void _cb_mbox_put_msg(void *arg)
{
    msg_t msg;
    msg.type = ((cb_arg_t *) arg)->msg_type;
    mbox_try_put(((cb_arg_t *) arg)->mbox_ptr, &msg);
}

/**
 * @brief Setup timer with a callback function.
 *
 * @param[in/out] timer   Ptr to timer, which should be set.
 * @param[in] duration    Duration after @p timer expires.
 * @param[in] cb          Function to be called after @p duration.
 * @param[in] arg         Arguments for @p cb.
 */
static void _setup_timeout(xtimer_t *timer, const uint32_t duration, const xtimer_callback_t cb,
                           cb_arg_t *arg)
{
    timer->callback = cb;
    timer->arg = arg;
    xtimer_set(timer, duration);
}

/**
 * @brief   Establishes a new TCP connection
 *
 * @param[in,out] tcb           TCB holding the connection information.
 * @param[in]     target_addr   Target address to connect to, if this is a active connection.
 * @param[in]     target_port   Target port to connect to, if this is a active connection.
 * @param[in]     local_addr    Local address to bind on, if this is a passive connection.
 * @param[in]     local_port    Local port to bind on, if this is a passive connection.
 * @param[in]     passive       Flag to indicate if this is a active or passive open.
 *
 * @returns   Zero on success.
 *            -EISCONN if TCB is already connected.
 *            -ENOMEM if the receive buffer for the TCB could not be allocated.
 *            -EADDRINUSE if @p local_port is already in use.
 *            -ETIMEDOUT if the connection opening timed out.
 *            -ECONNREFUSED if the connection was resetted by the peer.
 */
static int _gnrc_tcp_open(gnrc_tcp_tcb_t *tcb, char *target_addr, uint16_t target_port,
                          const char *local_addr, uint16_t local_port, uint8_t passive)
{
    msg_t msg;
    xtimer_t connection_timeout;
    cb_arg_t connection_timeout_arg = {MSG_TYPE_CONNECTION_TIMEOUT, &(tcb->mbox)};
    int8_t ret = 0;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Connection is already connected: Return -EISCONN */
    if (tcb->state != FSM_STATE_CLOSED) {
        mutex_unlock(&(tcb->function_lock));
        return -EISCONN;
    }

    /* Mark TCB as waiting for incomming messages */
    tcb->status |= STATUS_WAIT_FOR_MSG;

    /* 'Flush' mbox */
    while (mbox_try_get(&(tcb->mbox), &msg) != 0) {
    }

    /* Setup passive connection */
    if (passive) {
        /* Mark connection as passive opend */
        tcb->status |= STATUS_PASSIVE;
        if (local_addr == NULL) {
            tcb->status |= STATUS_ALLOW_ANY_ADDR;
        }
#ifdef MODULE_GNRC_IPV6
        /* If local address is specified: Copy it into TCB */
        else if (tcb->address_family == AF_INET6) {
            if (ipv6_addr_from_str((ipv6_addr_t *) tcb->local_addr,  local_addr) == NULL) {
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : Invalid peer addr\n");
                return -EINVAL;
            }
        }
#else
        /* Supress Compiler Warnings */
        (void) target_addr;
#endif
        /* Set port number to listen on */
        tcb->local_port = local_port;
    }
    /* Setup active connection */
    else {
        /* Parse target address and port number into TCB */
 #ifdef MODULE_GNRC_IPV6
        if ((target_addr != NULL) && (tcb->address_family == AF_INET6)) {

            /* Extract interface (optional) specifier from target address */
            int ll_iface = ipv6_addr_split_iface(target_addr);
            if (ipv6_addr_from_str((ipv6_addr_t *) tcb->peer_addr, target_addr) == NULL) {
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : Invalid peer addr\n");
                return -EINVAL;
            }

            /* In case the given address is link-local: Memorize the interface Id if existing. */
            if ((ll_iface > 0) && ipv6_addr_is_link_local((ipv6_addr_t *) tcb->peer_addr)) {
                tcb->ll_iface = ll_iface;
            }
        }
 #endif
        /* Assign port numbers, verfication happens in fsm */
        tcb->local_port = local_port;
        tcb->peer_port = target_port;

        /* Setup connection timeout: Put timeout message in TCBs mbox on expiration */
        _setup_timeout(&connection_timeout, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                       _cb_mbox_put_msg, &connection_timeout_arg);
    }

    /* Call FSM with event: CALL_OPEN */
    ret = _fsm(tcb, FSM_EVENT_CALL_OPEN, NULL, NULL, 0);
    if (ret == -ENOMEM) {
        DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : Out of receive buffers.\n");
    }
    else if(ret == -EADDRINUSE) {
        DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : local_port is already in use.\n");
    }

    /* Wait until a connection was established or closed */
    while (ret >= 0 && tcb->state != FSM_STATE_CLOSED && tcb->state != FSM_STATE_ESTABLISHED &&
           tcb->state != FSM_STATE_CLOSE_WAIT) {
        mbox_get(&(tcb->mbox), &msg);
        switch (msg.type) {
            case MSG_TYPE_CONNECTION_TIMEOUT:
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : CONNECTION_TIMEOUT\n");
                _fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                ret = -ETIMEDOUT;
                break;

            case MSG_TYPE_NOTIFY_USER:
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : MSG_TYPE_NOTIFY_USER\n");
                break;

            default:
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : other message type\n");
        }
    }

    /* Cleanup */
    xtimer_remove(&connection_timeout);
    if (tcb->state == FSM_STATE_CLOSED && ret == 0) {
        ret = -ECONNREFUSED;
    }
    tcb->status &= ~STATUS_WAIT_FOR_MSG;
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

/* External GNRC TCP API */
int gnrc_tcp_init(void)
{
    /* Guard: Check if thread is already running */
    if (gnrc_tcp_pid != KERNEL_PID_UNDEF) {
        return -1;
    }

    /* Initialize mutex for TCB list synchronization */
    mutex_init(&(_list_tcb_lock));

    /* Initialize TCB list */
    _list_tcb_head = NULL;
    _rcvbuf_init();

    /* Start TCP processing thread */
    return thread_create(_stack, sizeof(_stack), TCP_EVENTLOOP_PRIO,
                         THREAD_CREATE_STACKTEST, _event_loop, NULL,
                         "gnrc_tcp");
}

void gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb)
{
    memset(tcb, 0, sizeof(gnrc_tcp_tcb_t));
#ifdef MODULE_GNRC_IPV6
    tcb->address_family = AF_INET6;
#else
    DEBUG("gnrc_tcp.c : gnrc_tcp_tcb_init() : Address unspec, add netlayer module to makefile\n");
#endif
    tcb->rtt_var = RTO_UNINITIALIZED;
    tcb->srtt = RTO_UNINITIALIZED;
    tcb->rto = RTO_UNINITIALIZED;
    mbox_init(&(tcb->mbox), tcb->mbox_raw, GNRC_TCP_TCB_MBOX_SIZE);
    mutex_init(&(tcb->fsm_lock));
    mutex_init(&(tcb->function_lock));
}

int gnrc_tcp_open_active(gnrc_tcp_tcb_t *tcb, uint8_t address_family,
                         char *target_addr, uint16_t target_port,
                         uint16_t local_port)
{
    assert(tcb != NULL);
    assert(target_addr != NULL);
    assert(target_port != PORT_UNSPEC);

    /* Check if AF-Family of target_addr is supported */
#ifdef MODULE_GNRC_IPV6
    if (address_family != AF_INET6) {
        return -EAFNOSUPPORT;
    }
#else
    return -EAFNOSUPPORT;
#endif

    /* Check if AF-Family for target address matches internally used AF-Family */
    if (tcb->address_family != address_family) {
        return -EINVAL;
    }
    /* Proceed with connection opening */
    return _gnrc_tcp_open(tcb, target_addr, target_port, NULL, local_port, 0);
}

int gnrc_tcp_open_passive(gnrc_tcp_tcb_t *tcb, uint8_t address_family,
                          const char *local_addr, uint16_t local_port)
{
    assert(tcb != NULL);
    assert(local_port != PORT_UNSPEC);

    /* Check AF-Family support if local address was supplied */
    if (local_addr != NULL) {
#ifdef MODULE_GNRC_IPV6
        if (address_family != AF_INET6) {
            return -EAFNOSUPPORT;
        }
#else
        return -EAFNOSUPPORT;
#endif
        /* Check if AF-Family matches internally used AF-Family */
        if (tcb->address_family != address_family) {
            return -EINVAL;
        }
    }
    /* Proceed with connection opening */
    return _gnrc_tcp_open(tcb, NULL, 0, local_addr, local_port, 1);
}

ssize_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, const void *data, const size_t len,
                      const uint32_t timeout_duration_us)
{
    assert(tcb != NULL);
    assert(data != NULL);

    msg_t msg;
    xtimer_t connection_timeout;
    cb_arg_t connection_timeout_arg = {MSG_TYPE_CONNECTION_TIMEOUT, &(tcb->mbox)};
    xtimer_t user_timeout;
    cb_arg_t user_timeout_arg = {MSG_TYPE_USER_SPEC_TIMEOUT, &(tcb->mbox)};
    xtimer_t probe_timeout;
    cb_arg_t probe_timeout_arg = {MSG_TYPE_PROBE_TIMEOUT, &(tcb->mbox)};
    uint32_t probe_timeout_duration_us = 0;
    ssize_t ret = 0;
    bool probing_mode = false;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Check if connection is in a valid state */
    if (tcb->state != FSM_STATE_ESTABLISHED && tcb->state != FSM_STATE_CLOSE_WAIT) {
        mutex_unlock(&(tcb->function_lock));
        return -ENOTCONN;
    }

    /* Mark TCB as waiting for incomming messages */
    tcb->status |= STATUS_WAIT_FOR_MSG;

    /* 'Flush' mbox */
    while (mbox_try_get(&(tcb->mbox), &msg) != 0) {
    }

    /* Setup connection timeout: Put timeout message in tcb's mbox on expiration */
    _setup_timeout(&connection_timeout, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                   _cb_mbox_put_msg, &connection_timeout_arg);

    /* Setup user specified timeout if timeout_us is greater than zero */
    if (timeout_duration_us > 0) {
        _setup_timeout(&user_timeout, timeout_duration_us, _cb_mbox_put_msg, &user_timeout_arg);
    }

    /* Loop until something was sent and acked */
    while (ret == 0 || tcb->pkt_retransmit != NULL) {
        /* Check if the connections state is closed. If so, a reset was received */
        if (tcb->state == FSM_STATE_CLOSED) {
            ret = -ECONNRESET;
            break;
        }

        /* If the send window is closed: Setup Probing */
        if (tcb->snd_wnd <= 0) {
            /* If this is the first probe: Setup probing duration */
            if (!probing_mode) {
                probing_mode = true;
                probe_timeout_duration_us = tcb->rto;
            }
            /* Setup probe timeout */
            _setup_timeout(&probe_timeout, timeout_duration_us, _cb_mbox_put_msg,
                           &probe_timeout_arg);
        }

        /* Try to send data in case there nothing has been sent and we are not probing */
        if (ret == 0 && !probing_mode) {
            ret = _fsm(tcb, FSM_EVENT_CALL_SEND, NULL, (void *) data, len);
        }

        /* Wait for responses */
        mbox_get(&(tcb->mbox), &msg);
        switch (msg.type) {
            case MSG_TYPE_CONNECTION_TIMEOUT:
                DEBUG("gnrc_tcp.c : gnrc_tcp_send() : CONNECTION_TIMEOUT\n");
                _fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                ret = -ECONNABORTED;
                break;

            case MSG_TYPE_USER_SPEC_TIMEOUT:
                DEBUG("gnrc_tcp.c : gnrc_tcp_send() : USER_SPEC_TIMEOUT\n");
                _fsm(tcb, FSM_EVENT_CLEAR_RETRANSMIT, NULL, NULL, 0);
                ret = -ETIMEDOUT;
                break;

            case MSG_TYPE_PROBE_TIMEOUT:
                DEBUG("gnrc_tcp.c : gnrc_tcp_send() : PROBE_TIMEOUT\n");
                /* Send probe */
                _fsm(tcb, FSM_EVENT_SEND_PROBE, NULL, NULL, 0);
                probe_timeout_duration_us += probe_timeout_duration_us;

                /* Boundry check for time interval between probes */
                if (probe_timeout_duration_us < GNRC_TCP_PROBE_LOWER_BOUND) {
                    probe_timeout_duration_us = GNRC_TCP_PROBE_LOWER_BOUND;
                }
                else if (probe_timeout_duration_us > GNRC_TCP_PROBE_UPPER_BOUND) {
                    probe_timeout_duration_us = GNRC_TCP_PROBE_UPPER_BOUND;
                }
                break;

            case MSG_TYPE_NOTIFY_USER:
                DEBUG("gnrc_tcp.c : gnrc_tcp_send() : NOTIFY_USER\n");

                /* Connection is alive: Reset Connection Timeout */
                _setup_timeout(&connection_timeout, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                               _cb_mbox_put_msg, &connection_timeout_arg);

                /* If the window re-opened and we are probing: Stop it */
                if (tcb->snd_wnd > 0 && probing_mode) {
                    probing_mode = false;
                    xtimer_remove(&probe_timeout);
                }
                break;

            default:
                DEBUG("gnrc_tcp.c : gnrc_tcp_send() : other message type\n");
        }
    }

    /* Cleanup */
    xtimer_remove(&probe_timeout);
    xtimer_remove(&connection_timeout);
    xtimer_remove(&user_timeout);
    tcb->status &= ~STATUS_WAIT_FOR_MSG;
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

ssize_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb, void *data, const size_t max_len,
                      const uint32_t timeout_duration_us)
{
    assert(tcb != NULL);
    assert(data != NULL);

    msg_t msg;
    xtimer_t connection_timeout;
    cb_arg_t connection_timeout_arg = {MSG_TYPE_CONNECTION_TIMEOUT, &(tcb->mbox)};
    xtimer_t user_timeout;
    cb_arg_t user_timeout_arg = {MSG_TYPE_USER_SPEC_TIMEOUT, &(tcb->mbox)};
    ssize_t ret = 0;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Check if connection is in a valid state */
    if (tcb->state != FSM_STATE_ESTABLISHED && tcb->state != FSM_STATE_FIN_WAIT_1 &&
        tcb->state != FSM_STATE_FIN_WAIT_2 && tcb->state != FSM_STATE_CLOSE_WAIT) {
        mutex_unlock(&(tcb->function_lock));
        return -ENOTCONN;
    }

    /* If this call is non-blocking (timeout_duration_us == 0): Try to read data and return */
    if (timeout_duration_us == 0) {
        ret = _fsm(tcb, FSM_EVENT_CALL_RECV, NULL, data, max_len);
        if (ret == 0) {
            ret = -EAGAIN;
        }
        mutex_unlock(&(tcb->function_lock));
        return ret;
    }

    /* Mark TCB as waiting for incomming messages */
    tcb->status |= STATUS_WAIT_FOR_MSG;

    /* 'Flush' mbox */
    while (mbox_try_get(&(tcb->mbox), &msg) != 0) {
    }

    /* Setup connection timeout: Put timeout message in tcb's mbox on expiration */
    _setup_timeout(&connection_timeout, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                   _cb_mbox_put_msg, &connection_timeout_arg);

    /* Setup user specified timeout */
    _setup_timeout(&user_timeout, timeout_duration_us, _cb_mbox_put_msg, &user_timeout_arg);

    /* Processing loop */
    while (ret == 0) {
        /* Check if the connections state is closed. If so, a reset was received */
        if (tcb->state == FSM_STATE_CLOSED) {
            ret = -ECONNRESET;
            break;
        }

        /* Try to read available data */
        ret = _fsm(tcb, FSM_EVENT_CALL_RECV, NULL, data, max_len);

        /* If there was no data: Wait for next packet or until the timeout fires */
        if (ret <= 0) {
            mbox_get(&(tcb->mbox), &msg);
            switch (msg.type) {
                case MSG_TYPE_CONNECTION_TIMEOUT:
                    DEBUG("gnrc_tcp.c : gnrc_tcp_recv() : CONNECTION_TIMEOUT\n");
                    _fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                    ret = -ECONNABORTED;
                    break;

                case MSG_TYPE_USER_SPEC_TIMEOUT:
                    DEBUG("gnrc_tcp.c : gnrc_tcp_send() : USER_SPEC_TIMEOUT\n");
                    _fsm(tcb, FSM_EVENT_CLEAR_RETRANSMIT, NULL, NULL, 0);
                    ret = -ETIMEDOUT;
                    break;

                case MSG_TYPE_NOTIFY_USER:
                    DEBUG("gnrc_tcp.c : gnrc_tcp_recv() : NOTIFY_USER\n");
                    break;

                default:
                    DEBUG("gnrc_tcp.c : gnrc_tcp_recv() : other message type\n");
            }
        }
    }

    /* Cleanup */
    xtimer_remove(&connection_timeout);
    xtimer_remove(&user_timeout);
    tcb->status &= ~STATUS_WAIT_FOR_MSG;
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

void gnrc_tcp_close(gnrc_tcp_tcb_t *tcb)
{
    assert(tcb != NULL);

    msg_t msg;
    xtimer_t connection_timeout;
    cb_arg_t connection_timeout_arg = {MSG_TYPE_CONNECTION_TIMEOUT, &(tcb->mbox)};

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Return if connection is closed */
    if (tcb->state == FSM_STATE_CLOSED) {
        mutex_unlock(&(tcb->function_lock));
        return;
    }

    /* Mark TCB as waiting for incomming messages */
    tcb->status |= STATUS_WAIT_FOR_MSG;

    /* 'Flush' mbox */
    while (mbox_try_get(&(tcb->mbox), &msg) != 0) {
    }

    /* Setup connection timeout: Put timeout message in tcb's mbox on expiration */
    _setup_timeout(&connection_timeout, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                   _cb_mbox_put_msg, &connection_timeout_arg);

    /* Start connection teardown sequence */
    _fsm(tcb, FSM_EVENT_CALL_CLOSE, NULL, NULL, 0);

    /* Loop until the connection has been closed */
    while (tcb->state != FSM_STATE_CLOSED) {
        mbox_get(&(tcb->mbox), &msg);
        switch (msg.type) {
            case MSG_TYPE_CONNECTION_TIMEOUT:
                DEBUG("gnrc_tcp.c : gnrc_tcp_close() : CONNECTION_TIMEOUT\n");
                _fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                break;

            case MSG_TYPE_NOTIFY_USER:
                DEBUG("gnrc_tcp.c : gnrc_tcp_close() : NOTIFY_USER\n");
                break;

            default:
                DEBUG("gnrc_tcp.c : gnrc_tcp_close() : other message type\n");
        }
    }

    /* Cleanup */
    xtimer_remove(&connection_timeout);
    tcb->status &= ~STATUS_WAIT_FOR_MSG;
    mutex_unlock(&(tcb->function_lock));
}

void gnrc_tcp_abort(gnrc_tcp_tcb_t *tcb)
{
    assert(tcb != NULL);

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));
    if (tcb->state != FSM_STATE_CLOSED) {
        /* Call FSM ABORT event */
        _fsm(tcb, FSM_EVENT_CALL_ABORT, NULL, NULL, 0);
    }
    mutex_unlock(&(tcb->function_lock));
}

int gnrc_tcp_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr)
{
    uint16_t csum;

    if ((hdr == NULL) || (pseudo_hdr == NULL)) {
        return -EFAULT;
    }
    if (hdr->type != GNRC_NETTYPE_TCP) {
        return -EBADMSG;
    }

    csum = _pkt_calc_csum(hdr, pseudo_hdr, hdr->next);
    if (csum == 0) {
        return -ENOENT;
    }
    ((tcp_hdr_t *)hdr->data)->checksum = byteorder_htons(csum);
    return 0;
}

gnrc_pktsnip_t *gnrc_tcp_hdr_build(gnrc_pktsnip_t *payload, uint16_t src, uint16_t dst)
{
    gnrc_pktsnip_t *res;
    tcp_hdr_t *hdr;

    /* Allocate header */
    res = gnrc_pktbuf_add(payload, NULL, sizeof(tcp_hdr_t), GNRC_NETTYPE_TCP);
    if (res == NULL) {
        DEBUG("tcp: No space left in packet buffer\n");
        return NULL;
    }
    hdr = (tcp_hdr_t *) res->data;

    /* Clear Header */
    memset(hdr, 0, sizeof(tcp_hdr_t));

    /* Initialize header with sane defaults */
    hdr->src_port = byteorder_htons(src);
    hdr->dst_port = byteorder_htons(dst);
    hdr->checksum = byteorder_htons(0);
    hdr->off_ctl = byteorder_htons(TCP_HDR_OFFSET_MIN);
    return res;
}
