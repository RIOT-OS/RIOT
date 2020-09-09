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
#include <string.h>
#include <utlist.h>

#include "mbox.h"
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

#define TCP_MSG_QUEUE_SIZE (1 << CONFIG_GNRC_TCP_MSG_QUEUE_SIZE_EXP)

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
 *            -ECONNREFUSED if the connection was reset by the peer.
 */
static int _gnrc_tcp_open(gnrc_tcp_tcb_t *tcb, const gnrc_tcp_ep_t *remote,
                          const uint8_t *local_addr, uint16_t local_port, int passive)
{
    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    cb_arg_t connection_timeout_arg = {MSG_TYPE_CONNECTION_TIMEOUT, &mbox};
    int ret = 0;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* TCB is already connected: Return -EISCONN */
    if (tcb->state != FSM_STATE_CLOSED) {
        mutex_unlock(&(tcb->function_lock));
        return -EISCONN;
    }

    /* Setup messaging */
    _fsm_set_mbox(tcb, &mbox);

    /* Setup passive connection */
    if (passive) {
        /* Mark connection as passive opend */
        tcb->status |= STATUS_PASSIVE;
#ifdef MODULE_GNRC_IPV6
        /* If local address is specified: Copy it into TCB */
        if (local_addr && tcb->address_family == AF_INET6) {
            /* Store given address in TCB */
            if (memcpy(tcb->local_addr, local_addr, sizeof(tcb->local_addr)) == NULL) {
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : Invalid peer addr\n");
                return -EINVAL;
            }

            if (ipv6_addr_is_unspecified((ipv6_addr_t *) tcb->local_addr)) {
                tcb->status |= STATUS_ALLOW_ANY_ADDR;
            }
        }
#else
        /* Suppress Compiler Warnings */
        (void) remote;
        (void) local_addr;
#endif
        /* Set port number to listen on */
        tcb->local_port = local_port;
    }
    /* Setup active connection */
    else {
        assert(remote != NULL);

        /* Parse target address and port number into TCB */
 #ifdef MODULE_GNRC_IPV6
        if (tcb->address_family == AF_INET6) {

            /* Store Address information in TCB */
            if (memcpy(tcb->peer_addr, remote->addr.ipv6, sizeof(tcb->peer_addr)) == NULL) {
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : Invalid peer addr\n");
                return -EINVAL;
            }
            tcb->ll_iface = remote->netif;
        }
 #endif

        /* Assign port numbers, verification happens in fsm */
        tcb->local_port = local_port;
        tcb->peer_port = remote->port;

        /* Setup connection timeout: Put timeout message in TCBs mbox on expiration */
        _setup_timeout(&(tcb->timer_misc), CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                       _cb_mbox_put_msg, &connection_timeout_arg);
    }

    /* Call FSM with event: CALL_OPEN */
    ret = _fsm(tcb, FSM_EVENT_CALL_OPEN, NULL, NULL, 0);
    if (ret == -ENOMEM) {
        DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : Out of receive buffers.\n");
    }
    else if (ret == -EADDRINUSE) {
        DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : local_port is already in use.\n");
    }

    /* Wait until a connection was established or closed */
    while (ret >= 0 && tcb->state != FSM_STATE_CLOSED && tcb->state != FSM_STATE_ESTABLISHED &&
           tcb->state != FSM_STATE_CLOSE_WAIT) {
        mbox_get(&mbox, &msg);
        switch (msg.type) {
            case MSG_TYPE_NOTIFY_USER:
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : MSG_TYPE_NOTIFY_USER\n");

                /* Setup a timeout to be able to revert back to LISTEN state, in case the
                 * send SYN+ACK we received upon entering SYN_RCVD is never acknowledged
                 * by the peer. */
                if ((tcb->state == FSM_STATE_SYN_RCVD) && (tcb->status & STATUS_PASSIVE)) {
                    _setup_timeout(&(tcb->timer_misc),
                                   CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION, _cb_mbox_put_msg,
                                   &connection_timeout_arg);
                }
                break;

            case MSG_TYPE_CONNECTION_TIMEOUT:
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : CONNECTION_TIMEOUT\n");

                /* The connection establishment attempt timed out:
                 * 1) Active connections return -ETIMEOUT.
                 * 2) Passive connections stop the ongoing retransmissions and repeat the
                 *    open call to wait for the next connection attempt. */
                if (tcb->status & STATUS_PASSIVE) {
                    _fsm(tcb, FSM_EVENT_CLEAR_RETRANSMIT, NULL, NULL, 0);
                    _fsm(tcb, FSM_EVENT_CALL_OPEN, NULL, NULL, 0);
                }
                else {
                    _fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                    ret = -ETIMEDOUT;
                }
                break;

            default:
                DEBUG("gnrc_tcp.c : _gnrc_tcp_open() : other message type\n");
        }
    }

    /* Cleanup */
    _fsm_set_mbox(tcb, NULL);
    xtimer_remove(&(tcb->timer_misc));
    if (tcb->state == FSM_STATE_CLOSED && ret == 0) {
        ret = -ECONNREFUSED;
    }
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

/* External GNRC TCP API */
int gnrc_tcp_ep_init(gnrc_tcp_ep_t *ep, int family, const uint8_t *addr, size_t addr_size,
                     uint16_t port, uint16_t netif)
{
#ifdef MODULE_GNRC_IPV6
    if (family != AF_INET6) {
        return -EAFNOSUPPORT;
    }

    if (addr == NULL && addr_size == 0) {
        ipv6_addr_set_unspecified((ipv6_addr_t *) ep->addr.ipv6);
    }
    else if (addr_size == sizeof(ipv6_addr_t)) {
        memcpy(ep->addr.ipv6, addr, sizeof(ipv6_addr_t));
    }
    else {
        return -EINVAL;
    }
#else
    /* Suppress Compiler Warnings */
    (void) addr;
    (void) addr_size;
    return -EAFNOSUPPORT;
#endif

    ep->family = family;
    ep->port = port;
    ep->netif = netif;
    return 0;
}

int gnrc_tcp_ep_from_str(gnrc_tcp_ep_t *ep, const char *str)
{
    assert(str);

    unsigned port = 0;
    unsigned netif = 0;

    /* Examine given string */
    char *addr_begin = strchr(str, '[');
    char *addr_end = strchr(str, ']');

    /* 1) Ensure that str contains a single pair of brackets */
    if (!addr_begin || !addr_end || strchr(addr_begin + 1, '[') || strchr(addr_end + 1, ']')) {
        return -EINVAL;
    }
    /* 2) Ensure that the first character is the opening bracket */
    else if (addr_begin != str) {
        return -EINVAL;
    }

    /* 3) Examine optional port number */
    char *port_begin = strchr(addr_end, ':');
    if (port_begin) {
        /* 3.1) Ensure that there are characters left to parse after ':'. */
        if (*(++port_begin) == '\0') {
            return -EINVAL;
        }

        /* 3.2) Ensure that port is a number (atol, does not report errors) */
        for (char *ptr = port_begin; *ptr; ++ptr) {
            if ((*ptr < '0') || ('9' < *ptr)) {
                return -EINVAL;
            }
        }

        /* 3.3) Read and verify that given number port is within range */
        port = atol(port_begin);
        if (port > 0xFFFF) {
            return -EINVAL;
        }
    }

    /* 4) Examine optional interface identifier. */
    char *if_begin = strchr(str, '%');
    if (if_begin) {
        /* 4.1) Ensure that the identifier is not empty and within brackets. */
        if (addr_end <= (++if_begin)) {
            return -EINVAL;
        }

        /* 4.2) Ensure that the identifier is a number (atol, does not report errors) */
        for (char *ptr = if_begin; ptr != addr_end; ++ptr) {
            if ((*ptr < '0') || ('9' < *ptr)) {
                return -EINVAL;
            }
        }

        /* 4.3) Read and replace addr_end with if_begin. */
        netif = atol(if_begin);
        addr_end = if_begin - 1;
    }

#ifdef MODULE_GNRC_IPV6
    /* 5) Try to parse IP Address. Construct Endpoint on after success. */
    char tmp[IPV6_ADDR_MAX_STR_LEN];

    /* 5.1) Verify address length and copy address into temporary buffer.
     *      This is required to preserve constness of input.
     */
    int len = addr_end - (++addr_begin);

    if (0 <= len && len < (int) sizeof(tmp)) {
        memcpy(tmp, addr_begin, len);
        tmp[len] = '\0';
    }
    else {
        return -EINVAL;
    }

    /* 5.2) Try to read address into endpoint. */
    if (ipv6_addr_from_str((ipv6_addr_t *) ep->addr.ipv6, tmp) == NULL) {
        return -EINVAL;
    }
    ep->family = AF_INET6;
#else
    /* Suppress Compiler Warnings */
    (void) port;
    (void) netif;
    return -EINVAL;
#endif

    ep->port = (uint16_t) port;
    ep->netif = (uint16_t) netif;
    return 0;
}

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
    mutex_init(&(tcb->fsm_lock));
    mutex_init(&(tcb->function_lock));
}

int gnrc_tcp_open_active(gnrc_tcp_tcb_t *tcb, const gnrc_tcp_ep_t *remote, uint16_t local_port)
{
    assert(tcb != NULL);
    assert(remote != NULL);
    assert(remote->port != PORT_UNSPEC);

    /* Check if given AF-Family in remote is supported */
#ifdef MODULE_GNRC_IPV6
    if (remote->family != AF_INET6) {
        return -EAFNOSUPPORT;
    }
#else
    return -EAFNOSUPPORT;
#endif

    /* Check if AF-Family for target address matches internally used AF-Family */
    if (remote->family != tcb->address_family) {
        return -EINVAL;
    }

    /* Proceed with connection opening */
    return _gnrc_tcp_open(tcb, remote, NULL, local_port, 0);
}

int gnrc_tcp_open_passive(gnrc_tcp_tcb_t *tcb, const gnrc_tcp_ep_t *local)
{
    assert(tcb != NULL);
    assert(local != NULL);
    assert(local->port != PORT_UNSPEC);

    /* Check if given AF-Family in local is supported */
#ifdef MODULE_GNRC_IPV6
    if (local->family != AF_INET6) {
        return -EAFNOSUPPORT;
    }

    /* Check if AF-Family matches internally used AF-Family */
    if (local->family != tcb->address_family) {
        return -EINVAL;
    }

    /* Proceed with connection opening */
    return _gnrc_tcp_open(tcb, NULL, local->addr.ipv6, local->port, 1);
#else
    return -EAFNOSUPPORT;
#endif
}

ssize_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, const void *data, const size_t len,
                      const uint32_t timeout_duration_us)
{
    assert(tcb != NULL);
    assert(data != NULL);

    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    cb_arg_t connection_timeout_arg = {MSG_TYPE_CONNECTION_TIMEOUT, &mbox};
    xtimer_t user_timeout;
    cb_arg_t user_timeout_arg = {MSG_TYPE_USER_SPEC_TIMEOUT, &mbox};
    xtimer_t probe_timeout;
    cb_arg_t probe_timeout_arg = {MSG_TYPE_PROBE_TIMEOUT, &mbox};
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

    /* Setup messaging */
    _fsm_set_mbox(tcb, &mbox);

    /* Setup connection timeout: Put timeout message in tcb's mbox on expiration */
    _setup_timeout(&(tcb->timer_misc), CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                   _cb_mbox_put_msg, &connection_timeout_arg);

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
            _setup_timeout(&probe_timeout, probe_timeout_duration_us, _cb_mbox_put_msg,
                           &probe_timeout_arg);
        }

        /* Try to send data in case there nothing has been sent and we are not probing */
        if (ret == 0 && !probing_mode) {
            ret = _fsm(tcb, FSM_EVENT_CALL_SEND, NULL, (void *) data, len);
        }

        /* Wait for responses */
        mbox_get(&mbox, &msg);
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

                /* Boundary check for time interval between probes */
                if (probe_timeout_duration_us < CONFIG_GNRC_TCP_PROBE_LOWER_BOUND) {
                    probe_timeout_duration_us = CONFIG_GNRC_TCP_PROBE_LOWER_BOUND;
                }
                else if (probe_timeout_duration_us > CONFIG_GNRC_TCP_PROBE_UPPER_BOUND) {
                    probe_timeout_duration_us = CONFIG_GNRC_TCP_PROBE_UPPER_BOUND;
                }
                break;

            case MSG_TYPE_NOTIFY_USER:
                DEBUG("gnrc_tcp.c : gnrc_tcp_send() : NOTIFY_USER\n");

                /* Connection is alive: Reset Connection Timeout */
                _setup_timeout(&(tcb->timer_misc), CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
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
    _fsm_set_mbox(tcb, NULL);
    xtimer_remove(&(tcb->timer_misc));
    xtimer_remove(&probe_timeout);
    xtimer_remove(&user_timeout);
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

ssize_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb, void *data, const size_t max_len,
                      const uint32_t timeout_duration_us)
{
    assert(tcb != NULL);
    assert(data != NULL);

    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    cb_arg_t connection_timeout_arg = {MSG_TYPE_CONNECTION_TIMEOUT, &mbox};
    xtimer_t user_timeout;
    cb_arg_t user_timeout_arg = {MSG_TYPE_USER_SPEC_TIMEOUT, &mbox};
    ssize_t ret = 0;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Check if connection is in a valid state */
    if (tcb->state != FSM_STATE_ESTABLISHED && tcb->state != FSM_STATE_FIN_WAIT_1 &&
        tcb->state != FSM_STATE_FIN_WAIT_2 && tcb->state != FSM_STATE_CLOSE_WAIT) {
        mutex_unlock(&(tcb->function_lock));
        return -ENOTCONN;
    }

    /* If FIN was received (CLOSE_WAIT), no further data can be received. */
    /* Copy received data into given buffer and return number of bytes. Can be zero. */
    if (tcb->state == FSM_STATE_CLOSE_WAIT) {
        ret = _fsm(tcb, FSM_EVENT_CALL_RECV, NULL, data, max_len);
        mutex_unlock(&(tcb->function_lock));
        return ret;
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

    /* Setup messaging */
    _fsm_set_mbox(tcb, &mbox);

    /* Setup connection timeout: Put timeout message in tcb's mbox on expiration */
    _setup_timeout(&(tcb->timer_misc), CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                   _cb_mbox_put_msg, &connection_timeout_arg);
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

        /* If FIN was received (CLOSE_WAIT), no further data can be received. Leave event loop */
        if (tcb->state == FSM_STATE_CLOSE_WAIT) {
            break;
        }

        /* If there was no data: Wait for next packet or until the timeout fires */
        if (ret <= 0) {
            mbox_get(&mbox, &msg);
            switch (msg.type) {
                case MSG_TYPE_CONNECTION_TIMEOUT:
                    DEBUG("gnrc_tcp.c : gnrc_tcp_recv() : CONNECTION_TIMEOUT\n");
                    _fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                    ret = -ECONNABORTED;
                    break;

                case MSG_TYPE_USER_SPEC_TIMEOUT:
                    DEBUG("gnrc_tcp.c : gnrc_tcp_recv() : USER_SPEC_TIMEOUT\n");
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
    _fsm_set_mbox(tcb, NULL);
    xtimer_remove(&(tcb->timer_misc));
    xtimer_remove(&user_timeout);
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

void gnrc_tcp_close(gnrc_tcp_tcb_t *tcb)
{
    assert(tcb != NULL);

    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    cb_arg_t connection_timeout_arg = {MSG_TYPE_CONNECTION_TIMEOUT, &mbox};

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Return if connection is closed */
    if (tcb->state == FSM_STATE_CLOSED) {
        mutex_unlock(&(tcb->function_lock));
        return;
    }

    /* Setup messaging */
    _fsm_set_mbox(tcb, &mbox);

    /* Setup connection timeout: Put timeout message in tcb's mbox on expiration */
    _setup_timeout(&(tcb->timer_misc), CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                   _cb_mbox_put_msg, &connection_timeout_arg);

    /* Start connection teardown sequence */
    _fsm(tcb, FSM_EVENT_CALL_CLOSE, NULL, NULL, 0);

    /* Loop until the connection has been closed */
    while (tcb->state != FSM_STATE_CLOSED) {
        mbox_get(&mbox, &msg);
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
    _fsm_set_mbox(tcb, NULL);
    xtimer_remove(&(tcb->timer_misc));
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
        DEBUG("gnrc_tcp.c : gnrc_tcp_hdr_build() : No space left in packet buffer\n");
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
