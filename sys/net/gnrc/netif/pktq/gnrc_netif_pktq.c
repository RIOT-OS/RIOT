/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

static void _timer_pktq_sched(void *arg)
{
    gnrc_netif_t *netif = arg;
    event_post(&netif->evq[GNRC_NETIF_EVQ_INDEX_PRIO_LOW], &netif->event_pktq);
}

void gnrc_netif_pktq_init(gnrc_netif_t *netif)
{
    (void) netif;
#if CONFIG_GNRC_NETIF_PKTQ_TIMER_US > 0
    netif->send_queue.dequeue_timer.callback = _timer_pktq_sched;
    netif->send_queue.dequeue_timer.arg = netif;
#endif  /* CONFIG_GNRC_NETIF_PKTQ_TIMER_US >= 0 */
}

void gnrc_netif_pktq_sched_get(gnrc_netif_t *netif)
{
#if CONFIG_GNRC_NETIF_PKTQ_TIMER_US > 0
    assert(netif != NULL);
    /* Prevent timer from firing while we add this */
    unsigned state = irq_disable();
    ztimer_set(ZTIMER_USEC, &netif->send_queue.dequeue_timer,
               CONFIG_GNRC_NETIF_PKTQ_TIMER_US);
    irq_restore(state);
#elif CONFIG_GNRC_NETIF_PKTQ_TIMER_US == 0
    assert(netif != NULL);
    event_post(&netif->evq[GNRC_NETIF_EVQ_INDEX_PRIO_LOW], &netif->event_pktq);
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
