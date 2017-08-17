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
 * @}
 */

#include <errno.h>

#include "net/netdev.h"
#include "net/netstats/neighbor.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
//static char addr_str[30];

static bool l2_addr_equal(const uint8_t *a, uint8_t a_len, const uint8_t *b, uint8_t b_len);

void netstats_nb_init(netdev_t *dev)
{
    memset(dev->pstats, 0, sizeof(netstats_nb_t) * NETSTATS_NB_SIZE);
    dev->send_index = 0;
    dev->cb_index = 0;
}

void netstats_nb_create(netstats_nb_t *entry, const uint8_t *l2_addr, uint8_t l2_len)
{
    memset(entry, 0, sizeof(netstats_nb_t));
    memcpy(entry->l2_addr, l2_addr, l2_len);
    entry->l2_addr_len = l2_len;
    entry->etx = NETSTATS_NB_ETX_INIT * NETSTATS_NB_ETX_DIVISOR;
}

/* find the oldest inactive entry to replace. Empty entries are infinity old */
netstats_nb_t *netstats_nb_get_or_create(netdev_t *dev, const uint8_t *l2_addr, uint8_t len)
{
    netstats_nb_t *old_entry = NULL;
    netstats_nb_t *matching_entry = NULL;
    netstats_nb_t *stats = dev->pstats;

    for (int i = 0; i < NETSTATS_NB_SIZE; i++) {
        /* Check if this is the matching entry */
        if (l2_addr_equal(stats[i].l2_addr, stats[i].l2_addr_len,
                          l2_addr, len)) {
            matching_entry = &stats[i];
            break;
        }
        /* Entry is oldest if it is empty */
        else if (stats[i].l2_addr_len == 0) {
            old_entry = &stats[i];
        }
        /* Check if the entry is expired */
        else if (!(netstats_nb_isfresh(&stats[i]))) {
            if (old_entry == NULL) {
                old_entry = &stats[i];
            }
            /* Check if current entry is older than current oldest entry */
            else {
                old_entry = netstats_nb_comp(old_entry, &stats[i]);
            }
        }
    }
    if (matching_entry) {
        return matching_entry;
    }
    /* if there is no matching entry,
     * create a new entry if we have an expired one */
    if (old_entry) {
        netstats_nb_create(old_entry, l2_addr, len);
    }
    return old_entry;
}

netstats_nb_t *netstats_nb_get_next(netstats_nb_t *first, netstats_nb_t *prev)
{
    prev++;                                         /* get next entry */
    while (prev < (first + NETSTATS_NB_SIZE)) {     /* while not reached end */
        if (prev->l2_addr_len) {
            return prev;
        }
        prev++;
    }
    return NULL;
}

void netstats_nb_record(netdev_t *dev, const uint8_t *l2_addr, uint8_t len)
{
    if (!(len)) {
        /* Fill queue with a NOP */
        dev->stats_queue[dev->send_index] = NULL;
    }
    else {
        dev->stats_queue[dev->send_index] = netstats_nb_get_or_create(dev, l2_addr, len);
    }
    dev->send_index++;
    if (dev->send_index == 4) {
        dev->send_index = 0;
    }
}

netstats_nb_t *netstats_nb_get_recorded(netdev_t *dev)
{
    netstats_nb_t *stats = dev->stats_queue[dev->cb_index];

    dev->cb_index++;
    if (dev->cb_index == 4) {
        dev->cb_index = 0;
    }
    return stats;
}

netstats_nb_t *netstats_nb_update_tx(netdev_t *dev, netstats_nb_result_t result, uint8_t retries)
{
    netstats_nb_t *stats = netstats_nb_get_recorded(dev);

    if (result == NETSTATS_NB_BUSY) {
        return stats;
    }
    if (stats) {
#ifdef MODULE_NETSTATS_NEIGHBOR_EXT
        stats->tx_count += retries + 1;
        stats->tx_failed += retries;
#endif
        netstats_nb_update_etx(stats, result, retries);
        netstats_nb_incr_freshness(stats);
    }
    return stats;
}

#ifdef MODULE_NETSTATS_NEIGHBOR_EXT
netstats_nb_t *netstats_nb_update_rx(netdev_t *dev, const uint8_t *l2_addr, uint8_t l2_addr_len, uint8_t rssi, uint8_t lqi)
{
    netstats_nb_t *stats = netstats_nb_get_or_create(
        dev, l2_addr, l2_addr_len
        );

    if (stats) {
        if (stats->rx_count == 0) {
            stats->rssi = rssi;
            stats->lqi = lqi;
        }
        else {
            /* Exponential weighted moving average */
            stats->rssi = ((uint32_t)stats->rssi *
                           (NETSTATS_NB_EWMA_SCALE - NETSTATS_NB_EWMA_ALPHA) +
                           (uint32_t)rssi * NETSTATS_NB_EWMA_ALPHA
                           ) / NETSTATS_NB_EWMA_SCALE;
            stats->lqi = ((uint32_t)stats->lqi *
                          (NETSTATS_NB_EWMA_SCALE - NETSTATS_NB_EWMA_ALPHA) +
                          (uint32_t)lqi * NETSTATS_NB_EWMA_ALPHA
                          ) / NETSTATS_NB_EWMA_SCALE;
        }
        stats->rx_count++;
    }
    return stats;
}
#endif

void netstats_nb_update_etx(netstats_nb_t *stats, netstats_nb_result_t success, uint8_t retries)
{
    uint16_t packet_etx = 0;
    uint8_t ewma_alpha;

    /* If the stats are not fresh, use a larger alpha to average aggressive */
    if (netstats_nb_isfresh(stats)) {
        ewma_alpha = NETSTATS_NB_EWMA_ALPHA;
    }
    else {
        ewma_alpha = NETSTATS_NB_EWMA_ALPHA_RAMP;
    }

    if (success == NETSTATS_NB_SUCCESS) {
        /* Number of tries is retries + original atempt */
        packet_etx = (retries + 1) * NETSTATS_NB_ETX_DIVISOR;
    }
    else {
        packet_etx = NETSTATS_NB_ETX_NOACK_PENALTY * NETSTATS_NB_ETX_DIVISOR;
    }

    DEBUG("L2 neighbor: Calculated ETX of %u\n", packet_etx);
    /* Exponential weighted moving average */
    stats->etx = ((uint32_t)stats->etx *
                  (NETSTATS_NB_EWMA_SCALE - ewma_alpha) +
                  (uint32_t)packet_etx * ewma_alpha
                  ) / NETSTATS_NB_EWMA_SCALE;
}

void netstats_nb_incr_freshness(netstats_nb_t *stats)
{
    timex_t cur;

    xtimer_now_timex(&cur);
    /* First halve the freshness if applicable */
    netstats_nb_half_freshness(stats, &cur);
    /* Increment the freshness capped at FRESHNESS_MAX */
    stats->freshness = (stats->freshness >= NETSTATS_NB_FRESHNESS_MAX) ? \
                       NETSTATS_NB_FRESHNESS_MAX : stats->freshness + 1;
    stats->last_updated = cur.seconds;
}

bool netstats_nb_isfresh(netstats_nb_t *stats)
{
    timex_t cur;

    xtimer_now_timex(&cur);
    /* Half freshness if applicable to update to current freshness */
    netstats_nb_half_freshness(stats, &cur);
    return (stats->freshness >= NETSTATS_NB_FRESHNESS_TARGET && \
            cur.seconds - stats->last_updated < NETSTATS_NB_FRESHNESS_EXPIRATION);
}

void netstats_nb_half_freshness(netstats_nb_t *stats, timex_t *cur)
{
    uint8_t diff;

    diff = (cur->seconds - stats->last_halved) / NETSTATS_NB_FRESHNESS_HALF;
    stats->freshness >>= diff;
    if (diff) {
        /* Set to the last time point where this should have been halved */
        stats->last_halved = cur->seconds - diff;
    }
}

static bool l2_addr_equal(const uint8_t *a, uint8_t a_len, const uint8_t *b, uint8_t b_len)
{
    if (a_len != b_len) {
        return false;
    }
    if (memcmp(a, b, a_len) == 0) {
        return true;
    }
    return false;
}
