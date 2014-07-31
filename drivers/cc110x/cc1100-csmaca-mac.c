/******************************************************************************
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
*******************************************************************************/

/**
 * @file
 * @ingroup     dev_cc110x
 * @brief       ScatterWeb MSB-A2 mac-layer
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 2128 $
 *
 * @note        $Id: cc1100-csmaca-mac.c 2128 2010-05-12 12:07:59Z hillebra $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc1100.h"
#include "cc1100_phy.h"
#include "cc1100-csmaca-mac.h"
#include "protocol-multiplex.h"

#include "hwtimer.h"
#include "vtimer.h"

/*---------------------------------------------------------------------------*/

#define COLLISION_STATE_INITIAL 1
#define COLLISION_STATE_MEASURE 2
#define COLLISION_STATE_KEEP    3

static uint32_t send_csmaca_calls = 0;
static uint32_t send_csmaca_calls_cs_timeout = 0;
static int collision_count;
static double collisions_per_sec = 0;
static int collision_state = COLLISION_STATE_INITIAL;
static timex_t collision_measurement_start;

static volatile int cs_hwtimer_id = -1;
static volatile int cs_timeout_flag = 0;

/*---------------------------------------------------------------------------*/
static void cs_timeout_cb(void *ptr)
{
    (void) ptr;
    cs_timeout_flag = 1;
}

/*---------------------------------------------------------------------------*/
int cc1100_send_csmaca(radio_address_t address, protocol_t protocol, int priority, char *payload, int payload_len)
{
    uint16_t min_window_size;
    uint16_t max_window_size;
    uint16_t difs;
    uint16_t slottime;

    switch(priority) {
        case PRIORITY_ALARM:
            min_window_size = PRIO_ALARM_MIN_WINDOW_SIZE;
            max_window_size = PRIO_ALARM_MAX_WINDOW_SIZE;
            difs = PRIO_ALARM_DIFS;
            slottime = PRIO_ALARM_SLOTTIME;
            break;

        case PRIORITY_WARNING:
            min_window_size = PRIO_WARN_MIN_WINDOW_SIZE;
            max_window_size = PRIO_WARN_MAX_WINDOW_SIZE;
            difs = PRIO_WARN_DIFS;
            slottime = PRIO_WARN_SLOTTIME;
            break;

        default:
            min_window_size = PRIO_DATA_MIN_WINDOW_SIZE;
            max_window_size = PRIO_DATA_MAX_WINDOW_SIZE;
            difs = PRIO_DATA_DIFS;
            slottime = PRIO_DATA_SLOTTIME;
    }

    /* Calculate collisions per second */
    if (collision_state == COLLISION_STATE_INITIAL) {
        vtimer_now(&collision_measurement_start);
        collision_count = 0;
        collisions_per_sec = 0;
        collision_state = COLLISION_STATE_MEASURE;
    }
    else if (collision_state == COLLISION_STATE_MEASURE) {
        timex_t now;
        vtimer_now(&now);
        timex_t timespan = timex_sub(now, collision_measurement_start);

        if (timex_cmp(timespan, timex_set(1, 0)) > 0) {
            collisions_per_sec = (collision_count * 1000000) / (double) timex_uint64(timespan);

            if (collisions_per_sec > 0.5 && collisions_per_sec <= 2.2) {
                timex_t now;
                vtimer_now(&now);
                collision_measurement_start = now;
                collision_state = COLLISION_STATE_KEEP;
            }
            else if (collisions_per_sec > 2.2) {
                timex_t now;
                vtimer_now(&now);
                collision_measurement_start = now;
                collision_state = COLLISION_STATE_KEEP;
            }
            else {
                collision_state = COLLISION_STATE_INITIAL;
            }
        }
    }
    else if (collision_state == COLLISION_STATE_KEEP) {
        timex_t now;
        vtimer_now(&now);
        timex_t timespan = timex_sub(now, collision_measurement_start);

        if (timex_cmp(timespan, timex_set(5, 0)) > 0) {
            collision_state = COLLISION_STATE_INITIAL;
        }
    }

    /* Adjust initial window size according to collision rate */
    if (collisions_per_sec > 0.5 && collisions_per_sec <= 2.2) {
        min_window_size *= 2;
    }
    else if (collisions_per_sec > 2.2) {
        min_window_size *= 4;
    }

    uint16_t windowSize = min_window_size;      /* Start with window size of PRIO_XXX_MIN_WINDOW_SIZE */
    uint16_t backoff = 0;                       /* Backoff between 1 and windowSize */
    uint32_t total;                             /* Holds the total wait time before send try */
    uint32_t cs_timeout;                        /* Current carrier sense timeout value */

    if (protocol == 0) {
        return RADIO_INVALID_PARAM;             /* Not allowed, protocol id must be greater zero */
    }

    cc1100_phy_mutex_lock();                    /* Lock radio for exclusive access */

    /* Get carrier sense timeout based on overall error rate till now */
    send_csmaca_calls++;
    int fail_percentage = (send_csmaca_calls_cs_timeout * 100) / send_csmaca_calls;

    if (fail_percentage == 0) {
        fail_percentage = 1;
    }

    cs_timeout = CARRIER_SENSE_TIMEOUT / fail_percentage;

    if (cs_timeout < CARRIER_SENSE_TIMEOUT_MIN) {
        cs_timeout = CARRIER_SENSE_TIMEOUT_MIN;
    }

    cc1100_cs_init();                           /* Initialize carrier sensing */

window:

    if (backoff != 0) {
        goto cycle;    /* If backoff was 0 */
    }

    windowSize *= 2;                        /* ...double the current window size */

    if (windowSize > max_window_size) {
        windowSize = max_window_size;       /* This is the maximum size allowed */
    }

    backoff = rand() % windowSize;          /* ...and choose new backoff */

    backoff += (uint16_t) 1;
cycle:
    cs_timeout_flag = 0;                    /* Carrier sense timeout flag */
    cs_hwtimer_id = hwtimer_set(cs_timeout, /* Set hwtimer to set CS timeout flag */
                                cs_timeout_cb, NULL);

    while (cc1100_cs_read()) {          /* Wait until air is free */
        if (cs_timeout_flag) {
            send_csmaca_calls_cs_timeout++;
#ifndef CSMACA_MAC_AGGRESSIVE_MODE
            cc1100_phy_mutex_unlock();
            cc1100_go_after_tx();           /* Go from RX to default mode */
            return RADIO_CS_TIMEOUT;        /* Return immediately */
#endif
#ifdef CSMACA_MAC_AGGRESSIVE_MODE
            goto send;                      /* Send anyway */
#endif
        }
    }

    hwtimer_remove(cs_hwtimer_id);          /* Remove hwtimer */
    cc1100_cs_write_cca(1);                 /* Air is free now */
    cc1100_cs_set_enabled(true);

    if (cc1100_cs_read()) {
        goto window;    /* GDO0 triggers on rising edge, so */
    }

    /* test once after interrupt is enabled */
    if (backoff > 0) {
        backoff--;    /* Decrement backoff counter */
    }

    total = slottime;                       /* Calculate total wait time */
    total *= (uint32_t)backoff;             /* Slot vector set */
    total += difs;                          /* ...and standard DIFS wait time */
    cs_timeout_flag = 0;                    /* Carrier sense timeout flag */
    cs_hwtimer_id = hwtimer_set(total,      /* Set hwtimer to set CS timeout flag */
                                cs_timeout_cb, NULL);

    while (!cs_timeout_flag
          || !cc1100_cs_read_cca()) {   /* Wait until timeout is finished */
        if (cc1100_cs_read_cca() == 0) {    /* Is the air still free? */
            hwtimer_remove(cs_hwtimer_id);
            goto window;                    /* No. Go back to new wait period. */
        }
    }

    cc1100_cs_set_enabled(false);
#ifdef CSMACA_MAC_AGGRESSIVE_MODE
send:
#endif
    int res = cc1100_send(address, protocol, priority, payload, payload_len);

    if (res < 0) {
        collision_count++;
    }

    return res;
}
