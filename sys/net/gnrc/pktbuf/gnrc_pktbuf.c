/*
 * Copyright (C) 2017 Freie Universität Berlin
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

#include "mutex.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/tx_sync.h"

#include "pktbuf_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

mutex_t gnrc_pktbuf_mutex = MUTEX_INIT;

gnrc_pktsnip_t *gnrc_pktbuf_remove_snip(gnrc_pktsnip_t *pkt,
                                        gnrc_pktsnip_t *snip)
{
    pkt = gnrc_pkt_delete(pkt, snip);
    snip->next = NULL;
    gnrc_pktbuf_release(snip);

    return pkt;
}

gnrc_pktsnip_t *gnrc_pktbuf_reverse_snips(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *reversed = NULL, *ptr = pkt;

    while (ptr != NULL) {
        gnrc_pktsnip_t *next;

        /* try to write-protect snip as its next-pointer is changed below */
        pkt = gnrc_pktbuf_start_write(ptr); /* use pkt as temporary variable */
        if (pkt == NULL) {
            gnrc_pktbuf_release(reversed);
            gnrc_pktbuf_release(ptr);
            return NULL;
        }
        /* switch around pointers */
        next = pkt->next;
        pkt->next = reversed;
        reversed = pkt;
        ptr = next;
    }
    return reversed;
}

int gnrc_pktbuf_merge(gnrc_pktsnip_t *pkt)
{
    size_t offset = pkt->size;
    size_t size = gnrc_pkt_len(pkt);
    int res = 0;

    if (pkt->size == size) {
        return res;
    }

    /* Re-allocate data */
    res = gnrc_pktbuf_realloc_data(pkt, size);
    if (res != 0) {
        return res;
    }

    /* Copy data to new buffer */
    for (gnrc_pktsnip_t *ptr = pkt->next; ptr != NULL; ptr = ptr->next) {
        memcpy(((uint8_t *)pkt->data) + offset, ptr->data, ptr->size);
        offset += ptr->size;
    }

    /* Release old pktsnips and data*/
    gnrc_pktbuf_release(pkt->next);
    pkt->next = NULL;
    return res;
}

void gnrc_pktbuf_release_error(gnrc_pktsnip_t *pkt, uint32_t err)
{
    mutex_lock(&gnrc_pktbuf_mutex);
    while (pkt) {
        gnrc_pktsnip_t *tmp;
        assert(gnrc_pktbuf_contains(pkt));
        assert(pkt->users > 0);
        tmp = pkt->next;

        /* if the memory was freed, memory has been overwritten by CANARY */
        if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE &&
            pkt->users == GNRC_PKTBUF_CANARY) {
            puts("gnrc_pktbuf: double free detected\n");
            DEBUG_BREAKPOINT(3);
        }

        if (pkt->users == 1) {
            pkt->users = 0; /* not necessary but to be on the safe side */
            if (!IS_USED(MODULE_GNRC_TX_SYNC)
                || (pkt->type != GNRC_NETTYPE_TX_SYNC)) {
                gnrc_pktbuf_free_internal(pkt->data, pkt->size);
            }
            else {
                gnrc_tx_complete(pkt);
            }
            gnrc_pktbuf_free_internal(pkt, sizeof(gnrc_pktsnip_t));
        }
        else {
            pkt->users--;
        }
        DEBUG("pktbuf: report status code %" PRIu32 "\n", err);
        gnrc_neterr_report(pkt, err);
        pkt = tmp;
    }
    mutex_unlock(&gnrc_pktbuf_mutex);
}

/** @} */
