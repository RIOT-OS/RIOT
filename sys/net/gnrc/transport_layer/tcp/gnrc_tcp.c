/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     sys_net_tcp
 *
 * @file
 * @brief       Basic TCP implementation
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include <utlist.h>
#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "byteorder.h"
#include "random.h"
#include "xtimer.h"
#include "mutex.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"
#include "net/inet_csum.h"

#include "net/gnrc/tcp.h"
#include "net/gnrc/tcp/tcb.h"

#include "tcp_internal/fsm.h"
#include "tcp_internal/pkt.h"
#include "tcp_internal/demux.h"
#include "tcp_internal/option.h"
#include "tcp_internal/helper.h"
#include "tcp_internal/eventloop.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG (1)
#include "debug.h"

/**
 * @brief Allocate memory for TCP thread's stack
 */
#if ENABLE_DEBUG
static char _stack[GNRC_TCP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_TCP_STACK_SIZE];
#endif

/**
 * @brief TCPs eventloop pid, declared externally
 */
kernel_pid_t _tcp_pid = KERNEL_PID_UNDEF;

/**
 * @brief Head of Linked List, declared externally
 */
gnrc_tcp_tcb_t* _head_list_tcb;

#ifdef __cplusplus
extern "C" {
#endif

/* External GNRC_TCP API */
int8_t gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb)
{
    /* Init connection information */
    tcb->state = CLOSED;           /* CLOSED: State before any connection occurs */
    tcb->peer_addr = NULL;         /* Peer Address Unspecified */
    tcb->peer_addr_len = 0;        /* Peer Address Length is zero */
    tcb->peer_port = PORT_UNSPEC;  /* Port 0: Reserved. Marks Unassigned Port */
    tcb->local_port = PORT_UNSPEC; /* Port 0: Reserved. Marks Unassigned Port */

    /* Init Send Pointers */
    tcb->snd_una = 0;
    tcb->snd_nxt = 0;
    tcb->snd_wnd = 0;
    tcb->snd_ur = 0;
    tcb->snd_wl1 = 0;
    tcb->snd_wl2 = 0;
    tcb->iss = 0;

    /* Init Receive Pointers */
    tcb->rcv_nxt = 0;
    tcb->rcv_wnd = 0;
    tcb->irs = 0;

    /* Peers Maximum Window Size */
    tcb->mss = 0;

    /* Clear Retransmission and Timer */
    tcb->pkt_retransmit = NULL;

    /* Initialize IPC-Mechanisms */
    mutex_init(&(tcb->mtx));
    msg_init_queue(tcb->msg_queue, sizeof(tcb->msg_queue));

    /* TCB List init */
    tcb->owner = thread_getpid();
    tcb->next = NULL;
    LL_APPEND(_head_list_tcb, tcb);
    return 0;
}

int8_t gnrc_tcp_tcb_destroy(gnrc_tcp_tcb_t *tcb)
{
    /* Guard: only tcb owner is allowed to use this on supplied tcb call */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    /* Guard: connection must be closed */
    if(tcb->state != CLOSED){
        return -1;
    }

    /* Remove this tcb from linked tcb list */
    LL_DELETE(_head_list_tcb, tcb);
    return 0;
}


int8_t gnrc_tcp_open(gnrc_tcp_tcb_t *tcb, uint16_t local_port, uint8_t *peer_addr,
                   size_t peer_addr_len, uint16_t peer_port, uint8_t options)
{
    msg_t msg;
    msg_t msg_timeout;
    xtimer_t timer_timeout;

    /* Guards: Parameter checking */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }
    if(tcb->state != CLOSED){
        return -EALREADY;
    }
    if(peer_addr == NULL || peer_addr_len == 0){
        return -EFAULT;
    }
    if((options & AI_PASSIVE)){
        if(local_port == PORT_UNSPEC){
            return -EINVAL;
        }
    }else{
        if(peer_port == PORT_UNSPEC){
            return -EDESTADDRREQ;
        }
#ifdef MODULE_GNRC_IPV6
        if(ipv6_addr_is_unspecified((ipv6_addr_t*) peer_addr)){
            return -EDESTADDRREQ;
        }
#endif
    }

    /* Fill tcb with connection information.*/
    tcb->local_port = (options & AI_PASSIVE) ? local_port : _demux_find_free_port(peer_port);
    tcb->peer_port = (options & AI_PASSIVE) ? PORT_UNSPEC : peer_port;
    tcb->peer_addr = peer_addr;
    tcb->peer_addr_len = peer_addr_len;
    tcb->options = options;

    /* Setup user-call timeout, in case of an active connection. */
    if(!(options & AI_PASSIVE)){
        msg_timeout.type = MSG_TYPE_USER_TIMEOUT;
        xtimer_set_msg(&timer_timeout, GNRC_TCP_USER_TIMEOUT_DURATION, &msg_timeout, thread_getpid());
    }

    /* Event CALL_OPEN happens */
    _fsm(tcb, NULL, CALL_OPEN);

    /* Processing loop */
    while(1){
        msg_receive(&msg);
        switch (msg.type) {
            case MSG_TYPE_USER_TIMEOUT:
                DEBUG("tcp: USER_TIMEOUT\n");
                _fsm(tcb, NULL, TIMEOUT_USER);
                return -ETIME;

            case MSG_TYPE_STATE_CHANGE:
                DEBUG("tcp: STATE_CHANGE\n");
                if(tcb->state == CLOSED || tcb->state == ESTABLISHED || tcb->state == CLOSE_WAIT){
                    /* In case of success: Stop User Timeout */
                    xtimer_remove(&timer_timeout);
                    return 0;
                }
                break;

            default:
                DEBUG("tcp: unexpected message type received\n");
        }
    }
    /* Never reached */
    return 0;
}

int8_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, uint8_t *buf, size_t byte_count, bool push, bool urgent)
{
    (void) buf;
    (void) byte_count;
    (void) push;
    (void) urgent;

    /* Guard: only tcb owner is allowed to use this call, on tcb */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    if(tcb->state != ESTABLISHED && tcb->state != CLOSE_WAIT){
        return -1;
    }
    return 0;
}

int8_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb)
{
    /* Guard: only tcb owner is allowed to use this call, on tcb */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    if(tcb->state != ESTABLISHED){
        return -1;
    }
    return 0;
}

int8_t gnrc_tcp_close(gnrc_tcp_tcb_t *tcb)
{
    msg_t msg;
    msg_t msg_timeout;
    xtimer_t timer_timeout;

    /* Guard: only tcb owner is allowed to use this call, on tcb */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    /* Connection must be established or in close_wait for this call */
    if(tcb->state != ESTABLISHED && tcb->state != CLOSE_WAIT){
        return -1;
    }

    /* Setup User timeout */
    msg_timeout.type = MSG_TYPE_USER_TIMEOUT;
    xtimer_set_msg(&timer_timeout, GNRC_TCP_USER_TIMEOUT_DURATION, &msg_timeout, thread_getpid());

    /* Event CALL_CLOSE happens */
    _fsm(tcb, NULL, CALL_CLOSE);

    /* Event loop */
    while(1){
        msg_receive(&msg);
        switch(msg.type){
            case MSG_TYPE_USER_TIMEOUT:
                DEBUG("tcp: USER_TIMEOUT\n");
                _fsm(tcb, NULL, TIMEOUT_USER);
                return -ETIME;

            case MSG_TYPE_STATE_CHANGE:
                DEBUG("tcp: STATE_CHANGE\n");
                if(tcb->state == CLOSED){
                    /* In case of success: stop User Timeout */
                    xtimer_remove(&timer_timeout);
                    return 0;
                }
                break;

            default:
                DEBUG("tcp: unexpected message type received\n");
        }
    }

    /* Never reached */
    return 0;
}

int gnrc_tcp_init(void)
{
    /* Using uninitialized stack contents as seed. */
    uint32_t seed[SEED_LEN];

    /* Guard: Check if thread is already running */
    if (_tcp_pid != KERNEL_PID_UNDEF) {
        return -1;
    }

    /* initialize Pseudo Random Number Generator */
    genrand_init_by_array(seed, sizeof(seed));

    /* initialize Linked-List for TCB storage */
    _head_list_tcb = NULL;

    /* start TCP thread */
    return thread_create(_stack, sizeof(_stack), GNRC_TCP_PRIO,
                         0, _event_loop, NULL, "tcp");
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

gnrc_pktsnip_t *gnrc_tcp_hdr_build(gnrc_pktsnip_t *payload,
                               uint8_t *src, size_t src_len,
                               uint8_t *dst, size_t dst_len)
{
    gnrc_pktsnip_t *res;
    tcp_hdr_t *hdr;

    /* check parameters */
    if( src == NULL
     || dst == NULL
     || src_len != sizeof(uint16_t)
     || dst_len != sizeof(uint16_t)
    ){
        DEBUG("tcp: Address length was not sizeof(uint16_t)\n");
        return NULL;
    }

    /* allocate header */
    res = gnrc_pktbuf_add(payload, NULL, sizeof(tcp_hdr_t), GNRC_NETTYPE_TCP);
    if(res == NULL) {
        DEBUG("tcp: No space left in packet buffer\n");
        return NULL;
    }

    hdr = (tcp_hdr_t *) res->data;

    /* Clear Header */
    memset(hdr, 0, sizeof(tcp_hdr_t));

    /* Initialize Header with sane Defaults */
    hdr->src_port = byteorder_htons(*((uint16_t *)src));
    hdr->dst_port = byteorder_htons(*((uint16_t *)dst));
    hdr->checksum = byteorder_htons(0);
    hdr->off_ctl = byteorder_htons(OPTION_OFFSET_MAX);

    return res;
}

