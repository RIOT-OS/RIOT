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

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <utlist.h>

#include "evtimer.h"
#include "evtimer_mbox.h"
#include "mbox.h"
#include "net/af.h"
#include "net/tcp.h"
#include "net/gnrc.h"
#include "net/gnrc/tcp.h"
#include "include/gnrc_tcp_common.h"
#include "include/gnrc_tcp_fsm.h"
#include "include/gnrc_tcp_pkt.h"
#include "include/gnrc_tcp_eventloop.h"
#include "include/gnrc_tcp_rcvbuf.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define TCP_MSG_QUEUE_SIZE (1 << CONFIG_GNRC_TCP_MSG_QUEUE_SIZE_EXP)

/**
 * @brief Central MBOX evtimer used by gnrc_tcp
 */
static evtimer_t _tcp_mbox_timer;

static void _sched_mbox(evtimer_mbox_event_t *event, uint32_t offset,
                        uint16_t type, mbox_t *mbox)
{
    TCP_DEBUG_ENTER;
    event->event.offset = offset;
    event->msg.type = type;
    evtimer_add_mbox(&_tcp_mbox_timer, event, mbox);
    TCP_DEBUG_LEAVE;
}

static void _sched_connection_timeout(evtimer_mbox_event_t *event, mbox_t *mbox)
{
    TCP_DEBUG_ENTER;
    _sched_mbox(event, CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS,
                MSG_TYPE_CONNECTION_TIMEOUT, mbox);
    TCP_DEBUG_LEAVE;
}

static void _unsched_mbox(evtimer_mbox_event_t *event)
{
    TCP_DEBUG_ENTER;
    evtimer_del(&_tcp_mbox_timer, (evtimer_event_t *)event);
    TCP_DEBUG_LEAVE;
}

static void _close(gnrc_tcp_tcb_t *tcb)
{
    TCP_DEBUG_ENTER;

    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    _gnrc_tcp_fsm_state_t state = 0;

    /* Return if connection is closed */
    state = _gnrc_tcp_fsm_get_state(tcb);
    if (state == FSM_STATE_CLOSED) {
        TCP_DEBUG_LEAVE;
        return;
    }

    /* Setup messaging */
    _gnrc_tcp_fsm_set_mbox(tcb, &mbox);

    /* Setup connection timeout */
    _sched_connection_timeout(&tcb->event_misc, &mbox);

    /* Start connection teardown sequence */
    _gnrc_tcp_fsm(tcb, FSM_EVENT_CALL_CLOSE, NULL, NULL, 0);

    /* Loop until the connection has been closed */
    state = _gnrc_tcp_fsm_get_state(tcb);
    while ((state != FSM_STATE_CLOSED) && (state != FSM_STATE_LISTEN)) {
        mbox_get(&mbox, &msg);
        switch (msg.type) {
            case MSG_TYPE_CONNECTION_TIMEOUT:
                TCP_DEBUG_INFO("Received MSG_TYPE_CONNECTION_TIMEOUT.");
                _gnrc_tcp_fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                break;

            case MSG_TYPE_NOTIFY_USER:
                TCP_DEBUG_INFO("Received MSG_TYPE_NOTIFY_USER.");
                break;

            default:
                TCP_DEBUG_ERROR("Received unexpected message.");
        }
        state = _gnrc_tcp_fsm_get_state(tcb);
    }

    /* Cleanup */
    _gnrc_tcp_fsm_set_mbox(tcb, NULL);
    _unsched_mbox(&tcb->event_misc);
    TCP_DEBUG_LEAVE;
}


static void _abort(gnrc_tcp_tcb_t *tcb)
{
    TCP_DEBUG_ENTER;
    if (_gnrc_tcp_fsm_get_state(tcb) != FSM_STATE_CLOSED) {
        _gnrc_tcp_fsm(tcb, FSM_EVENT_CALL_ABORT, NULL, NULL, 0);
    }
    TCP_DEBUG_LEAVE;
}

/* External GNRC TCP API */
int gnrc_tcp_ep_init(gnrc_tcp_ep_t *ep, int family, const uint8_t *addr, size_t addr_size,
                     uint16_t port, uint16_t netif)
{
    TCP_DEBUG_ENTER;
#ifdef MODULE_GNRC_IPV6
    if (family != AF_INET6) {
        TCP_DEBUG_ERROR("-EAFNOSUPPORT: Parameter family is not AF_INET6.")
        TCP_DEBUG_LEAVE;
        return -EAFNOSUPPORT;
    }

    if (addr == NULL && addr_size == 0) {
        ipv6_addr_set_unspecified((ipv6_addr_t *) ep->addr.ipv6);
    }
    else if (addr_size == sizeof(ipv6_addr_t)) {
        memcpy(ep->addr.ipv6, addr, sizeof(ipv6_addr_t));
    }
    else {
        TCP_DEBUG_ERROR("-EINVAL: Parameter addr is invalid.")
        TCP_DEBUG_LEAVE;
        return -EINVAL;
    }
#else
    /* Suppress Compiler Warnings */
    (void) addr;
    (void) addr_size;
    TCP_DEBUG_ERROR("-EAFNOSUPPORT: No network layer configured.")
    TCP_DEBUG_LEAVE;
    return -EAFNOSUPPORT;
#endif

    ep->family = family;
    ep->port = port;
    ep->netif = netif;
    TCP_DEBUG_LEAVE;
    return 0;
}

int gnrc_tcp_ep_from_str(gnrc_tcp_ep_t *ep, const char *str)
{
    TCP_DEBUG_ENTER;
    assert(str);

    unsigned port = 0;
    unsigned netif = 0;

    /* Examine given string */
    char *addr_begin = strchr(str, '[');
    char *addr_end = strchr(str, ']');

    /* 1) Ensure that str contains a single pair of brackets */
    if (!addr_begin || !addr_end || strchr(addr_begin + 1, '[') || strchr(addr_end + 1, ']')) {
        TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
        TCP_DEBUG_LEAVE;
        return -EINVAL;
    }
    /* 2) Ensure that the first character is the opening bracket */
    else if (addr_begin != str) {
        TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
        TCP_DEBUG_LEAVE;
        return -EINVAL;
    }

    /* 3) Examine optional port number */
    char *port_begin = strchr(addr_end, ':');
    if (port_begin) {
        /* 3.1) Ensure that there are characters left to parse after ':'. */
        if (*(++port_begin) == '\0') {
            TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
            TCP_DEBUG_LEAVE;
            return -EINVAL;
        }

        /* 3.2) Ensure that port is a number (atol, does not report errors) */
        for (char *ptr = port_begin; *ptr; ++ptr) {
            if ((*ptr < '0') || ('9' < *ptr)) {
                TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
                TCP_DEBUG_LEAVE;
                return -EINVAL;
            }
        }

        /* 3.3) Read and verify that given number port is within range */
        port = atol(port_begin);
        if (port > 0xFFFF) {
            TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
            TCP_DEBUG_LEAVE;
            return -EINVAL;
        }
    }

    /* 4) Examine optional interface identifier. */
    char *if_begin = strchr(str, '%');
    if (if_begin) {
        /* 4.1) Ensure that the identifier is not empty and within brackets. */
        if (addr_end <= (++if_begin)) {
            TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
            TCP_DEBUG_LEAVE;
            return -EINVAL;
        }

        /* 4.2) Ensure that the identifier is a number (atol, does not report errors) */
        for (char *ptr = if_begin; ptr != addr_end; ++ptr) {
            if ((*ptr < '0') || ('9' < *ptr)) {
                TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
                TCP_DEBUG_LEAVE;
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
        TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
        TCP_DEBUG_LEAVE;
        return -EINVAL;
    }

    /* 5.2) Try to read address into endpoint. */
    if (ipv6_addr_from_str((ipv6_addr_t *) ep->addr.ipv6, tmp) == NULL) {
        TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
        TCP_DEBUG_LEAVE;
        return -EINVAL;
    }
    ep->family = AF_INET6;
#else
    /* Suppress Compiler Warnings */
    (void) port;
    (void) netif;
    TCP_DEBUG_ERROR("-EINVAL: Invalid address string.");
    TCP_DEBUG_LEAVE;
    return -EINVAL;
#endif

    ep->port = (uint16_t) port;
    ep->netif = (uint16_t) netif;
    TCP_DEBUG_LEAVE;
    return 0;
}

int gnrc_tcp_init(void)
{
    TCP_DEBUG_ENTER;
    /* Initialize receive buffers */
    _gnrc_tcp_rcvbuf_init();

    /* Initialize timers */
    evtimer_init_mbox(&_tcp_mbox_timer);

    /* Start TCP processing thread */
    kernel_pid_t pid = _gnrc_tcp_eventloop_init();
    TCP_DEBUG_LEAVE;
    return pid;
}

void gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb)
{
    TCP_DEBUG_ENTER;
    assert(tcb != NULL);

    memset(tcb, 0, sizeof(gnrc_tcp_tcb_t));
#ifdef MODULE_GNRC_IPV6
    tcb->address_family = AF_INET6;
#else
    TCP_DEBUG_ERROR("Missing network layer. Add module to makefile.");
#endif
    tcb->rtt_var = RTO_UNINITIALIZED;
    tcb->srtt = RTO_UNINITIALIZED;
    tcb->rto = RTO_UNINITIALIZED;
    mutex_init(&(tcb->fsm_lock));
    mutex_init(&(tcb->function_lock));
    TCP_DEBUG_LEAVE;
}

void gnrc_tcp_tcb_queue_init(gnrc_tcp_tcb_queue_t *queue)
{
    TCP_DEBUG_ENTER;
    assert(queue != NULL);

    mutex_init(&queue->lock);
    queue->tcbs = NULL;
    queue->tcbs_len = 0;
    TCP_DEBUG_LEAVE;
}

int gnrc_tcp_open(gnrc_tcp_tcb_t *tcb, const gnrc_tcp_ep_t *remote, uint16_t local_port)
{
    /* Sanity checking */
    TCP_DEBUG_ENTER;
    assert(tcb != NULL);
    assert(remote != NULL);
    assert(remote->port != PORT_UNSPEC);

    /* Verify remote ep */
#ifdef MODULE_GNRC_IPV6
    if (remote->family != AF_INET6) {
        TCP_DEBUG_ERROR("-EAFNOSUPPORT: remote AF-Family not supported.");
        TCP_DEBUG_LEAVE;
        return -EAFNOSUPPORT;
    }
#else
    TCP_DEBUG_ERROR("-EAFNOSUPPORT: AF-Family not supported.");
    TCP_DEBUG_LEAVE;
    return -EAFNOSUPPORT;
#endif

    /* Protect TCB against usage in other TCP functions */
    mutex_lock(&(tcb->function_lock));

    /* Check if AF-Family for target address matches internally used AF-Family */
    if (remote->family != tcb->address_family) {
        mutex_unlock(&(tcb->function_lock));
        TCP_DEBUG_ERROR("-EINVAL: local and remote AF-Family don't match.");
        TCP_DEBUG_LEAVE;
        return -EINVAL;
    }

    /* TCB is already connected: Return -EISCONN */
    _gnrc_tcp_fsm_state_t state = _gnrc_tcp_fsm_get_state(tcb);
    if (state != FSM_STATE_CLOSED) {
        mutex_unlock(&(tcb->function_lock));
        TCP_DEBUG_ERROR("-EISCONN: TCB already connected.");
        TCP_DEBUG_LEAVE;
        return -EISCONN;
    }

    /* Setup messaging */
    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    _gnrc_tcp_fsm_set_mbox(tcb, &mbox);

    /* Parse target address and port number into TCB */
#ifdef MODULE_GNRC_IPV6
    if (tcb->address_family == AF_INET6) {

        /* Store Address information in TCB */
        if (memcpy(tcb->peer_addr, remote->addr.ipv6, sizeof(tcb->peer_addr)) == NULL) {
            TCP_DEBUG_ERROR("-EINVAL: Invalid peer address.");
            TCP_DEBUG_LEAVE;
            return -EINVAL;
        }
        tcb->ll_iface = remote->netif;
    }
#endif

    /* Assign port numbers, verification happens during connection establishment. */
    tcb->local_port = local_port;
    tcb->peer_port = remote->port;

    /* Setup connection timeout */
    _sched_connection_timeout(&tcb->event_misc, &mbox);

    /* Call FSM with event: CALL_OPEN */
    int ret = _gnrc_tcp_fsm(tcb, FSM_EVENT_CALL_OPEN, NULL, NULL, 0);
    if (ret == -ENOMEM) {
        TCP_DEBUG_ERROR("-ENOMEM: All receive buffers are in use.");
    }
    else if (ret == -EADDRINUSE) {
        TCP_DEBUG_ERROR("-EADDRINUSE: local_port is already in use.");
    }

    /* Wait until a connection was established or closed */
    state = _gnrc_tcp_fsm_get_state(tcb);
    while (ret >= 0 && state != FSM_STATE_CLOSED && state != FSM_STATE_ESTABLISHED &&
           state != FSM_STATE_CLOSE_WAIT) {
        mbox_get(&mbox, &msg);
        switch (msg.type) {
            case MSG_TYPE_NOTIFY_USER:
                TCP_DEBUG_INFO("Received MSG_TYPE_NOTIFY_USER.");
                break;

            case MSG_TYPE_CONNECTION_TIMEOUT:
                TCP_DEBUG_INFO("Received MSG_TYPE_CONNECTION_TIMEOUT.");
                _gnrc_tcp_fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                TCP_DEBUG_ERROR("-ETIMEDOUT: Connection timed out.");
                ret = -ETIMEDOUT;
                break;

            default:
                TCP_DEBUG_ERROR("Received unexpected message.");
        }
        state = _gnrc_tcp_fsm_get_state(tcb);
    }

    /* Cleanup */
    _unsched_mbox(&tcb->event_misc);
    _gnrc_tcp_fsm_set_mbox(tcb, NULL);
    state = _gnrc_tcp_fsm_get_state(tcb);
    if (state == FSM_STATE_CLOSED && ret == 0) {
        TCP_DEBUG_ERROR("-ECONNREFUSED: Connection was refused by peer.");
        ret = -ECONNREFUSED;
    }
    mutex_unlock(&(tcb->function_lock));
    TCP_DEBUG_LEAVE;
    return ret;
}

int gnrc_tcp_listen(gnrc_tcp_tcb_queue_t *queue, gnrc_tcp_tcb_t *tcbs, size_t tcbs_len,
                    const gnrc_tcp_ep_t *local)
{
    /* Sanity checks */
    assert(queue != NULL);
    assert(tcbs != NULL);
    assert(tcbs_len > 0);
    assert(local != NULL);
    assert(local->port != PORT_UNSPEC);

    /* Verfiy given endpoint */
#ifdef MODULE_GNRC_IPV6
    if (local->family != AF_INET6) {
        TCP_DEBUG_ERROR("-EAFNOSUPPORT: AF-Family not supported.");
        TCP_DEBUG_LEAVE;
        return -EAFNOSUPPORT;
    }
#else
    TCP_DEBUG_ERROR("-EAFNOSUPPORT: AF-Family not supported.");
    TCP_DEBUG_LEAVE;
    return -EAFNOSUPPORT;
#endif

    /* Protect TCP Data structures against usage in other TCP function*/
    mutex_lock(&queue->lock);

    for (size_t i = 0; i < tcbs_len; ++i) {
        mutex_lock(&(tcbs[i].function_lock));
    }

    /* Setup and verify each TCB */
    int ret = 0;
    for (size_t i = 0; i < tcbs_len; ++i) {
        gnrc_tcp_tcb_t *tcb = &(tcbs[i]);

        /* Verify current TCB */
        if (tcb->address_family != local->family) {
            TCP_DEBUG_ERROR("-EINVAL: local and remote AF-Family don't match.");
            ret = -EINVAL;
        }
        else if (_gnrc_tcp_fsm_get_state(tcb) != FSM_STATE_CLOSED) {
            TCP_DEBUG_ERROR("-EISCONN: tcb is already connected.");
            ret = -EISCONN;
        }

        /* Setup TCB for incoming connections attempts */
        if (!ret)
        {
#ifdef MODULE_GNRC_IPV6
            if (tcb->address_family == AF_INET6) {
                memcpy(tcb->local_addr, local->addr.ipv6, sizeof(tcb->local_addr));

                if (ipv6_addr_is_unspecified((ipv6_addr_t *) tcb->local_addr)) {
                    tcb->status |= STATUS_ALLOW_ANY_ADDR;
                }
            }
#endif
            tcb->local_port = local->port;
            tcb->status |= STATUS_LISTENING;

            /* Open connection */
            ret = _gnrc_tcp_fsm(tcb, FSM_EVENT_CALL_OPEN, NULL, NULL, 0);
        }

        /* If anything goes wrong, discard all potentially opened connections. */
        if (ret) {
            for (size_t j = 0; j <= i; ++j) {
                tcb->status &= ~(STATUS_LISTENING);
                _abort(tcb);
            }
            break;
        }
    }

    /* If everything went well: setup queue and unlock all TCBs */
    if (!ret) {
        queue->tcbs = tcbs;
        queue->tcbs_len = tcbs_len;
    }

    for (size_t i = 0; i < tcbs_len; ++i) {
        mutex_unlock(&(tcbs[i].function_lock));
    }
    mutex_unlock(&queue->lock);
    TCP_DEBUG_LEAVE;
    return ret;
}

int gnrc_tcp_accept(gnrc_tcp_tcb_queue_t *queue, gnrc_tcp_tcb_t **tcb,
                    const uint32_t user_timeout_duration_ms)
{
    TCP_DEBUG_ENTER;
    assert(queue != NULL);
    assert(tcb != NULL);

    int ret = 0;
    int avail_tcbs = 0;
    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    evtimer_mbox_event_t event_user_timeout;
    gnrc_tcp_tcb_t *tmp = NULL;
    _gnrc_tcp_fsm_state_t state = 0;

    /* Search for non-accepted established connections */
    *tcb = NULL;
    mutex_lock(&queue->lock);
    for (size_t i = 0; i < queue->tcbs_len; ++i) {
        tmp = &(queue->tcbs[i]);

        if (tmp->status & STATUS_ACCEPTED) {
            continue;
        }

        state = _gnrc_tcp_fsm_get_state(tmp);
        if (state == FSM_STATE_ESTABLISHED || state == FSM_STATE_CLOSE_WAIT) {
            tmp->status |= STATUS_ACCEPTED;
            *tcb = tmp;
            break;
        }
        ++avail_tcbs;
    }

    /* Return if a connection was found, queue is not listening, accept was called as non-blocking
     * or all TCBs were already accepted.
     */
    if ((*tcb) || (queue->tcbs == NULL) || (user_timeout_duration_ms == 0) ||
       (avail_tcbs == 0)) {
        if (*tcb) {
            TCP_DEBUG_INFO("Accepting connection.");
            ret = 0;
        }
        else if (queue->tcbs == NULL) {
            TCP_DEBUG_ERROR("-EINVAL: Queue is not listening.");
            ret = -EINVAL;
        }
        else if (avail_tcbs == 0) {
            TCP_DEBUG_ERROR("-ENOMEM: All TCBs are currently accepted.");
            ret = -ENOMEM;
        }
        else if (user_timeout_duration_ms == 0) {
            TCP_DEBUG_ERROR("-EAGAIN: Would block. Try again.");
            ret = -EAGAIN;
        }
        mutex_unlock(&queue->lock);
        TCP_DEBUG_LEAVE;
        return ret;
    }

    /* Setup TCBs for message exchange with the FSM */
    for (size_t i = 0; i < queue->tcbs_len; ++i) {
        tmp = &(queue->tcbs[i]);

        /* Setup only not accepted TCBS */
        if (!(tmp->status & STATUS_ACCEPTED)) {
            mutex_lock(&(tmp->function_lock));
            tmp->status |= STATUS_LOCKED;
            _gnrc_tcp_fsm_set_mbox(tmp, &mbox);
        }
    }

    /* Setup User specified Timeout */
    if (user_timeout_duration_ms != GNRC_TCP_NO_TIMEOUT) {
        _sched_mbox(&event_user_timeout, user_timeout_duration_ms,
                    MSG_TYPE_USER_SPEC_TIMEOUT, &mbox);
    }

    /* Wait until a connection was established */
    while (ret >= 0 && *tcb == NULL) {
        mbox_get(&mbox, &msg);
        switch (msg.type) {
            case MSG_TYPE_NOTIFY_USER:
                TCP_DEBUG_INFO("Received MSG_TYPE_NOTIFY_USER.");

                tmp = (gnrc_tcp_tcb_t *) msg.content.ptr;
                state = _gnrc_tcp_fsm_get_state(tmp);
                if (state == FSM_STATE_ESTABLISHED || state == FSM_STATE_CLOSE_WAIT) {
                    tmp->status |= STATUS_ACCEPTED;
                    *tcb = tmp;
                }
                break;

            case MSG_TYPE_USER_SPEC_TIMEOUT:
                TCP_DEBUG_INFO("Received MSG_TYPE_USER_SPEC_TIMEOUT.");
                TCP_DEBUG_ERROR("-ETIMEDOUT: User specified timeout expired.");
                ret = -ETIMEDOUT;
                break;

            default:
                TCP_DEBUG_ERROR("Received unexpected message.");
        }
    }

    /* Cleanup */
    _unsched_mbox(&event_user_timeout);
    for (size_t i = 0; i < queue->tcbs_len; ++i) {
        tmp = &(queue->tcbs[i]);

        if (tmp->status & STATUS_LOCKED) {
            tmp->status &= ~(STATUS_LOCKED);
            _gnrc_tcp_fsm_set_mbox(tmp, NULL);
            mutex_unlock(&(tmp->function_lock));
        }
    }
    mutex_unlock(&queue->lock);
    TCP_DEBUG_LEAVE;
    return ret;
}

ssize_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, const void *data, const size_t len,
                      const uint32_t timeout_duration_ms)
{
    TCP_DEBUG_ENTER;
    assert(tcb != NULL);
    assert(data != NULL);

    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    evtimer_mbox_event_t event_user_timeout;
    evtimer_mbox_event_t event_probe_timeout;
    uint32_t probe_timeout_duration_ms = 0;
    ssize_t ret = 0;
    bool probing_mode = false;
    _gnrc_tcp_fsm_state_t state = 0;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Check if connection is in a valid state */
    state = _gnrc_tcp_fsm_get_state(tcb);
    if (state != FSM_STATE_ESTABLISHED && state != FSM_STATE_CLOSE_WAIT) {
        mutex_unlock(&(tcb->function_lock));
        TCP_DEBUG_ERROR("-ENOTCONN: TCB is not connected.");
        TCP_DEBUG_LEAVE;
        return -ENOTCONN;
    }

    /* Setup messaging */
    _gnrc_tcp_fsm_set_mbox(tcb, &mbox);

    /* Setup connection timeout */
    _sched_connection_timeout(&tcb->event_misc, &mbox);

    if ((0 < timeout_duration_ms) && (timeout_duration_ms != GNRC_TCP_NO_TIMEOUT)) {
        _sched_mbox(&event_user_timeout, timeout_duration_ms,
                    MSG_TYPE_USER_SPEC_TIMEOUT, &mbox);
    }

    /* Loop until something was sent and acked */
    while (ret == 0 || tcb->pkt_retransmit != NULL) {
        state = _gnrc_tcp_fsm_get_state(tcb);

        /* Check if the connections state is closed. If so, a reset was received */
        if (state == FSM_STATE_CLOSED) {
            TCP_DEBUG_ERROR("-ECONNRESET: Connection was reset by peer.");
            ret = -ECONNRESET;
            break;
        }

        /* If the send window is closed: Setup Probing */
        if (tcb->snd_wnd <= 0) {
            /* If this is the first probe: Setup probing duration */
            if (!probing_mode) {
                probing_mode = true;
                probe_timeout_duration_ms = tcb->rto;
            }
            /* Setup probe timeout */
            _unsched_mbox(&event_probe_timeout);
            _sched_mbox(&event_probe_timeout, probe_timeout_duration_ms,
                        MSG_TYPE_PROBE_TIMEOUT, &mbox);
        }

        /* Try to send data in case there nothing has been sent and we are not probing */
        if (ret == 0 && !probing_mode) {
            ret = _gnrc_tcp_fsm(tcb, FSM_EVENT_CALL_SEND, NULL, (void *) data, len);
        }

        /* Wait for responses */
        mbox_get(&mbox, &msg);
        switch (msg.type) {
            case MSG_TYPE_CONNECTION_TIMEOUT:
                TCP_DEBUG_INFO("Received MSG_TYPE_CONNECTION_TIMEOUT.");
                _gnrc_tcp_fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                TCP_DEBUG_ERROR("-ECONNABORTED: Connection timed out.");
                ret = -ECONNABORTED;
                break;

            case MSG_TYPE_USER_SPEC_TIMEOUT:
                TCP_DEBUG_INFO("Received MSG_TYPE_USER_SPEC_TIMEOUT.");
                _gnrc_tcp_fsm(tcb, FSM_EVENT_CLEAR_RETRANSMIT, NULL, NULL, 0);
                TCP_DEBUG_ERROR("-ETIMEDOUT: User specified timeout expired.");
                ret = -ETIMEDOUT;
                break;

            case MSG_TYPE_PROBE_TIMEOUT:
                TCP_DEBUG_INFO("Received MSG_TYPE_PROBE_TIMEOUT.");
                /* Send probe */
                _gnrc_tcp_fsm(tcb, FSM_EVENT_SEND_PROBE, NULL, NULL, 0);
                probe_timeout_duration_ms += probe_timeout_duration_ms;

                /* Boundary check for time interval between probes */
                if (probe_timeout_duration_ms < CONFIG_GNRC_TCP_PROBE_LOWER_BOUND_MS) {
                    probe_timeout_duration_ms = CONFIG_GNRC_TCP_PROBE_LOWER_BOUND_MS;
                }
                else if (probe_timeout_duration_ms > CONFIG_GNRC_TCP_PROBE_UPPER_BOUND_MS) {
                    probe_timeout_duration_ms = CONFIG_GNRC_TCP_PROBE_UPPER_BOUND_MS;
                }
                break;

            case MSG_TYPE_NOTIFY_USER:
                TCP_DEBUG_INFO("Received MSG_TYPE_NOTIFY_USER.");

                /* Connection is alive: Reset Connection Timeout */
                _unsched_mbox(&tcb->event_misc);
                _sched_connection_timeout(&tcb->event_misc, &mbox);

                /* If the window re-opened and we are probing: Stop it */
                if (tcb->snd_wnd > 0 && probing_mode) {
                    probing_mode = false;
                    _unsched_mbox(&event_probe_timeout);
                }
                break;

            default:
                TCP_DEBUG_ERROR("Received unexpected message.");
        }
    }

    /* Cleanup */
    _gnrc_tcp_fsm_set_mbox(tcb, NULL);
    _unsched_mbox(&tcb->event_misc);
    _unsched_mbox(&event_probe_timeout);
    _unsched_mbox(&event_user_timeout);
    mutex_unlock(&(tcb->function_lock));
    TCP_DEBUG_LEAVE;
    return ret;
}

ssize_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb, void *data, const size_t max_len,
                      const uint32_t timeout_duration_ms)
{
    TCP_DEBUG_ENTER;
    assert(tcb != NULL);
    assert(data != NULL);

    msg_t msg;
    msg_t msg_queue[TCP_MSG_QUEUE_SIZE];
    mbox_t mbox = MBOX_INIT(msg_queue, TCP_MSG_QUEUE_SIZE);
    evtimer_mbox_event_t event_user_timeout;
    ssize_t ret = 0;
    _gnrc_tcp_fsm_state_t state = 0;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Check if connection is in a valid state */
    state = _gnrc_tcp_fsm_get_state(tcb);
    if (state != FSM_STATE_ESTABLISHED && state != FSM_STATE_FIN_WAIT_1 &&
        state != FSM_STATE_FIN_WAIT_2 && state != FSM_STATE_CLOSE_WAIT) {
        mutex_unlock(&(tcb->function_lock));
        TCP_DEBUG_ERROR("-ENOTCONN: TCB is not connected.");
        TCP_DEBUG_LEAVE;
        return -ENOTCONN;
    }

    /* If FIN was received (CLOSE_WAIT), no further data can be received. */
    /* Copy received data into given buffer and return number of bytes. Can be zero. */
    if (state == FSM_STATE_CLOSE_WAIT) {
        ret = _gnrc_tcp_fsm(tcb, FSM_EVENT_CALL_RECV, NULL, data, max_len);
        mutex_unlock(&(tcb->function_lock));
        TCP_DEBUG_LEAVE;
        return ret;
    }

    /* If this call is non-blocking (timeout_duration_ms == 0): Try to read data and return */
    if (timeout_duration_ms == 0) {
        ret = _gnrc_tcp_fsm(tcb, FSM_EVENT_CALL_RECV, NULL, data, max_len);
        if (ret == 0) {
            TCP_DEBUG_ERROR("-EAGAIN: Not data available, try later again.");
            ret = -EAGAIN;
        }
        mutex_unlock(&(tcb->function_lock));
        TCP_DEBUG_LEAVE;
        return ret;
    }

    /* Setup messaging */
    _gnrc_tcp_fsm_set_mbox(tcb, &mbox);

    /* Setup connection timeout */
    _sched_connection_timeout(&tcb->event_misc, &mbox);

    if (timeout_duration_ms != GNRC_TCP_NO_TIMEOUT) {
        _sched_mbox(&event_user_timeout, timeout_duration_ms,
                    MSG_TYPE_USER_SPEC_TIMEOUT, &mbox);
    }

    /* Processing loop */
    while (ret == 0) {
        /* Check if the connections state is closed. If so, a reset was received */
        state = _gnrc_tcp_fsm_get_state(tcb);
        if (state == FSM_STATE_CLOSED) {
            TCP_DEBUG_ERROR("-ECONNRESET: Connection was reset by peer.");
            ret = -ECONNRESET;
            break;
        }

        /* Try to read available data */
        ret = _gnrc_tcp_fsm(tcb, FSM_EVENT_CALL_RECV, NULL, data, max_len);

        /* If FIN was received (CLOSE_WAIT), no further data can be received. Leave event loop */
        if (state == FSM_STATE_CLOSE_WAIT) {
            break;
        }

        /* If there was no data: Wait for next packet or until the timeout fires */
        if (ret <= 0) {
            mbox_get(&mbox, &msg);
            switch (msg.type) {
                case MSG_TYPE_CONNECTION_TIMEOUT:
                    TCP_DEBUG_INFO("Received MSG_TYPE_CONNECTION_TIMEOUT.");
                    _gnrc_tcp_fsm(tcb, FSM_EVENT_TIMEOUT_CONNECTION, NULL, NULL, 0);
                    TCP_DEBUG_ERROR("-ECONNABORTED: Connection timed out.");
                    ret = -ECONNABORTED;
                    break;

                case MSG_TYPE_USER_SPEC_TIMEOUT:
                    TCP_DEBUG_INFO("Received MSG_TYPE_USER_SPEC_TIMEOUT.");
                    _gnrc_tcp_fsm(tcb, FSM_EVENT_CLEAR_RETRANSMIT, NULL, NULL, 0);
                    TCP_DEBUG_ERROR("-ETIMEDOUT: User specified timeout expired.");
                    ret = -ETIMEDOUT;
                    break;

                case MSG_TYPE_NOTIFY_USER:
                    TCP_DEBUG_INFO("Received MSG_TYPE_NOTIFY_USER.");
                    break;

                default:
                    TCP_DEBUG_ERROR("Received unexpected message.");
            }
        }
    }

    /* Cleanup */
    _gnrc_tcp_fsm_set_mbox(tcb, NULL);
    _unsched_mbox(&tcb->event_misc);
    _unsched_mbox(&event_user_timeout);
    mutex_unlock(&(tcb->function_lock));
    TCP_DEBUG_LEAVE;
    return ret;
}

void gnrc_tcp_close(gnrc_tcp_tcb_t *tcb)
{
    TCP_DEBUG_ENTER;
    assert(tcb != NULL);

    mutex_lock(&(tcb->function_lock));
    _close(tcb);
    mutex_unlock(&(tcb->function_lock));

    TCP_DEBUG_LEAVE;
}

void gnrc_tcp_abort(gnrc_tcp_tcb_t *tcb)
{
    TCP_DEBUG_ENTER;
    assert(tcb != NULL);

    mutex_lock(&(tcb->function_lock));
    _abort(tcb);
    mutex_unlock(&(tcb->function_lock));

    TCP_DEBUG_LEAVE;
}

void gnrc_tcp_stop_listen(gnrc_tcp_tcb_queue_t *queue)
{
    TCP_DEBUG_ENTER;
    assert(queue != NULL);

    /* Close all connections associated with the given queue */
    mutex_lock(&(queue->lock));
    for (size_t i = 0; i < queue->tcbs_len; ++i) {
        gnrc_tcp_tcb_t *tcb = &(queue->tcbs[i]);
        mutex_lock(&(tcb->function_lock));

        /* Clear LISTENING status causing re-opening on close */
        tcb->status &= ~(STATUS_LISTENING);
        _close(tcb);

        mutex_unlock(&(tcb->function_lock));
    }

    /* Cleanup */
    queue->tcbs = NULL;
    queue->tcbs_len = 0;
    mutex_unlock(&(queue->lock));
    TCP_DEBUG_LEAVE;
}

int gnrc_tcp_get_local(gnrc_tcp_tcb_t *tcb, gnrc_tcp_ep_t *ep)
{
    TCP_DEBUG_ENTER;
    assert(tcb != NULL);
    assert(ep != NULL);

    int ret = 0;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));
    _gnrc_tcp_fsm_state_t state =_gnrc_tcp_fsm_get_state(tcb);

    /* Check if connection is established */
    if ((state == FSM_STATE_ESTABLISHED) || (state == FSM_STATE_CLOSE_WAIT)) {
        /* Construct endpoint from connection parameters */
        ep->family = tcb->address_family;
        ep->port = tcb->local_port;
        ep->netif = tcb->ll_iface;
#ifdef MODULE_GNRC_IPV6
        if (ep->family == AF_INET6) {
            memcpy(ep->addr.ipv6, tcb->local_addr, sizeof(ep->addr.ipv6));
        }
#endif
    } else {
        TCP_DEBUG_ERROR("-EADDRNOTAVAIL: TCB is not connected.");
        ret = -EADDRNOTAVAIL;
    }

    mutex_unlock(&(tcb->function_lock));
    TCP_DEBUG_LEAVE;
    return ret;
}

int gnrc_tcp_get_remote(gnrc_tcp_tcb_t *tcb, gnrc_tcp_ep_t *ep)
{
    TCP_DEBUG_ENTER;
    assert(tcb != NULL);
    assert(ep != NULL);

    int ret = 0;

    /* Lock the TCB for this function call */
    mutex_lock(&(tcb->function_lock));
    _gnrc_tcp_fsm_state_t state =_gnrc_tcp_fsm_get_state(tcb);

    /* Check if connection is established */
    if ((state == FSM_STATE_ESTABLISHED) || (state == FSM_STATE_CLOSE_WAIT)) {
        /* Construct endpoint from connection parameters */
        ep->family = tcb->address_family;
        ep->port = tcb->peer_port;
        ep->netif = 0;
#ifdef MODULE_GNRC_IPV6
        if (ep->family == AF_INET6) {
            memcpy(ep->addr.ipv6, tcb->peer_addr, sizeof(ep->addr.ipv6));
        }
#endif
    } else {
        TCP_DEBUG_ERROR("-ENOTCONN: TCB is not connected.");
        ret = -ENOTCONN;
    }

    mutex_unlock(&(tcb->function_lock));
    TCP_DEBUG_LEAVE;
    return ret;
}

int gnrc_tcp_queue_get_local(gnrc_tcp_tcb_queue_t *queue, gnrc_tcp_ep_t *ep)
{
    TCP_DEBUG_ENTER;
    assert(queue != NULL);
    assert(ep != NULL);

    int ret = 0;

    /* Lock the TCB queue for this function call */
    mutex_lock(&(queue->lock));

    /* Check if queue has associated TCBs */
    if (queue->tcbs) {
        /* There are listening TCBs: Construct ep from first TCB. */
        gnrc_tcp_tcb_t *tcb = queue->tcbs;
        mutex_lock(&(tcb->function_lock));

        /* Construct endpoint from tcbs connection parameters */
        ep->family = tcb->address_family;
        ep->port = tcb->local_port;
        ep->netif = 0;
#ifdef MODULE_GNRC_IPV6
        if (ep->family == AF_INET6) {
            if (tcb->status & STATUS_ALLOW_ANY_ADDR) {
                ipv6_addr_set_unspecified((ipv6_addr_t *) ep->addr.ipv6);
            } else {
                memcpy(ep->addr.ipv6, tcb->local_addr, sizeof(ep->addr.ipv6));
            }
        }
#endif
        mutex_unlock(&(tcb->function_lock));
    } else {
        TCP_DEBUG_ERROR("-EADDRNOTAVAIL: queue was never listening.");
        ret = -EADDRNOTAVAIL;
    }

    mutex_unlock(&(queue->lock));
    TCP_DEBUG_LEAVE;
    return ret;
}

int gnrc_tcp_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr)
{
    TCP_DEBUG_ENTER;
    uint16_t csum;

    if ((hdr == NULL) || (pseudo_hdr == NULL)) {
        TCP_DEBUG_ERROR("-EFAULT: hdr or pseudo_hdr is NULL.");
        TCP_DEBUG_LEAVE;
        return -EFAULT;
    }
    if (hdr->type != GNRC_NETTYPE_TCP) {
        TCP_DEBUG_ERROR("-EBADMSG: Variable hdr is no TCP header.");
        TCP_DEBUG_LEAVE;
        return -EBADMSG;
    }

    csum = _gnrc_tcp_pkt_calc_csum(hdr, pseudo_hdr, hdr->next);
    if (csum == 0) {
        TCP_DEBUG_ERROR("-ENOENT");
        TCP_DEBUG_LEAVE;
        return -ENOENT;
    }
    ((tcp_hdr_t *)hdr->data)->checksum = byteorder_htons(csum);

    TCP_DEBUG_LEAVE;
    return 0;
}

gnrc_pktsnip_t *gnrc_tcp_hdr_build(gnrc_pktsnip_t *payload, uint16_t src, uint16_t dst)
{
    TCP_DEBUG_ENTER;
    gnrc_pktsnip_t *res;
    tcp_hdr_t *hdr;

    /* Allocate header */
    res = gnrc_pktbuf_add(payload, NULL, sizeof(tcp_hdr_t), GNRC_NETTYPE_TCP);
    if (res == NULL) {
        TCP_DEBUG_ERROR("pktbuf is full.");
        TCP_DEBUG_LEAVE;
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

    TCP_DEBUG_LEAVE;
    return res;
}
