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
 * @brief       Implementation of RX state machine of LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <assert.h>

#include "net/gnrc.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "net/gnrc/mac/internal.h"
#include "net/gnrc/lwmac/timeout.h"
#include "include/rx_state_machine.h"
#include "include/lwmac_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef LOG_LEVEL
/**
 * @brief Default log level define
 */
#define LOG_LEVEL LOG_WARNING
#endif
#include "log.h"

/**
 * @brief   Flag to track if the receiver has got a broadcast packet
 */
#define GNRC_LWMAC_RX_FOUND_BROADCAST         (0x01U)

/**
 * @brief   Flag to track if the receiver has got a WR packet
 */
#define GNRC_LWMAC_RX_FOUND_WR                (0x02U)

/**
 * @brief   Flag to track if the receiver has got a data packet
 */
#define GNRC_LWMAC_RX_FOUND_DATA              (0x04U)

static uint8_t _packet_process_in_wait_for_wr(gnrc_netif_t *netif)
{
    uint8_t rx_info = 0;
    gnrc_pktsnip_t *pkt;

    assert(netif != NULL);

    while ((pkt = gnrc_priority_pktqueue_pop(&netif->mac.rx.queue)) != NULL) {
        LOG_DEBUG("[LWMAC-rx] Inspecting pkt @ %p\n", pkt);

        /* Parse packet */
        gnrc_lwmac_packet_info_t info;

        if (_gnrc_lwmac_parse_packet(pkt, &info) != 0) {
            LOG_DEBUG("[LWMAC-rx] Packet could not be parsed\n");
            gnrc_pktbuf_release(pkt);
            continue;
        }

        if (info.header->type == GNRC_LWMAC_FRAMETYPE_BROADCAST) {
            _gnrc_lwmac_dispatch_defer(netif->mac.rx.dispatch_buffer, pkt);
            gnrc_mac_dispatch(&netif->mac.rx);
            rx_info |= GNRC_LWMAC_RX_FOUND_BROADCAST;
            /* quit listening period to avoid receiving duplicate broadcast packets */
            gnrc_lwmac_set_quit_rx(netif, true);
            /* quit TX in this cycle to avoid collisions with broadcast packets */
            gnrc_lwmac_set_quit_tx(netif, true);
            break;
        }

        if (info.header->type != GNRC_LWMAC_FRAMETYPE_WR) {
            LOG_DEBUG("[LWMAC-rx] Packet is not WR: 0x%02x\n", info.header->type);
            gnrc_pktbuf_release(pkt);
            continue;
        }

        /* No need to keep pkt anymore */
        gnrc_pktbuf_release(pkt);

        if (!(memcmp(&info.dst_addr.addr, &netif->l2addr,
                     netif->l2addr_len) == 0)) {
            LOG_DEBUG("[LWMAC-rx] Packet is WR but not for us\n");
            /* quit TX in this cycle to avoid collisions with other senders, since
             * found ongoing WR (preamble) stream */
            gnrc_lwmac_set_quit_tx(netif, true);
            continue;
        }

        /* If reach here, the node gets a WR for itself. */
        /* Save source address for later addressing */
        netif->mac.rx.l2_addr = info.src_addr;

        rx_info |= GNRC_LWMAC_RX_FOUND_WR;
        break;
    }

    return rx_info;
}

/* return false if send wa failed, otherwise return true */
static bool _send_wa(gnrc_netif_t *netif)
{
    gnrc_pktsnip_t *pkt;
    gnrc_pktsnip_t *pkt_lwmac;
    gnrc_netif_hdr_t *nethdr_wa;

    assert(netif != NULL);
    assert(netif->mac.rx.l2_addr.len != 0);

    /* if found ongoing transmission,
     * quit sending WA for collision avoidance. */
    if (_gnrc_lwmac_get_netdev_state(netif) == NETOPT_STATE_RX) {
        netif->mac.rx.rx_bad_exten_count++;
        return false;
    }

    /* Assemble WA packet */
    gnrc_lwmac_frame_wa_t lwmac_hdr;
    lwmac_hdr.header.type = GNRC_LWMAC_FRAMETYPE_WA;
    lwmac_hdr.dst_addr = netif->mac.rx.l2_addr;

    uint32_t phase_now = _gnrc_lwmac_phase_now();

    /* Embed the current 'relative phase timing' (counted from the start of this cycle)
     * of the receiver into its WA packet, thus to allow the sender to infer the
     * receiver's exact wake-up timing */
    if (phase_now > _gnrc_lwmac_ticks_to_phase(netif->mac.prot.lwmac.last_wakeup)) {
        lwmac_hdr.current_phase = (phase_now -
                                   _gnrc_lwmac_ticks_to_phase(netif->mac.prot.lwmac.last_wakeup));
    }
    else {
        lwmac_hdr.current_phase = (phase_now +
                                   RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US)) -
                                   _gnrc_lwmac_ticks_to_phase(netif->mac.prot.lwmac.last_wakeup);
    }

    pkt = gnrc_pktbuf_add(NULL, &lwmac_hdr, sizeof(lwmac_hdr), GNRC_NETTYPE_LWMAC);
    if (pkt == NULL) {
        LOG_ERROR("ERROR: [LWMAC-rx] Cannot allocate pktbuf of type GNRC_NETTYPE_LWMAC\n");
        gnrc_lwmac_set_quit_rx(netif, true);
        return false;
    }
    pkt_lwmac = pkt;

    pkt = gnrc_pktbuf_add(pkt, NULL,
                          sizeof(gnrc_netif_hdr_t) + netif->mac.rx.l2_addr.len,
                          GNRC_NETTYPE_NETIF);
    if (pkt == NULL) {
        LOG_ERROR("ERROR: [LWMAC-rx] Cannot allocate pktbuf of type GNRC_NETTYPE_NETIF\n");
        gnrc_pktbuf_release(pkt_lwmac);
        gnrc_lwmac_set_quit_rx(netif, true);
        return false;
    }

    /* We wouldn't get here if add the NETIF header had failed, so no
       sanity checks needed */
    nethdr_wa = (gnrc_netif_hdr_t *)(gnrc_pktsnip_search_type(pkt,
                                                              GNRC_NETTYPE_NETIF)->data);
    /* Construct NETIF header and insert address for WA packet */
    gnrc_netif_hdr_init(nethdr_wa, 0, netif->mac.rx.l2_addr.len);

    /* Send WA as broadcast*/
    nethdr_wa->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;

    /* Disable Auto ACK */
    netopt_enable_t autoack = NETOPT_DISABLE;
    netif->dev->driver->set(netif->dev, NETOPT_AUTOACK, &autoack,
                            sizeof(autoack));

    /* Send WA */
    if (_gnrc_lwmac_transmit(netif, pkt) < 0) {
        LOG_ERROR("ERROR: [LWMAC-rx] Send WA failed.");
        gnrc_pktbuf_release(pkt);
        gnrc_lwmac_set_quit_rx(netif, true);
        return false;
    }

    /* Enable Auto ACK again for data reception */
    autoack = NETOPT_ENABLE;
    netif->dev->driver->set(netif->dev, NETOPT_AUTOACK, &autoack,
                            sizeof(autoack));

    return true;
}

static uint8_t _packet_process_in_wait_for_data(gnrc_netif_t *netif)
{
    uint8_t rx_info = 0;
    gnrc_pktsnip_t *pkt;

    assert(netif != NULL);

    pkt = NULL;

    while ((pkt = gnrc_priority_pktqueue_pop(&netif->mac.rx.queue)) != NULL) {
        LOG_DEBUG("[LWMAC-rx] Inspecting pkt @ %p\n", pkt);

        /* Parse packet */
        gnrc_lwmac_packet_info_t info;

        if (_gnrc_lwmac_parse_packet(pkt, &info) != 0) {
            LOG_DEBUG("[LWMAC-rx] Packet could not be parsed\n");
            gnrc_pktbuf_release(pkt);
            continue;
        }

        if (info.header->type == GNRC_LWMAC_FRAMETYPE_BROADCAST) {
            _gnrc_lwmac_dispatch_defer(netif->mac.rx.dispatch_buffer, pkt);
            gnrc_mac_dispatch(&netif->mac.rx);
            /* quit listening period to avoid receiving duplicate broadcast packets */
            gnrc_lwmac_set_quit_rx(netif, true);
            continue;
        }

        if (!(memcmp(&info.src_addr.addr, &netif->mac.rx.l2_addr.addr,
                     netif->mac.rx.l2_addr.len) == 0)) {
            LOG_DEBUG("[LWMAC-rx] Packet is not from destination\n");
            gnrc_pktbuf_release(pkt);
            /* Reset timeout to wait for the data packet */
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA);
            gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA, CONFIG_GNRC_LWMAC_DATA_DELAY_US);
            continue;
        }

        if (!(memcmp(&info.dst_addr.addr, &netif->l2addr,
                     netif->l2addr_len) == 0)) {
            LOG_DEBUG("[LWMAC-rx] Packet is not for us\n");
            gnrc_pktbuf_release(pkt);
            /* Reset timeout to wait for the data packet */
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA);
            gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA, CONFIG_GNRC_LWMAC_DATA_DELAY_US);
            continue;
        }

        /* Sender maybe didn't get the WA */
        if (info.header->type == GNRC_LWMAC_FRAMETYPE_WR) {
            LOG_DEBUG("[LWMAC-rx] Found a WR while waiting for DATA\n");
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA);
            rx_info |= GNRC_LWMAC_RX_FOUND_WR;
            /* Push WR back to rx queue */
            gnrc_mac_queue_rx_packet(&netif->mac.rx, 0, pkt);
            break;
        }

        switch (info.header->type) {
            case GNRC_LWMAC_FRAMETYPE_DATA:
            case GNRC_LWMAC_FRAMETYPE_DATA_PENDING: {
                /* Receiver gets the data packet */
                _gnrc_lwmac_dispatch_defer(netif->mac.rx.dispatch_buffer, pkt);
                gnrc_mac_dispatch(&netif->mac.rx);
                LOG_DEBUG("[LWMAC-rx] Found DATA!\n");
                gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA);
                rx_info |= GNRC_LWMAC_RX_FOUND_DATA;
                return rx_info;
            }
            default: {
                gnrc_pktbuf_release(pkt);
            }
        }
    }

    return rx_info;
}

void gnrc_lwmac_rx_start(gnrc_netif_t *netif)
{
    if (netif == NULL) {
        return;
    }

    /* RX address should have been reset, probably not stopped then */
    assert(netif->mac.rx.l2_addr.len == 0);

    /* Don't attempt to send a WA if channel is busy to get timings right */
    netif->mac.mac_info &= ~GNRC_NETIF_MAC_INFO_CSMA_ENABLED;
    netopt_enable_t csma_disable = NETOPT_DISABLE;
    netif->dev->driver->set(netif->dev, NETOPT_CSMA, &csma_disable,
                            sizeof(csma_disable));

    netif->mac.rx.state = GNRC_LWMAC_RX_STATE_INIT;
}

void gnrc_lwmac_rx_stop(gnrc_netif_t *netif)
{
    if (!netif) {
        return;
    }

    gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA);
    netif->mac.rx.state = GNRC_LWMAC_RX_STATE_STOPPED;
    netif->mac.rx.l2_addr.len = 0;
}

/* Returns whether rescheduling is needed or not */
static bool _lwmac_rx_update(gnrc_netif_t *netif)
{
    bool reschedule = false;

    if (!netif) {
        return reschedule;
    }

    switch (netif->mac.rx.state) {
        case GNRC_LWMAC_RX_STATE_INIT: {
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA);
            netif->mac.rx.state = GNRC_LWMAC_RX_STATE_WAIT_FOR_WR;
            reschedule = true;
            break;
        }
        case GNRC_LWMAC_RX_STATE_WAIT_FOR_WR: {
            LOG_DEBUG("[LWMAC-rx] GNRC_LWMAC_RX_STATE_WAIT_FOR_WR\n");

            uint8_t rx_info = _packet_process_in_wait_for_wr(netif);

            /* if found broadcast packet, goto rx successful */
            if (rx_info & GNRC_LWMAC_RX_FOUND_BROADCAST) {
                netif->mac.rx.state = GNRC_LWMAC_RX_STATE_SUCCESSFUL;
                reschedule = true;
                break;
            }

            if (!(rx_info & GNRC_LWMAC_RX_FOUND_WR)) {
                LOG_DEBUG("[LWMAC-rx] No WR found, stop RX\n");
                netif->mac.rx.rx_bad_exten_count++;
                netif->mac.rx.state = GNRC_LWMAC_RX_STATE_FAILED;
                reschedule = true;
                break;
            }

            gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
            /* Found WR packet (preamble), goto next state to send WA (preamble-ACK) */
            netif->mac.rx.state = GNRC_LWMAC_RX_STATE_SEND_WA;
            reschedule = true;
            break;
        }
        case GNRC_LWMAC_RX_STATE_SEND_WA: {
            LOG_DEBUG("[LWMAC-rx] GNRC_LWMAC_RX_STATE_SEND_WA\n");

            if (!_send_wa(netif)) {
                netif->mac.rx.state = GNRC_LWMAC_RX_STATE_FAILED;
                reschedule = true;
                break;
            }

            netif->mac.rx.state = GNRC_LWMAC_RX_STATE_WAIT_WA_SENT;
            reschedule = false;
            break;
        }
        case GNRC_LWMAC_RX_STATE_WAIT_WA_SENT: {
            LOG_DEBUG("[LWMAC-rx] GNRC_LWMAC_RX_STATE_WAIT_WA_SENT\n");

            if (gnrc_netif_get_tx_feedback(netif) == TX_FEEDBACK_UNDEF) {
                LOG_DEBUG("[LWMAC-rx] WA not yet completely sent\n");
                break;
            }

            /* When reach here, WA has been sent, set timeout for expected data arrival */
            gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA, CONFIG_GNRC_LWMAC_DATA_DELAY_US);

            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_IDLE);
            netif->mac.rx.state = GNRC_LWMAC_RX_STATE_WAIT_FOR_DATA;
            reschedule = false;
            break;
        }
        case GNRC_LWMAC_RX_STATE_WAIT_FOR_DATA: {
            LOG_DEBUG("[LWMAC-rx] GNRC_LWMAC_RX_STATE_WAIT_FOR_DATA\n");

            uint8_t rx_info = _packet_process_in_wait_for_data(netif);

            /* If WA got lost we wait for data but we will be hammered with WR
             * packets. So a WR indicates a lost WA => reset RX state machine. */
            if (rx_info & GNRC_LWMAC_RX_FOUND_WR) {
                LOG_INFO("[LWMAC-rx] WA probably got lost, reset RX state machine\n");
                /* Start over again */
                netif->mac.rx.state = GNRC_LWMAC_RX_STATE_INIT;
                reschedule = true;
                break;
            }

            /* Only timeout if no packet (presumably the expected data) is being
             * received. This won't be blocked by WRs as they restart the state
             * machine (see above).
             */
            if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_DATA)) {
                if (!gnrc_netif_get_rx_started(netif)) {
                    LOG_INFO("[LWMAC-rx] DATA timed out\n");
                    netif->mac.rx.rx_bad_exten_count++;
                    netif->mac.rx.state = GNRC_LWMAC_RX_STATE_FAILED;
                    reschedule = true;
                }
                else {
                    /* If radio is receiving packet, reset wait data timeout */
                    gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_DATA,
                                           CONFIG_GNRC_LWMAC_DATA_DELAY_US);
                }
                break;
            }

            if (!(rx_info & GNRC_LWMAC_RX_FOUND_DATA)) {
                LOG_DEBUG("[LWMAC-rx] No DATA yet\n");
                break;
            }

            netif->mac.rx.state = GNRC_LWMAC_RX_STATE_SUCCESSFUL;
            reschedule = true;
            break;
        }
        case GNRC_LWMAC_RX_STATE_SUCCESSFUL:
        case GNRC_LWMAC_RX_STATE_FAILED: {
            break;
        }
        case GNRC_LWMAC_RX_STATE_STOPPED: {
            LOG_DEBUG("[LWMAC-rx] Reception state machine is stopped\n");
        }
    }
    return reschedule;
}

void gnrc_lwmac_rx_update(gnrc_netif_t *netif)
{
    /* Update until no rescheduling needed */
    while (_lwmac_rx_update(netif)) {}
}
