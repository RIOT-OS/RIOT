/*
 * Copyright (C) 2018 University of California, Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       CSMA Implementation for AT86RF2xx without Deaf Listening
 *
 * @author      Sam Kumar <samkumar@cs.berkeley.edu>
 *
 * @}
 */

#include "net/gnrc.h"
#include "net/gnrc/netdev.h"
#include "net/netdev.h"
#include "random.h"
#include "xtimer.h"

#include "at86rf2xx.h"
#include "at86rf2xx_csma.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef AT86RF2XX_SOFTWARE_CSMA_MIN_BACKOFF_EXP
#define AT86RF2XX_SOFTWARE_CSMA_MIN_BACKOFF_EXP 3
#endif

#ifndef AT86RF2XX_SOFTWARE_CSMA_MAX_BACKOFF_EXP
#define AT86RF2XX_SOFTWARE_CSMA_MAX_BACKOFF_EXP 5
#endif

#ifndef AT86RF2XX_SOFTWARE_CSMA_MAX_TRIES
#define AT86RF2XX_SOFTWARE_CSMA_MAX_TRIES 5
#endif

#ifndef AT86RF2XX_SOFTWARE_CSMA_BACKOFF_MICROS
#define AT86RF2XX_SOFTWARE_CSMA_BACKOFF_MICROS 320
#endif

#ifndef AT86RF2XX_SOFTWARE_LINK_MAX_TRIES
#define AT86RF2XX_SOFTWARE_LINK_MAX_TRIES 5
#endif

#ifndef AT86RF2XX_SOFTWARE_LINK_RETRY_DELAY_MICROS
#define AT86RF2XX_SOFTWARE_LINK_RETRY_DELAY_MICROS 10000
#endif

#ifdef AT86RF2XX_SOFTWARE_CSMA

bool at86rf2xx_csma_probe_and_send_if_pending(at86rf2xx_t* dev) {
    if (dev->csma_should_probe_and_send) {
        dev->csma_should_probe_and_send = false;


        bool success = at86rf2xx_tx_prepare(dev);
        if (!success) {
            dev->csma_should_probe_and_send = true;
            return false;
        }
        at86rf2xx_tx_load(dev, dev->csma_buf, dev->csma_buf_len, 0);
        at86rf2xx_tx_exec(dev);
    }

    return true;
}

bool at86rf2xx_csma_clear_pending(at86rf2xx_t* dev) {
    bool pending = dev->csma_should_probe_and_send;
    dev->csma_should_probe_and_send = false;
    return pending;
}

static void try_send_packet(void* arg) {
    at86rf2xx_t* dev = arg;
    dev->csma_should_probe_and_send = true;

    int state = irq_disable();
    dev->pending_irq++;
    irq_restore(state);

    /*
     * arg is actually of the type at86rf2xx_t*, but at86rf2xx_t actually
     * "inherits" netdev_t, so this is OK.
     */
    netdev_t* ndev = arg;

    if (ndev->event_callback) {
        ndev->event_callback(ndev, NETDEV_EVENT_ISR);
    }
}

static void csma_backoff_and_send(at86rf2xx_t* dev, uint32_t additional_delay_micros) {
    uint8_t csma_try_index = AT86RF2XX_SOFTWARE_CSMA_MAX_TRIES - dev->csma_num_tries_left;
    uint8_t be = AT86RF2XX_SOFTWARE_CSMA_MIN_BACKOFF_EXP + csma_try_index;
    if (be > AT86RF2XX_SOFTWARE_CSMA_MAX_BACKOFF_EXP) {
        be = AT86RF2XX_SOFTWARE_CSMA_MAX_BACKOFF_EXP;
    }
    uint32_t max_possible_csma_backoff = ((uint32_t) AT86RF2XX_SOFTWARE_CSMA_BACKOFF_MICROS) << be;
    if (max_possible_csma_backoff == 0 && additional_delay_micros == 0) {
        try_send_packet(dev);
    } else {
        uint32_t csma_backoff_micros = random_uint32_range(0, max_possible_csma_backoff);
        uint32_t total_delay = csma_backoff_micros + additional_delay_micros;
        xtimer_set(&dev->csma_backoff_timer, total_delay);
    }
}

int at86rf2xx_csma_load(at86rf2xx_t* dev, uint8_t* frame, size_t frame_len, size_t offset) {
    assert(!dev->csma_in_progress);

    if (offset + frame_len > AT86RF2XX_MAX_PKT_LENGTH) {
        return -EOVERFLOW;
    }

    memcpy(&dev->csma_buf[offset], frame, frame_len);
    dev->csma_buf_len = offset + frame_len;
    return 0;
}

void perform_link_try_with_csma(at86rf2xx_t* dev, uint32_t additional_delay_micros) {
    assert(!dev->csma_in_progress);

    dev->csma_num_tries_left = AT86RF2XX_SOFTWARE_CSMA_MAX_TRIES;
    dev->csma_in_progress = true;
    dev->csma_should_probe_and_send = false;

    csma_backoff_and_send(dev, additional_delay_micros);
}

void at86rf2xx_csma_send(at86rf2xx_t* dev, uint8_t num_link_tries) {
    assert(!dev->link_tx_in_progress);
    if (num_link_tries == 0) {
        num_link_tries = AT86RF2XX_SOFTWARE_LINK_MAX_TRIES;
    }

    dev->csma_backoff_timer.arg = dev;
    dev->link_tx_num_tries_left = num_link_tries;
    dev->link_tx_in_progress = true;

    perform_link_try_with_csma(dev, 0);
}

void at86rf2xx_csma_csma_try_succeeded(at86rf2xx_t* dev) {
    assert(!dev->csma_should_probe_and_send);
    dev->csma_in_progress = false;
    dev->csma_num_tries_left = 0;
}

bool at86rf2xx_csma_csma_try_failed(at86rf2xx_t* dev) {
    assert(!dev->csma_should_probe_and_send);
    dev->csma_num_tries_left--;
    if (dev->csma_num_tries_left == 0) {
        dev->csma_in_progress = false;

        // Don't try again; inform upper layer of channel access failure.
        return false;
    }

    // Perform the next CSMA retry after a backoff.
    csma_backoff_and_send(dev, 0);
    return true;
}

void at86rf2xx_csma_link_tx_try_succeeded(at86rf2xx_t* dev) {
    assert(!dev->csma_in_progress);
    assert(dev->link_tx_in_progress);
    dev->link_tx_in_progress = false;
    dev->link_tx_num_tries_left = 0;
}

bool at86rf2xx_csma_link_tx_try_failed(at86rf2xx_t* dev) {
    assert(!dev->csma_in_progress);
    assert(dev->link_tx_in_progress);
    dev->link_tx_num_tries_left--;
    if (dev->link_tx_num_tries_left == 0) {
        dev->link_tx_in_progress = false;

        // Don't try again; inform upper layer that transmission failed.
        return false;
    }

    // Perform the next link retransmission.
    perform_link_try_with_csma(dev, AT86RF2XX_SOFTWARE_LINK_RETRY_DELAY_MICROS);
    return true;
}

void at86rf2xx_csma_init(at86rf2xx_t* dev) {
    memset(&dev->csma_backoff_timer, 0x00, sizeof(dev->csma_backoff_timer));
    dev->csma_backoff_timer.callback = try_send_packet;
    dev->csma_in_progress = false;
    dev->csma_num_tries_left = 0;
    dev->link_tx_in_progress = false;
    dev->link_tx_num_tries_left = 0;
}

#endif
