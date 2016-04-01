/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_lwmac
 * @file
 * @brief       Implementation of TX state machine
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @}
 */

#include <periph/rtt.h>
#include <net/gnrc.h>
#include <net/gnrc/lwmac/lwmac.h>
#include <net/gnrc/lwmac/packet_queue.h>

#include "include/tx_state_machine.h"
#include "include/timeout.h"
#include "include/lwmac_internal.h"
#include "include/lwmac_types.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define LOG_LEVEL LOG_WARNING
#include "log.h"

#undef LOG_ERROR
#undef LOG_WARNING
#undef LOG_INFO
#undef LOG_DEBUG

#define LOG_ERROR(...) LOG(LOG_ERROR, "ERROR: [lwmac-tx] " __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_WARNING, "WARNING: [lwmac-tx] " __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_INFO, "[lwmac-tx] " __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_DEBUG, "[lwmac-tx] " __VA_ARGS__)

/* Break out of switch and mark the need for rescheduling */
#define GOTO_TX_STATE(tx_state, do_resched) lwmac->tx.state = tx_state; \
                                reschedule = do_resched; \
                                break

/******************************************************************************/

void lwmac_tx_start(lwmac_t* lwmac, gnrc_pktsnip_t* pkt, lwmac_tx_neighbour_t* neighbour)
{
    assert(lwmac != NULL);
    assert(pkt != NULL);
    assert(neighbour != NULL);

    if(lwmac->tx.packet) {
        LOG_WARNING("Starting but tx.packet is still set\n");
        gnrc_pktbuf_release(lwmac->tx.packet);
    }

    lwmac->tx.packet = pkt;
    lwmac->tx.current_neighbour = neighbour;
    lwmac->tx.state = TX_STATE_INIT;
    lwmac->tx.wr_sent = 0;
}

/******************************************************************************/

void lwmac_tx_stop(lwmac_t* lwmac)
{
    if(!lwmac)
        return;

    lwmac_clear_timeout(lwmac, TIMEOUT_WR);
    lwmac_clear_timeout(lwmac, TIMEOUT_NO_RESPONSE);
    lwmac_clear_timeout(lwmac, TIMEOUT_NEXT_BROADCAST);
    lwmac_clear_timeout(lwmac, TIMEOUT_BROADCAST_END);
    lwmac->tx.state = TX_STATE_STOPPED;

    /* Release packet in case of failure */
    if(lwmac->tx.packet) {
        gnrc_pktbuf_release(lwmac->tx.packet);
        lwmac->tx.packet = NULL;
    }
    lwmac->tx.current_neighbour = NULL;
}

/******************************************************************************/

/* Returns whether rescheduling is needed or not */
static bool _lwmac_tx_update(lwmac_t* lwmac)
{
    bool reschedule = false;

    if(!lwmac)
        return reschedule;

    switch(lwmac->tx.state)
    {
    case TX_STATE_INIT:
    {
        lwmac_clear_timeout(lwmac, TIMEOUT_WR);
        lwmac_clear_timeout(lwmac, TIMEOUT_NO_RESPONSE);
        lwmac_clear_timeout(lwmac, TIMEOUT_NEXT_BROADCAST);
        lwmac_clear_timeout(lwmac, TIMEOUT_BROADCAST_END);

        if(_packet_is_broadcast(lwmac->tx.packet)) {
            /* Set CSMA retries as configured and enable */
            uint8_t csma_retries = LWMAC_BROADCAST_CSMA_RETRIES;
			lwmac->netdev2_driver->set(lwmac->netdev->dev, NETOPT_CSMA_RETRIES,
                                        &csma_retries, sizeof(csma_retries));
            netopt_enable_t csma_enable = NETOPT_ENABLE;
			lwmac->netdev2_driver->set(lwmac->netdev->dev, NETOPT_CSMA, &csma_enable, sizeof(csma_enable));

            GOTO_TX_STATE(TX_STATE_SEND_BROADCAST, true);
        } else {
            /* Don't attempt to send a WR if channel is busy to get timings
             * right, will be changed for sending DATA packet */
            netopt_enable_t csma_disable = NETOPT_DISABLE;
			lwmac->netdev2_driver->set(lwmac->netdev->dev, NETOPT_CSMA, &csma_disable, sizeof(csma_disable));

            GOTO_TX_STATE(TX_STATE_SEND_WR, true);
        }
    }
    case TX_STATE_SEND_BROADCAST:
    {
        gnrc_pktsnip_t* pkt = lwmac->tx.packet;
        bool first = false;

        if(lwmac_timeout_is_running(lwmac, TIMEOUT_BROADCAST_END)) {
            if(lwmac_timeout_is_expired(lwmac, TIMEOUT_BROADCAST_END)) {
                lwmac_clear_timeout(lwmac, TIMEOUT_NEXT_BROADCAST);
                gnrc_pktbuf_release(pkt);
                lwmac->tx.packet = NULL;
                GOTO_TX_STATE(TX_STATE_SUCCESSFUL, true);
            }
        } else {
            LOG_INFO("Initialize broadcasting\n");
            lwmac_set_timeout(lwmac, TIMEOUT_BROADCAST_END, LWMAC_BROADCAST_DURATION_US);

            /* Prepare packet with LwMAC header*/
            lwmac_frame_broadcast_t hdr = {};
            hdr.header.type = FRAMETYPE_BROADCAST;
            hdr.seq_nr = lwmac->tx.bcast_seqnr++;

            pkt->next = gnrc_pktbuf_add(pkt->next, &hdr, sizeof(hdr), GNRC_NETTYPE_LWMAC);

            /* No Auto-ACK for broadcast packets */
            netopt_enable_t autoack = NETOPT_DISABLE;
			lwmac->netdev2_driver->set(lwmac->netdev->dev, NETOPT_AUTOACK, &autoack, sizeof(autoack));

            first = true;
        }

        if( lwmac_timeout_is_expired(lwmac, TIMEOUT_NEXT_BROADCAST) ||
            first ) {
            /* Don't let the packet be released yet, we want to send it again */
            gnrc_pktbuf_hold(pkt, 1);

			lwmac->netdev->send(lwmac->netdev, pkt);
            _set_netdev_state(lwmac, NETOPT_STATE_TX);

            lwmac_set_timeout(lwmac, TIMEOUT_NEXT_BROADCAST, LWMAC_TIME_BETWEEN_BROADCAST_US);
            LOG_INFO("Broadcast sent\n");
        }

        break;
    }
    case TX_STATE_SEND_WR:
    {
        LOG_DEBUG("TX_STATE_SEND_WR\n");

        gnrc_pktsnip_t* pkt;
        gnrc_netif_hdr_t *nethdr;
        uint8_t* dst_addr = NULL;
        int addr_len;

        /* Get destination address */
        addr_len = _get_dest_address(lwmac->tx.packet, &dst_addr);
        if(addr_len <= 0 || addr_len > 8) {
            LOG_ERROR("Invalid address length: %i\n", addr_len);
            GOTO_TX_STATE(TX_STATE_FAILED, true);
        }

        /* Assemble WR */
        lwmac_frame_wr_t wr_hdr = {};
        wr_hdr.header.type = FRAMETYPE_WR;

        pkt = gnrc_pktbuf_add(NULL, &wr_hdr, sizeof(wr_hdr), GNRC_NETTYPE_LWMAC);
        if(pkt == NULL) {
            LOG_ERROR("Cannot allocate pktbuf of type GNRC_NETTYPE_LWMAC\n");
            GOTO_TX_STATE(TX_STATE_FAILED, true);
        }

        pkt = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_netif_hdr_t) + addr_len, GNRC_NETTYPE_NETIF);
        if(pkt == NULL) {
            LOG_ERROR("Cannot allocate pktbuf of type GNRC_NETTYPE_NETIF\n");
            GOTO_TX_STATE(TX_STATE_FAILED, true);
        }

        /* We wouldn't get here if adding the NETIF header had failed, so no
           sanity checks needed */
        nethdr = (gnrc_netif_hdr_t*) _gnrc_pktbuf_find(pkt, GNRC_NETTYPE_NETIF);

        /* Construct NETIF header and insert address for WR packet */
        gnrc_netif_hdr_init(nethdr, 0, addr_len);
        gnrc_netif_hdr_set_dst_addr(nethdr, dst_addr, addr_len);

        /* Disable Auto ACK */
        netopt_enable_t autoack = NETOPT_DISABLE;
		lwmac->netdev2_driver->set(lwmac->netdev->dev, NETOPT_AUTOACK, &autoack, sizeof(autoack));

        /* Prepare WR, this will discard any frame in the transceiver that has
         * possibly arrived in the meantime but we don't care at this point. */
		lwmac->netdev->send(lwmac->netdev, pkt);

        /* First WR, try to catch wakeup phase */
        if(lwmac->tx.wr_sent == 0) {

            /* Calculate wakeup time */
            uint32_t wait_until;
            wait_until  = _phase_to_ticks(lwmac->tx.current_neighbour->phase);
            wait_until -= RTT_US_TO_TICKS(LWMAC_WR_BEFORE_PHASE_US);

            /* This output blocks a long time and can prevent correct timing */
            LOG_DEBUG("Phase length:  %"PRIu32"\n", RTT_US_TO_TICKS(LWMAC_WAKEUP_INTERVAL_US));
            LOG_DEBUG("Wait until:    %"PRIu32"\n", wait_until);
            LOG_DEBUG("     phase:    %"PRIu32"\n", _ticks_to_phase(wait_until));
            LOG_DEBUG("Ticks to wait: %"PRIu32"\n", (long int)wait_until - rtt_get_counter());

            /* Wait until calculated wakeup time of destination */
            while(rtt_get_counter() < wait_until);
        }

        /* Save timestamp in case this WR reaches the destination node so that
         * we know it's wakeup phase relative to ours for the next time. This
         * is not exactly the time the WR was completely sent, but the timing
         * we can reproduce here. */
        lwmac->tx.timestamp = rtt_get_counter();

        /* Trigger sending frame */
        _set_netdev_state(lwmac, NETOPT_STATE_TX);

        /* Flush RX queue, TODO: maybe find a way without loosing RX packets */
        packet_queue_flush(&lwmac->rx.queue);

        GOTO_TX_STATE(TX_STATE_WAIT_WR_SENT, false);
    }
    case TX_STATE_WAIT_WR_SENT:
    {
        LOG_DEBUG("TX_STATE_WAIT_WR_SENT\n");

        if(lwmac->tx_feedback == TX_FEEDBACK_UNDEF) {
            LOG_DEBUG("WR not yet completely sent\n");
            break;
        }

        if(lwmac->tx.wr_sent == 0) {
            lwmac_set_timeout(lwmac, TIMEOUT_NO_RESPONSE, LWMAC_WAKEUP_INTERVAL_US);
        }

        lwmac->tx.wr_sent++;

        /* This is not needed anymore, because WRs are sent without CSMA/CA */
//        if(lwmac->tx_feedback == TX_FEEDBACK_BUSY) {
//            LOG_DEBUG("WR could not be sent, retry\n");
//            GOTO_TX_STATE(TX_STATE_SEND_WR, true);
//        }

        /* Set timeout for next WR in case no WA will be received */
        lwmac_set_timeout(lwmac, TIMEOUT_WR, LWMAC_TIME_BETWEEN_WR_US);

        /* Debug WR timing */
        LOG_DEBUG("Destination phase was: %"PRIu32"\n", lwmac->tx.current_neighbour->phase);
        LOG_DEBUG("Phase when sent was:   %"PRIu32"\n", _ticks_to_phase(lwmac->tx.timestamp));
        LOG_DEBUG("Ticks when sent was:   %"PRIu32"\n", lwmac->tx.timestamp);

        GOTO_TX_STATE(TX_STATE_WAIT_FOR_WA, false);
    }
    case TX_STATE_WAIT_FOR_WA:
    {
        LOG_DEBUG("TX_STATE_WAIT_FOR_WA\n");

        gnrc_pktsnip_t* pkt;
        bool found_wa = false;
        bool postponed = false;
        bool from_expected_destination = false;

        if(lwmac_timeout_is_expired(lwmac, TIMEOUT_NO_RESPONSE)) {
            LOG_DEBUG("No response from destination\n");
            GOTO_TX_STATE(TX_STATE_FAILED, true);
        }

        if(lwmac_timeout_is_expired(lwmac, TIMEOUT_WR)) {
            GOTO_TX_STATE(TX_STATE_SEND_WR, true);
        }

        if(_get_netdev_state(lwmac) == NETOPT_STATE_RX) {
            LOG_WARNING("Wait for completion of frame reception\n");
            break;
        }

        while( (pkt = packet_queue_pop(&lwmac->rx.queue)) != NULL )
        {
            LOG_DEBUG("Inspecting pkt @ %p\n", pkt);

            /* Parse packet */
            lwmac_packet_info_t info;
            int ret = _parse_packet(pkt, &info);

            if(ret != 0) {
                LOG_DEBUG("Packet could not be parsed: %i\n", ret);
                gnrc_pktbuf_release(pkt);
                continue;
            }

            if(_addr_match(&info.src_addr, &lwmac->tx.current_neighbour->l2_addr)) {
                from_expected_destination = true;
            }

            if(info.header->type == FRAMETYPE_BROADCAST) {
                _dispatch_defer(lwmac->rx.dispatch_buffer, pkt);
                /* Drop pointer to it can't get released */
                pkt = NULL;
            }

            /* Check if destination is talking to another node. It will sleep
             * after a finished transaction so there's no point in trying any
             * further now. */
            if( !_addr_match(&info.dst_addr, &lwmac->l2_addr) &&
                 from_expected_destination) {
                _queue_tx_packet(lwmac, lwmac->tx.packet);
                /* drop pointer so it wont be free'd */
                lwmac->tx.packet = NULL;
                postponed = true;
                gnrc_pktbuf_release(pkt);
                break;
            }

            if(info.header->type == FRAMETYPE_BROADCAST) {
                continue;
            }

            if(info.header->type != FRAMETYPE_WA) {
                LOG_DEBUG("Packet is not WA: 0x%02x\n", info.header->type);
                gnrc_pktbuf_release(pkt);
                continue;
            }

            /* No need to keep pkt anymore */
            gnrc_pktbuf_release(pkt);

            if(!from_expected_destination) {
                LOG_DEBUG("Packet is not from expected destination\n");
                break;
            }

            /* All checks passed so this must be a valid WA */
            lwmac_clear_timeout(lwmac, TIMEOUT_WR);
            lwmac_clear_timeout(lwmac, TIMEOUT_NO_RESPONSE);
            found_wa = true;
            break;
        }

        if(postponed) {
            LOG_INFO("Destination is talking to another node, postpone\n");
            GOTO_TX_STATE(TX_STATE_FAILED, true);
        }

        if(!found_wa) {
            LOG_DEBUG("No WA yet\n");
            break;
        }

        /* WR arrived at destination, so calculate destination wakeup phase
         * based on the timestamp of the WR we sent */
         uint32_t new_phase = _ticks_to_phase(lwmac->tx.timestamp);

        /* Save newly calculated phase for destination */
        lwmac->tx.current_neighbour->phase = new_phase;
        LOG_INFO("New phase: %"PRIu32"\n", new_phase);

        /* We've got our WA, so discard the rest, TODO: no flushing */
        packet_queue_flush(&lwmac->rx.queue);

        GOTO_TX_STATE(TX_STATE_SEND_DATA, true);
    }
    case TX_STATE_SEND_DATA:
    {
        LOG_DEBUG("TX_STATE_SEND_DATA\n");

        gnrc_pktsnip_t* pkt = lwmac->tx.packet;

        /* Enable Auto ACK again */
        netopt_enable_t autoack = NETOPT_ENABLE;
		lwmac->netdev2_driver->set(lwmac->netdev->dev, NETOPT_AUTOACK, &autoack, sizeof(autoack));

        /* It's okay to retry sending DATA. Timing doesn't matter anymore and
         * destination is waiting for a certain amount of time. */
        uint8_t csma_retries = LWMAC_DATA_CSMA_RETRIES;
		lwmac->netdev2_driver->set(lwmac->netdev->dev, NETOPT_CSMA_RETRIES, &csma_retries, sizeof(csma_retries));

        netopt_enable_t csma_enable = NETOPT_ENABLE;
		lwmac->netdev2_driver->set(lwmac->netdev->dev, NETOPT_CSMA, &csma_enable, sizeof(csma_enable));

        /* Insert lwMAC header above NETIF header */
        lwmac_hdr_t hdr = {FRAMETYPE_DATA};
        pkt->next = gnrc_pktbuf_add(pkt->next, &hdr, sizeof(hdr), GNRC_NETTYPE_LWMAC);

        /* Send data */
		lwmac->netdev->send(lwmac->netdev, pkt);
        _set_netdev_state(lwmac, NETOPT_STATE_TX);

        /* Packet has been released by netdev, so drop pointer */
        lwmac->tx.packet = NULL;

        GOTO_TX_STATE(TX_STATE_WAIT_FEEDBACK, false);
    }
    case TX_STATE_WAIT_FEEDBACK:
    {
        LOG_DEBUG("TX_STATE_WAIT_FEEDBACK\n");
        if(lwmac->tx_feedback == TX_FEEDBACK_UNDEF) {
            break;
        } else if(lwmac->tx_feedback == TX_FEEDBACK_SUCCESS) {
            GOTO_TX_STATE(TX_STATE_SUCCESSFUL, true);
        } else if(lwmac->tx_feedback == TX_FEEDBACK_NOACK) {
            LOG_ERROR("Not ACKED\n");
            GOTO_TX_STATE(TX_STATE_FAILED, true);
        } else if(lwmac->tx_feedback == TX_FEEDBACK_BUSY) {
            LOG_ERROR("Channel busy \n");
            GOTO_TX_STATE(TX_STATE_FAILED, true);
        }

        LOG_ERROR("Tx feedback unhandled: %i\n", lwmac->tx_feedback);
        GOTO_TX_STATE(TX_STATE_FAILED, true);
    }
    case TX_STATE_SUCCESSFUL:
    case TX_STATE_FAILED:
        break;
    case TX_STATE_STOPPED:
        LOG_DEBUG("Transmission state machine is stopped\n");
    }

    return reschedule;
}

/******************************************************************************/

void lwmac_tx_update(lwmac_t* lwmac)
{
    /* Update until no rescheduling needed */
    while(_lwmac_tx_update(lwmac));
}

/******************************************************************************/
