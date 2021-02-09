/*
 * Copyright (C) Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Neighbor level stats for netdev
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 * @}
 */

#include <errno.h>

#include "net/l2util.h"
#include "net/netdev.h"
#include "net/netstats/neighbor.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static inline void _lock(netif_t *dev)
{
    mutex_lock(&dev->neighbors.lock);
}

static inline void _unlock(netif_t *dev)
{
    mutex_unlock(&dev->neighbors.lock);
}

/**
 * @brief Compare the freshness of two records
 *
 * @param[in] a     pointer to the first record
 * @param[in] b     pointer to the second record
 * @param[in] now   current timestamp in seconds
 *
 * @return       pointer to the least fresh record
 */
static inline netstats_nb_t *netstats_nb_comp(const netstats_nb_t *a,
                                              const netstats_nb_t *b,
                                              uint16_t now)
{
    return (netstats_nb_t *)(((now - a->last_updated) > now - b->last_updated) ? a : b);
}

static void half_freshness(netstats_nb_t *stats, uint16_t now_sec)
{
    uint8_t diff = (now_sec - stats->last_halved) / NETSTATS_NB_FRESHNESS_HALF;
    stats->freshness >>= diff;

    if (diff) {
        /* Set to the last time point where this should have been halved */
        stats->last_halved = now_sec - diff;
    }
}

static void incr_freshness(netstats_nb_t *stats)
{
    uint16_t now = xtimer_now_usec() / US_PER_SEC;;

    /* First halve the freshness if applicable */
    half_freshness(stats, now);

    /* Increment the freshness capped at FRESHNESS_MAX */
    if (stats->freshness < NETSTATS_NB_FRESHNESS_MAX) {
        stats->freshness++;
    }

    stats->last_updated = now;
}

static bool isfresh(netstats_nb_t *stats)
{
    uint16_t now = xtimer_now_usec() / US_PER_SEC;

    /* Half freshness if applicable to update to current freshness */
    half_freshness(stats, now);

    return (stats->freshness >= NETSTATS_NB_FRESHNESS_TARGET) &&
           (now - stats->last_updated < NETSTATS_NB_FRESHNESS_EXPIRATION);
}

bool netstats_nb_isfresh(netif_t *dev, netstats_nb_t *stats)
{
    bool ret;

    _lock(dev);
    ret = isfresh(stats);
    _unlock(dev);

    return ret;
}

void netstats_nb_init(netif_t *dev)
{
    mutex_init(&dev->neighbors.lock);

    _lock(dev);
    memset(dev->neighbors.pstats, 0, sizeof(netstats_nb_t) * NETSTATS_NB_SIZE);
    cib_init(&dev->neighbors.stats_idx, NETSTATS_NB_QUEUE_SIZE);
    _unlock(dev);
}

static void netstats_nb_create(netstats_nb_t *entry, const uint8_t *l2_addr, uint8_t l2_len)
{
    memset(entry, 0, sizeof(netstats_nb_t));
    memcpy(entry->l2_addr, l2_addr, l2_len);
    entry->l2_addr_len = l2_len;

#ifdef MODULE_NETSTATS_NEIGHBOR_ETX
    entry->etx = NETSTATS_NB_ETX_INIT * NETSTATS_NB_ETX_DIVISOR;
#endif
}

bool netstats_nb_get(netif_t *dev, const uint8_t *l2_addr, uint8_t len, netstats_nb_t *out)
{
    _lock(dev);

    netstats_nb_t *stats = dev->neighbors.pstats;
    bool found = false;

    for (int i = 0; i < NETSTATS_NB_SIZE; i++) {

        /* Check if this is the matching entry */
        if (l2util_addr_equal(stats[i].l2_addr, stats[i].l2_addr_len, l2_addr, len)) {
            *out = stats[i];
            found = true;
            break;
        }
    }

    _unlock(dev);
    return found;
}

/* find the oldest inactive entry to replace. Empty entries are infinity old */
static netstats_nb_t *netstats_nb_get_or_create(netif_t *dev, const uint8_t *l2_addr, uint8_t len)
{
    netstats_nb_t *old_entry = NULL;
    netstats_nb_t *stats = dev->neighbors.pstats;
    uint16_t now = xtimer_now_usec() / US_PER_SEC;

    for (int i = 0; i < NETSTATS_NB_SIZE; i++) {

        /* Check if this is the matching entry */
        if (l2util_addr_equal(stats[i].l2_addr, stats[i].l2_addr_len, l2_addr, len)) {
            return &stats[i];
        }

        /* Entry is oldest if it is empty */
        if (stats[i].l2_addr_len == 0) {
            old_entry = &stats[i];
        }
        /* Check if the entry is expired */
        else if (!isfresh(&stats[i])) {
            /* Entry is oldest if it is expired */
            if (old_entry == NULL) {
                old_entry = &stats[i];
            }
            /* don't replace old entry if there are still empty ones */
            else if (old_entry->l2_addr_len > 0) {
                /* Check if current entry is older than current oldest entry */
                old_entry = netstats_nb_comp(old_entry, &stats[i], now);
            }
        }
    }

    /* if there is no matching entry,
     * create a new entry if we have an expired one */
    if (old_entry) {
        netstats_nb_create(old_entry, l2_addr, len);
    }

    return old_entry;
}

void netstats_nb_record(netif_t *dev, const uint8_t *l2_addr, uint8_t len)
{
    _lock(dev);

    int idx = cib_put(&dev->neighbors.stats_idx);

    if (idx < 0) {
        DEBUG("%s: put buffer empty\n", __func__);
        goto out;
    }

    DEBUG("put %d\n", idx);

    if (len == 0) {
        /* Fill queue with a NOP */
        dev->neighbors.stats_queue[idx] = NULL;
    } else {
        dev->neighbors.stats_queue[idx] = netstats_nb_get_or_create(dev, l2_addr, len);
        dev->neighbors.stats_queue_time_tx[idx] = xtimer_now_usec();
    }

out:
    _unlock(dev);
}

/* Get the first available neighbor in the transmission queue
 * and increment pointer. */
static netstats_nb_t *netstats_nb_get_recorded(netif_t *dev, uint32_t *time_tx)
{
    netstats_nb_t *res;
    int idx = cib_get(&dev->neighbors.stats_idx);

    if (idx < 0) {
        DEBUG("%s: can't get record\n", __func__);
        return NULL;
    }

    DEBUG("get %d (%d left)\n", idx, cib_avail(&dev->neighbors.stats_idx));

    res = dev->neighbors.stats_queue[idx];
    dev->neighbors.stats_queue[idx] = NULL;

    *time_tx = dev->neighbors.stats_queue_time_tx[idx];

    return res;
}

__attribute__((unused))
static uint32_t _ewma(bool fresh, uint32_t old_val, uint32_t new_val)
{
    uint8_t ewma_alpha;

    if (old_val == 0) {
        return new_val;
    }

    /* If the stats are not fresh, use a larger alpha to average aggressive */
    if (fresh) {
        ewma_alpha = NETSTATS_NB_EWMA_ALPHA;
    } else {
        ewma_alpha = NETSTATS_NB_EWMA_ALPHA_RAMP;
    }

    /* Exponential weighted moving average */
    return (old_val * (NETSTATS_NB_EWMA_SCALE - ewma_alpha)
         +  new_val * ewma_alpha) / NETSTATS_NB_EWMA_SCALE;
}

static void netstats_nb_update_etx(netstats_nb_t *stats, netstats_nb_result_t result,
                                   uint8_t transmissions, bool fresh)
{
    /* don't do anything if driver does not report ETX */
    if (transmissions == 0) {
        return;
    }

    if (result != NETSTATS_NB_SUCCESS) {
        transmissions = NETSTATS_NB_ETX_NOACK_PENALTY;
    }

#ifdef MODULE_NETSTATS_NEIGHBOR_ETX
    stats->etx = _ewma(fresh, stats->etx, transmissions * NETSTATS_NB_ETX_DIVISOR);
#else
    (void)stats;
    (void)result;
    (void)transmissions;
    (void)fresh;
#endif
}

static void netstats_nb_update_time(netstats_nb_t *stats, netstats_nb_result_t result,
                                    uint32_t duration, bool fresh)
{
    /* TX time already got a penalty due to retransmissions */
    if (result != NETSTATS_NB_SUCCESS) {
        duration *= 2;
    }

#if MODULE_NETSTATS_NEIGHBOR_TX_TIME
    stats->time_tx_avg = _ewma(fresh, stats->time_tx_avg, duration);
#else
    (void)stats;
    (void)result;
    (void)duration;
    (void)fresh;
#endif
}

static void netstats_nb_update_rssi(netstats_nb_t *stats, uint8_t rssi, bool fresh)
{
#ifdef MODULE_NETSTATS_NEIGHBOR_RSSI
    stats->rssi = _ewma(fresh, stats->rssi, rssi);
#else
    (void)stats;
    (void)rssi;
    (void)fresh;
#endif
}

static void netstats_nb_update_lqi(netstats_nb_t *stats, uint8_t lqi, bool fresh)
{
#ifdef MODULE_NETSTATS_NEIGHBOR_LQI
    stats->lqi = _ewma(fresh, stats->lqi, lqi);
#else
    (void)stats;
    (void)lqi;
    (void)fresh;
#endif
}

static void netstats_nb_incr_count_tx(netstats_nb_t *stats, netstats_nb_result_t result)
{
#ifdef MODULE_NETSTATS_NEIGHBOR_COUNT
    stats->tx_count++;

    /* gracefully handle overflow */
    if (stats->tx_count == 0) {
        stats->tx_count = ~stats->tx_count;
        stats->tx_count = stats->tx_count >> 4;
        stats->tx_fail  = stats->tx_fail  >> 4;
    }

    if (result != NETSTATS_NB_SUCCESS) {
        stats->tx_fail++;
    }
#else
    (void)stats;
    (void)result;
#endif
}

static void netstats_nb_incr_count_rx(netstats_nb_t *stats)
{
#ifdef MODULE_NETSTATS_NEIGHBOR_COUNT
    stats->rx_count++;
#else
    (void)stats;
#endif
}

netstats_nb_t *netstats_nb_update_tx(netif_t *dev, netstats_nb_result_t result,
                                     uint8_t transmissions)
{
    uint32_t now = xtimer_now_usec();
    netstats_nb_t *stats;
    uint32_t time_tx = 0;

    _lock(dev);

    /* Buggy drivers don't always generate TX done events.
     * Discard old events to prevent the tx start <-> tx done correlation
     * from getting out of sync. */
    do {
        stats = netstats_nb_get_recorded(dev, &time_tx);
    } while (cib_avail(&dev->neighbors.stats_idx)
             && ((now - time_tx) > NETSTATS_NB_TX_TIMEOUT_MS * US_PER_MS));

    /* Nothing to do for multicast or if packet was not sent */
    if (result == NETSTATS_NB_BUSY || stats == NULL) {
        goto out;
    }

    bool fresh = isfresh(stats);

    netstats_nb_update_time(stats, result, now - time_tx, fresh);
    netstats_nb_update_etx(stats, result, transmissions, fresh);
    netstats_nb_incr_count_tx(stats, result);

    incr_freshness(stats);

out:
    _unlock(dev);
    return stats;
}

netstats_nb_t *netstats_nb_update_rx(netif_t *dev, const uint8_t *l2_addr,
                                     uint8_t l2_addr_len, uint8_t rssi, uint8_t lqi)
{
    _lock(dev);

    netstats_nb_t *stats = netstats_nb_get_or_create(dev, l2_addr, l2_addr_len);

    if (stats != NULL) {
        bool fresh = isfresh(stats);

        netstats_nb_update_rssi(stats, rssi, fresh);
        netstats_nb_update_lqi(stats, lqi, fresh);
        netstats_nb_incr_count_rx(stats);

        incr_freshness(stats);
    }

    _unlock(dev);
    return stats;
}
