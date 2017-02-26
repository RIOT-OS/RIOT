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
#include "net/af.h"
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
 * @brief Allocate memory for TCP thread's stack
 */
#if ENABLE_DEBUG
static char _stack[TCP_EVENTLOOP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[TCP_EVENTLOOP_STACK_SIZE];
#endif

/**
 * @brief TCPs eventloop pid, declared externally
 */
kernel_pid_t gnrc_tcp_pid = KERNEL_PID_UNDEF;

/**
 * @brief Head of liked list of active connections
 */
gnrc_tcp_tcb_t *_list_tcb_head;

/**
 * @brief Mutex to protect the connection list
 */
mutex_t _list_tcb_lock;

/**
 * @brief   Establishes a new TCP connection
 *
 * @param[in/out] tcb       This connections Transmission control block.
 * @param[in] target_addr   Target Address to connect to, if this is a active connection.
 * @param[in] target_port   Target Port to connect to, if this is a active connection.
 * @param[in] local_addr    Local Address to bind on, if this is a passive connection.
 * @param[in] local_port    Local Port to bind on, if this is a passive connection.
 * @param[in] passive       Flag to indicate if this is a active or passive open.
 *
 * @return   0 on success.
 * @return   -EISCONN if transmission control block is already in use.
 * @return   -ENOMEM if the receive buffer for the tcb could not be allocated.
 *           Increase "GNRC_TCP_RCV_BUFFERS".
 * @return   -EADDRINUSE if @p local_port is already used by another connection. Only active mode.
 * @return   -ETIMEDOUT if the connection could not be opened. Only active mode.
 * @return   -ECONNREFUSED if the connection was resetted by the peer.
 */
static int _gnrc_tcp_open(gnrc_tcp_tcb_t *tcb, const uint8_t *target_addr, uint16_t target_port,
                          const uint8_t *local_addr, uint16_t local_port, uint8_t passive)
{
    msg_t msg;                           /* Message for incomming Messages */
    msg_t connection_timeout_msg;        /* Connection Timeout Message */
    xtimer_t connection_timeout_timer;   /* Connection Timeout Timer */
    int8_t ret = 0;                      /* Return Value */

    /* Lock the tcb for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Connection is already connected: Return -EISCONN */
    if (tcb->state != FSM_STATE_CLOSED) {
        mutex_unlock(&(tcb->function_lock));
        return -EISCONN;
    }

    /* Setup connection (common parts) */
    msg_init_queue(tcb->msg_queue, GNRC_TCP_TCB_MSG_QUEUE_SIZE);
    tcb->owner = thread_getpid();

    /* Setup passive connection */
    if (passive) {
        /* Set Status Flags */
        tcb->status |= STATUS_PASSIVE;
        if (local_addr == NULL) {
            tcb->status |= STATUS_ALLOW_ANY_ADDR;
        }
        /* If local address is specified: Copy it into tcb */
        else {
            switch (tcb->address_family) {
#ifdef MODULE_GNRC_IPV6
                case AF_INET6:
                    memcpy(tcb->local_addr, local_addr, sizeof(ipv6_addr_t));
                    break;
#endif
            }
        }
        /* Assign Port to listen on, to tcb */
        tcb->local_port = local_port;
    }
    /* Setup active connection */
    else {
        /* Copy Target Address and Port into tcb structure */
        if (target_addr != NULL) {
            switch (tcb->address_family) {
 #ifdef MODULE_GNRC_IPV6
                case AF_INET6:
                    memcpy(tcb->peer_addr, target_addr, sizeof(ipv6_addr_t));
                    break;
 #endif
            }
        }
        /* Copy Port Information, verfication happens in fsm */
        tcb->local_port = local_port;
        tcb->peer_port = target_port;

        /* Setup Timeout: If connection could not be established before */
        /* the timer expired, the connection attempt failed */
        connection_timeout_msg.type = MSG_TYPE_CONNECTION_TIMEOUT;
        xtimer_set_msg(&connection_timeout_timer, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                       &connection_timeout_msg, tcb->owner);
    }

    /* Call FSM with Event: CALL_OPEN */
    ret = _fsm(tcb, FSM_EVENT_CALL_OPEN, NULL, NULL, 0);
    if (ret == -ENOMEM) {
        DEBUG("gnrc_tcp.c : gnrc_tcp_connect() : Out of receive buffers.\n");
    }
    else if(ret == -EADDRINUSE) {
        DEBUG("gnrc_tcp.c : gnrc_tcp_connect() : local_port is already in use.\n");
    }

    /* Wait until a connection was established or closed */
    while (ret >= 0 && tcb->state != FSM_STATE_CLOSED && tcb->state != FSM_STATE_ESTABLISHED &&
           tcb->state != FSM_STATE_CLOSE_WAIT) {
        msg_receive(&msg);
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
    xtimer_remove(&connection_timeout_timer);
    if (tcb->state == FSM_STATE_CLOSED && ret == 0) {
        ret = -ECONNREFUSED;
    }
    tcb->owner = KERNEL_PID_UNDEF;
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

/* External GNRC_TCP API */
int gnrc_tcp_init(void)
{
    /* Guard: Check if thread is already running */
    if (gnrc_tcp_pid != KERNEL_PID_UNDEF) {
        return -1;
    }

    /* Initialize Mutex for linked-list synchronization */
    mutex_init(&(_list_tcb_lock));

    /* Initialize Linked-List for connection storage */
    _list_tcb_head = NULL;

    /* Initialize receive buffers */
    _rcvbuf_init();

    /* Start TCP processing loop */
    return thread_create(_stack, sizeof(_stack), TCP_EVENTLOOP_PRIO, 0, _event_loop, NULL,
                         "gnrc_tcp");
}

void gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb)
{
#ifdef MODULE_GNRC_IPV6
    tcb->address_family = AF_INET6;
    ipv6_addr_set_unspecified((ipv6_addr_t *) tcb->local_addr);
    ipv6_addr_set_unspecified((ipv6_addr_t *) tcb->peer_addr);
#else
    tcb->address_family = AF_UNSPEC;
    DEBUG("gnrc_tcp.c : gnrc_tcp_tcb_init() : Address unspec, add netlayer module to makefile\n");
#endif
    tcb->local_port = PORT_UNSPEC;
    tcb->peer_port = PORT_UNSPEC;
    tcb->state = FSM_STATE_CLOSED;
    tcb->status = 0;
    tcb->snd_una = 0;
    tcb->snd_nxt = 0;
    tcb->snd_wnd = 0;
    tcb->snd_wl1 = 0;
    tcb->snd_wl2 = 0;
    tcb->rcv_nxt = 0;
    tcb->rcv_wnd = 0;
    tcb->iss = 0;
    tcb->irs = 0;
    tcb->mss = 0;
    tcb->rtt_start = 0;
    tcb->rtt_var = RTO_UNINITIALIZED;
    tcb->srtt = RTO_UNINITIALIZED;
    tcb->rto = RTO_UNINITIALIZED;
    tcb->retries = 0;
    tcb->pkt_retransmit = NULL;
    tcb->owner = KERNEL_PID_UNDEF;
    tcb->rcv_buf_raw = NULL;
    mutex_init(&(tcb->fsm_lock));
    mutex_init(&(tcb->function_lock));
    tcb->next = NULL;
}

int gnrc_tcp_open_active(gnrc_tcp_tcb_t *tcb,  const uint8_t address_family,
                         const uint8_t *target_addr, const uint16_t target_port,
                         const uint16_t local_port)
{
    assert(tcb != NULL);
    assert(target_addr != NULL);
    assert(target_port != PORT_UNSPEC);

    /* Check AF-Family Support from target_addr */
    switch (address_family) {
#ifdef MODULE_GNRC_IPV6
        case AF_INET6:
            break;
#endif
        default:
            return -EAFNOSUPPORT;
    }
    /* Check if AF-Family for Target Address matches internally used AF-Family */
    if (tcb->address_family != address_family) {
        return -EINVAL;
    }
    return _gnrc_tcp_open(tcb, target_addr, target_port, NULL, local_port, 0);
}

int gnrc_tcp_open_passive(gnrc_tcp_tcb_t *tcb,  const uint8_t address_family,
                          const uint8_t *local_addr, const uint16_t local_port)
{
    assert(tcb != NULL);
    assert(local_port != GNRC_TCP_PORT_UNSPEC);

    /* Check AF-Family support if local address was supplied */
    if (local_addr != NULL) {
        switch (address_family) {
#ifdef MODULE_GNRC_IPV6
            case AF_INET6:
                break;
#endif
            default:
                return -EAFNOSUPPORT;
        }
        /* Check if AF-Family matches internally used AF-Family */
        if (tcb->address_family != address_family) {
            return -EINVAL;
        }
    }
    return _gnrc_tcp_open(tcb, NULL, 0, local_addr, local_port, 1);
}

ssize_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, const void *data, const size_t len,
                      const uint32_t timeout_duration_us)
{
    assert(tcb != NULL);
    assert(data != NULL);

    msg_t msg;                                /* Message for incomming Messages */
    msg_t connection_timeout_msg;             /* Connection Timeout Message */
    msg_t probe_timeout_msg;                  /* Probe Timeout Message */
    msg_t user_timeout_msg;                   /* User Specified Timeout Message */
    xtimer_t connection_timeout_timer;        /* Connection Timeout Timer */
    xtimer_t probe_timeout_timer;             /* Probe Timeout Timer */
    xtimer_t user_timeout_timer;              /* User Specified Timeout Timer */
    uint32_t probe_timeout_duration_us = 0;   /* Probe Timeout Duration in microseconds */
    ssize_t ret = 0;                          /* Return Value */
    bool probing = false;                     /* True if this connection is probing */

    /* Lock the tcb for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Check if connection is in a valid state */
    if (tcb->state != FSM_STATE_ESTABLISHED && tcb->state != FSM_STATE_CLOSE_WAIT) {
        mutex_unlock(&(tcb->function_lock));
        return -ENOTCONN;
    }

    /* Re-init message queue, take ownership. FSM can send Messages to this thread now */
    msg_init_queue(tcb->msg_queue, GNRC_TCP_TCB_MSG_QUEUE_SIZE);
    tcb->owner = thread_getpid();

    /* Setup Connection Timeout */
    connection_timeout_msg.type = MSG_TYPE_CONNECTION_TIMEOUT;
    xtimer_set_msg(&connection_timeout_timer, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                   &connection_timeout_msg, tcb->owner);

    /* Setup User specified timeout if timeout_us is greater than zero */
    if (timeout_duration_us > 0) {
        user_timeout_msg.type = MSG_TYPE_USER_SPEC_TIMEOUT;
        xtimer_set_msg(&user_timeout_timer, timeout_duration_us, &user_timeout_msg, tcb->owner);
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
            if (!probing) {
                probing = true;
                probe_timeout_duration_us = tcb->rto;
            }
            /* Initialize Probe Timer */
            probe_timeout_msg.type = MSG_TYPE_PROBE_TIMEOUT;
            xtimer_set_msg(&probe_timeout_timer, probe_timeout_duration_us, &probe_timeout_msg,
                           tcb->owner);
        }

        /* Try to send data in case there nothing has been sent and we are not probing */
        if (ret == 0 && !probing) {
            ret = _fsm(tcb, FSM_EVENT_CALL_SEND, NULL, (void *) data, len);
        }

        /* Wait for responses */
        msg_receive(&msg);
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
                /* Send Probe */
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
                xtimer_set_msg(&connection_timeout_timer, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                               &connection_timeout_msg, tcb->owner);

                /* If the window re-opened and we are probing: Stop it */
                if (tcb->snd_wnd > 0 && probing) {
                    probing = false;
                    xtimer_remove(&probe_timeout_timer);
                }
                break;

            default:
                DEBUG("gnrc_tcp.c : gnrc_tcp_send() : other message type\n");
        }
    }

    /* Cleanup */
    xtimer_remove(&probe_timeout_timer);
    xtimer_remove(&connection_timeout_timer);
    xtimer_remove(&user_timeout_timer);
    tcb->owner = KERNEL_PID_UNDEF;
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

ssize_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb, void *data, const size_t max_len,
                      const uint32_t timeout_duration_us)
{
    assert(tcb != NULL);
    assert(data != NULL);

    msg_t msg;                           /* Message for incomming Messages */
    msg_t connection_timeout_msg;        /* Connection Timeout Message */
    msg_t user_timeout_msg;              /* User Specified Timeout Message */
    xtimer_t connection_timeout_timer;   /* Connection Timeout Timer */
    xtimer_t user_timeout_timer;         /* User Specified Timeout Timer */
    ssize_t ret = 0;                     /* Return Value */

    /* Lock the tcb for this function call */
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

    /* If this call is blocking, setup messages and timers */
    msg_init_queue(tcb->msg_queue, GNRC_TCP_TCB_MSG_QUEUE_SIZE);
    tcb->owner = thread_getpid();

    /* Setup Connection Timeout */
    connection_timeout_msg.type = MSG_TYPE_CONNECTION_TIMEOUT;
    xtimer_set_msg(&connection_timeout_timer, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                   &connection_timeout_msg, tcb->owner);

    /* Setup User Specified Timeout */
    user_timeout_msg.type = MSG_TYPE_USER_SPEC_TIMEOUT;
    xtimer_set_msg(&user_timeout_timer, timeout_duration_us, &user_timeout_msg, tcb->owner);

    /* Processing Loop */
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
            msg_receive(&msg);
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
    xtimer_remove(&connection_timeout_timer);
    xtimer_remove(&user_timeout_timer);
    tcb->owner = KERNEL_PID_UNDEF;
    mutex_unlock(&(tcb->function_lock));
    return ret;
}

int gnrc_tcp_close(gnrc_tcp_tcb_t *tcb)
{
    assert(tcb != NULL);

    msg_t msg;                           /* Message for incomming Messages */
    msg_t connection_timeout_msg;        /* Connection Timeout Message */
    xtimer_t connection_timeout_timer;   /* Connection Timeout Timer */

    /* Lock the tcb for this function call */
    mutex_lock(&(tcb->function_lock));

    /* Start connection teardown if the connection was not closed before */
    if (tcb->state != FSM_STATE_CLOSED) {
        /* Take ownership */
        msg_init_queue(tcb->msg_queue, GNRC_TCP_TCB_MSG_QUEUE_SIZE);
        tcb->owner = thread_getpid();

        /* Setup Connection Timeout */
        connection_timeout_msg.type = MSG_TYPE_CONNECTION_TIMEOUT;
        xtimer_set_msg(&connection_timeout_timer, GNRC_TCP_CONNECTION_TIMEOUT_DURATION,
                       &connection_timeout_msg, tcb->owner);

        /* Start connection teardown sequence */
        _fsm(tcb, FSM_EVENT_CALL_CLOSE, NULL, NULL, 0);

        /* Loop until the connection has been closed */
        while (tcb->state != FSM_STATE_CLOSED) {
            msg_receive(&msg);
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
    }

    /* Cleanup */
    xtimer_remove(&connection_timeout_timer);
    tcb->owner = KERNEL_PID_UNDEF;
    mutex_unlock(&(tcb->function_lock));
    return 0;
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

    /* allocate header */
    res = gnrc_pktbuf_add(payload, NULL, sizeof(tcp_hdr_t), GNRC_NETTYPE_TCP);
    if (res == NULL) {
        DEBUG("tcp: No space left in packet buffer\n");
        return NULL;
    }
    hdr = (tcp_hdr_t *) res->data;

    /* Clear Header */
    memset(hdr, 0, sizeof(tcp_hdr_t));

    /* Initialize Header with sane Defaults */
    hdr->src_port = byteorder_htons(src);
    hdr->dst_port = byteorder_htons(dst);
    hdr->checksum = byteorder_htons(0);
    hdr->off_ctl = byteorder_htons(TCP_HDR_OFFSET_MIN);
    return res;
}
