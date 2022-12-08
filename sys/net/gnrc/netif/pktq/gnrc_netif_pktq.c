/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>

#include "net/gnrc/pktqueue.h"
#include "net/gnrc/netif/conf.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/netif/pktq.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static mutex_t _pool_lock = MUTEX_INIT;
static gnrc_pktqueue_t _pool[CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE];

static gnrc_pktqueue_t *_get_free_entry(gnrc_pktsnip_t *pkt)
{
    gnrc_pktqueue_t *entry = NULL;

    mutex_lock(&_pool_lock);
    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE; i++) {
        if (_pool[i].pkt == NULL) {
            _pool[i].pkt = pkt;
            entry = &_pool[i];
            break;
        }
    }
    mutex_unlock(&_pool_lock);

    return entry;
}

unsigned gnrc_netif_pktq_usage(void)
{
    unsigned res = 0;

    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE; i++) {
        if (_pool[i].pkt != NULL) {
            res++;
        }
    }
    return res;
}

int gnrc_netif_pktq_put(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(netif != NULL);
    assert(pkt != NULL);

    gnrc_pktqueue_t *entry = _get_free_entry(pkt);

    if (entry == NULL) {
        return -1;
    }
    gnrc_pktqueue_add(&netif->send_queue.queue, entry);
    return 0;
}

void gnrc_netif_pktq_sched_get(gnrc_netif_t *netif)
{
#if CONFIG_GNRC_NETIF_PKTQ_TIMER_US > 0
    assert(netif != NULL);
    netif->send_queue.dequeue_msg.type = GNRC_NETIF_PKTQ_DEQUEUE_MSG;
    /* Prevent timer from firing while we add this.
     * Otherwise the system might crash: The timer handler sets
     * netif->send_queue.dequeue_msg.sender_pid to KERNEL_PID_ISR while
     * the message is added to the timer, causing the next round of the timer
     * handler to try to send the message to IPC, leaving the system in an
     * invalid state. */
    unsigned state = irq_disable();
    xtimer_set_msg(&netif->send_queue.dequeue_timer,
                   CONFIG_GNRC_NETIF_PKTQ_TIMER_US,
                   &netif->send_queue.dequeue_msg, netif->pid);
    irq_restore(state);
#elif CONFIG_GNRC_NETIF_PKTQ_TIMER_US == 0
    assert(netif != NULL);
    event_post(&netif->evq, &netif->event_dequeue);
#else
    (void)netif;
#endif  /* CONFIG_GNRC_NETIF_PKTQ_TIMER_US >= 0 */
}

int gnrc_netif_pktq_push_back(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(netif != NULL);
    assert(pkt != NULL);

    gnrc_pktqueue_t *entry = _get_free_entry(pkt);

    if (entry == NULL) {
        return -1;
    }
    LL_PREPEND(netif->send_queue.queue, entry);
    return 0;
}

/** @} */
