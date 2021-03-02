/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_gomach
 * @{
 *
 * @file
 * @brief       Implementation of GoMacH
 *
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "random.h"
#include "timex.h"
#include "periph/rtt.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc.h"
#include "net/gnrc/nettype.h"
#include "net/netdev.h"
#include "net/gnrc/mac/internal.h"
#include "net/gnrc/gomach/gomach.h"
#include "net/gnrc/gomach/timeout.h"
#include "include/gomach_internal.h"

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
 * @brief  GoMacH thread's PID
 */
static kernel_pid_t gomach_pid;

static void _gomach_init(gnrc_netif_t *netif);
static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);
static void _gomach_msg_handler(gnrc_netif_t *netif, msg_t *msg);
static void _gomach_event_cb(netdev_t *dev, netdev_event_t event);

static const gnrc_netif_ops_t gomach_ops = {
    .init = _gomach_init,
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
    .msg_handler = _gomach_msg_handler,
};

int gnrc_netif_gomach_create(gnrc_netif_t *netif, char *stack, int stacksize,
                             char priority, const char *name, netdev_t *dev)
{
    return gnrc_netif_create(netif, stack, stacksize, priority, name, dev,
                             &gomach_ops);
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    netdev_ieee802154_rx_info_t rx_info;
    netdev_ieee802154_t *state = (netdev_ieee802154_t *)netif->dev;
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
            gnrc_pktsnip_t *ieee802154_hdr;
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
            netif->mac.prot.gomach.rx_pkt_lqi = rx_info.lqi;
            netif->mac.prot.gomach.rx_pkt_rssi = rx_info.rssi;
        }

        DEBUG("_recv_ieee802154: reallocating.\n");
        gnrc_pktbuf_realloc_data(pkt, nread);
    }

    return pkt;
}

static void gomach_reinit_radio(gnrc_netif_t *netif)
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

    /* Enable RX-start and TX-started and TX-END interrupts. */
    netopt_enable_t enable = NETOPT_ENABLE;
    netif->dev->driver->set(netif->dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
    netif->dev->driver->set(netif->dev, NETOPT_RX_END_IRQ, &enable, sizeof(enable));
    netif->dev->driver->set(netif->dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

}

static void _gomach_rtt_cb(void *arg)
{
    msg_t msg;

    (void)arg;
    msg.content.value = GNRC_GOMACH_EVENT_RTT_NEW_CYCLE;
    msg.type = GNRC_GOMACH_EVENT_RTT_TYPE;
    msg_send(&msg, gomach_pid);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

static void _gomach_rtt_handler(uint32_t event, gnrc_netif_t *netif)
{
    switch (event & 0xffff) {
        case GNRC_GOMACH_EVENT_RTT_NEW_CYCLE: {
            /* Start duty-cycle scheme. */
            if (!gnrc_gomach_get_duty_cycle_start(netif)) {
                gnrc_gomach_set_duty_cycle_start(netif, true);
                rtt_clear_alarm();
                /* Record the new cycle's starting time. */
                netif->mac.prot.gomach.last_wakeup = rtt_get_counter();
            }
            else {
                /* The duty-cycle scheme has already started,
                 * record the new cycle's starting time. */
                netif->mac.prot.gomach.last_wakeup = rtt_get_alarm();
                gnrc_gomach_set_enter_new_cycle(netif, true);
            }

            netif->mac.prot.gomach.last_wakeup_phase_us = xtimer_now_usec64();

            /* Set next cycle's starting time. */
            uint32_t alarm = netif->mac.prot.gomach.last_wakeup +
                             RTT_US_TO_TICKS(CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US);
            rtt_set_alarm(alarm, _gomach_rtt_cb, NULL);

            /* Update neighbors' public channel phases. */
            gnrc_gomach_update_neighbor_pubchan(netif);
            gnrc_gomach_set_update(netif, true);
        } break;
        default: {
            LOG_ERROR("ERROR: [GOMACH] error RTT message type\n");
            break;
        }
    }
}

static void gomach_bcast_init(gnrc_netif_t *netif)
{
    /* Disable auto-ACK when sending broadcast packets, thus not to receive packet. */
    gnrc_gomach_set_autoack(netif, NETOPT_DISABLE);

    /* Firstly turn the radio to public channel 1. */
    gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.pub_channel_1);
    gnrc_gomach_set_on_pubchan_1(netif, true);

    netif->mac.tx.broadcast_seq++;

    /* Assemble the broadcast packet. */
    gnrc_pktsnip_t *pkt = netif->mac.tx.packet;
    gnrc_pktsnip_t *payload = netif->mac.tx.packet->next;

    gnrc_gomach_frame_broadcast_t gomach_broadcast_hdr;
    gomach_broadcast_hdr.header.type = GNRC_GOMACH_FRAME_BROADCAST;
    gomach_broadcast_hdr.seq_nr = netif->mac.tx.broadcast_seq;
    pkt->next = gnrc_pktbuf_add(pkt->next, &gomach_broadcast_hdr,
                                sizeof(gomach_broadcast_hdr),
                                GNRC_NETTYPE_GOMACH);
    if (pkt->next == NULL) {
        /* Make append payload after netif header again */
        netif->mac.tx.packet->next = payload;
        gnrc_pktbuf_release(netif->mac.tx.packet);
        netif->mac.tx.packet = NULL;
        LOG_ERROR("ERROR: [GOMACH] bcast: no memory to assemble bcast packet, drop packet.\n");
        LOG_ERROR("ERROR: [GOMACH] bcast failed, go to listen mode.\n");
        netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_END;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_BCAST_FINISH,
                            CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US);

    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
    netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_SEND;
    gnrc_gomach_set_update(netif, true);
}

static bool _gomach_send_bcast_busy_handle(gnrc_netif_t *netif)
{
    /* Quit sending broadcast packet if we found ongoing transmissions,
     * for collision avoidance. */
    if ((gnrc_gomach_get_netdev_state(netif) == NETOPT_STATE_RX) ||
        (gnrc_netif_get_rx_started(netif) == true)) {
        LOG_DEBUG("[GOMACH] bcast: found ongoing transmission, quit broadcast.\n");
        /* Queue the broadcast packet back to the queue. */
        gnrc_pktsnip_t *payload = netif->mac.tx.packet->next->next;

        /* remove gomach header */
        netif->mac.tx.packet->next->next = NULL;
        gnrc_pktbuf_release(netif->mac.tx.packet->next);

        /* make append payload after netif header again */
        netif->mac.tx.packet->next = payload;

        if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
            LOG_DEBUG("[GOMACH] bcast: TX queue full, release packet.\n");
            gnrc_pktbuf_release(netif->mac.tx.packet);
        }
        netif->mac.tx.packet = NULL;

        netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_END;
        gnrc_gomach_set_update(netif, true);
        return false;
    }
    return true;
}

static void gomach_send_bcast_packet(gnrc_netif_t *netif)
{
    /* Quit sending broadcast packet if we found ongoing transmissions,
     * for collision avoidance. */
    if (!_gomach_send_bcast_busy_handle(netif)) {
        return;
    }

    gnrc_pktbuf_hold(netif->mac.tx.packet, 1);

    /* Start sending the broadcast packet. */
    gnrc_gomach_send(netif, netif->mac.tx.packet, NETOPT_DISABLE);

    netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_WAIT_TX_FINISH;
    gnrc_gomach_set_update(netif, false);
}

static void gomach_wait_bcast_tx_finish(gnrc_netif_t *netif)
{
    if (gnrc_gomach_get_tx_finish(netif)) {
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_BCAST_INTERVAL,
                                CONFIG_GNRC_GOMACH_BCAST_INTERVAL_US);
        netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_WAIT_NEXT_TX;
        gnrc_gomach_set_update(netif, false);
    }

    /* This is to handle no-TX-complete issue. In case there is no no-TX-complete event,
     * we will quit broadcasting, i.e., not getting stuck here. */
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_BCAST_FINISH)) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_BCAST_INTERVAL);
        netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_END;
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_wait_bcast_wait_next_tx(gnrc_netif_t *netif)
{
    /* Quit sending broadcast packet if we found ongoing transmissions,
     * for collision avoidance. */
    if (!_gomach_send_bcast_busy_handle(netif)) {
        return;
    }

    /* If the whole broadcast duration timeouts, release the packet and go to t2u end. */
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_BCAST_FINISH)) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_BCAST_INTERVAL);
        gnrc_pktbuf_release(netif->mac.tx.packet);
        netif->mac.tx.packet = NULL;
        netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_END;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    /* Toggle the radio channel and go to send the next broadcast packet. */
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_BCAST_INTERVAL)) {
        if (gnrc_gomach_get_on_pubchan_1(netif)) {
            gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.pub_channel_2);
            gnrc_gomach_set_on_pubchan_1(netif, false);
        }
        else {
            gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.pub_channel_1);
            gnrc_gomach_set_on_pubchan_1(netif, true);
        }

        netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_SEND;
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_bcast_end(gnrc_netif_t *netif)
{
    gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_SLEEP);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_BCAST_INTERVAL);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_BCAST_FINISH);

    if (netif->mac.tx.packet) {
        gnrc_pktbuf_release(netif->mac.tx.packet);
        netif->mac.tx.packet = NULL;
    }
    netif->mac.tx.current_neighbor = NULL;

    /* Reset the t2u state. */
    netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_INIT;

    /* Switch to the listen mode. */
    netif->mac.prot.gomach.basic_state = GNRC_GOMACH_LISTEN;
    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP;
    gnrc_gomach_set_enter_new_cycle(netif, false);
    gnrc_gomach_set_update(netif, true);
}

static void gomach_bcast_update(gnrc_netif_t *netif)
{
    /* State machine of GoMacH's broadcast procedure. */
    switch (netif->mac.tx.bcast_state) {
        case GNRC_GOMACH_BCAST_INIT: {
            gomach_bcast_init(netif);
            break;
        }
        case GNRC_GOMACH_BCAST_SEND: {
            gomach_send_bcast_packet(netif);
            break;
        }
        case GNRC_GOMACH_BCAST_WAIT_TX_FINISH: {
            gomach_wait_bcast_tx_finish(netif);
            break;
        }
        case GNRC_GOMACH_BCAST_WAIT_NEXT_TX: {
            gomach_wait_bcast_wait_next_tx(netif);
            break;
        }
        case GNRC_GOMACH_BCAST_END: {
            gomach_bcast_end(netif);
            break;
        }
        default: break;
    }
}

static void gomach_init_prepare(gnrc_netif_t *netif)
{
    rtt_clear_alarm();

    /* Random delay for avoiding the same wake-up phase among devices. */
    uint32_t random_backoff = random_uint32_range(0, CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US);
    xtimer_usleep(random_backoff);

    gnrc_gomach_set_quit_cycle(netif, false);
    netif->mac.prot.gomach.subchannel_occu_flags = 0;

    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);

    /* Since devices don't broadcast beacons on default, so no need to collect beacons.
     * Go to announce its chosen sub-channel sequence. */
    netif->mac.prot.gomach.init_state = GNRC_GOMACH_INIT_ANNC_SUBCHAN;
    gnrc_gomach_set_update(netif, true);
}

static void gomach_init_announce_subchannel(gnrc_netif_t *netif)
{
    /* Choose a sub-channel for the device. */
    gnrc_gomach_init_choose_subchannel(netif);

    /* Announce the device's chosen sub-channel sequence to its neighbors. */
    gnrc_gomach_bcast_subchann_seq(netif, NETOPT_ENABLE);

    netif->mac.prot.gomach.init_state = GNRC_GOMACH_INIT_WAIT_FEEDBACK;
    gnrc_gomach_set_update(netif, false);
}

static void gomach_init_wait_announce_feedback(gnrc_netif_t *netif)
{
    if (gnrc_gomach_get_tx_finish(netif)) {
        gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
        netif->mac.prot.gomach.init_state = GNRC_GOMACH_INIT_END;
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_init_end(gnrc_netif_t *netif)
{
    /* Reset initialization state. */
    netif->mac.prot.gomach.init_state = GNRC_GOMACH_INIT_PREPARE;
    /* Switch to duty-cycle listen mode. */
    netif->mac.prot.gomach.basic_state = GNRC_GOMACH_LISTEN;
    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_CP_INIT;

    /* Start duty-cycle scheme. */
    gnrc_gomach_set_duty_cycle_start(netif, false);
    _gomach_rtt_handler(GNRC_GOMACH_EVENT_RTT_NEW_CYCLE, netif);
    gnrc_gomach_set_update(netif, true);
}

static void gomach_t2k_init(gnrc_netif_t *netif)
{
    /* Turn off radio to conserve power */
    gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_SLEEP);

    gnrc_gomach_set_quit_cycle(netif, false);

    /* Set waiting timer for the targeted device! */
    long int wait_phase_duration = netif->mac.tx.current_neighbor->cp_phase -
                                   gnrc_gomach_phase_now(netif);

    if (wait_phase_duration < 0) {
        wait_phase_duration += CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US;
    }

    /* Upon several times of t2k failure, we now doubt that the phase-lock may fail due to drift.
     * Here is the phase-lock auto-adjust scheme, trying to catch the neighbot's phase in case of
     * phase-lock failure due to timer drift.
     * Firstly, put the calculated phase ahead, check whether the neighbor's phase has gone ahead
     * of the recorded one */
    if (netif->mac.tx.no_ack_counter == (CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD - 2)) {
        if ((uint32_t)wait_phase_duration < CONFIG_GNRC_GOMACH_CP_DURATION_US) {
            wait_phase_duration = (wait_phase_duration +
                                   CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US) -
                                  CONFIG_GNRC_GOMACH_CP_DURATION_US;
        }
        else {
            wait_phase_duration = wait_phase_duration - CONFIG_GNRC_GOMACH_CP_DURATION_US;
        }
    }
    /* If this is the last t2k trial, the phase-lock auto-adjust scheme delays the estimated phase
     *  a little bit, to see if the real phase is behind the original calculated one. */
    if (netif->mac.tx.no_ack_counter == (CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD - 1)) {
        wait_phase_duration = wait_phase_duration + CONFIG_GNRC_GOMACH_CP_DURATION_US;
        if ((uint32_t)wait_phase_duration > CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US) {
            wait_phase_duration = wait_phase_duration - CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US;
        }
    }

    if ((uint32_t)wait_phase_duration > CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US) {
        wait_phase_duration = wait_phase_duration % CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US;
    }
    gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_CP, (uint32_t)wait_phase_duration);

    /* Flush the rx-queue. */
    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);

    netif->mac.tx.tx_busy_count = 0;

    netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_WAIT_CP;
    gnrc_gomach_set_update(netif, false);
}

static void gomach_t2k_wait_cp(gnrc_netif_t *netif)
{
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_WAIT_CP)) {
        gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_IDLE);
        /* Turn radio onto the neighbor's public channel, which will not change in this cycle. */
        gnrc_gomach_turn_channel(netif, netif->mac.tx.current_neighbor->pub_chanseq);

        /* Disable auto-ack, don't try to receive packet! */
        gnrc_gomach_set_autoack(netif, NETOPT_DISABLE);
        /* Require ACK for the packet waiting to be sent! */
        gnrc_gomach_set_ack_req(netif, NETOPT_ENABLE);

        /* Enable csma for sending the packet! */
        netopt_enable_t csma_enable = NETOPT_ENABLE;
        netif->dev->driver->set(netif->dev, NETOPT_CSMA, &csma_enable,
                                sizeof(netopt_enable_t));

        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_TRANS_IN_CP;
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_t2k_trans_in_cp(gnrc_netif_t *netif)
{
    /* To-do: should we add a rx-start security check and quit t2k when found
     * ongoing transmissions? */

    /* If we are retransmitting the packet, use the same sequence number for the
     * packet to avoid duplicate packet reception at the receiver side. */
    if ((netif->mac.tx.no_ack_counter > 0) || (netif->mac.tx.tx_busy_count > 0)) {
        netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
        device_state->seq = netif->mac.tx.tx_seq;
    }

    /* Send the data packet here. */
    int res = gnrc_gomach_send_data(netif, NETOPT_ENABLE);
    if (res < 0) {
        LOG_ERROR("ERROR: [GOMACH] t2k transmission fail: %d, drop packet.\n", res);
        netif->mac.tx.no_ack_counter = 0;

        /* If res is < 0, the data packet will not be released in send().
         * so need to release the data here. */
        if (netif->mac.tx.packet != NULL) {
            gnrc_pktbuf_release(netif->mac.tx.packet);
            netif->mac.tx.packet = NULL;
        }

        netif->mac.tx.current_neighbor = NULL;
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR, CONFIG_GNRC_GOMACH_NO_TX_ISR_US);

    netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_WAIT_CPTX_FEEDBACK;
    gnrc_gomach_set_update(netif, false);
}

static void _cp_tx_success(gnrc_netif_t *netif)
{
    /* Since the packet will not be released by the sending function,
     * so, here, if TX success, we first release the packet. */
    gnrc_pktbuf_release(netif->mac.tx.packet);
    netif->mac.tx.packet = NULL;

    /* Here is the phase-lock auto-adjust scheme. Use the new adjusted
     * phase upon success. Here the new phase will be put ahead to the
     * original phase. */
    if (netif->mac.tx.no_ack_counter == (CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD - 2)) {
        if (netif->mac.tx.current_neighbor->cp_phase >= CONFIG_GNRC_GOMACH_CP_DURATION_US) {
            netif->mac.tx.current_neighbor->cp_phase -= CONFIG_GNRC_GOMACH_CP_DURATION_US;
        }
        else {
            netif->mac.tx.current_neighbor->cp_phase += CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US;
            netif->mac.tx.current_neighbor->cp_phase -= CONFIG_GNRC_GOMACH_CP_DURATION_US;
        }
    }
    /* Here is the phase-lock auto-adjust scheme. Use the new adjusted
     * phase upon success. Here the new phase will be put behind the original
     * phase. */
    if (netif->mac.tx.no_ack_counter == (CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD - 1)) {
        netif->mac.tx.current_neighbor->cp_phase +=
            (CONFIG_GNRC_GOMACH_CP_DURATION_US + 20 * US_PER_MS);

        if (netif->mac.tx.current_neighbor->cp_phase >=
            CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US) {
            netif->mac.tx.current_neighbor->cp_phase -=
                CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US;
        }
    }

    netif->mac.tx.no_ack_counter = 0;
    netif->mac.tx.t2u_fail_count = 0;

    /* If has pending packets, join the vTDMA period, first wait for receiver's beacon. */
    if (gnrc_priority_pktqueue_length(&netif->mac.tx.current_neighbor->queue) > 0) {
        netif->mac.tx.vtdma_para.slots_num = 0;
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_BEACON,
                                GNRC_GOMACH_WAIT_BEACON_TIME_US);
        gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_WAIT_BEACON;
    }
    else {
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
    }
    gnrc_gomach_set_update(netif, true);
}

static bool _cp_tx_busy(gnrc_netif_t *netif)
{
    /* If the channel busy counter is below threshold, retry CSMA immediately,
     * by knowing that the CP will be automatically extended. */
    if (netif->mac.tx.tx_busy_count < CONFIG_GNRC_GOMACH_TX_BUSY_THRESHOLD) {
        netif->mac.tx.tx_busy_count++;

        /* Store the TX sequence number for this packet. Always use the same
         * sequence number for sending the same packet, to avoid duplicated
         * packet reception at the receiver. */
        netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
        netif->mac.tx.tx_seq = device_state->seq - 1;

        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_TRANS_IN_CP;
        gnrc_gomach_set_update(netif, true);
        return false;
    }
    return true;
}

static void _cp_tx_default(gnrc_netif_t *netif)
{
    netif->mac.tx.no_ack_counter++;

    LOG_DEBUG("[GOMACH] t2k %d times No-ACK.\n", netif->mac.tx.no_ack_counter);

    /* This packet will be retried. Store the TX sequence number for this packet.
     * Always use the same sequence number for sending the same packet. */
    netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
    netif->mac.tx.tx_seq = device_state->seq - 1;

    /* If no_ack_counter reaches the threshold, regarded as phase-lock failed. So
     * retry to send the packet in t2u, i.e., try to phase-lock with the receiver
     * again. */
    if (netif->mac.tx.no_ack_counter >= CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD) {
        LOG_DEBUG("[GOMACH] t2k failed, go to t2u.\n");
        /* Here, we don't queue the packet again, but keep it in tx.packet. */
        netif->mac.tx.current_neighbor->mac_type = GNRC_GOMACH_TYPE_UNKNOWN;
        netif->mac.tx.t2u_retry_counter = 0;
    }

    netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
    gnrc_gomach_set_update(netif, true);
}

static void gomach_t2k_wait_cp_txfeedback(gnrc_netif_t *netif)
{
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR)) {
        /* No TX-ISR, go to sleep. */
        netif->mac.tx.no_ack_counter++;

        netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
        netif->mac.tx.tx_seq = device_state->seq - 1;

        /* Here, we don't queue the packet again, but keep it in tx.packet. */
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
        gnrc_gomach_set_update(netif, true);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);
        return;
    }

    if (gnrc_gomach_get_tx_finish(netif)) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);

        switch (gnrc_netif_get_tx_feedback(netif)) {
            case TX_FEEDBACK_SUCCESS: {
                _cp_tx_success(netif);
                break;
            }
            case TX_FEEDBACK_BUSY:
                if (!_cp_tx_busy(netif)) {
                    return;
                }
                /* Intentionally falls through */
            case TX_FEEDBACK_NOACK:
            default: {
                _cp_tx_default(netif);
                break;
            }
        }
    }
}

static void gomach_t2k_wait_beacon(gnrc_netif_t *netif)
{
    /* Process the beacon if we receive it. */
    if (gnrc_gomach_get_pkt_received(netif)) {
        gnrc_gomach_set_pkt_received(netif, false);
        gnrc_gomach_packet_process_in_wait_beacon(netif);
    }

    /* If we need to quit t2k, don't release the current neighbor pointer. In the
     * next cycle, we will try to send to the same receiver. */
    if (gnrc_gomach_get_quit_cycle(netif)) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_BEACON);
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    if (netif->mac.tx.vtdma_para.slots_num > 0) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_BEACON);

        /* If the sender gets allocated slots, go to attend the receiver's vTDMA for
         * burst sending all the pending packets to the receiver. */
        if (netif->mac.tx.vtdma_para.slots_num > 0) {
            /* Switch the radio to the sub-channel of the receiver. */
            gnrc_gomach_turn_channel(netif, netif->mac.tx.vtdma_para.sub_channel_seq);

            /* If the allocated slots period is not right behind the beacon, i.e., not the first
             * one, turn off the radio and wait for its own slots period. */
            if (netif->mac.tx.vtdma_para.slots_position > 0) {
                gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_SLEEP);

                uint32_t wait_slots_duration = netif->mac.tx.vtdma_para.slots_position *
                                               CONFIG_GNRC_GOMACH_VTDMA_SLOT_SIZE_US;
                gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_SLOTS,
                                        wait_slots_duration);

                netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_WAIT_SLOTS;
                gnrc_gomach_set_update(netif, true);
            }
            else {
                /* If the allocated slots period is the first one in vTDMA,
                 * start sending packets. */
                gnrc_pktsnip_t *pkt =
                    gnrc_priority_pktqueue_pop(&(netif->mac.tx.current_neighbor->queue));
                if (pkt != NULL) {
                    netif->mac.tx.packet = pkt;
                    netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_VTDMA_TRANS;
                }
                else {
                    LOG_ERROR("ERROR: [GOMACH] t2k vTDMA: null packet.\n");
                    netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
                }
                gnrc_gomach_set_update(netif, true);
            }
        }
        else {
            /* No slots get allocated, go to t2k end. */
            netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
            gnrc_gomach_set_update(netif, true);
        }
        return;
    }

    /* If no beacon during waiting period, go to t2k end.
     * Or, if we have received beacon, but find no allocated slots,
     * go to t2k as well. */
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_WAIT_BEACON) ||
        !gnrc_gomach_timeout_is_running(netif, GNRC_GOMACH_TIMEOUT_WAIT_BEACON)) {
        gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
        LOG_DEBUG("[GOMACH] t2k: no beacon.\n");
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_t2k_wait_own_slots(gnrc_netif_t *netif)
{
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_WAIT_SLOTS)) {
        /* The node is now in its scheduled slots period, start burst sending packets. */
        gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_IDLE);

        gnrc_pktsnip_t *pkt = gnrc_priority_pktqueue_pop(&(netif->mac.tx.current_neighbor->queue));
        if (pkt != NULL) {
            netif->mac.tx.packet = pkt;
            netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_VTDMA_TRANS;
        }
        else {
            LOG_ERROR("ERROR: [GOMACH] t2k vTDMA: null packet.\n");
            netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
        }
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_t2k_trans_in_slots(gnrc_netif_t *netif)
{
    /* If this packet is being retransmitted, use the same recorded MAC sequence number. */
    if (netif->mac.tx.no_ack_counter > 0) {
        netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
        device_state->seq = netif->mac.tx.tx_seq;
    }

    /* Send data packet in its allocated slots (scheduled slots period). */
    int res = gnrc_gomach_send_data(netif, NETOPT_DISABLE);
    if (res < 0) {
        LOG_ERROR("ERROR: [GOMACH] t2k vTDMA transmission fail: %d, drop packet.\n", res);

        /* If res is < 0, the data packet will not be released in send().
         * so need to release the data here. */
        if (netif->mac.tx.packet != NULL) {
            gnrc_pktbuf_release(netif->mac.tx.packet);
            netif->mac.tx.packet = NULL;
        }
        netif->mac.tx.current_neighbor = NULL;
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR, CONFIG_GNRC_GOMACH_NO_TX_ISR_US);

    netif->mac.tx.vtdma_para.slots_num--;
    netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_WAIT_VTDMA_FEEDBACK;
    gnrc_gomach_set_update(netif, false);
}

static void _t2k_wait_vtdma_tx_success(gnrc_netif_t *netif)
{
    /* First release the packet. */
    gnrc_pktbuf_release(netif->mac.tx.packet);
    netif->mac.tx.packet = NULL;
    netif->mac.tx.no_ack_counter = 0;

    /* If the sender has pending packets and scheduled slots,
     * continue vTDMA transmission. */
    if ((netif->mac.tx.vtdma_para.slots_num > 0) &&
        (gnrc_priority_pktqueue_length(&netif->mac.tx.current_neighbor->queue) > 0)) {
        gnrc_pktsnip_t *pkt = gnrc_priority_pktqueue_pop(&netif->mac.tx.current_neighbor->queue);
        if (pkt != NULL) {
            netif->mac.tx.packet = pkt;
            netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_VTDMA_TRANS;
        }
        else {
            LOG_ERROR("ERROR: [GOMACH] t2k vTDMA: null packet.\n");
            netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
        }
    }
    else {
        /* If no scheduled slots or pending packets, go to t2k end. */
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
    }
    gnrc_gomach_set_update(netif, true);
}

static void _t2k_wait_vtdma_tx_default(gnrc_netif_t *netif)
{
    /* In case of transmission failure in vTDMA, retransmit the packet in the next
     * scheduled slot, or the next cycle's t2k procedure. */

    /* Firstly, mark the current TX packet as not ACKed and record the MAC sequence
     * number, such that the MAC will use the same MAC sequence to send it.
     * Also, by marking no_ack_counter as non-zero, the neighbor and packet pointers
     *  will then not be released in t2k-end. Then, the packet can be retried right in
     *  the following cycle. */
    netif->mac.tx.no_ack_counter = 1;

    netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
    netif->mac.tx.tx_seq = device_state->seq - 1;

    /* Do not release the packet here, continue sending the same packet. ***/
    if (netif->mac.tx.vtdma_para.slots_num > 0) {
        LOG_DEBUG("[GOMACH] no ACK in vTDMA, retry in next slot.\n");
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_VTDMA_TRANS;
    }
    else {
        /* If no slots for sending, retry in next cycle's t2r, without releasing
         * tx.packet pointer. */
        LOG_DEBUG("[GOMACH] no ACK in vTDMA, retry in next cycle.\n");

        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
    }
    gnrc_gomach_set_update(netif, true);
}

static void gomach_t2k_wait_vtdma_transfeedback(gnrc_netif_t *netif)
{
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR)) {
        /* No TX-ISR, go to sleep. */
        netif->mac.tx.no_ack_counter++;

        netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
        netif->mac.tx.tx_seq = device_state->seq - 1;

        /* Here, we don't queue the packet again, but keep it in tx.packet. */
        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_END;
        gnrc_gomach_set_update(netif, true);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);
        return;
    }

    if (gnrc_gomach_get_tx_finish(netif)) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);

        switch (gnrc_netif_get_tx_feedback(netif)) {
            case TX_FEEDBACK_SUCCESS: {
                _t2k_wait_vtdma_tx_success(netif);
                break;
            }
            case TX_FEEDBACK_BUSY:
            case TX_FEEDBACK_NOACK:
            default: {
                _t2k_wait_vtdma_tx_default(netif);
                break;
            }
        }
    }
}

static void gomach_t2k_end(gnrc_netif_t *netif)
{
    gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_SLEEP);

    /* In GoMacH, normally, in case of transmission failure, no packet will be released
    * in t2k. Failed packet will only be released in t2u. In case of continuous t2k
    * failures, the MAC will goto t2u to retry the packet without releasing it here. */
    if ((netif->mac.tx.packet != NULL) && (netif->mac.tx.no_ack_counter == 0)) {
        LOG_ERROR("ERROR: [GOMACH] t2k: releasing unexpected packet!\n");
        gnrc_pktbuf_release(netif->mac.tx.packet);
        netif->mac.tx.packet = NULL;
    }

    /* In case no_ack_counter is not 0 here, it means we will retry to send the packet
     * in t2k or t2u, then, don't release the neighbor pointer. */
    if (netif->mac.tx.no_ack_counter == 0) {
        netif->mac.tx.current_neighbor = NULL;
    }

    /* Clear all timeouts. */
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_CP);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_BEACON);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_SLOTS);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);

    /* Reset t2k_state to the initial state. */
    netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_INIT;

    netif->mac.prot.gomach.basic_state = GNRC_GOMACH_LISTEN;
    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP;
    gnrc_gomach_set_enter_new_cycle(netif, false);
    gnrc_gomach_set_update(netif, true);
}

static void gomach_t2k_update(gnrc_netif_t *netif)
{
    /* State machine of GoMacH's t2k (transmit to phase-known device) procedure. */
    switch (netif->mac.tx.t2k_state) {
        case GNRC_GOMACH_T2K_INIT: {
            gomach_t2k_init(netif);
            break;
        }
        case GNRC_GOMACH_T2K_WAIT_CP: {
            gomach_t2k_wait_cp(netif);
            break;
        }
        case GNRC_GOMACH_T2K_TRANS_IN_CP: {
            gomach_t2k_trans_in_cp(netif);
            break;
        }
        case GNRC_GOMACH_T2K_WAIT_CPTX_FEEDBACK: {
            gomach_t2k_wait_cp_txfeedback(netif);
            break;
        }
        case GNRC_GOMACH_T2K_WAIT_BEACON: {
            gomach_t2k_wait_beacon(netif);
            break;
        }
        case GNRC_GOMACH_T2K_WAIT_SLOTS: {
            gomach_t2k_wait_own_slots(netif);
            break;
        }
        case GNRC_GOMACH_T2K_VTDMA_TRANS: {
            gomach_t2k_trans_in_slots(netif);
            break;
        }
        case GNRC_GOMACH_T2K_WAIT_VTDMA_FEEDBACK: {
            gomach_t2k_wait_vtdma_transfeedback(netif);
            break;
        }
        case GNRC_GOMACH_T2K_END: {
            gomach_t2k_end(netif);
            break;
        }
        default: break;
    }
}

static void gomach_t2u_init(gnrc_netif_t *netif)
{
    /* since t2u is right following CP period (wake-up period), the radio is still on,
     * so we don't need to turn on it again. */

    LOG_DEBUG("[GOMACH] t2u initialization.\n");

    gnrc_netif_set_rx_started(netif, false);
    gnrc_gomach_set_quit_cycle(netif, false);
    gnrc_gomach_set_pkt_received(netif, false);
    netif->mac.tx.preamble_sent = 0;
    gnrc_gomach_set_got_preamble_ack(netif, false);
    gnrc_gomach_set_buffer_full(netif, false);

    /* Start sending the preamble firstly on public channel 1. */
    gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.pub_channel_1);

    /* Disable auto-ACK here! Don't try to reply ACK to any node. */
    gnrc_gomach_set_autoack(netif, NETOPT_DISABLE);

    gnrc_gomach_set_on_pubchan_1(netif, true);

    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);

    netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_PREAMBLE_PREPARE;
    gnrc_gomach_set_update(netif, true);
}

static void gomach_t2u_send_preamble_prepare(gnrc_netif_t *netif)
{
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL);

    if (netif->mac.tx.preamble_sent != 0) {
        /* Toggle the radio channel after each preamble transmission. */
        if (gnrc_gomach_get_on_pubchan_1(netif)) {
            gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.pub_channel_2);
            gnrc_gomach_set_on_pubchan_1(netif, false);
        }
        else {
            gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.pub_channel_1);
            gnrc_gomach_set_on_pubchan_1(netif, true);
        }
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL,
                                CONFIG_GNRC_GOMACH_MAX_PREAM_INTERVAL_US);
    }
    else {
        /* Here, for the first preamble, we set the pream_max_interval timeout to
         * 5*MAX_PREAM_INTERVAL due to the fact that the first preamble is
         * using csma for sending, and csma costs some time before actually sending
         * the packet. */
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL,
                                (5 * CONFIG_GNRC_GOMACH_MAX_PREAM_INTERVAL_US));
    }

    gnrc_gomach_set_max_pream_interv(netif, false);
    netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_SEND_PREAMBLE;
    gnrc_gomach_set_update(netif, true);
}

static void gomach_t2u_send_preamble(gnrc_netif_t *netif)
{
    /* Now, start sending preamble. */
    int res;

    /* The first preamble is sent with csma for collision avoidance. */
    if (netif->mac.tx.preamble_sent == 0) {
        res = gnrc_gomach_send_preamble(netif, NETOPT_ENABLE);
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAM_DURATION,
                                GNRC_GOMACH_PREAMBLE_DURATION_US);
    }
    else {
        res = gnrc_gomach_send_preamble(netif, NETOPT_DISABLE);
    }

    if (res < 0) {
        LOG_ERROR("ERROR: [GOMACH] t2u send preamble failed: %d\n", res);
    }

    /* In case that packet-buffer is full, quit t2u and release packet. */
    if (res == -ENOBUFS) {
        LOG_ERROR("ERROR: [GOMACH] t2u: no pkt-buffer for sending preamble.\n");
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAM_DURATION);

        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    gnrc_netif_set_rx_started(netif, false);
    netif->mac.tx.preamble_sent++;
    netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_WAIT_PREAMBLE_TX;
    gnrc_gomach_set_update(netif, false);
}

static void gomach_t2u_wait_preamble_tx(gnrc_netif_t *netif)
{
    if (gnrc_gomach_get_tx_finish(netif)) {
        /* Set preamble interval timeout. This is a very short timeout (1ms),
         * just to catch the rx-start event of receiving possible preamble-ACK. */
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE,
                                CONFIG_GNRC_GOMACH_PREAMBLE_INTERVAL_US);

        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_WAIT_PREAMBLE_ACK;
        gnrc_gomach_set_update(netif, false);
        return;
    }

    /* This is mainly to handle no-TX-complete error. Once the max preamble interval
     * timeout expired here (i.e., no-TX-complete error), we will quit waiting here
     * and go to send the next preamble, thus the MAC will not get stuck here. */
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL)) {
        gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_PREAMBLE_PREPARE;
        gnrc_gomach_set_update(netif, true);
        return;
    }
}

static bool _handle_in_t2u_send_preamble(gnrc_netif_t *netif)
{
    /* If packet buffer is full, release one packet to release memory,
     * and reload the next packet.
     * In t2u, we need at least some minimum memory to build the preamble packet. */
    if (gnrc_gomach_get_buffer_full(netif)) {
        gnrc_gomach_set_buffer_full(netif, false);

        gnrc_gomach_set_update(netif, true);

        /* To-do: should we release all the buffered packets in the queue to
         * release memory in such a critical situation? */
        LOG_DEBUG("[GOMACH] t2u: pkt-buffer full, release one pkt.\n");

        if (netif->mac.tx.packet != NULL) {
            gnrc_pktbuf_release(netif->mac.tx.packet);
            netif->mac.tx.packet = NULL;
            netif->mac.tx.no_ack_counter = 0;
        }
        /* Reload the next packet in the neighbor's queue. */
        gnrc_pktsnip_t *pkt = gnrc_priority_pktqueue_pop(&netif->mac.tx.current_neighbor->queue);

        if (pkt != NULL) {
            netif->mac.tx.packet = pkt;
        }
        else {
            LOG_DEBUG("[GOMACH] t2u: null packet, quit t2u.\n");
            netif->mac.tx.current_neighbor = NULL;
            netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
            return false;
        }
    }

    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL)) {
        gnrc_gomach_set_max_pream_interv(netif, true);
        return true;
    }

    /* if we are receiving packet, wait until RX is completed. */
    if ((!gnrc_gomach_timeout_is_running(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END)) &&
        gnrc_netif_get_rx_started(netif) &&
        (!gnrc_gomach_get_max_pream_interv(netif))) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL);

        /* Set a timeout to wait for the complete of reception. */
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);

        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END,
                                CONFIG_GNRC_GOMACH_WAIT_RX_END_US);
        return false;
    }

    if (gnrc_gomach_get_pkt_received(netif)) {
        gnrc_gomach_set_pkt_received(netif, false);
        gnrc_gomach_process_pkt_in_wait_preamble_ack(netif);
    }

    /* Quit t2u if we have to, e.g., the device found ongoing bcast of other devices. */
    if (gnrc_gomach_get_quit_cycle(netif)) {
        LOG_DEBUG("[GOMACH] quit t2u.\n");
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAM_DURATION);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL);

        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
        gnrc_gomach_set_update(netif, true);
        return false;
    }

    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END)) {
        gnrc_gomach_set_max_pream_interv(netif, true);
    }
    return true;
}

static void gomach_t2u_wait_preamble_ack(gnrc_netif_t *netif)
{
    if (!_handle_in_t2u_send_preamble(netif)) {
        return;
    }

    if (gnrc_gomach_get_got_preamble_ack(netif)) {
        /* Require ACK for the packet waiting to be sent! */
        gnrc_gomach_set_ack_req(netif, NETOPT_ENABLE);

        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAM_DURATION);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_SEND_DATA;
        netif->mac.tx.t2u_fail_count = 0;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_PREAM_DURATION)) {
        netif->mac.tx.t2u_retry_counter++;

        /* If we reach the maximum t2u retry limit, release the data packet. */
        if (netif->mac.tx.t2u_retry_counter >= CONFIG_GNRC_GOMACH_T2U_RETYR_THRESHOLD) {
            LOG_DEBUG("[GOMACH] t2u failed: no preamble-ACK.\n");
            netif->mac.tx.t2u_retry_counter = 0;
            netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
            gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE);
            gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
            gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL);

            netif->mac.tx.t2u_fail_count++;
        }
        else {
            /* If we haven't reach the maximum t2u limit, try again. Set quit_current_cycle
             * flag to true such that we will release the current neighbor pointer. */
            gnrc_gomach_set_quit_cycle(netif, true);
            netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
        }

        gnrc_gomach_set_update(netif, true);
        return;
    }

    /* If we didn't catch the RX-start event, go to send the next preamble. */
    if ((gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE)) ||
        gnrc_gomach_get_max_pream_interv(netif)) {
        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_PREAMBLE_PREPARE;
        gnrc_gomach_set_update(netif, true);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
    }
}

static void gomach_t2u_send_data(gnrc_netif_t *netif)
{
    /* If we are retrying to send the data, reload its original MAC sequence. */
    if (netif->mac.tx.no_ack_counter > 0) {
        netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
        device_state->seq = netif->mac.tx.tx_seq;
    }

    /* Here, we send the data to the receiver. */
    int res = gnrc_gomach_send_data(netif, NETOPT_ENABLE);
    if (res < 0) {
        LOG_ERROR("ERROR: [GOMACH] t2u data sending error: %d.\n", res);

        /* If res is < 0, the data packet will not be released in send().
         * so need to release the data here. */
        if (netif->mac.tx.packet != NULL) {
            gnrc_pktbuf_release(netif->mac.tx.packet);
            netif->mac.tx.packet = NULL;
        }

        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR, CONFIG_GNRC_GOMACH_NO_TX_ISR_US);

    netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_WAIT_DATA_TX;
    gnrc_gomach_set_update(netif, false);
}

static void _t2u_data_tx_success(gnrc_netif_t *netif)
{
    /* If transmission succeeded, release the data. */
    gnrc_pktbuf_release(netif->mac.tx.packet);
    netif->mac.tx.packet = NULL;

    netif->mac.tx.no_ack_counter = 0;
    netif->mac.tx.t2u_retry_counter = 0;

    /* Attend the vTDMA procedure if the sender has pending packets for the receiver. */
    if (gnrc_priority_pktqueue_length(&netif->mac.tx.current_neighbor->queue) > 0) {
        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_INIT;
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAM_DURATION);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL);

        /* Switch to t2k procedure and wait for the beacon of the receiver. */
        netif->mac.tx.vtdma_para.slots_num = 0;
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_BEACON,
                                GNRC_GOMACH_WAIT_BEACON_TIME_US);
        gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);

        netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_WAIT_BEACON;
        netif->mac.tx.transmit_state = GNRC_GOMACH_TRANS_TO_KNOWN;
    }
    else {
        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
    }
}

static void _t2u_data_tx_fail(gnrc_netif_t *netif)
{
    netif->mac.tx.t2u_retry_counter++;
    /* If we meet t2u retry limit, release the packet. */
    if (netif->mac.tx.t2u_retry_counter >= CONFIG_GNRC_GOMACH_T2U_RETYR_THRESHOLD) {
        LOG_DEBUG("[GOMACH] t2u send data failed on channel %d,"
                  " drop packet.\n", netif->mac.tx.current_neighbor->pub_chanseq);
        gnrc_pktbuf_release(netif->mac.tx.packet);
        netif->mac.tx.packet = NULL;
        netif->mac.tx.current_neighbor = NULL;
        netif->mac.tx.no_ack_counter = 0;
        netif->mac.tx.t2u_retry_counter = 0;
        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
    }
    else {
        /* Record the MAC sequence of the data, retry t2u in next cycle. */
        netif->mac.tx.no_ack_counter = CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD;
        netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
        netif->mac.tx.tx_seq = device_state->seq - 1;

        LOG_DEBUG("[GOMACH] t2u send data failed on channel %d.\n",
                  netif->mac.tx.current_neighbor->pub_chanseq);
        /* Set quit_current_cycle to true, thus not to release current_neighbour pointer
         * in t2u-end */
        gnrc_gomach_set_quit_cycle(netif, true);
        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;
    }
}

static void gomach_t2u_wait_tx_feedback(gnrc_netif_t *netif)
{
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR)) {
        /* No TX-ISR, go to sleep. */
        netif->mac.tx.t2u_retry_counter++;

        netif->mac.tx.no_ack_counter = CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD;
        netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
        netif->mac.tx.tx_seq = device_state->seq - 1;

        gnrc_gomach_set_quit_cycle(netif, true);
        netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_END;

        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);
        gnrc_gomach_set_update(netif, true);
        return;
    }

    if (gnrc_gomach_get_tx_finish(netif)) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);

        if (gnrc_netif_get_tx_feedback(netif) == TX_FEEDBACK_SUCCESS) {
            _t2u_data_tx_success(netif);
        }
        else {
            _t2u_data_tx_fail(netif);
        }
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_t2u_end(gnrc_netif_t *netif)
{
    gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_SLEEP);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAMBLE);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_PREAM_DURATION);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL);
    gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);

    /* In case quit_current_cycle is true, don't release neighbor pointer,
     * will retry t2u immediately in next cycle.*/
    if (!gnrc_gomach_get_quit_cycle(netif)) {
        if (netif->mac.tx.packet != NULL) {
            gnrc_pktbuf_release(netif->mac.tx.packet);
            netif->mac.tx.packet = NULL;
            netif->mac.tx.no_ack_counter = 0;
            LOG_WARNING("WARNING: [GOMACH] t2u: drop packet.\n");
        }
        netif->mac.tx.current_neighbor = NULL;
    }

    /* Reset t2u state. */
    netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_INIT;

    /* Resume to listen state and go to sleep. */
    netif->mac.prot.gomach.basic_state = GNRC_GOMACH_LISTEN;
    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP;
    gnrc_gomach_set_enter_new_cycle(netif, false);
    gnrc_gomach_set_update(netif, true);
}

static void gomach_t2u_update(gnrc_netif_t *netif)
{
    /* State machine of GoMacH's t2u (transmit to phase-unknown device) procedure. */
    switch (netif->mac.tx.t2u_state) {
        case GNRC_GOMACH_T2U_INIT: {
            gomach_t2u_init(netif);
            break;
        }
        case GNRC_GOMACH_T2U_PREAMBLE_PREPARE: {
            gomach_t2u_send_preamble_prepare(netif);
            break;
        }
        case GNRC_GOMACH_T2U_SEND_PREAMBLE: {
            gomach_t2u_send_preamble(netif);
            break;
        }
        case GNRC_GOMACH_T2U_WAIT_PREAMBLE_TX: {
            gomach_t2u_wait_preamble_tx(netif);
            break;
        }
        case GNRC_GOMACH_T2U_WAIT_PREAMBLE_ACK: {
            gomach_t2u_wait_preamble_ack(netif);
            break;
        }
        case GNRC_GOMACH_T2U_SEND_DATA: {
            gomach_t2u_send_data(netif);
            break;
        }
        case GNRC_GOMACH_T2U_WAIT_DATA_TX: {
            gomach_t2u_wait_tx_feedback(netif);
            break;
        }
        case GNRC_GOMACH_T2U_END: {
            gomach_t2u_end(netif);
            break;
        }
        default: break;
    }
}

static void _gomach_phase_backoff(gnrc_netif_t *netif)
{
    /* Execute phase backoff for avoiding CP (wake-up period) overlap. */
    rtt_clear_alarm();
    xtimer_usleep(netif->mac.prot.gomach.backoff_phase_us);

    rtt_set_counter(0);
    netif->mac.prot.gomach.last_wakeup = rtt_get_counter();

    uint32_t alarm = netif->mac.prot.gomach.last_wakeup +
                     RTT_US_TO_TICKS(CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US);

    rtt_set_alarm(alarm, _gomach_rtt_cb, NULL);

    gnrc_gomach_update_neighbor_phase(netif);

    LOG_INFO("INFO: [GOMACH] phase backoffed: %lu us.\n",
             (unsigned long)netif->mac.prot.gomach.backoff_phase_us);
}

static void gomach_listen_init(gnrc_netif_t *netif)
{
    /* Reset last_seq_info, for avoiding receiving duplicate packets.
     * To-do: remove this in the future? */
    for (uint8_t i = 0; i < GNRC_GOMACH_DUPCHK_BUFFER_SIZE; i++) {
        if (netif->mac.rx.check_dup_pkt.last_nodes[i].node_addr.len != 0) {
            netif->mac.rx.check_dup_pkt.last_nodes[i].life_cycle++;
            if (netif->mac.rx.check_dup_pkt.last_nodes[i].life_cycle >=
                CONFIG_GNRC_GOMACH_RX_DUPCHK_UNIT_LIFE) {
                netif->mac.rx.check_dup_pkt.last_nodes[i].node_addr.len = 0;
                netif->mac.rx.check_dup_pkt.last_nodes[i].node_addr.addr[0] = 0;
                netif->mac.rx.check_dup_pkt.last_nodes[i].node_addr.addr[1] = 0;
                netif->mac.rx.check_dup_pkt.last_nodes[i].seq = 0;
                netif->mac.rx.check_dup_pkt.last_nodes[i].life_cycle = 0;
            }
        }
    }

    if (netif->mac.tx.t2u_fail_count >= CONFIG_GNRC_GOMACH_MAX_T2U_RETYR_THRESHOLD) {
        netif->mac.tx.t2u_fail_count = 0;
        LOG_DEBUG("[GOMACH]: Re-initialize radio.");
        gomach_reinit_radio(netif);
    }
    gnrc_gomach_set_enter_new_cycle(netif, false);

    /* Set listen period timeout. */
    uint32_t listen_period = random_uint32_range(0, CONFIG_GNRC_GOMACH_CP_RANDOM_END_US) +
                             CONFIG_GNRC_GOMACH_CP_DURATION_US;
    gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_END, listen_period);
    gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_MAX, GNRC_GOMACH_CP_DURATION_MAX_US);
    gnrc_netif_set_rx_started(netif, false);
    gnrc_gomach_set_pkt_received(netif, false);
    netif->mac.prot.gomach.cp_extend_count = 0;
    gnrc_gomach_set_quit_cycle(netif, false);
    gnrc_gomach_set_unintd_preamble(netif, false);
    gnrc_gomach_set_beacon_fail(netif, false);
    gnrc_gomach_set_cp_end(netif, false);
    gnrc_gomach_set_got_preamble(netif, false);

    /* Flush RX queue and turn on radio. */
    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
    gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_IDLE);

    /* Turn to current public channel. */
    gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.cur_pub_channel);

    /* Enable Auto-ACK for data packet reception. */
    gnrc_gomach_set_autoack(netif, NETOPT_ENABLE);

    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_CP_LISTEN;
    gnrc_gomach_set_update(netif, false);
}

static void _cp_listen_get_pkt(gnrc_netif_t *netif)
{
    gnrc_gomach_cp_packet_process(netif);

    /* If the device has replied a preamble-ACK, it must waits for the data.
     * Here, we extend the CP. */
    if (gnrc_gomach_get_got_preamble(netif)) {
        gnrc_gomach_set_got_preamble(netif, false);
        gnrc_gomach_set_cp_end(netif, false);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_END);
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_END,
                                CONFIG_GNRC_GOMACH_CP_DURATION_US);
    }
    else if ((!gnrc_gomach_get_unintd_preamble(netif)) &&
             (!gnrc_gomach_get_quit_cycle(netif))) {
        gnrc_gomach_set_got_preamble(netif, false);
        gnrc_gomach_set_cp_end(netif, false);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_END);
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_END,
                                CONFIG_GNRC_GOMACH_CP_DURATION_US);
    }
}

static void _cp_listen_end(gnrc_netif_t *netif)
{
    /* If we found ongoing reception, wait for reception complete. */
    if ((gnrc_gomach_get_netdev_state(netif) == NETOPT_STATE_RX) &&
        (netif->mac.prot.gomach.cp_extend_count < CONFIG_GNRC_GOMACH_CP_EXTEND_THRESHOLD)) {
        netif->mac.prot.gomach.cp_extend_count++;
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
        gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END,
                                CONFIG_GNRC_GOMACH_WAIT_RX_END_US);
    }
    else {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_END);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_MAX);
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_CP_END;
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_listen_cp_listen(gnrc_netif_t *netif)
{
    if (gnrc_gomach_get_pkt_received(netif)) {
        gnrc_gomach_set_pkt_received(netif, false);
        _cp_listen_get_pkt(netif);
    }

    /* If we have reached the maximum CP duration, quit CP. */
    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_CP_MAX)) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_END);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_MAX);
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_CP_END;
        gnrc_gomach_set_update(netif, true);
        return;
    }

    if ((gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_CP_END))) {
        gnrc_gomach_set_cp_end(netif, true);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_CP_END);
    }

    /* If CP duration timeouted or we must quit CP, go to CP end. */
    if (gnrc_gomach_get_cp_end(netif) || gnrc_gomach_get_quit_cycle(netif)) {
        _cp_listen_end(netif);
    }
}

static void gomach_listen_cp_end(gnrc_netif_t *netif)
{
    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
    gnrc_mac_dispatch(&netif->mac.rx);

    /* If we need to quit communications in this cycle, go to sleep. */
    if (gnrc_gomach_get_quit_cycle(netif)) {
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_INIT;
    }
    else {
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SEND_BEACON;
    }
    gnrc_gomach_set_update(netif, true);
}

static void gomach_listen_send_beacon(gnrc_netif_t *netif)
{
    /* First check if there are slots needed to be allocated. */
    uint8_t slot_num = 0;

    for (uint8_t i = 0; i < GNRC_GOMACH_SLOSCH_UNIT_COUNT; i++) {
        if (netif->mac.rx.slosch_list[i].queue_indicator > 0) {
            slot_num += netif->mac.rx.slosch_list[i].queue_indicator;
            break;
        }
    }

    if (slot_num > 0) {
        /* Disable auto-ACK. Thus not to receive packet (attempt to reply ACK) anymore. */
        gnrc_gomach_set_autoack(netif, NETOPT_DISABLE);

        /* Assemble and send the beacon. */
        int res = gnrc_gomach_send_beacon(netif);
        if (res < 0) {
            LOG_ERROR("ERROR: [GOMACH] send beacon error: %d.\n", res);
            gnrc_gomach_set_beacon_fail(netif, true);
            gnrc_gomach_set_update(netif, true);
        }
        else {
            gnrc_gomach_set_update(netif, false);
        }
    }
    else {
        /* No need to send beacon, go to next state. */
        gnrc_gomach_set_beacon_fail(netif, true);
        gnrc_gomach_set_update(netif, true);
    }

    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_WAIT_BEACON_TX;
}

static void _no_vtdma_after_cp(gnrc_netif_t *netif)
{
    /* If the device hasn't allocated transmission slots, check whether it has packets
     * to transmit to neighbor. */
    if (gnrc_gomach_find_next_tx_neighbor(netif)) {
        /* Now, we have packet to send. */

        if (netif->mac.tx.current_neighbor == &netif->mac.tx.neighbors[0]) {
            /* The packet is for broadcasting. */

            /* If we didn't find ongoing preamble stream, go to send broadcast packet. */
            if (!gnrc_gomach_get_unintd_preamble(netif)) {
                netif->mac.prot.gomach.basic_state = GNRC_GOMACH_TRANSMIT;
                netif->mac.tx.transmit_state = GNRC_GOMACH_BROADCAST;
            }
            else {
                /* If we find ongoing preamble stream, go to sleep. */
                netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_INIT;
            }
        }
        else {
            /* The packet waiting to be sent is for unicast. */
            switch (netif->mac.tx.current_neighbor->mac_type) {
                case GNRC_GOMACH_TYPE_UNKNOWN: {
                    /* The neighbor's phase is unknown yet, try to run t2u (transmission
                     * to unknown device) procedure to phase-lock the neighbor. */

                    /* If we didn't find ongoing preamble stream, go to t2u procedure. */
                    if (!gnrc_gomach_get_unintd_preamble(netif)) {
                        netif->mac.prot.gomach.basic_state = GNRC_GOMACH_TRANSMIT;
                        netif->mac.tx.transmit_state = GNRC_GOMACH_TRANS_TO_UNKNOWN;
                    }
                    else {
                        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_INIT;
                    }
                    break;
                }
                case GNRC_GOMACH_TYPE_KNOWN: {
                    /* If the neighbor's phase is known, go to t2k (transmission
                     * to known device) procedure. Here, we don't worry that the t2k
                     * unicast transmission will interrupt with possible ongoing
                     * preamble transmissions of other devices. */
                    netif->mac.prot.gomach.basic_state = GNRC_GOMACH_TRANSMIT;
                    netif->mac.tx.transmit_state = GNRC_GOMACH_TRANS_TO_KNOWN;
                    break;
                }
                default: {
                    LOG_ERROR("ERROR: [GOMACH] vTDMA: unknown MAC type of "
                              "the neighbor.\n");
                    break;
                }
            }
        }
    }
    else {
        /* No packet to send, go to sleep. */
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_INIT;
    }

    gnrc_gomach_set_update(netif, true);
}

static void gomach_listen_wait_beacon_tx(gnrc_netif_t *netif)
{
    if ((gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR))) {
        /* No TX-ISR, go to sleep. */
        LOG_DEBUG("[GOMACH]: no TX-finish ISR.");
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_INIT;
        gnrc_gomach_set_update(netif, true);
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);
        return;
    }

    if (gnrc_gomach_get_tx_finish(netif) ||
        gnrc_gomach_get_beacon_fail(netif)) {
        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_NO_TX_ISR);

        if ((netif->mac.rx.vtdma_manag.total_slots_num > 0) &&
            (!gnrc_gomach_get_beacon_fail(netif))) {
            /* If the device has allocated transmission slots to other nodes,
             *  switch to vTDMA period to receive packets. */
            netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_VTDMA_INIT;
            gnrc_gomach_set_update(netif, true);
        }
        else {
            _no_vtdma_after_cp(netif);
        }
    }
}

static void gomach_vtdma_init(gnrc_netif_t *netif)
{
    /* Switch the radio to the device's sub-channel. */
    gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.sub_channel_seq);

    /* Enable Auto ACK again for data reception */
    gnrc_gomach_set_autoack(netif, NETOPT_ENABLE);

    /* Set the vTDMA period timeout. */
    uint32_t vtdma_duration = netif->mac.rx.vtdma_manag.total_slots_num *
                              CONFIG_GNRC_GOMACH_VTDMA_SLOT_SIZE_US;
    gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_VTDMA, vtdma_duration);

    gnrc_gomach_set_vTDMA_end(netif, false);

    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_VTDMA;
    gnrc_gomach_set_update(netif, false);
}

static void gomach_vtdma(gnrc_netif_t *netif)
{
    /* Process received packet here. */
    if (gnrc_gomach_get_pkt_received(netif)) {
        gnrc_gomach_set_pkt_received(netif, false);
        gnrc_gomach_packet_process_in_vtdma(netif);
    }

    if (gnrc_gomach_timeout_is_expired(netif, GNRC_GOMACH_TIMEOUT_VTDMA)) {
        gnrc_gomach_set_vTDMA_end(netif, true);
    }

    /* Go to vTDMA end after vTDMA timeout expires. */
    if (gnrc_gomach_get_vTDMA_end(netif)) {
        /* Wait for reception complete if found ongoing transmission. */
        if (gnrc_gomach_get_netdev_state(netif) == NETOPT_STATE_RX) {
            gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
            gnrc_gomach_set_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END,
                                    CONFIG_GNRC_GOMACH_WAIT_RX_END_US);
            return;
        }

        gnrc_gomach_clear_timeout(netif, GNRC_GOMACH_TIMEOUT_WAIT_RX_END);
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_VTDMA_END;
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_vtdma_end(gnrc_netif_t *netif)
{
    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
    gnrc_mac_dispatch(&netif->mac.rx);

    /* Switch the radio to the public-channel. */
    gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.cur_pub_channel);

    /* Check if there is packet to send. */
    if (gnrc_gomach_find_next_tx_neighbor(netif)) {
        if (netif->mac.tx.current_neighbor == &netif->mac.tx.neighbors[0]) {
            /* The packet is for broadcasting. */
            if (!gnrc_gomach_get_unintd_preamble(netif)) {
                netif->mac.prot.gomach.basic_state = GNRC_GOMACH_TRANSMIT;
                netif->mac.tx.transmit_state = GNRC_GOMACH_BROADCAST;
            }
            else {
                netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_INIT;
            }
        }
        else {
            switch (netif->mac.tx.current_neighbor->mac_type) {
                /* The packet waiting to be sent is for unicast. */
                case GNRC_GOMACH_TYPE_UNKNOWN: {
                    /* The neighbor's phase is unknown yet, try to run t2u (transmission
                     * to unknown device) procedure to phase-lock the neighbor. */
                    if (!gnrc_gomach_get_unintd_preamble(netif)) {
                        netif->mac.prot.gomach.basic_state = GNRC_GOMACH_TRANSMIT;
                        netif->mac.tx.transmit_state = GNRC_GOMACH_TRANS_TO_UNKNOWN;
                    }
                    else {
                        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_INIT;
                    }
                } break;
                case GNRC_GOMACH_TYPE_KNOWN: {
                    /* If the neighbor's phase is known, go to t2k (transmission
                     * to known device) procedure. Here, we don't worry that the t2k
                     * unicast transmission will interrupt with possible ongoing
                     * preamble transmissions of other devices. */
                    netif->mac.prot.gomach.basic_state = GNRC_GOMACH_TRANSMIT;
                    netif->mac.tx.transmit_state = GNRC_GOMACH_TRANS_TO_KNOWN;
                } break;
                default: {
                    LOG_ERROR("ERROR: [GOMACH] vTDMA: unknown MAC type of the neighbor.\n");
                    break;
                }
            }
        }
    }
    else {
        /* No packet to send, go to sleep. */
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_INIT;
    }

    gnrc_gomach_set_update(netif, true);
}

static void gomach_sleep_init(gnrc_netif_t *netif)
{
    /* Turn off the radio during sleep period to conserve power. */
    gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_SLEEP);
    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP;
    gnrc_gomach_set_update(netif, true);
}

static void gomach_sleep(gnrc_netif_t *netif)
{
    /* If we are entering a new cycle, quit sleeping. */
    if (gnrc_gomach_get_enter_new_cycle(netif)) {
        netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_SLEEP_END;
        gnrc_gomach_set_update(netif, true);
    }
}

static void gomach_sleep_end(gnrc_netif_t *netif)
{
    /* Run phase-backoff if needed, select a new wake-up phase. */
    if (gnrc_gomach_get_phase_backoff(netif)) {
        gnrc_gomach_set_phase_backoff(netif, false);
        _gomach_phase_backoff(netif);
    }

    /* Go to CP (start of the new cycle), start listening on the public-channel. */
    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_CP_INIT;
    gnrc_gomach_set_update(netif, true);
}

static void gomach_update(gnrc_netif_t *netif)
{
    switch (netif->mac.prot.gomach.basic_state) {
        case GNRC_GOMACH_INIT: {
            /* State machine of GoMacH's initialization procedure. */
            switch (netif->mac.prot.gomach.init_state) {
                case GNRC_GOMACH_INIT_PREPARE: {
                    gomach_init_prepare(netif);
                    break;
                }
                case GNRC_GOMACH_INIT_ANNC_SUBCHAN: {
                    gomach_init_announce_subchannel(netif);
                    break;
                }
                case GNRC_GOMACH_INIT_WAIT_FEEDBACK: {
                    gomach_init_wait_announce_feedback(netif);
                    break;
                }
                case GNRC_GOMACH_INIT_END: {
                    gomach_init_end(netif);
                    break;
                }
                default: break;
            }
            break;
        }
        case GNRC_GOMACH_LISTEN: {
            /* State machine of GoMacH's duty-cycled listen procedure. */
            switch (netif->mac.rx.listen_state) {
                case GNRC_GOMACH_LISTEN_CP_INIT: {
                    gomach_listen_init(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_CP_LISTEN: {
                    gomach_listen_cp_listen(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_CP_END: {
                    gomach_listen_cp_end(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_SEND_BEACON: {
                    gomach_listen_send_beacon(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_WAIT_BEACON_TX: {
                    gomach_listen_wait_beacon_tx(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_VTDMA_INIT: {
                    gomach_vtdma_init(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_VTDMA: {
                    gomach_vtdma(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_VTDMA_END: {
                    gomach_vtdma_end(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_SLEEP_INIT: {
                    gomach_sleep_init(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_SLEEP: {
                    gomach_sleep(netif);
                    break;
                }
                case GNRC_GOMACH_LISTEN_SLEEP_END: {
                    gomach_sleep_end(netif);
                    break;
                }
                default: break;
            }
            break;
        }
        case GNRC_GOMACH_TRANSMIT: {
            /* State machine of GoMacH's basic transmission scheme. */
            switch (netif->mac.tx.transmit_state) {
                case GNRC_GOMACH_TRANS_TO_UNKNOWN: {
                    gomach_t2u_update(netif);
                    break;
                }
                case GNRC_GOMACH_TRANS_TO_KNOWN: {
                    gomach_t2k_update(netif);
                    break;
                }
                case GNRC_GOMACH_BROADCAST: {
                    gomach_bcast_update(netif);
                    break;
                }
                default: break;
            }
            break;
        }
        default: break;
    }
}

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, pkt)) {
        /* TX packet queue full, release the packet. */
        DEBUG("[GOMACH] TX queue full, drop packet.\n");
        gnrc_pktbuf_release(pkt);
    }
    gnrc_gomach_set_update(netif, true);

    while (gnrc_gomach_get_update(netif)) {
        gnrc_gomach_set_update(netif, false);
        gomach_update(netif);
    }

    return 0;
}

static void _gomach_msg_handler(gnrc_netif_t *netif, msg_t *msg)
{
    switch (msg->type) {
        case GNRC_GOMACH_EVENT_RTT_TYPE: {
            _gomach_rtt_handler(msg->content.value, netif);
            break;
        }
        case GNRC_GOMACH_EVENT_TIMEOUT_TYPE: {
            /* GoMacH timeout expires. */
            gnrc_gomach_timeout_make_expire((gnrc_gomach_timeout_t *) msg->content.ptr);
            gnrc_gomach_set_update(netif, true);
            break;
        }
#if (GNRC_MAC_ENABLE_DUTYCYCLE_RECORD == 1)
        case GNRC_MAC_TYPE_GET_DUTYCYCLE: {
            /* Output GoMacH's current radio duty-cycle. */
            uint64_t duty;
            duty = xtimer_now_usec64();
            duty = (netif->mac.prot.gomach.awake_duration_sum_ticks) * 100 /
                   (duty - netif->mac.prot.gomach.system_start_time_ticks);
            printf("[GoMacH]: achieved radio duty-cycle: %lu %% \n",
                   (unsigned long)duty);
            break;
        }
#endif
        default: {
            DEBUG("[GoMacH]: Unknown command %" PRIu16 "\n", msg->type);
            break;
        }
    }

    while (gnrc_gomach_get_update(netif)) {
        gnrc_gomach_set_update(netif, false);
        gomach_update(netif);
    }
}

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event     type of event
 */
static void _gomach_event_cb(netdev_t *dev, netdev_event_t event)
{
    gnrc_netif_t *netif = (gnrc_netif_t *) dev->context;

    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;

        msg.type = NETDEV_MSG_TYPE_EVENT;
        msg.content.ptr = (void *) netif;

        if (msg_send(&msg, netif->pid) <= 0) {
            DEBUG("[GOMACH] gnrc_netdev: possibly lost interrupt.\n");
        }
    }
    else {
        DEBUG("gnrc_netdev: event triggered -> %i\n", event);
        switch (event) {
            case NETDEV_EVENT_RX_STARTED: {
                gnrc_netif_set_rx_started(netif, true);
                gnrc_gomach_set_update(netif, true);
                break;
            }
            case NETDEV_EVENT_RX_COMPLETE: {
                gnrc_gomach_set_update(netif, true);

                gnrc_pktsnip_t *pkt = netif->ops->recv(netif);
                if (pkt == NULL) {
                    gnrc_gomach_set_buffer_full(netif, true);

                    LOG_DEBUG("[GOMACH] gnrc_netdev: packet is NULL, memory full?\n");
                    gnrc_gomach_set_pkt_received(netif, false);
                    gnrc_netif_set_rx_started(netif, false);
                    break;
                }

                if (!gnrc_netif_get_rx_started(netif)) {
                    LOG_DEBUG("[GOMACH] gnrc_netdev: maybe sending kicked in "
                              "and frame buffer is now corrupted?\n");
                    gnrc_pktbuf_release(pkt);
                    gnrc_netif_set_rx_started(netif, false);
                    break;
                }

                gnrc_netif_set_rx_started(netif, false);

                if (!gnrc_mac_queue_rx_packet(&netif->mac.rx, 0, pkt)) {
                    LOG_ERROR("ERROR: [GOMACH] gnrc_netdev: can't push RX packet, queue full?\n");
                    gnrc_pktbuf_release(pkt);
                    gnrc_gomach_set_pkt_received(netif, false);
                    break;
                }
                else {
                    gnrc_gomach_set_pkt_received(netif, true);
                }
                break;
            }
            case NETDEV_EVENT_TX_COMPLETE: {
                gnrc_netif_set_tx_feedback(netif, TX_FEEDBACK_SUCCESS);
                gnrc_gomach_set_tx_finish(netif, true);
                gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_IDLE);
                gnrc_gomach_set_update(netif, true);
                break;
            }
            case NETDEV_EVENT_TX_NOACK: {
                gnrc_netif_set_tx_feedback(netif, TX_FEEDBACK_NOACK);
                gnrc_gomach_set_tx_finish(netif, true);
                gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_IDLE);
                gnrc_gomach_set_update(netif, true);
                break;
            }
            case NETDEV_EVENT_TX_MEDIUM_BUSY: {
                gnrc_netif_set_tx_feedback(netif, TX_FEEDBACK_BUSY);
                gnrc_gomach_set_tx_finish(netif, true);
                gnrc_gomach_set_netdev_state(netif, NETOPT_STATE_IDLE);
                gnrc_gomach_set_update(netif, true);
                break;
            }
            default: {
                DEBUG("WARNING [GoMacH]: unhandled event %u.\n", event);
            }
        }
    }

    while (gnrc_gomach_get_update(netif)) {
        gnrc_gomach_set_update(netif, false);
        gomach_update(netif);
    }
}

static void _gomach_init(gnrc_netif_t *netif)
{
    netdev_t *dev;

    gnrc_netif_default_init(netif);
    dev = netif->dev;
    dev->event_callback = _gomach_event_cb;

    /* Initialize RTT. */
    rtt_init();

    /* Store pid globally, so that IRQ can use it to send message. */
    gomach_pid = netif->pid;

    /* Set MAC address length. */
    uint16_t src_len = IEEE802154_LONG_ADDRESS_LEN;
    dev->driver->set(dev, NETOPT_SRC_LEN, &src_len, sizeof(src_len));

    /* Get the MAC address of the device. */
    netif->l2addr_len = dev->driver->get(dev,
                                         NETOPT_ADDRESS_LONG,
                                         netif->l2addr,
                                         sizeof(netif->l2addr));

    /* Initialize GoMacH's state machines. */
    netif->mac.prot.gomach.basic_state = GNRC_GOMACH_INIT;
    netif->mac.prot.gomach.init_state = GNRC_GOMACH_INIT_PREPARE;
    netif->mac.rx.listen_state = GNRC_GOMACH_LISTEN_CP_INIT;
    netif->mac.tx.transmit_state = GNRC_GOMACH_TRANS_TO_UNKNOWN;
    netif->mac.tx.bcast_state = GNRC_GOMACH_BCAST_INIT;
    netif->mac.tx.t2k_state = GNRC_GOMACH_T2K_INIT;
    netif->mac.tx.t2u_state = GNRC_GOMACH_T2U_INIT;

    /* Initialize GoMacH's channels. */
    netif->mac.prot.gomach.sub_channel_seq = 13;
    netif->mac.prot.gomach.pub_channel_1 = 26;
    netif->mac.prot.gomach.pub_channel_2 = 11;
    netif->mac.prot.gomach.cur_pub_channel = netif->mac.prot.gomach.pub_channel_1;
    gnrc_gomach_turn_channel(netif, netif->mac.prot.gomach.cur_pub_channel);

    /* Enable RX-start and TX-started and TX-END interrupts. */
    netopt_enable_t enable = NETOPT_ENABLE;
    dev->driver->set(dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

    /* Initialize broadcast sequence number. This at least differs from board
     * to board. */
    netif->mac.tx.broadcast_seq = netif->l2addr[netif->l2addr_len - 1];

    /* Reset all timeouts just to be sure. */
    gnrc_gomach_reset_timeouts(netif);

    /* Initialize GoMacH's other key parameters. */
    netif->mac.tx.no_ack_counter = 0;
    gnrc_gomach_set_enter_new_cycle(netif, false);
    netif->mac.rx.vtdma_manag.sub_channel_seq = 26;
    netif->mac.prot.gomach.subchannel_occu_flags = 0;
    gnrc_gomach_set_pkt_received(netif, false);
    gnrc_gomach_set_update(netif, false);
    gnrc_gomach_set_duty_cycle_start(netif, false);
    gnrc_gomach_set_quit_cycle(netif, false);

    gnrc_gomach_set_beacon_fail(netif, false);
    gnrc_gomach_set_buffer_full(netif, false);
    gnrc_gomach_set_phase_backoff(netif, false);
    netif->mac.rx.check_dup_pkt.queue_head = 0;
    netif->mac.tx.last_tx_neighbor_id = 0;

    netdev_ieee802154_t *device_state = (netdev_ieee802154_t *)netif->dev;
    device_state->seq = netif->l2addr[netif->l2addr_len - 1];

    /* Initialize GoMacH's duplicate-check scheme. */
    for (uint8_t i = 0; i < GNRC_GOMACH_DUPCHK_BUFFER_SIZE; i++) {
        netif->mac.rx.check_dup_pkt.last_nodes[i].node_addr.len = 0;
    }

    /* Set the random seed. */
    uint32_t seed = 0;
    seed = netif->l2addr[netif->l2addr_len - 2];
    seed = seed << 8;
    seed |= netif->l2addr[netif->l2addr_len - 1];
    random_init(seed);

    netif->mac.tx.t2u_fail_count = 0;

#if (GNRC_MAC_ENABLE_DUTYCYCLE_RECORD == 1)
    /* Start duty cycle recording */
    netif->mac.prot.gomach.system_start_time_ticks = xtimer_now_usec64();
    netif->mac.prot.gomach.last_radio_on_time_ticks =
        netif->mac.prot.gomach.system_start_time_ticks;
    netif->mac.prot.gomach.awake_duration_sum_ticks = 0;
    netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_RADIO_IS_ON;
#endif

    gnrc_gomach_set_update(netif, true);

    while (gnrc_gomach_get_update(netif)) {
        gnrc_gomach_set_update(netif, false);
        gomach_update(netif);
    }
}
