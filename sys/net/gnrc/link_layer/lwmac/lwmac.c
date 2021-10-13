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

#include "od.h"
#include "timex.h"
#include "random.h"
#include "periph/rtt.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc/lwmac/types.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "net/gnrc/mac/internal.h"
#include "net/gnrc/lwmac/timeout.h"
#include "include/tx_state_machine.h"
#include "include/rx_state_machine.h"
#include "include/lwmac_internal.h"

#ifndef LOG_LEVEL
/**
 * @brief Default log level define
 */
#define LOG_LEVEL LOG_WARNING
#endif

#include "log.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief  LWMAC thread's PID
 */
kernel_pid_t lwmac_pid;

static void rtt_cb(void *arg);
static void lwmac_set_state(gnrc_netif_t *netif, gnrc_lwmac_state_t newstate);
static void lwmac_schedule_update(gnrc_netif_t *netif);
static void rtt_handler(uint32_t event, gnrc_netif_t *netif);
static void _lwmac_init(gnrc_netif_t *netif);
static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);
static void _lwmac_msg_handler(gnrc_netif_t *netif, msg_t *msg);

static const gnrc_netif_ops_t lwmac_ops = {
    .init = _lwmac_init,
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
    .msg_handler = _lwmac_msg_handler,
};

int gnrc_netif_lwmac_create(gnrc_netif_t *netif, char *stack, int stacksize,
                            char priority, const char *name, netdev_t *dev)
{
    return gnrc_netif_create(netif, stack, stacksize, priority, name, dev,
                             &lwmac_ops);
}

static void lwmac_reinit_radio(gnrc_netif_t *netif)
{
    /* Initialize low-level driver. */
    netif->dev->driver->init(netif->dev);

    /* Set MAC address length. */
    uint16_t src_len = netif->l2addr_len;
    netif->dev->driver->set(netif->dev, NETOPT_SRC_LEN, &src_len, sizeof(src_len));

    /* Set the MAC address of the device. */
    if (netif->l2addr_len == IEEE802154_LONG_ADDRESS_LEN) {
        netif->dev->driver->set(netif->dev,
                                NETOPT_ADDRESS_LONG,
                                netif->l2addr,
                                sizeof(netif->l2addr));
    }
    else {
        netif->dev->driver->set(netif->dev,
                                NETOPT_ADDR_LEN,
                                netif->l2addr,
                                sizeof(netif->l2addr));
    }

    /* Check if RX-start and TX-started and TX-END interrupts are supported */
    if (IS_ACTIVE(DEVELHELP)) {
        netopt_enable_t enable;
        netif->dev->driver->get(netif->dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
        assert(enable == NETOPT_ENABLE);
        netif->dev->driver->get(netif->dev, NETOPT_RX_END_IRQ, &enable, sizeof(enable));
        assert(enable == NETOPT_ENABLE);
        netif->dev->driver->get(netif->dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));
        assert(enable == NETOPT_ENABLE);
    }
}

static gnrc_pktsnip_t *_make_netif_hdr(uint8_t *mhr)
{
    gnrc_pktsnip_t *snip;
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN], dst[IEEE802154_LONG_ADDRESS_LEN];
    int src_len, dst_len;
    le_uint16_t _pan_tmp;   /* TODO: hand-up PAN IDs to GNRC? */

    dst_len = ieee802154_get_dst(mhr, dst, &_pan_tmp);
    src_len = ieee802154_get_src(mhr, src, &_pan_tmp);
    if ((dst_len < 0) || (src_len < 0)) {
        DEBUG("_make_netif_hdr: unable to get addresses\n");
        return NULL;
    }
    /* allocate space for header */
    snip = gnrc_netif_hdr_build(src, (size_t)src_len, dst, (size_t)dst_len);
    if (snip == NULL) {
        DEBUG("_make_netif_hdr: no space left in packet buffer\n");
        return NULL;
    }
    /* set broadcast flag for broadcast destination */
    if ((dst_len == 2) && (dst[0] == 0xff) && (dst[1] == 0xff)) {
        gnrc_netif_hdr_t *hdr = snip->data;
        hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }
    return snip;
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    netdev_ieee802154_rx_info_t rx_info;
    netdev_ieee802154_t *state = container_of(dev, netdev_ieee802154_t, netdev);
    gnrc_pktsnip_t *pkt = NULL;
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);

    if (bytes_expected > 0) {
        int nread;

        pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected, GNRC_NETTYPE_UNDEF);
        if (pkt == NULL) {
            DEBUG("_recv_ieee802154: cannot allocate pktsnip.\n");
            return NULL;
        }
        nread = dev->driver->recv(dev, pkt->data, bytes_expected, &rx_info);
        if (nread <= 0) {
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        if (!(state->flags & NETDEV_IEEE802154_RAW)) {
            gnrc_pktsnip_t *ieee802154_hdr, *netif_hdr;
            gnrc_netif_hdr_t *hdr;
            size_t mhr_len = ieee802154_get_frame_hdr_len(pkt->data);

            if (mhr_len == 0) {
                DEBUG("_recv_ieee802154: illegally formatted frame received\n");
                gnrc_pktbuf_release(pkt);
                return NULL;
            }
            nread -= mhr_len;
            /* mark IEEE 802.15.4 header */
            ieee802154_hdr = gnrc_pktbuf_mark(pkt, mhr_len, GNRC_NETTYPE_UNDEF);
            if (ieee802154_hdr == NULL) {
                DEBUG("_recv_ieee802154: no space left in packet buffer\n");
                gnrc_pktbuf_release(pkt);
                return NULL;
            }
            netif_hdr = _make_netif_hdr(ieee802154_hdr->data);
            if (netif_hdr == NULL) {
                DEBUG("_recv_ieee802154: no space left in packet buffer\n");
                gnrc_pktbuf_release(pkt);
                return NULL;
            }

            hdr = netif_hdr->data;

#ifdef MODULE_L2FILTER
            if (!l2filter_pass(dev->filter, gnrc_netif_hdr_get_src_addr(hdr),
                               hdr->src_l2addr_len)) {
                gnrc_pktbuf_release(pkt);
                gnrc_pktbuf_release(netif_hdr);
                DEBUG("_recv_ieee802154: packet dropped by l2filter\n");
                return NULL;
            }
#endif

            hdr->lqi = rx_info.lqi;
            hdr->rssi = rx_info.rssi;
            gnrc_netif_hdr_set_netif(hdr, netif);
            pkt->type = state->proto;
            if (IS_ACTIVE(ENABLE_DEBUG)) {
                char src_str[GNRC_NETIF_HDR_L2ADDR_PRINT_LEN];

                DEBUG("_recv_ieee802154: received packet from %s of length %u\n",
                      gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(hdr),
                                             hdr->src_l2addr_len,
                                             src_str),
                      nread);

                if (IS_USED(MODULE_OD)) {
                    od_hex_dump(pkt->data, nread, OD_WIDTH_DEFAULT);
                }
            }
            gnrc_pktbuf_remove_snip(pkt, ieee802154_hdr);
            pkt = gnrc_pkt_append(pkt, netif_hdr);
        }

        DEBUG("_recv_ieee802154: reallocating.\n");
        gnrc_pktbuf_realloc_data(pkt, nread);
    }

    return pkt;
}

static gnrc_mac_tx_neighbor_t *_next_tx_neighbor(gnrc_netif_t *netif)
{
    gnrc_mac_tx_neighbor_t *next = NULL;
    uint32_t phase_nearest = GNRC_LWMAC_PHASE_MAX;

    for (unsigned i = 0; i < CONFIG_GNRC_MAC_NEIGHBOR_COUNT; i++) {
        if (gnrc_priority_pktqueue_length(&netif->mac.tx.neighbors[i].queue) > 0) {
            /* Unknown destinations are initialized with their phase at the end
             * of the local interval, so known destinations that still wakeup
             * in this interval will be preferred. */
            uint32_t phase_check = _gnrc_lwmac_ticks_until_phase(netif->mac.tx.neighbors[i].phase);

            if (phase_check <= phase_nearest) {
                next = &(netif->mac.tx.neighbors[i]);
                phase_nearest = phase_check;
                DEBUG("[LWMAC-int] Advancing queue #%u\n", i);
            }
        }
    }

    return next;
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

inline void lwmac_schedule_update(gnrc_netif_t *netif)
{
    gnrc_lwmac_set_reschedule(netif, true);
}

void lwmac_set_state(gnrc_netif_t *netif, gnrc_lwmac_state_t newstate)
{
    gnrc_lwmac_state_t oldstate = netif->mac.prot.lwmac.state;

    if (newstate == oldstate) {
        return;
    }

    if (newstate >= GNRC_LWMAC_STATE_COUNT) {
        LOG_ERROR("ERROR: [LWMAC] Trying to set invalid state %u\n", newstate);
        return;
    }

    /* Already change state, but might be reverted to oldstate when needed */
    netif->mac.prot.lwmac.state = newstate;

    /* Actions when leaving old state */
    switch (oldstate) {
        case GNRC_LWMAC_RECEIVING:
        case GNRC_LWMAC_TRANSMITTING: {
            /* Enable duty cycling again */
            rtt_handler(GNRC_LWMAC_EVENT_RTT_RESUME, netif);
            break;
        }
        case GNRC_LWMAC_SLEEPING: {
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);
            break;
        }
        default:
            break;
    }

    /* Actions when entering new state */
    switch (newstate) {
        /*********************** Operation states *********************************/
        case GNRC_LWMAC_LISTENING: {
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_IDLE);
            break;
        }
        case GNRC_LWMAC_SLEEPING: {
            /* Put transceiver to sleep */
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_SLEEP);
            /* We may have come here through RTT handler, so timeout may still be active */
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);

            if (gnrc_lwmac_get_phase_backoff(netif)) {
                gnrc_lwmac_set_phase_backoff(netif, false);
                uint32_t alarm;

                rtt_clear_alarm();
                alarm = random_uint32_range(
                            RTT_US_TO_TICKS((3 * GNRC_LWMAC_WAKEUP_DURATION_US / 2)),
                            RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US -
                                            (3 * GNRC_LWMAC_WAKEUP_DURATION_US / 2)));
                LOG_WARNING("WARNING: [LWMAC] phase backoffed: %lu us\n",
                            (unsigned long)RTT_TICKS_TO_US(alarm));
                netif->mac.prot.lwmac.last_wakeup = netif->mac.prot.lwmac.last_wakeup + alarm;
                alarm = _next_inphase_event(netif->mac.prot.lwmac.last_wakeup,
                            RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US));
                rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            }

            /* Return immediately, so no rescheduling */
            return;
        }
        /* Trying to send data */
        case GNRC_LWMAC_TRANSMITTING: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, netif);         /**< No duty cycling while RXing */
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_IDLE); /**< Power up netdev */
            break;
        }
        /* Receiving incoming data */
        case GNRC_LWMAC_RECEIVING: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, netif);         /**< No duty cycling while TXing */
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_IDLE); /**< Power up netdev */
            break;
        }
        case GNRC_LWMAC_STOPPED: {
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_OFF);
            break;
        }
        /*********************** Control states ***********************************/
        case GNRC_LWMAC_START: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_START, netif);
            lwmac_set_state(netif, GNRC_LWMAC_LISTENING);
            netif->mac.tx.preamble_fail_counts = 0;
            break;
        }
        case GNRC_LWMAC_STOP: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_STOP, netif);
            lwmac_set_state(netif, GNRC_LWMAC_STOPPED);
            break;
        }
        case GNRC_LWMAC_RESET: {
            LOG_WARNING("WARNING: [LWMAC] Reset not yet implemented\n");
            lwmac_set_state(netif, GNRC_LWMAC_STOP);
            lwmac_set_state(netif, GNRC_LWMAC_START);
            break;
        }
        /**************************************************************************/
        default: {
            LOG_DEBUG("[LWMAC] No actions for entering state %u\n", newstate);
            return;
        }
    }

    lwmac_schedule_update(netif);
}

static void _sleep_management(gnrc_netif_t *netif)
{
    /* If a packet is scheduled, no other (possible earlier) packet can be
     * sent before the first one is handled, even no broadcast
     */
    if (!gnrc_lwmac_timeout_is_running(netif, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP)) {
        gnrc_mac_tx_neighbor_t *neighbour;

        /* Check if there is packet remaining for retransmission */
        if (netif->mac.tx.current_neighbor != NULL) {
            neighbour = netif->mac.tx.current_neighbor;
        }
        else {
            /* Check if there are broadcasts to send and transmit immediately */
            if (gnrc_priority_pktqueue_length(&(netif->mac.tx.neighbors[0].queue)) > 0) {
                netif->mac.tx.current_neighbor = &(netif->mac.tx.neighbors[0]);
                lwmac_set_state(netif, GNRC_LWMAC_TRANSMITTING);
                return;
            }
            neighbour = _next_tx_neighbor(netif);
        }

        if (neighbour != NULL) {
            /* if phase is unknown, send immediately. */
            if (neighbour->phase > RTT_TICKS_TO_US(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US)) {
                netif->mac.tx.current_neighbor = neighbour;
                gnrc_lwmac_set_tx_continue(netif, false);
                netif->mac.tx.tx_burst_count = 0;
                lwmac_set_state(netif, GNRC_LWMAC_TRANSMITTING);
                return;
            }

            /* Offset in microseconds when the earliest (phase) destination
             * node wakes up that we have packets for. */
            uint32_t time_until_tx = RTT_TICKS_TO_US(_gnrc_lwmac_ticks_until_phase(neighbour->phase));

            /* If there's not enough time to prepare a WR to catch the phase
             * postpone to next interval */
            if (time_until_tx < CONFIG_GNRC_LWMAC_WR_PREPARATION_US) {
                time_until_tx += CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US;
            }
            time_until_tx -= CONFIG_GNRC_LWMAC_WR_PREPARATION_US;

            /* add a random time before goto TX, for avoiding one node for
             * always holding the medium (if the receiver's phase is recorded earlier in this
             * particular node) */
            uint32_t random_backoff;
            random_backoff = random_uint32_range(0, CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US);
            time_until_tx = time_until_tx + random_backoff;

            gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP, time_until_tx);

            /* Register neighbour to be the next */
            netif->mac.tx.current_neighbor = neighbour;

            /* Stop dutycycling, we're preparing to send. This prevents the
             * timeout arriving late, so that the destination phase would
             * be missed. */
            /* TODO: bad for power savings */
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, netif);
        }
    }
    else if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP)) {
        LOG_DEBUG("[LWMAC] Got timeout for dest wakeup, ticks: %" PRIu32 "\n", rtt_get_counter());
        gnrc_lwmac_set_tx_continue(netif, false);
        netif->mac.tx.tx_burst_count = 0;
        lwmac_set_state(netif, GNRC_LWMAC_TRANSMITTING);
    }
}

static void _rx_management_failed(gnrc_netif_t *netif)
{
    /* This may happen frequently because we'll receive WA from
     * every node in range. */
    LOG_DEBUG("[LWMAC] Reception was NOT successful\n");
    gnrc_lwmac_rx_stop(netif);

    if (netif->mac.rx.rx_bad_exten_count >= CONFIG_GNRC_LWMAC_MAX_RX_EXTENSION_NUM) {
        gnrc_lwmac_set_quit_rx(netif, true);
    }

    /* Here we check if we are close to the end of the cycle. If yes,
     * go to sleep. Firstly, get the relative phase. */
    uint32_t phase = rtt_get_counter();
    if (phase < netif->mac.prot.lwmac.last_wakeup) {
        phase = (RTT_US_TO_TICKS(GNRC_LWMAC_PHASE_MAX) - netif->mac.prot.lwmac.last_wakeup) +
                phase;
    }
    else {
        phase = phase - netif->mac.prot.lwmac.last_wakeup;
    }
    /* If the relative phase is beyond 4/5 cycle time, go to sleep. */
    if (phase > (4 * RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US) / 5)) {
        gnrc_lwmac_set_quit_rx(netif, true);
    }

    if (gnrc_lwmac_get_quit_rx(netif)) {
        lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
    }
    else {
        /* Go back to LISTENING for keep hearing on the channel */
        lwmac_set_state(netif, GNRC_LWMAC_LISTENING);
    }
}

static void _rx_management_success(gnrc_netif_t *netif)
{
    LOG_DEBUG("[LWMAC] Reception was successful\n");
    gnrc_lwmac_rx_stop(netif);
    /* Dispatch received packets, timing is not critical anymore */
    gnrc_mac_dispatch(&netif->mac.rx);

    /* Here we check if we are close to the end of the cycle. If yes,
     * go to sleep. Firstly, get the relative phase. */
    uint32_t phase = rtt_get_counter();
    if (phase < netif->mac.prot.lwmac.last_wakeup) {
        phase = (RTT_US_TO_TICKS(GNRC_LWMAC_PHASE_MAX) - netif->mac.prot.lwmac.last_wakeup) +
                phase;
    }
    else {
        phase = phase - netif->mac.prot.lwmac.last_wakeup;
    }
    /* If the relative phase is beyond 4/5 cycle time, go to sleep. */
    if (phase > (4 * RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US) / 5)) {
        gnrc_lwmac_set_quit_rx(netif, true);
    }

    if (gnrc_lwmac_get_quit_rx(netif)) {
        lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
    }
    else {
        /* Go back to LISTENING after successful reception */
        lwmac_set_state(netif, GNRC_LWMAC_LISTENING);
    }
}

static void _rx_management(gnrc_netif_t *netif)
{
    gnrc_lwmac_rx_state_t state_rx = netif->mac.rx.state;

    switch (state_rx) {
        case GNRC_LWMAC_RX_STATE_STOPPED: {
            gnrc_lwmac_rx_start(netif);
            gnrc_lwmac_rx_update(netif);
            break;
        }
        case GNRC_LWMAC_RX_STATE_FAILED: {
            _rx_management_failed(netif);
            break;
        }
        case GNRC_LWMAC_RX_STATE_SUCCESSFUL: {
            _rx_management_success(netif);
            break;
        }
        default:
            gnrc_lwmac_rx_update(netif);
    }

    /* If state has changed, reschedule main state machine */
    if (state_rx != netif->mac.rx.state) {
        lwmac_schedule_update(netif);
    }
}

static void _tx_management_stopped(gnrc_netif_t *netif)
{
    /* If there is packet remaining for retransmission,
     * retransmit it (i.e., the retransmission scheme of LWMAC). */
    if (netif->mac.tx.packet != NULL) {
        LOG_WARNING("WARNING: [LWMAC] TX %d times retry\n",
                    netif->mac.tx.tx_retry_count);
        netif->mac.tx.state = GNRC_LWMAC_TX_STATE_INIT;
        netif->mac.tx.wr_sent = 0;
        gnrc_lwmac_tx_update(netif);
    }
    else {
        gnrc_pktsnip_t *pkt;

        if ((pkt = gnrc_priority_pktqueue_pop(
                 &netif->mac.tx.current_neighbor->queue))) {
            netif->mac.tx.tx_retry_count = 0;
            gnrc_lwmac_tx_start(netif, pkt, netif->mac.tx.current_neighbor);
            gnrc_lwmac_tx_update(netif);
        }
        else {
            /* Shouldn't happen, but never observed this case */
            lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
        }
    }
}

static void _tx_management_success(gnrc_netif_t *netif)
{
    if (netif->mac.tx.current_neighbor == &(netif->mac.tx.neighbors[0])) {
        LOG_INFO("[LWMAC] Broadcast transmission done\n");
    }

    gnrc_lwmac_tx_stop(netif);

    /* In case have pending packets for the same receiver, continue to
     * send immediately, before the maximum transmit-limit */
    if ((gnrc_lwmac_get_tx_continue(netif)) &&
        (netif->mac.tx.tx_burst_count < GNRC_LWMAC_MAX_TX_BURST_PKT_NUM)) {
        lwmac_schedule_update(netif);
    }
    else {
        lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
    }
}

static void _tx_management(gnrc_netif_t *netif)
{
    gnrc_lwmac_tx_state_t state_tx = netif->mac.tx.state;

    switch (state_tx) {
        case GNRC_LWMAC_TX_STATE_STOPPED:
            _tx_management_stopped(netif);
            break;

        case GNRC_LWMAC_TX_STATE_FAILED:
            /* If transmission failure, do not try burst transmissions and quit other
             * transmission attempts in this cycle for collision avoidance */
            gnrc_lwmac_set_tx_continue(netif, false);
            gnrc_lwmac_set_quit_tx(netif, true);
            /* TX packet will be dropped, no automatic resending here. */

            /* Re-initialize the radio when needed. */
            if (netif->mac.tx.preamble_fail_counts >= CONFIG_GNRC_LWMAC_RADIO_REINIT_THRESHOLD) {
                netif->mac.tx.preamble_fail_counts = 0;
                LOG_INFO("[LWMAC] Re-initialize radio.");
                lwmac_reinit_radio(netif);
            }
            /* Intentionally falls through */

        case GNRC_LWMAC_TX_STATE_SUCCESSFUL:
            _tx_management_success(netif);
            break;

        default:
            gnrc_lwmac_tx_update(netif);
            break;
    }

    /* If state has changed, reschedule main state machine */
    if (state_tx != netif->mac.tx.state) {
        lwmac_schedule_update(netif);
    }
}

static void _lwmac_update_listening(gnrc_netif_t *netif)
{
    /* In case has pending packet to send, clear rtt alarm thus to goto
     * transmission initialization (in SLEEPING management) right after the
     * listening period */
    if ((_next_tx_neighbor(netif) != NULL) ||
        (netif->mac.tx.current_neighbor != NULL)) {
        rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, netif);
    }

    /* Set timeout for if there's no successful rx transaction that will
     * change state to SLEEPING. */
    if (!gnrc_lwmac_timeout_is_running(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD)) {
        gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD, GNRC_LWMAC_WAKEUP_DURATION_US);
    }
    else if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD)) {
        /* Dispatch first as there still may be broadcast packets. */
        gnrc_mac_dispatch(&netif->mac.rx);

        netif->mac.prot.lwmac.state = GNRC_LWMAC_SLEEPING;
        /* Enable duty cycling again */
        rtt_handler(GNRC_LWMAC_EVENT_RTT_RESUME, netif);

        _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_SLEEP);
        gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);

        /* if there is a packet for transmission, schedule update to start
         * transmission initialization immediately. */
        gnrc_mac_tx_neighbor_t *neighbour = _next_tx_neighbor(netif);
        if ((neighbour != NULL) || (netif->mac.tx.current_neighbor != NULL)) {
            /* This triggers packet sending procedure in sleeping immediately. */
            lwmac_schedule_update(netif);
            return;
        }
    }

    if (gnrc_priority_pktqueue_length(&netif->mac.rx.queue) > 0) {
        /* Do wake-up extension in each packet reception. */
        gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);
        lwmac_set_state(netif, GNRC_LWMAC_RECEIVING);
    }
}

/* Main state machine. Call whenever something happens */
static bool lwmac_update(gnrc_netif_t *netif)
{
    gnrc_lwmac_set_reschedule(netif, false);

    switch (netif->mac.prot.lwmac.state) {
        case GNRC_LWMAC_SLEEPING: {
            /* Quit scheduling transmission if 'quit-tx' flag is found set, thus
             * to avoid potential collisions with ongoing transmissions of other
             * neighbor nodes */
            if (gnrc_lwmac_get_quit_tx(netif)) {
                return false;
            }

            _sleep_management(netif);
            break;
        }
        case GNRC_LWMAC_LISTENING: {
            _lwmac_update_listening(netif);
            break;
        }
        case GNRC_LWMAC_RECEIVING: {
            _rx_management(netif);
            break;
        }
        case GNRC_LWMAC_TRANSMITTING: {
            _tx_management(netif);
            break;
        }
        default:
            LOG_DEBUG("[LWMAC] No actions in state %u\n", netif->mac.prot.lwmac.state);
    }

    return gnrc_lwmac_get_reschedule(netif);
}

static void rtt_cb(void *arg)
{
    msg_t msg;

    msg.content.value = (uint16_t)((uintptr_t) arg);
    msg.type = GNRC_LWMAC_EVENT_RTT_TYPE;
    msg_send(&msg, lwmac_pid);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void rtt_handler(uint32_t event, gnrc_netif_t *netif)
{
    uint32_t alarm;

    switch (event & 0xffff) {
        case GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING: {
            /* A new cycle starts, set sleep timing and initialize related MAC-info flags. */
            netif->mac.prot.lwmac.last_wakeup = rtt_get_alarm();
            alarm = _next_inphase_event(netif->mac.prot.lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_DURATION_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING);
            gnrc_lwmac_set_quit_tx(netif, false);
            gnrc_lwmac_set_quit_rx(netif, false);
            gnrc_lwmac_set_phase_backoff(netif, false);
            netif->mac.rx.rx_bad_exten_count = 0;
            lwmac_set_state(netif, GNRC_LWMAC_LISTENING);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING: {
            /* Set next wake-up timing. */
            alarm = _next_inphase_event(netif->mac.prot.lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
            break;
        }
        /* Set initial wake-up alarm that starts the cycle */
        case GNRC_LWMAC_EVENT_RTT_START: {
            LOG_DEBUG("[LWMAC] RTT: Initialize duty cycling\n");
            alarm = rtt_get_counter() + RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_DURATION_US);
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING);
            gnrc_lwmac_set_dutycycle_active(netif, true);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_STOP:
        case GNRC_LWMAC_EVENT_RTT_PAUSE: {
            rtt_clear_alarm();
            LOG_DEBUG("[LWMAC] RTT: Stop duty cycling, now in state %u\n",
                      netif->mac.prot.lwmac.state);
            gnrc_lwmac_set_dutycycle_active(netif, false);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_RESUME: {
            LOG_DEBUG("[LWMAC] RTT: Resume duty cycling\n");
            rtt_clear_alarm();
            alarm = _next_inphase_event(netif->mac.prot.lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            gnrc_lwmac_set_dutycycle_active(netif, true);
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
static void _lwmac_event_cb(netdev_t *dev, netdev_event_t event)
{
    gnrc_netif_t *netif = (gnrc_netif_t *) dev->context;

    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;

        msg.type = NETDEV_MSG_TYPE_EVENT;
        msg.content.ptr = (void *) netif;

        if (msg_send(&msg, netif->pid) <= 0) {
            LOG_WARNING("WARNING: [LWMAC] gnrc_netdev: possibly lost interrupt.\n");
        }
    }
    else {
        DEBUG("gnrc_netdev: event triggered -> %i\n", event);
        switch (event) {
            case NETDEV_EVENT_RX_STARTED: {
                LOG_DEBUG("[LWMAC] NETDEV_EVENT_RX_STARTED\n");
                gnrc_netif_set_rx_started(netif, true);
                break;
            }
            case NETDEV_EVENT_RX_COMPLETE: {
                LOG_DEBUG("[LWMAC] NETDEV_EVENT_RX_COMPLETE\n");
                gnrc_pktsnip_t *pkt = netif->ops->recv(netif);

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
                    gnrc_netif_set_rx_started(netif, false);
                    break;
                }

                gnrc_netif_set_rx_started(netif, false);

                if (!gnrc_mac_queue_rx_packet(&netif->mac.rx, 0, pkt)) {
                    LOG_ERROR("ERROR: [LWMAC] Can't push RX packet @ %p, memory full?\n", pkt);
                    gnrc_pktbuf_release(pkt);
                    break;
                }
                lwmac_schedule_update(netif);
                break;
            }
            case NETDEV_EVENT_TX_STARTED: {
                gnrc_netif_set_tx_feedback(netif, TX_FEEDBACK_UNDEF);
                gnrc_netif_set_rx_started(netif, false);
                break;
            }
            case NETDEV_EVENT_TX_COMPLETE: {
                gnrc_netif_set_tx_feedback(netif, TX_FEEDBACK_SUCCESS);
                gnrc_netif_set_rx_started(netif, false);
                lwmac_schedule_update(netif);
                break;
            }
            case NETDEV_EVENT_TX_NOACK: {
                gnrc_netif_set_tx_feedback(netif, TX_FEEDBACK_NOACK);
                gnrc_netif_set_rx_started(netif, false);
                lwmac_schedule_update(netif);
                break;
            }
            case NETDEV_EVENT_TX_MEDIUM_BUSY: {
                gnrc_netif_set_tx_feedback(netif, TX_FEEDBACK_BUSY);
                gnrc_netif_set_rx_started(netif, false);
                lwmac_schedule_update(netif);
                break;
            }
            default:
                LOG_WARNING("WARNING: [LWMAC] Unhandled netdev event: %u\n", event);
        }
    }

    /* Execute main state machine because something just happened*/
    while (gnrc_lwmac_get_reschedule(netif)) {
        lwmac_update(netif);
    }
}

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, pkt)) {
        gnrc_pktbuf_release(pkt);
        LOG_WARNING("WARNING: [LWMAC] TX queue full, drop packet\n");
        return -ENOBUFS;
    }

    lwmac_schedule_update(netif);

    /* Execute main state machine because something just happened*/
    while (gnrc_lwmac_get_reschedule(netif)) {
        lwmac_update(netif);
    }

    return 0;
}

static void _lwmac_msg_handler(gnrc_netif_t *netif, msg_t *msg)
{
    switch (msg->type) {
        /* RTT raised an interrupt */
        case GNRC_LWMAC_EVENT_RTT_TYPE: {
            if (gnrc_lwmac_get_dutycycle_active(netif)) {
                rtt_handler(msg->content.value, netif);
                lwmac_schedule_update(netif);
            }
            else {
                LOG_DEBUG("[LWMAC] Ignoring late RTT event while duty-cycling is off\n");
            }
            break;
        }

        /* An LWMAC timeout occurred */
        case GNRC_LWMAC_EVENT_TIMEOUT_TYPE: {
            gnrc_lwmac_timeout_make_expire((gnrc_lwmac_timeout_t *) msg->content.ptr);
            lwmac_schedule_update(netif);
            break;
        }
#if (GNRC_MAC_ENABLE_DUTYCYCLE_RECORD == 1)
        case GNRC_MAC_TYPE_GET_DUTYCYCLE: {
            /* Output LWMAC's radio duty-cycle ratio */
            uint64_t duty = (uint64_t) rtt_get_counter();
            duty = ((uint64_t) netif->mac.prot.lwmac.awake_duration_sum_ticks) * 100 /
                   (duty - (uint64_t)netif->mac.prot.lwmac.system_start_time_ticks);
            printf("[LWMAC]: achieved radio duty-cycle: %u %% \n", (unsigned) duty);
            break;
        }
#endif
        default: {
            DEBUG("[LWMAC]: unknown message type 0x%04x "
                  "(no message handler defined)\n", msg->type);
            break;
        }
    }

    /* Execute main state machine because something just happened*/
    while (gnrc_lwmac_get_reschedule(netif)) {
        lwmac_update(netif);
    }
}

static void _lwmac_init(gnrc_netif_t *netif)
{
    netdev_t *dev;

    gnrc_netif_default_init(netif);
    dev = netif->dev;
    dev->event_callback = _lwmac_event_cb;

    /* RTT is used for scheduling wakeup */
    rtt_init();

    /* Store pid globally, so that IRQ can use it to send msg */
    lwmac_pid = netif->pid;

    /* Enable RX- and TX-started interrupts  */
    netopt_enable_t enable = NETOPT_ENABLE;
    dev->driver->set(dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

    uint16_t src_len = IEEE802154_LONG_ADDRESS_LEN;
    dev->driver->set(dev, NETOPT_SRC_LEN, &src_len, sizeof(src_len));

    /* Get own address from netdev */
    netif->l2addr_len = dev->driver->get(dev, NETOPT_ADDRESS_LONG,
                                         &netif->l2addr,
                                         IEEE802154_LONG_ADDRESS_LEN);

    /* Initialize broadcast sequence number. This at least differs from board
     * to board */
    netif->mac.tx.bcast_seqnr = netif->l2addr[0];

    /* Reset all timeouts just to be sure */
    gnrc_lwmac_reset_timeouts(netif);

    /* Start duty cycling */
    lwmac_set_state(netif, GNRC_LWMAC_START);

#if (GNRC_MAC_ENABLE_DUTYCYCLE_RECORD == 1)
    /* Start duty cycle recording */
    netif->mac.prot.lwmac.system_start_time_ticks = rtt_get_counter();
    netif->mac.prot.lwmac.last_radio_on_time_ticks = netif->mac.prot.lwmac.system_start_time_ticks;
    netif->mac.prot.lwmac.awake_duration_sum_ticks = 0;
    netif->mac.prot.lwmac.lwmac_info |= GNRC_LWMAC_RADIO_IS_ON;
#endif
}
