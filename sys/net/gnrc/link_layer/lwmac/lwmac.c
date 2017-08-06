/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_lwmac
 * @{
 *
 * @file
 * @brief       Implementation of the LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel_types.h"
#include "msg.h"
#include "thread.h"
#include "timex.h"
#include "random.h"
#include "periph/rtt.h"
#include "net/gnrc.h"
#include "net/netdev.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/lwmac/types.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "net/gnrc/mac/internal.h"
#include "net/gnrc/lwmac/timeout.h"
#include "include/tx_state_machine.h"
#include "include/rx_state_machine.h"
#include "include/lwmac_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef LOG_LEVEL
/**
 * @brief Default log level define
 */
#define LOG_LEVEL LOG_WARNING
#endif

#include "log.h"

/**
 * @brief  LWMAC thread's PID
 */
kernel_pid_t lwmac_pid;

static void rtt_cb(void *arg);
static void lwmac_set_state(gnrc_netdev_t *gnrc_netdev, gnrc_lwmac_state_t newstate);
static void lwmac_schedule_update(gnrc_netdev_t *gnrc_netdev);
static void rtt_handler(uint32_t event, gnrc_netdev_t *gnrc_netdev);

static gnrc_mac_tx_neighbor_t *_next_tx_neighbor(gnrc_netdev_t *gnrc_netdev)
{
    int next = -1;

    uint32_t phase_nearest = GNRC_LWMAC_PHASE_MAX;

    for (int i = 0; i < GNRC_MAC_NEIGHBOR_COUNT; i++) {
        if (gnrc_priority_pktqueue_length(&gnrc_netdev->tx.neighbors[i].queue) > 0) {
            /* Unknown destinations are initialized with their phase at the end
             * of the local interval, so known destinations that still wakeup
             * in this interval will be preferred. */
            uint32_t phase_check = _gnrc_lwmac_ticks_until_phase(gnrc_netdev->tx.neighbors[i].phase);

            if (phase_check <= phase_nearest) {
                next = i;
                phase_nearest = phase_check;
                DEBUG("[LWMAC-int] Advancing queue #%d\n", i);
            }
        }
    }

    return (next < 0) ? NULL : &(gnrc_netdev->tx.neighbors[next]);
}

static uint32_t _next_inphase_event(uint32_t last, uint32_t interval)
{
    /* Counter did overflow since last wakeup */
    if (rtt_get_counter() < last) {
        /* TODO: Not sure if this was tested :) */
        uint32_t tmp = -last;
        tmp /= interval;
        tmp++;
        last += tmp * interval;
    }

    /* Add margin to next wakeup so that it will be at least 2ms in the future */
    while (last < (rtt_get_counter() + GNRC_LWMAC_RTT_EVENT_MARGIN_TICKS)) {
        last += interval;
    }

    return last;
}

inline void lwmac_schedule_update(gnrc_netdev_t *gnrc_netdev)
{
    gnrc_netdev_lwmac_set_reschedule(gnrc_netdev, true);
}

void lwmac_set_state(gnrc_netdev_t *gnrc_netdev, gnrc_lwmac_state_t newstate)
{
    gnrc_lwmac_state_t oldstate = gnrc_netdev->lwmac.state;

    if (newstate == oldstate) {
        return;
    }

    if (newstate >= GNRC_LWMAC_STATE_COUNT) {
        LOG_ERROR("ERROR: [LWMAC] Trying to set invalid state %u\n", newstate);
        return;
    }

    /* Already change state, but might be reverted to oldstate when needed */
    gnrc_netdev->lwmac.state = newstate;

    /* Actions when leaving old state */
    switch (oldstate) {
        case GNRC_LWMAC_RECEIVING:
        case GNRC_LWMAC_TRANSMITTING: {
            /* Enable duty cycling again */
            rtt_handler(GNRC_LWMAC_EVENT_RTT_RESUME, gnrc_netdev);
#if (GNRC_LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
            /* Output duty-cycle ratio */
            uint64_t duty;
            duty = (uint64_t) rtt_get_counter();
            duty = ((uint64_t) gnrc_netdev->lwmac.awake_duration_sum_ticks) * 100 /
                   (duty - (uint64_t)gnrc_netdev->lwmac.system_start_time_ticks);
            printf("[LWMAC]: achieved duty-cycle: %lu %% \n", (uint32_t)duty);
#endif
            break;
        }
        case GNRC_LWMAC_SLEEPING: {
            gnrc_lwmac_clear_timeout(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);
            break;
        }
        default:
            break;
    }

    /* Actions when entering new state */
    switch (newstate) {
        /*********************** Operation states *********************************/
        case GNRC_LWMAC_LISTENING: {
            _gnrc_lwmac_set_netdev_state(gnrc_netdev, NETOPT_STATE_IDLE);
            break;
        }
        case GNRC_LWMAC_SLEEPING: {
            /* Put transceiver to sleep */
            _gnrc_lwmac_set_netdev_state(gnrc_netdev, NETOPT_STATE_SLEEP);
            /* We may have come here through RTT handler, so timeout may still be active */
            gnrc_lwmac_clear_timeout(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);

            if (gnrc_netdev_lwmac_get_phase_backoff(gnrc_netdev)) {
                gnrc_netdev_lwmac_set_phase_backoff(gnrc_netdev, false);
                uint32_t alarm;

                rtt_clear_alarm();
                alarm = random_uint32_range(RTT_US_TO_TICKS((3 * GNRC_LWMAC_WAKEUP_DURATION_US / 2)),
                                            RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US -
                                                            (3 * GNRC_LWMAC_WAKEUP_DURATION_US / 2)));
                LOG_WARNING("WARNING: [LWMAC] phase backoffed: %lu us\n", RTT_TICKS_TO_US(alarm));
                gnrc_netdev->lwmac.last_wakeup = gnrc_netdev->lwmac.last_wakeup + alarm;
                alarm = _next_inphase_event(gnrc_netdev->lwmac.last_wakeup,
                                            RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US));
                rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            }

            /* Return immediately, so no rescheduling */
            return;
        }
        /* Trying to send data */
        case GNRC_LWMAC_TRANSMITTING: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, gnrc_netdev);    /**< No duty cycling while RXing */
            _gnrc_lwmac_set_netdev_state(gnrc_netdev, NETOPT_STATE_IDLE);  /**< Power up netdev */
            break;
        }
        /* Receiving incoming data */
        case GNRC_LWMAC_RECEIVING: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, gnrc_netdev);    /**< No duty cycling while TXing */
            _gnrc_lwmac_set_netdev_state(gnrc_netdev, NETOPT_STATE_IDLE);  /**< Power up netdev */
            break;
        }
        case GNRC_LWMAC_STOPPED: {
            _gnrc_lwmac_set_netdev_state(gnrc_netdev, NETOPT_STATE_OFF);
            break;
        }
        /*********************** Control states ***********************************/
        case GNRC_LWMAC_START: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_START, gnrc_netdev);
            lwmac_set_state(gnrc_netdev, GNRC_LWMAC_LISTENING);
            break;
        }
        case GNRC_LWMAC_STOP: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_STOP, gnrc_netdev);
            lwmac_set_state(gnrc_netdev, GNRC_LWMAC_STOPPED);
            break;
        }
        case GNRC_LWMAC_RESET: {
            LOG_WARNING("WARNING: [LWMAC] Reset not yet implemented\n");
            lwmac_set_state(gnrc_netdev, GNRC_LWMAC_STOP);
            lwmac_set_state(gnrc_netdev, GNRC_LWMAC_START);
            break;
        }
        /**************************************************************************/
        default: {
            LOG_DEBUG("[LWMAC] No actions for entering state %u\n", newstate);
            return;
        }
    }

    lwmac_schedule_update(gnrc_netdev);
}

static void _sleep_management(gnrc_netdev_t *gnrc_netdev)
{
    /* If a packet is scheduled, no other (possible earlier) packet can be
     * sent before the first one is handled, even no broadcast
     */
    if (!gnrc_lwmac_timeout_is_running(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP)) {
        gnrc_mac_tx_neighbor_t *neighbour;

        /* Check if there is packet remaining for retransmission */
        if (gnrc_netdev->tx.current_neighbor != NULL) {
            neighbour = gnrc_netdev->tx.current_neighbor;
        }
        else {
            /* Check if there are broadcasts to send and transmit immediately */
            if (gnrc_priority_pktqueue_length(&(gnrc_netdev->tx.neighbors[0].queue)) > 0) {
                gnrc_netdev->tx.current_neighbor = &(gnrc_netdev->tx.neighbors[0]);
                lwmac_set_state(gnrc_netdev, GNRC_LWMAC_TRANSMITTING);
                return;
            }
            neighbour = _next_tx_neighbor(gnrc_netdev);
        }

        if (neighbour != NULL) {
            /* if phase is unknown, send immediately. */
            if (neighbour->phase > RTT_TICKS_TO_US(GNRC_LWMAC_WAKEUP_INTERVAL_US)) {
                gnrc_netdev->tx.current_neighbor = neighbour;
                gnrc_netdev_lwmac_set_tx_continue(gnrc_netdev, false);
                gnrc_netdev->tx.tx_burst_count = 0;
                lwmac_set_state(gnrc_netdev, GNRC_LWMAC_TRANSMITTING);
                return;
            }

            /* Offset in microseconds when the earliest (phase) destination
             * node wakes up that we have packets for. */
            int time_until_tx = RTT_TICKS_TO_US(_gnrc_lwmac_ticks_until_phase(neighbour->phase));

            /* If there's not enough time to prepare a WR to catch the phase
             * postpone to next interval */
            if (time_until_tx < GNRC_LWMAC_WR_PREPARATION_US) {
                time_until_tx += GNRC_LWMAC_WAKEUP_INTERVAL_US;
            }
            time_until_tx -= GNRC_LWMAC_WR_PREPARATION_US;

            /* add a random time before goto TX, for avoiding one node for
             * always holding the medium (if the receiver's phase is recorded earlier in this
             * particular node) */
            uint32_t random_backoff;
            random_backoff = random_uint32_range(0, GNRC_LWMAC_TIME_BETWEEN_WR_US);
            time_until_tx = time_until_tx + random_backoff;

            gnrc_lwmac_set_timeout(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP, time_until_tx);

            /* Register neighbour to be the next */
            gnrc_netdev->tx.current_neighbor = neighbour;

            /* Stop dutycycling, we're preparing to send. This prevents the
             * timeout arriving late, so that the destination phase would
             * be missed. */
            /* TODO: bad for power savings */
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, gnrc_netdev);
        }
    }
    else if (gnrc_lwmac_timeout_is_expired(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP)) {
        LOG_DEBUG("[LWMAC] Got timeout for dest wakeup, ticks: %" PRIu32 "\n", rtt_get_counter());
        gnrc_netdev_lwmac_set_tx_continue(gnrc_netdev, false);
        gnrc_netdev->tx.tx_burst_count = 0;
        lwmac_set_state(gnrc_netdev, GNRC_LWMAC_TRANSMITTING);
    }
}

static void _rx_management_failed(gnrc_netdev_t *gnrc_netdev)
{
    /* This may happen frequently because we'll receive WA from
     * every node in range. */
    LOG_DEBUG("[LWMAC] Reception was NOT successful\n");
    gnrc_lwmac_rx_stop(gnrc_netdev);

    if (gnrc_netdev->rx.rx_bad_exten_count >= GNRC_LWMAC_MAX_RX_EXTENSION_NUM) {
        gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, true);
    }

    /* Here we check if we are close to the end of the cycle. If yes,
     * go to sleep. Firstly, get the relative phase. */
    uint32_t phase = rtt_get_counter();
    if (phase < gnrc_netdev->lwmac.last_wakeup) {
        phase = (RTT_US_TO_TICKS(GNRC_LWMAC_PHASE_MAX) - gnrc_netdev->lwmac.last_wakeup) +
                 phase;
    }
    else {
        phase = phase - gnrc_netdev->lwmac.last_wakeup;
    }
    /* If the relative phase is beyond 4/5 cycle time, go to sleep. */
    if (phase > (4*RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US)/5)) {
        gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, true);
    }

    if (gnrc_netdev_lwmac_get_quit_rx(gnrc_netdev)) {
        lwmac_set_state(gnrc_netdev, GNRC_LWMAC_SLEEPING);
    }
    else {
        /* Go back to LISTENING for keep hearing on the channel */
        lwmac_set_state(gnrc_netdev, GNRC_LWMAC_LISTENING);
    }
}

static void _rx_management_success(gnrc_netdev_t *gnrc_netdev)
{
    LOG_DEBUG("[LWMAC] Reception was successful\n");
    gnrc_lwmac_rx_stop(gnrc_netdev);
    /* Dispatch received packets, timing is not critical anymore */
    gnrc_mac_dispatch(&gnrc_netdev->rx);

    /* Here we check if we are close to the end of the cycle. If yes,
     * go to sleep. Firstly, get the relative phase. */
    uint32_t phase = rtt_get_counter();
    if (phase < gnrc_netdev->lwmac.last_wakeup) {
        phase = (RTT_US_TO_TICKS(GNRC_LWMAC_PHASE_MAX) - gnrc_netdev->lwmac.last_wakeup) +
                 phase;
    }
    else {
        phase = phase - gnrc_netdev->lwmac.last_wakeup;
    }
    /* If the relative phase is beyond 4/5 cycle time, go to sleep. */
    if (phase > (4*RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US)/5)) {
        gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, true);
    }

    if (gnrc_netdev_lwmac_get_quit_rx(gnrc_netdev)) {
        lwmac_set_state(gnrc_netdev, GNRC_LWMAC_SLEEPING);
    }
    else {
        /* Go back to LISTENING after successful reception */
        lwmac_set_state(gnrc_netdev, GNRC_LWMAC_LISTENING);
    }
}
static void _rx_management(gnrc_netdev_t *gnrc_netdev)
{
    gnrc_lwmac_rx_state_t state_rx = gnrc_netdev->rx.state;

    switch (state_rx) {
        case GNRC_LWMAC_RX_STATE_STOPPED: {
            gnrc_lwmac_rx_start(gnrc_netdev);
            gnrc_lwmac_rx_update(gnrc_netdev);
            break;
        }
        case GNRC_LWMAC_RX_STATE_FAILED: {
            _rx_management_failed(gnrc_netdev);
            break;
        }
        case GNRC_LWMAC_RX_STATE_SUCCESSFUL: {
            _rx_management_success(gnrc_netdev);
            break;
        }
        default:
            gnrc_lwmac_rx_update(gnrc_netdev);
    }

    /* If state has changed, reschedule main state machine */
    if (state_rx != gnrc_netdev->rx.state) {
        lwmac_schedule_update(gnrc_netdev);
    }
}

static void _tx_management_stopped(gnrc_netdev_t *gnrc_netdev)
{
    gnrc_pktsnip_t *pkt;

    /* If there is packet remaining for retransmission,
     * retransmit it (i.e., the retransmission scheme of LWMAC). */
    if (gnrc_netdev->tx.packet != NULL) {
        LOG_WARNING("WARNING: [LWMAC] TX %d times retry\n",
                         gnrc_netdev->tx.tx_retry_count);
        gnrc_netdev->tx.state = GNRC_LWMAC_TX_STATE_INIT;
        gnrc_netdev->tx.wr_sent = 0;
        gnrc_lwmac_tx_update(gnrc_netdev);
    }
    else {
        if ((pkt = gnrc_priority_pktqueue_pop(
                 &gnrc_netdev->tx.current_neighbor->queue))) {
            gnrc_netdev->tx.tx_retry_count = 0;
            gnrc_lwmac_tx_start(gnrc_netdev, pkt, gnrc_netdev->tx.current_neighbor);
            gnrc_lwmac_tx_update(gnrc_netdev);
        }
        else {
            /* Shouldn't happen, but never observed this case */
            lwmac_set_state(gnrc_netdev, GNRC_LWMAC_SLEEPING);
        }
    }
}

static void _tx_management_success(gnrc_netdev_t *gnrc_netdev)
{
    if (gnrc_netdev->tx.current_neighbor == &(gnrc_netdev->tx.neighbors[0])) {
        LOG_INFO("[LWMAC] Broadcast transmission done\n");
    }

    gnrc_lwmac_tx_stop(gnrc_netdev);

    /* In case have pending packets for the same receiver, continue to
     * send immediately, before the maximum transmit-limit */
    if ((gnrc_netdev_lwmac_get_tx_continue(gnrc_netdev)) &&
        (gnrc_netdev->tx.tx_burst_count < GNRC_LWMAC_MAX_TX_BURST_PKT_NUM)) {
        lwmac_schedule_update(gnrc_netdev);
    }
    else {
        lwmac_set_state(gnrc_netdev, GNRC_LWMAC_SLEEPING);
    }
}

static void _tx_management(gnrc_netdev_t *gnrc_netdev)
{
    gnrc_lwmac_tx_state_t state_tx = gnrc_netdev->tx.state;

    switch (state_tx) {
        case GNRC_LWMAC_TX_STATE_STOPPED: {
            _tx_management_stopped(gnrc_netdev);
            break;
        }
        case GNRC_LWMAC_TX_STATE_FAILED: {
            /* If transmission failure, do not try burst transmissions and quit other
             * transmission attempts in this cycle for collision avoidance */
            gnrc_netdev_lwmac_set_tx_continue(gnrc_netdev, false);
            gnrc_netdev_lwmac_set_quit_tx(gnrc_netdev, true);
            /* falls through */
            /* TX packet will therefore be dropped. No automatic resending here,
             * we did our best.
             */
        }
        case GNRC_LWMAC_TX_STATE_SUCCESSFUL: {
            _tx_management_success(gnrc_netdev);
            break;
        }
        default:
            gnrc_lwmac_tx_update(gnrc_netdev);
    }

    /* If state has changed, reschedule main state machine */
    if (state_tx != gnrc_netdev->tx.state) {
        lwmac_schedule_update(gnrc_netdev);
    }
}

static void _lwmac_update_listening(gnrc_netdev_t *gnrc_netdev)
{
    /* In case has pending packet to send, clear rtt alarm thus to goto
     * transmission initialization (in SLEEPING management) right after the
     * listening period */
    if ((_next_tx_neighbor(gnrc_netdev) != NULL) ||
        (gnrc_netdev->tx.current_neighbor != NULL)) {
        rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, gnrc_netdev);
    }

    /* Set timeout for if there's no successful rx transaction that will
     * change state to SLEEPING. */
    if (!gnrc_lwmac_timeout_is_running(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD)) {
        gnrc_lwmac_set_timeout(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD, GNRC_LWMAC_WAKEUP_DURATION_US);
    }
    else if (gnrc_lwmac_timeout_is_expired(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD)) {
        /* Dispatch first as there still may be broadcast packets. */
        gnrc_mac_dispatch(&gnrc_netdev->rx);

        gnrc_netdev->lwmac.state = GNRC_LWMAC_SLEEPING;
        /* Enable duty cycling again */
        rtt_handler(GNRC_LWMAC_EVENT_RTT_RESUME, gnrc_netdev);

        _gnrc_lwmac_set_netdev_state(gnrc_netdev, NETOPT_STATE_SLEEP);
        gnrc_lwmac_clear_timeout(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);

        /* if there is a packet for transmission, schedule update to start
         * transmission initialization immediately. */
        gnrc_mac_tx_neighbor_t *neighbour = _next_tx_neighbor(gnrc_netdev);
        if ((neighbour != NULL) || (gnrc_netdev->tx.current_neighbor != NULL)) {
            /* This triggers packet sending procedure in sleeping immediately. */
            lwmac_schedule_update(gnrc_netdev);
            return;
        }
    }

    if (gnrc_priority_pktqueue_length(&gnrc_netdev->rx.queue) > 0) {
        /* Do wake-up extension in each packet reception. */
        gnrc_lwmac_clear_timeout(gnrc_netdev, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);
        lwmac_set_state(gnrc_netdev, GNRC_LWMAC_RECEIVING);
    }
}

/* Main state machine. Call whenever something happens */
static bool lwmac_update(gnrc_netdev_t *gnrc_netdev)
{
    gnrc_netdev_lwmac_set_reschedule(gnrc_netdev, false);

    switch (gnrc_netdev->lwmac.state) {
        case GNRC_LWMAC_SLEEPING: {
            /* Quit scheduling transmission if 'quit-tx' flag is found set, thus
             * to avoid potential collisions with ongoing transmissions of other
             * neighbor nodes */
            if (gnrc_netdev_lwmac_get_quit_tx(gnrc_netdev)) {
                return false;
            }

            _sleep_management(gnrc_netdev);
            break;
        }
        case GNRC_LWMAC_LISTENING: {
            _lwmac_update_listening(gnrc_netdev);
            break;
        }
        case GNRC_LWMAC_RECEIVING: {
            _rx_management(gnrc_netdev);
            break;
        }
        case GNRC_LWMAC_TRANSMITTING: {
            _tx_management(gnrc_netdev);
            break;
        }
        default:
            LOG_DEBUG("[LWMAC] No actions in state %u\n", gnrc_netdev->lwmac.state);
    }

    return gnrc_netdev_lwmac_get_reschedule(gnrc_netdev);
}

static void rtt_cb(void *arg)
{
    msg_t msg;

    msg.content.value = ((uint32_t) arg) & 0xffff;
    msg.type = GNRC_LWMAC_EVENT_RTT_TYPE;
    msg_send(&msg, lwmac_pid);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void rtt_handler(uint32_t event, gnrc_netdev_t *gnrc_netdev)
{
    uint32_t alarm;

    switch (event & 0xffff) {
        case GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING: {
            /* A new cycle starts, set sleep timing and initialize related MAC-info flags. */
            gnrc_netdev->lwmac.last_wakeup = rtt_get_alarm();
            alarm = _next_inphase_event(gnrc_netdev->lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_DURATION_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING);
            gnrc_netdev_lwmac_set_quit_tx(gnrc_netdev, false);
            gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, false);
            gnrc_netdev_lwmac_set_phase_backoff(gnrc_netdev, false);
            gnrc_netdev->rx.rx_bad_exten_count = 0;
            lwmac_set_state(gnrc_netdev, GNRC_LWMAC_LISTENING);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING: {
            /* Set next wake-up timing. */
            alarm = _next_inphase_event(gnrc_netdev->lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            lwmac_set_state(gnrc_netdev, GNRC_LWMAC_SLEEPING);
            break;
        }
        /* Set initial wake-up alarm that starts the cycle */
        case GNRC_LWMAC_EVENT_RTT_START: {
            LOG_DEBUG("[LWMAC] RTT: Initialize duty cycling\n");
            alarm = rtt_get_counter() + RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_DURATION_US);
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING);
            gnrc_netdev_lwmac_set_dutycycle_active(gnrc_netdev, true);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_STOP:
        case GNRC_LWMAC_EVENT_RTT_PAUSE: {
            rtt_clear_alarm();
            LOG_DEBUG("[LWMAC] RTT: Stop duty cycling, now in state %u\n",
                           gnrc_netdev->lwmac.state);
            gnrc_netdev_lwmac_set_dutycycle_active(gnrc_netdev, false);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_RESUME: {
            LOG_DEBUG("[LWMAC] RTT: Resume duty cycling\n");
            rtt_clear_alarm();
            alarm = _next_inphase_event(gnrc_netdev->lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            gnrc_netdev_lwmac_set_dutycycle_active(gnrc_netdev, true);
            break;
        }
        default:
            break;
    }
}

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    gnrc_netdev_t *gnrc_netdev = (gnrc_netdev_t *) dev->context;

    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;

        msg.type = NETDEV_MSG_TYPE_EVENT;
        msg.content.ptr = (void *) gnrc_netdev;

        if (msg_send(&msg, gnrc_netdev->pid) <= 0) {
            LOG_WARNING("WARNING: [LWMAC] gnrc_netdev: possibly lost interrupt.\n");
        }
    }
    else {
        DEBUG("gnrc_netdev: event triggered -> %i\n", event);
        switch (event) {
            case NETDEV_EVENT_RX_STARTED: {
                LOG_DEBUG("[LWMAC] NETDEV_EVENT_RX_STARTED\n");
                gnrc_netdev_set_rx_started(gnrc_netdev, true);
                break;
            }
            case NETDEV_EVENT_RX_COMPLETE: {
                LOG_DEBUG("[LWMAC] NETDEV_EVENT_RX_COMPLETE\n");

                gnrc_pktsnip_t *pkt = gnrc_netdev->recv(gnrc_netdev);

                /* Prevent packet corruption when a packet is sent before the previous
                 * received packet has been downloaded. This happens e.g. when a timeout
                 * expires that causes the tx state machine to send a packet. When a
                 * packet arrives after the timeout, the notification is queued but the
                 * tx state machine continues to send and then destroys the received
                 * packet in the frame buffer. After completion, the queued notification
                 * will be handled a corrupted packet will be downloaded. Therefore
                 * keep track that RX_STARTED is followed by RX_COMPLETE.
                 *
                 * TODO: transceivers might have 2 frame buffers, so make this optional
                 */
                if (pkt == NULL) {
                    gnrc_netdev_set_rx_started(gnrc_netdev, false);
                    break;
                }

                gnrc_netdev_set_rx_started(gnrc_netdev, false);

                if (!gnrc_mac_queue_rx_packet(&gnrc_netdev->rx, 0, pkt)) {
                    LOG_ERROR("ERROR: [LWMAC] Can't push RX packet @ %p, memory full?\n", pkt);
                    gnrc_pktbuf_release(pkt);
                    break;
                }
                lwmac_schedule_update(gnrc_netdev);
                break;
            }
            case NETDEV_EVENT_TX_STARTED: {
                gnrc_netdev_set_tx_feedback(gnrc_netdev, TX_FEEDBACK_UNDEF);
                gnrc_netdev_set_rx_started(gnrc_netdev, false);
                break;
            }
            case NETDEV_EVENT_TX_COMPLETE: {
                gnrc_netdev_set_tx_feedback(gnrc_netdev, TX_FEEDBACK_SUCCESS);
                gnrc_netdev_set_rx_started(gnrc_netdev, false);
                lwmac_schedule_update(gnrc_netdev);
                break;
            }
            case NETDEV_EVENT_TX_NOACK: {
                gnrc_netdev_set_tx_feedback(gnrc_netdev, TX_FEEDBACK_NOACK);
                gnrc_netdev_set_rx_started(gnrc_netdev, false);
                lwmac_schedule_update(gnrc_netdev);
                break;
            }
            case NETDEV_EVENT_TX_MEDIUM_BUSY: {
                gnrc_netdev_set_tx_feedback(gnrc_netdev, TX_FEEDBACK_BUSY);
                gnrc_netdev_set_rx_started(gnrc_netdev, false);
                lwmac_schedule_update(gnrc_netdev);
                break;
            }
            default:
                LOG_WARNING("WARNING: [LWMAC] Unhandled netdev event: %u\n", event);
        }
    }
}

/**
 * @brief   Startup code and event loop of the LWMAC layer
 *
 * @param[in] args          expects a pointer to the underlying netdev device
 *
 * @return                  never returns
 */
static void *_lwmac_thread(void *args)
{
    gnrc_netdev_t *gnrc_netdev = (gnrc_netdev_t *)args;
    netdev_t *dev = gnrc_netdev->dev;

    gnrc_netdev->pid = thread_getpid();

    gnrc_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[GNRC_LWMAC_IPC_MSG_QUEUE_SIZE];

    LOG_INFO("[LWMAC] Starting LWMAC\n");

    /* RTT is used for scheduling wakeup */
    rtt_init();

    /* Store pid globally, so that IRQ can use it to send msg */
    lwmac_pid = thread_getpid();

    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, GNRC_LWMAC_IPC_MSG_QUEUE_SIZE);

    /* register the event callback with the device driver */
    dev->event_callback = _event_cb;
    dev->context = (void *) gnrc_netdev;

    /* register the device to the network stack*/
    gnrc_netif_add(thread_getpid());

    /* initialize low-level driver */
    dev->driver->init(dev);

    /* Enable RX- and TX-started interrupts  */
    netopt_enable_t enable = NETOPT_ENABLE;
    dev->driver->set(dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

    uint16_t src_len = 8;
    dev->driver->set(dev, NETOPT_SRC_LEN, &src_len, sizeof(src_len));

    /* Get own address from netdev */
    gnrc_netdev->l2_addr_len = dev->driver->get(dev, NETOPT_ADDRESS_LONG,
                                                &gnrc_netdev->l2_addr,
                                                IEEE802154_LONG_ADDRESS_LEN);
    assert(gnrc_netdev->l2_addr_len > 0);

    /* Initialize broadcast sequence number. This at least differs from board
     * to board */
    gnrc_netdev->tx.bcast_seqnr = gnrc_netdev->l2_addr[0];

    /* Reset all timeouts just to be sure */
    gnrc_lwmac_reset_timeouts(gnrc_netdev);

    /* Start duty cycling */
    lwmac_set_state(gnrc_netdev, GNRC_LWMAC_START);

#if (GNRC_LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
    /* Start duty cycle recording */
    gnrc_netdev->lwmac.system_start_time_ticks = rtt_get_counter();
    gnrc_netdev->lwmac.last_radio_on_time_ticks = gnrc_netdev->lwmac.system_start_time_ticks;
    gnrc_netdev->lwmac.awake_duration_sum_ticks = 0;
    gnrc_netdev->lwmac.lwmac_info |= GNRC_LWMAC_RADIO_IS_ON;
#endif

    /* start the event loop */
    while (1) {
        msg_receive(&msg);

        /* Handle NETDEV, NETAPI, RTT and TIMEOUT messages */
        switch (msg.type) {
            /* RTT raised an interrupt */
            case GNRC_LWMAC_EVENT_RTT_TYPE: {
                if (gnrc_netdev_lwmac_get_dutycycle_active(gnrc_netdev)) {
                    rtt_handler(msg.content.value, gnrc_netdev);
                    lwmac_schedule_update(gnrc_netdev);
                }
                else {
                    LOG_DEBUG("[LWMAC] Ignoring late RTT event while dutycycling is off\n");
                }
                break;
            }
            /* An LWMAC timeout occured */
            case GNRC_LWMAC_EVENT_TIMEOUT_TYPE: {
                gnrc_lwmac_timeout_make_expire((gnrc_lwmac_timeout_t *) msg.content.ptr);
                lwmac_schedule_update(gnrc_netdev);
                break;
            }
            /* Transceiver raised an interrupt */
            case NETDEV_MSG_TYPE_EVENT: {
                LOG_DEBUG("[LWMAC] GNRC_NETDEV_MSG_TYPE_EVENT received\n");
                /* Forward event back to driver */
                dev->driver->isr(dev);
                break;
            }
            /* TX: Queue for sending */
            case GNRC_NETAPI_MSG_TYPE_SND: {
                /* TODO: how to announce failure to upper layers? */
                LOG_DEBUG("[LWMAC] GNRC_NETAPI_MSG_TYPE_SND received\n");
                gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) msg.content.ptr;

                if (!gnrc_mac_queue_tx_packet(&gnrc_netdev->tx, 0, pkt)) {
                    gnrc_pktbuf_release(pkt);
                    LOG_WARNING("WARNING: [LWMAC] TX queue full, drop packet\n");
                }

                lwmac_schedule_update(gnrc_netdev);
                break;
            }
            /* NETAPI set/get. Can't this be refactored away from here? */
            case GNRC_NETAPI_MSG_TYPE_SET: {
                LOG_DEBUG("[LWMAC] GNRC_NETAPI_MSG_TYPE_SET received\n");
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;

                /* Depending on option forward to NETDEV or handle here */
                switch (opt->opt) {
                    /* Handle state change requests */
                    case NETOPT_STATE: {
                        netopt_state_t *state = (netopt_state_t *) opt->data;
                        res = opt->data_len;
                        switch (*state) {
                            case NETOPT_STATE_OFF: {
                                lwmac_set_state(gnrc_netdev, GNRC_LWMAC_STOP);
                                break;
                            }
                            case NETOPT_STATE_IDLE: {
                                lwmac_set_state(gnrc_netdev, GNRC_LWMAC_START);
                                break;
                            }
                            case NETOPT_STATE_RESET: {
                                lwmac_set_state(gnrc_netdev, GNRC_LWMAC_RESET);
                                break;
                            }
                            default:
                                res = -EINVAL;
                                LOG_ERROR("ERROR: [LWMAC] NETAPI tries to set unsupported"
                                               " state %u\n",*state);
                        }
                        lwmac_schedule_update(gnrc_netdev);
                        break;
                    }
                    /* Forward to netdev by default*/
                    default:
                        /* set option for device driver */
                        res = dev->driver->set(dev, opt->opt, opt->data, opt->data_len);
                        LOG_DEBUG("[LWMAC] Response of netdev->set: %i\n", res);
                }

                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            }
            case GNRC_NETAPI_MSG_TYPE_GET: {
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                LOG_DEBUG("[LWMAC] GNRC_NETAPI_MSG_TYPE_GET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* get option from device driver */
                res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
                LOG_DEBUG("[LWMAC] Response of netdev->get: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            }
            default:
                LOG_ERROR("ERROR: [LWMAC] Unknown command %" PRIu16 "\n", msg.type);
                break;
        }

        /* Execute main state machine because something just happend*/
        while (gnrc_netdev_lwmac_get_reschedule(gnrc_netdev)) {
            lwmac_update(gnrc_netdev);
        }
    }

    LOG_ERROR("ERROR: [LWMAC] terminated\n");

    /* never reached */
    return NULL;
}

kernel_pid_t gnrc_lwmac_init(char *stack, int stacksize, char priority,
                             const char *name, gnrc_netdev_t *dev)
{
    kernel_pid_t res;

    /* check if given netdev device is defined and the driver is set */
    if (dev == NULL || dev->dev == NULL) {
        LOG_ERROR("ERROR: [LWMAC] No netdev supplied or driver not set\n");
        return -ENODEV;
    }

    /* create new LWMAC thread */
    res = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
                        _lwmac_thread, (void *)dev, name);
    if (res <= 0) {
        LOG_ERROR("ERROR: [LWMAC] Couldn't create thread\n");
        return -EINVAL;
    }

    return res;
}
