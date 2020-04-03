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
 * @brief       Implementation of TX state machine of LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include "periph/rtt.h"
#include "net/gnrc.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "random.h"
#include "net/gnrc/mac/internal.h"
#include "net/gnrc/lwmac/timeout.h"
#include "include/tx_state_machine.h"
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
 * @brief   Flag to track if send packet success
 */
#define GNRC_LWMAC_TX_SUCCESS         (0x01U)

/**
 * @brief   Flag to track if send packet fail
 */
#define GNRC_LWMAC_TX_FAIL            (0x02U)

static uint8_t _send_bcast(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    uint8_t tx_info = 0;
    gnrc_pktsnip_t *pkt = netif->mac.tx.packet;
    bool first = false;

    if (gnrc_lwmac_timeout_is_running(netif, GNRC_LWMAC_TIMEOUT_BROADCAST_END)) {
        if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_BROADCAST_END)) {
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_NEXT_BROADCAST);
            gnrc_pktbuf_release(pkt);
            netif->mac.tx.packet = NULL;
            tx_info |= GNRC_LWMAC_TX_SUCCESS;
            return tx_info;
        }
    }
    else {
        LOG_INFO("[LWMAC-tx] Initialize broadcasting\n");
        gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_BROADCAST_END,
                               GNRC_LWMAC_BROADCAST_DURATION_US);

        gnrc_pktsnip_t *pkt_payload;

        /* Prepare packet with LWMAC header*/
        gnrc_lwmac_frame_broadcast_t hdr;
        hdr.header.type = GNRC_LWMAC_FRAMETYPE_BROADCAST;
        hdr.seq_nr = netif->mac.tx.bcast_seqnr++;

        pkt_payload = pkt->next;
        pkt->next = gnrc_pktbuf_add(pkt->next, &hdr, sizeof(hdr), GNRC_NETTYPE_LWMAC);
        if (pkt->next == NULL) {
            LOG_ERROR("ERROR: [LWMAC-tx] Cannot allocate pktbuf of type FRAMETYPE_BROADCAST\n");
            netif->mac.tx.packet->next = pkt_payload;
            /* Drop the broadcast packet */
            LOG_ERROR("ERROR: [LWMAC-tx] Memory maybe full, drop the broadcast packet\n");
            gnrc_pktbuf_release(netif->mac.tx.packet);
            /* clear packet point to avoid TX retry */
            netif->mac.tx.packet = NULL;
            tx_info |= GNRC_LWMAC_TX_FAIL;
            return tx_info;
        }

        /* No Auto-ACK for broadcast packets */
        netopt_enable_t autoack = NETOPT_DISABLE;
        netif->dev->driver->set(netif->dev, NETOPT_AUTOACK, &autoack,
                                sizeof(autoack));
        first = true;
    }

    if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_NEXT_BROADCAST) ||
        first) {
        /* if found ongoing transmission, quit this cycle for collision avoidance.
        * Broadcast packet will be re-queued and try to send in the next cycle. */
        if (_gnrc_lwmac_get_netdev_state(netif) == NETOPT_STATE_RX) {
            /* save pointer to netif header */
            gnrc_pktsnip_t *netif_snip = pkt->next->next;

            /* remove LWMAC header */
            pkt->next->next = NULL;
            gnrc_pktbuf_release(pkt->next);

            /* make append netif header after payload again */
            pkt->next = netif_snip;

            if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
                gnrc_pktbuf_release(netif->mac.tx.packet);
                LOG_WARNING("WARNING: [LWMAC-tx] TX queue full, drop packet\n");
            }
            /* drop pointer so it won't be free'd */
            netif->mac.tx.packet = NULL;
            tx_info |= GNRC_LWMAC_TX_FAIL;
            return tx_info;
        }

        /* Don't let the packet be released yet, we want to send it again */
        gnrc_pktbuf_hold(pkt, 1);

        int res = _gnrc_lwmac_transmit(netif, pkt);
        if (res < 0) {
            LOG_ERROR("ERROR: [LWMAC-tx] Send broadcast pkt failed.");
            tx_info |= GNRC_LWMAC_TX_FAIL;
            return tx_info;
        }

        gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_NEXT_BROADCAST,
                               GNRC_LWMAC_TIME_BETWEEN_BROADCAST_US);
        LOG_INFO("[LWMAC-tx] Broadcast sent\n");
    }

    return tx_info;
}

static uint8_t _send_wr(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    uint8_t tx_info = 0;
    gnrc_pktsnip_t *pkt;
    gnrc_pktsnip_t *pkt_lwmac;
    gnrc_netif_hdr_t *nethdr;

    /* if found ongoing transmission, quit this cycle for collision avoidance.
     * Data packet will be re-queued and try to send in the next cycle. */
    if (_gnrc_lwmac_get_netdev_state(netif) == NETOPT_STATE_RX) {
        if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
            gnrc_pktbuf_release(netif->mac.tx.packet);
            LOG_WARNING("WARNING: [LWMAC-tx] TX queue full, drop packet\n");
        }
        /* drop pointer so it won't be free'd */
        netif->mac.tx.packet = NULL;
        tx_info |= GNRC_LWMAC_TX_FAIL;
        return tx_info;
    }

    /* Assemble WR */
    gnrc_lwmac_frame_wr_t wr_hdr;
    wr_hdr.header.type = GNRC_LWMAC_FRAMETYPE_WR;
    memcpy(&(wr_hdr.dst_addr.addr), netif->mac.tx.current_neighbor->l2_addr,
           netif->mac.tx.current_neighbor->l2_addr_len);
    wr_hdr.dst_addr.len = netif->mac.tx.current_neighbor->l2_addr_len;

    pkt = gnrc_pktbuf_add(NULL, &wr_hdr, sizeof(wr_hdr), GNRC_NETTYPE_LWMAC);
    if (pkt == NULL) {
        LOG_ERROR("ERROR: [LWMAC-tx] Cannot allocate pktbuf of type GNRC_NETTYPE_LWMAC\n");
        gnrc_pktbuf_release(netif->mac.tx.packet);
        LOG_ERROR("ERROR: [LWMAC-tx] Memory maybe full, drop the data packet\n");
        /* clear packet point to avoid TX retry */
        netif->mac.tx.packet = NULL;
        tx_info |= GNRC_LWMAC_TX_FAIL;
        return tx_info;
    }

    /* track the location of this lwmac_frame_wr_t header */
    pkt_lwmac = pkt;

    pkt = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_netif_hdr_t), GNRC_NETTYPE_NETIF);
    if (pkt == NULL) {
        LOG_ERROR("ERROR: [LWMAC-tx] Cannot allocate pktbuf of type GNRC_NETTYPE_NETIF\n");
        gnrc_pktbuf_release(pkt_lwmac);
        LOG_ERROR("ERROR: [LWMAC-tx] Memory maybe full, drop the data packet\n");
        gnrc_pktbuf_release(netif->mac.tx.packet);
        /* clear packet point to avoid TX retry */
        netif->mac.tx.packet = NULL;
        tx_info |= GNRC_LWMAC_TX_FAIL;
        return tx_info;
    }

    /* We wouldn't get here if adding the NETIF header had failed, so no
     * sanity checks needed */
    nethdr = (gnrc_netif_hdr_t *) (gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF))->data;

    /* Construct NETIF header and insert address for WR packet */
    gnrc_netif_hdr_init(nethdr, 0, 0);

    /* Send WR as broadcast*/
    nethdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;

    /* Disable Auto ACK */
    netopt_enable_t autoack = NETOPT_DISABLE;
    netif->dev->driver->set(netif->dev, NETOPT_AUTOACK, &autoack,
                            sizeof(autoack));

    /* Prepare WR, this will discard any frame in the transceiver that has
    * possibly arrived in the meantime but we don't care at this point. */
    int res = _gnrc_lwmac_transmit(netif, pkt);
    if (res < 0) {
        LOG_ERROR("ERROR: [LWMAC-tx] Send WR failed.");
        gnrc_pktbuf_release(pkt);
        tx_info |= GNRC_LWMAC_TX_FAIL;
        return tx_info;
    }

    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);
    return tx_info;
}

static uint8_t _packet_process_in_wait_for_wa(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    uint8_t tx_info = 0;
    gnrc_pktsnip_t *pkt;
    bool found_wa = false;
    bool postponed = false;
    bool from_expected_destination = false;

    while ((pkt = gnrc_priority_pktqueue_pop(&netif->mac.rx.queue)) != NULL) {
        LOG_DEBUG("[LWMAC-tx] Inspecting pkt @ %p\n", pkt);

        /* Parse packet */
        gnrc_lwmac_packet_info_t info;
        int ret = _gnrc_lwmac_parse_packet(pkt, &info);

        if (ret != 0) {
            LOG_DEBUG("[LWMAC-tx] Packet could not be parsed: %i\n", ret);
            gnrc_pktbuf_release(pkt);
            continue;
        }

        if (memcmp(&info.src_addr.addr, &netif->mac.tx.current_neighbor->l2_addr,
                   netif->mac.tx.current_neighbor->l2_addr_len) == 0) {
            from_expected_destination = true;
        }

        if (info.header->type == GNRC_LWMAC_FRAMETYPE_BROADCAST) {
            _gnrc_lwmac_dispatch_defer(netif->mac.rx.dispatch_buffer, pkt);
            gnrc_mac_dispatch(&netif->mac.rx);
            /* Drop pointer to it can't get released */
            pkt = NULL;
            continue;
        }

        /* Check if destination is talking to another node. It will sleep
         * after a finished transaction so there's no point in trying any
         * further now. */
        if (!(memcmp(&info.dst_addr.addr, &netif->l2addr,
                     netif->l2addr_len) == 0) && from_expected_destination) {
            if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
                gnrc_pktbuf_release(netif->mac.tx.packet);
                LOG_WARNING("WARNING: [LWMAC-tx] TX queue full, drop packet\n");
            }
            /* drop pointer so it won't be free'd */
            netif->mac.tx.packet = NULL;
            postponed = true;
            gnrc_pktbuf_release(pkt);
            break;
        }

        /* if found anther node is also trying to send data,
         * quit this cycle for collision avoidance. */
        if (info.header->type == GNRC_LWMAC_FRAMETYPE_WR) {
            if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
                gnrc_pktbuf_release(netif->mac.tx.packet);
                LOG_WARNING("WARNING: [LWMAC-tx] TX queue full, drop packet\n");
            }
            /* drop pointer so it won't be free'd */
            netif->mac.tx.packet = NULL;
            postponed = true;
            gnrc_pktbuf_release(pkt);
            break;
        }

        if (info.header->type != GNRC_LWMAC_FRAMETYPE_WA) {
            LOG_DEBUG("[LWMAC-tx] Packet is not WA: 0x%02x\n", info.header->type);
            gnrc_pktbuf_release(pkt);
            continue;
        }

        if (from_expected_destination) {
            /* calculate the phase of the receiver based on WA */
            netif->mac.tx.timestamp = _gnrc_lwmac_phase_now();
            gnrc_lwmac_frame_wa_t *wa_hdr;
            wa_hdr = (gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_LWMAC))->data;

            if (netif->mac.tx.timestamp >= wa_hdr->current_phase) {
                netif->mac.tx.timestamp = netif->mac.tx.timestamp -
                                          wa_hdr->current_phase;
            }
            else {
                netif->mac.tx.timestamp += RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US);
                netif->mac.tx.timestamp -= wa_hdr->current_phase;
            }

            uint32_t own_phase;
            own_phase = _gnrc_lwmac_ticks_to_phase(netif->mac.prot.lwmac.last_wakeup);

            if (own_phase >= netif->mac.tx.timestamp) {
                own_phase = own_phase - netif->mac.tx.timestamp;
            }
            else {
                own_phase = netif->mac.tx.timestamp - own_phase;
            }

            if ((own_phase < RTT_US_TO_TICKS((3 * GNRC_LWMAC_WAKEUP_DURATION_US / 2))) ||
                (own_phase > RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US -
                                             (3 * GNRC_LWMAC_WAKEUP_DURATION_US / 2)))) {
                gnrc_lwmac_set_phase_backoff(netif, true);
                LOG_WARNING("WARNING: [LWMAC-tx] phase close\n");
            }
        }

        /* No need to keep pkt anymore */
        gnrc_pktbuf_release(pkt);

        if (!from_expected_destination) {
            LOG_DEBUG("[LWMAC-tx] Packet is not from expected destination\n");
            break;
        }

        /* All checks passed so this must be a valid WA */
        gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WR);

        found_wa = true;
        break;
    }

    if (postponed) {
        LOG_INFO("[LWMAC-tx] Destination is talking to another node, postpone\n");
        tx_info |= GNRC_LWMAC_TX_FAIL;
        return tx_info;
    }

    if (!found_wa) {
        LOG_DEBUG("[LWMAC-tx] No WA yet\n");
        return tx_info;
    }

    /* Save newly calculated phase for destination */
    netif->mac.tx.current_neighbor->phase = netif->mac.tx.timestamp;
    LOG_INFO("[LWMAC-tx] New phase: %" PRIu32 "\n", netif->mac.tx.timestamp);

    /* We've got our WA, so discard the rest, TODO: no flushing */
    gnrc_priority_pktqueue_flush(&netif->mac.rx.queue);

    tx_info |= GNRC_LWMAC_TX_SUCCESS;
    return tx_info;
}

/* return false if send data failed, otherwise return true */
static bool _send_data(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    gnrc_pktsnip_t *pkt = netif->mac.tx.packet;
    gnrc_pktsnip_t *pkt_payload;

    assert(pkt != NULL);
    /* Enable Auto ACK again */
    netopt_enable_t autoack = NETOPT_ENABLE;
    netif->dev->driver->set(netif->dev, NETOPT_AUTOACK,
                            &autoack, sizeof(autoack));

    /* It's okay to retry sending DATA. Timing doesn't matter anymore and
     * destination is waiting for a certain amount of time. */
    uint8_t csma_retries = GNRC_LWMAC_DATA_CSMA_RETRIES;
    netif->dev->driver->set(netif->dev, NETOPT_CSMA_RETRIES,
                            &csma_retries, sizeof(csma_retries));

    netif->mac.mac_info |= GNRC_NETIF_MAC_INFO_CSMA_ENABLED;
    netopt_enable_t csma_enable = NETOPT_ENABLE;
    netif->dev->driver->set(netif->dev, NETOPT_CSMA,
                            &csma_enable, sizeof(csma_enable));

    pkt_payload = pkt->next;

    /* Insert LWMAC header above NETIF header. The burst (consecutive) transmission
     * scheme works here (sender side). If the sender finds it has pending packets
     * for the receiver (and under burst limit), it sets the packet type to
     * FRAMETYPE_DATA_PENDING, to notice the receiver for next incoming packet.
     * In case the sender has no more packet for the receiver, it simply sets the
     * data type to FRAMETYPE_DATA. */
    gnrc_lwmac_hdr_t hdr;
    if ((gnrc_priority_pktqueue_length(&netif->mac.tx.current_neighbor->queue) > 0) &&
        (netif->mac.tx.tx_burst_count < GNRC_LWMAC_MAX_TX_BURST_PKT_NUM)) {
        hdr.type = GNRC_LWMAC_FRAMETYPE_DATA_PENDING;
        gnrc_lwmac_set_tx_continue(netif, true);
        netif->mac.tx.tx_burst_count++;
    }
    else {
        hdr.type = GNRC_LWMAC_FRAMETYPE_DATA;
        gnrc_lwmac_set_tx_continue(netif, false);
    }

    pkt->next = gnrc_pktbuf_add(pkt->next, &hdr, sizeof(hdr), GNRC_NETTYPE_LWMAC);
    if (pkt->next == NULL) {
        LOG_ERROR("ERROR: [LWMAC-tx] Cannot allocate pktbuf of type GNRC_NETTYPE_LWMAC\n");
        LOG_ERROR("ERROR: [LWMAC-tx] Memory maybe full, drop the data packet\n");
        netif->mac.tx.packet->next = pkt_payload;
        gnrc_pktbuf_release(netif->mac.tx.packet);
        /* clear packet point to avoid TX retry */
        netif->mac.tx.packet = NULL;
        return false;
    }

    /* if found ongoing transmission, quit this cycle for collision avoidance.
     * Data packet will be re-queued and try to send in the next cycle. */
    if (_gnrc_lwmac_get_netdev_state(netif) == NETOPT_STATE_RX) {
        /* save pointer to netif header */
        gnrc_pktsnip_t *netif_snip = pkt->next->next;

        /* remove LWMAC header */
        pkt->next->next = NULL;
        gnrc_pktbuf_release(pkt->next);

        /* make append netif header after payload again */
        pkt->next = netif_snip;

        if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
            gnrc_pktbuf_release(netif->mac.tx.packet);
            LOG_WARNING("WARNING: [LWMAC-tx] TX queue full, drop packet\n");
        }
        /* drop pointer so it won't be free'd */
        netif->mac.tx.packet = NULL;
        return false;
    }

    /* Send data */
    int res = _gnrc_lwmac_transmit(netif, pkt);
    if (res < 0) {
        LOG_ERROR("ERROR: [LWMAC-tx] Send data failed.");
        gnrc_pktbuf_release(pkt);
        /* clear packet point to avoid TX retry */
        netif->mac.tx.packet = NULL;
        return false;
    }

    /* Packet has been released by netdev, so drop pointer */
    netif->mac.tx.packet = NULL;

    DEBUG("[LWMAC-tx]: spent %lu WR in TX\n",
          (unsigned long)netif->mac.tx.wr_sent);

#if (LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
    netif->mac.prot.lwmac.pkt_start_sending_time_ticks =
        rtt_get_counter() - netif->mac.prot.lwmac.pkt_start_sending_time_ticks;
    DEBUG("[LWMAC-tx]: pkt sending delay in TX: %lu us\n",
          RTT_TICKS_TO_US(netif->mac.prot.lwmac.pkt_start_sending_time_ticks));
#endif

    return true;
}

void gnrc_lwmac_tx_start(gnrc_netif_t *netif,
                         gnrc_pktsnip_t *pkt,
                         gnrc_mac_tx_neighbor_t *neighbor)
{
    assert(netif != NULL);
    assert(pkt != NULL);
    assert(neighbor != NULL);

    if (netif->mac.tx.packet) {
        LOG_WARNING("WARNING: [LWMAC-tx] Starting but tx.packet is still set\n");
        gnrc_pktbuf_release(netif->mac.tx.packet);
    }

    netif->mac.tx.packet = pkt;
    netif->mac.tx.current_neighbor = neighbor;
    netif->mac.tx.state = GNRC_LWMAC_TX_STATE_INIT;
    netif->mac.tx.wr_sent = 0;

#if (LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
    netif->mac.prot.lwmac.pkt_start_sending_time_ticks = rtt_get_counter();
#endif
}

void gnrc_lwmac_tx_stop(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WR);
    gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_NO_RESPONSE);
    gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_NEXT_BROADCAST);
    gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_BROADCAST_END);
    netif->mac.tx.state = GNRC_LWMAC_TX_STATE_STOPPED;

    /* Release packet in case of failure */
    if (netif->mac.tx.packet) {
        if (netif->mac.tx.tx_retry_count >= GNRC_LWMAC_MAX_DATA_TX_RETRIES) {
            netif->mac.tx.tx_retry_count = 0;
            gnrc_pktbuf_release(netif->mac.tx.packet);
            netif->mac.tx.packet = NULL;
            LOG_WARNING("WARNING: [LWMAC-tx] Drop TX packet\n");
        }
        else {
            netif->mac.tx.tx_retry_count++;
            return;
        }
    }

    if (!gnrc_lwmac_get_tx_continue(netif)) {
        netif->mac.tx.current_neighbor = NULL;
    }
}

/* Returns whether rescheduling is needed or not */
static bool _lwmac_tx_update(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    bool reschedule = false;

    switch (netif->mac.tx.state) {
        case GNRC_LWMAC_TX_STATE_INIT: {
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WR);
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_NO_RESPONSE);
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_NEXT_BROADCAST);
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_BROADCAST_END);

            /* if found ongoing transmission,
             * quit this cycle for collision avoidance. */
            if (_gnrc_lwmac_get_netdev_state(netif) == NETOPT_STATE_RX) {
                if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
                    gnrc_pktbuf_release(netif->mac.tx.packet);
                    LOG_WARNING("WARNING: [LWMAC-tx] TX queue full, drop packet\n");
                }
                /* drop pointer so it won't be free'd */
                netif->mac.tx.packet = NULL;
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            /* check if the packet is for broadcast */
            if (gnrc_netif_hdr_get_flag(netif->mac.tx.packet) &
                (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
                /* Set CSMA retries as configured and enable */
                uint8_t csma_retries = GNRC_LWMAC_BROADCAST_CSMA_RETRIES;
                netif->dev->driver->set(netif->dev, NETOPT_CSMA_RETRIES,
                                        &csma_retries, sizeof(csma_retries));
                netif->mac.mac_info |= GNRC_NETIF_MAC_INFO_CSMA_ENABLED;
                netopt_enable_t csma_enable = NETOPT_ENABLE;
                netif->dev->driver->set(netif->dev, NETOPT_CSMA,
                                        &csma_enable, sizeof(csma_enable));

                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_SEND_BROADCAST;
                reschedule = true;
                break;
            }
            else {
                /* Use CSMA for the first WR */
                netif->mac.mac_info |= GNRC_NETIF_MAC_INFO_CSMA_ENABLED;
                netopt_enable_t csma_disable = NETOPT_ENABLE;
                netif->dev->driver->set(netif->dev, NETOPT_CSMA,
                                        &csma_disable, sizeof(csma_disable));
                /* Set a timeout for the maximum transmission procedure */
                gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_NO_RESPONSE, GNRC_LWMAC_PREAMBLE_DURATION_US);

                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_SEND_WR;
                reschedule = true;
                break;
            }
        }
        case GNRC_LWMAC_TX_STATE_SEND_BROADCAST: {
            uint8_t tx_info = _send_bcast(netif);

            if (tx_info & GNRC_LWMAC_TX_SUCCESS) {
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_SUCCESSFUL;
                reschedule = true;
                break;
            }

            if (tx_info & GNRC_LWMAC_TX_FAIL) {
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            break;
        }
        case GNRC_LWMAC_TX_STATE_SEND_WR: {
            /* In case of no Tx-isr error (e.g., no Tx-isr), goto TX failure. */
            if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_NO_RESPONSE)) {
                LOG_WARNING("WARNING: [LWMAC-tx] No response from destination, "
                            "probably no TX-ISR\n");
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }
            LOG_DEBUG("[LWMAC-tx] GNRC_LWMAC_TX_STATE_SEND_WR\n");
            uint8_t tx_info = _send_wr(netif);

            if (tx_info & GNRC_LWMAC_TX_FAIL) {
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            netif->mac.tx.state = GNRC_LWMAC_TX_STATE_WAIT_WR_SENT;
            reschedule = false;
            break;
        }
        case GNRC_LWMAC_TX_STATE_WAIT_WR_SENT: {
            LOG_DEBUG("[LWMAC-tx] GNRC_LWMAC_TX_STATE_WAIT_WR_SENT\n");

            /* In case of no Tx-isr error (e.g., no Tx-isr), goto TX failure. */
            if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_NO_RESPONSE)) {
                LOG_WARNING("WARNING: [LWMAC-tx] No response from destination\n");
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            if (gnrc_netif_get_tx_feedback(netif) == TX_FEEDBACK_UNDEF) {
                LOG_DEBUG("[LWMAC-tx] WR not yet completely sent\n");
                break;
            }

            /* If found ongoing transmission, goto TX failure, i.e., postpone transmission to
             * next cycle. This is mainly for collision avoidance. */
            if (gnrc_netif_get_tx_feedback(netif) == TX_FEEDBACK_BUSY) {
                if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
                    gnrc_pktbuf_release(netif->mac.tx.packet);
                    LOG_WARNING("WARNING: [LWMAC-tx] TX queue full, drop packet\n");
                }
                /* clear packet point to avoid TX retry */
                netif->mac.tx.packet = NULL;
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            if (netif->mac.tx.wr_sent == 0) {
                /* Only the first WR use CSMA */
                netif->mac.mac_info &= ~GNRC_NETIF_MAC_INFO_CSMA_ENABLED;
                netopt_enable_t csma_disable = NETOPT_DISABLE;
                netif->dev->driver->set(netif->dev, NETOPT_CSMA,
                                        &csma_disable, sizeof(csma_disable));
            }

            netif->mac.tx.wr_sent++;

            /* Set timeout for next WR in case no WA will be received */
            gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_WR, GNRC_LWMAC_TIME_BETWEEN_WR_US);

            /* Debug WR timing */
            LOG_DEBUG("[LWMAC-tx] Destination phase was: %" PRIu32 "\n",
                      netif->mac.tx.current_neighbor->phase);
            LOG_DEBUG("[LWMAC-tx] Phase when sent was:   %" PRIu32 "\n",
                      _gnrc_lwmac_ticks_to_phase(netif->mac.tx.timestamp));
            LOG_DEBUG("[LWMAC-tx] Ticks when sent was:   %" PRIu32 "\n",
                      netif->mac.tx.timestamp);
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_IDLE);
            netif->mac.tx.state = GNRC_LWMAC_TX_STATE_WAIT_FOR_WA;
            reschedule = false;
            break;
        }
        case GNRC_LWMAC_TX_STATE_WAIT_FOR_WA: {
            LOG_DEBUG("[LWMAC-tx] GNRC_LWMAC_TX_STATE_WAIT_FOR_WA\n");

            if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_NO_RESPONSE)) {
                LOG_WARNING("WARNING: [LWMAC-tx] No response from destination\n");
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_WR)) {
                /* In case the sender is in consecutive (burst) transmission to the receiver,
                 * meaning that the sender has already successfully sent at least one data to
                 * the receiver, then the sender will only spend one WR for triggering the next
                 * transmission procedure. And, if this WR doesn't work (no WA replied), the
                 * sender regards consecutive transmission failed.
                 */
                if (gnrc_lwmac_get_tx_continue(netif)) {
                    LOG_DEBUG("[LWMAC-tx] Tx burst fail\n");
                    if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, netif->mac.tx.packet)) {
                        gnrc_pktbuf_release(netif->mac.tx.packet);
                        LOG_WARNING("WARNING: [LWMAC-tx] TX queue full, drop packet\n");
                    }
                    /* drop pointer so it won't be free'd */
                    netif->mac.tx.packet = NULL;

                    netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                    reschedule = true;
                    break;
                }
                else {
                    /* If this is the first transmission to the receiver for locating the
                     * latter's wake-up period, the sender just keep sending WRs until it
                     * finds the WA.
                     */
                    netif->mac.tx.state = GNRC_LWMAC_TX_STATE_SEND_WR;
                    reschedule = true;
                    break;
                }
            }

            if (_gnrc_lwmac_get_netdev_state(netif) == NETOPT_STATE_RX) {
                /* Wait for completion of frame reception */
                break;
            }

            uint8_t tx_info = _packet_process_in_wait_for_wa(netif);

            if (tx_info & GNRC_LWMAC_TX_FAIL) {
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            if (tx_info & GNRC_LWMAC_TX_SUCCESS) {
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_SEND_DATA;
                reschedule = true;
                break;
            }
            else {
                /* No WA yet */
                break;
            }
        }
        case GNRC_LWMAC_TX_STATE_SEND_DATA: {
            LOG_DEBUG("[LWMAC-tx] GNRC_LWMAC_TX_STATE_SEND_DATA\n");

            if (!_send_data(netif)) {
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            netif->mac.tx.state = GNRC_LWMAC_TX_STATE_WAIT_FEEDBACK;
            reschedule = false;
            break;
        }
        case GNRC_LWMAC_TX_STATE_WAIT_FEEDBACK: {
            /* In case of no Tx-isr error, goto TX failure. */
            if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_NO_RESPONSE)) {
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            LOG_DEBUG("[LWMAC-tx] GNRC_LWMAC_TX_STATE_WAIT_FEEDBACK\n");
            if (gnrc_netif_get_tx_feedback(netif) == TX_FEEDBACK_UNDEF) {
                break;
            }
            else if (gnrc_netif_get_tx_feedback(netif) == TX_FEEDBACK_SUCCESS) {
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_SUCCESSFUL;
                reschedule = true;
                break;
            }
            else if (gnrc_netif_get_tx_feedback(netif) == TX_FEEDBACK_NOACK) {
                LOG_ERROR("ERROR: [LWMAC-tx] Not ACKED\n");
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }
            else if (gnrc_netif_get_tx_feedback(netif) == TX_FEEDBACK_BUSY) {
                LOG_ERROR("ERROR: [LWMAC-tx] Channel busy \n");
                netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
                reschedule = true;
                break;
            }

            LOG_ERROR("ERROR: [LWMAC-tx] Tx feedback unhandled: %i\n",
                      gnrc_netif_get_tx_feedback(netif));
            netif->mac.tx.state = GNRC_LWMAC_TX_STATE_FAILED;
            reschedule = true;
            break;
        }
        case GNRC_LWMAC_TX_STATE_SUCCESSFUL:
        case GNRC_LWMAC_TX_STATE_FAILED: {
            break;
        }
        case GNRC_LWMAC_TX_STATE_STOPPED: {
            LOG_DEBUG("[LWMAC-tx] Transmission state machine is stopped\n");
        }
    }

    return reschedule;
}

void gnrc_lwmac_tx_update(gnrc_netif_t *netif)
{
    /* Update until no rescheduling needed */
    while (_lwmac_tx_update(netif)) {}
}
