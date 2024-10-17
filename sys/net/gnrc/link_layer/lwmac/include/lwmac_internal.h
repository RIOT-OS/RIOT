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
 * @brief       Interface definition for internal functions of LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef LWMAC_INTERNAL_H
#define LWMAC_INTERNAL_H

#include <stdint.h>

#include "periph/rtt.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/mac/types.h"
#include "net/gnrc/lwmac/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Flag to track if the sender can continue to transmit packet to
 *          the receiver in its TX procedure.
 *
 * LWMAC supports burst transmission based on the pending-bit technique.
 * Namely, if the sender has multi packets for the same receiver, it can
 * successively transmit its packets back to back with this flag set up,
 * with the awareness that the receiver will also keep awake for receptions.
 */
#define GNRC_LWMAC_TX_CONTINUE          (0x0008U)

/**
 * @brief   Flag to track if the sender should quit Tx in current cycle.
 *
 * This flag is mainly for collision avoidance. In case a node overhears
 * ongoing broadcast packets stream or other ongoing transmissions of
 * other communication pairs during its wake-up period, it sets up this
 * flag, which quits all its potential transmission attempts in this current
 * cycle (started by the wake-up period), thus not to collide with other
 * (neighbor) nodes' transmissions.
 */
#define GNRC_LWMAC_QUIT_TX              (0x0010U)

/**
 * @brief   Flag to track if the device need to reselect a new wake-up phase.
 *
 * This flag is mainly for potential collision avoidance. In multi-hop scenario,
 * it could be dangerous that a sender's wake-up phase is close to its receiver's,
 * which may lead to collisions when the sender is sending to the receiver while
 * the sender's son nodes are also sending to the sender. To avoid this, in case a
 * sender finds its phase close to its receiver's, it sets up this flag and then
 * randomly reselects a new wake-up phase.
 */
#define GNRC_LWMAC_PHASE_BACKOFF        (0x0020U)

/**
 * @brief   Flag to track if the device needs to quit the wake-up (listening) procedure.
 *
 * LWMAC adopts an auto wake-up extension scheme. That is, normally, after each data
 * reception in the wake-up period, it extends the wake-up period to another basic
 * duration, thus to receive more potential incoming packets, which is also correlated to
 * the pending-bit transmission scheme to support burst transmissions to boost throughput.
 * However, in some situations, like receiving broadcast (stream) packet, the receiver
 * should immediately goto sleep (by setting up this flag) after one reception, thus not
 * to receive duplicate broadcast packets.
 */
#define GNRC_LWMAC_QUIT_RX              (0x0040U)

/**
 * @brief Type to pass information about parsing.
 */
typedef struct {
    gnrc_lwmac_hdr_t *header;       /**< LWMAC header of packet */
    gnrc_lwmac_l2_addr_t src_addr;  /**< copied source address of packet  */
    gnrc_lwmac_l2_addr_t dst_addr;  /**< copied destination address of packet */
} gnrc_lwmac_packet_info_t;

/**
 * @brief Next RTT event must be at least this far in the future.
 *
 * When setting an RTT alarm to short in the future it could be possible that
 * the counter already passed the calculated alarm before it could be set.
 */
#define GNRC_LWMAC_RTT_EVENT_MARGIN_TICKS    (RTT_MS_TO_TICKS(2))

/**
 * @brief set the @ref GNRC_LWMAC_TX_CONTINUE flag of the device
 *
 * @param[in] netif        ptr to the network interface
 * @param[in] tx_continue  value for LWMAC tx-continue flag
 *
 */
static inline void gnrc_lwmac_set_tx_continue(gnrc_netif_t *netif, bool tx_continue)
{
    if (tx_continue) {
        netif->mac.mac_info |= GNRC_LWMAC_TX_CONTINUE;
    }
    else {
        netif->mac.mac_info &= ~GNRC_LWMAC_TX_CONTINUE;
    }
}

/**
 * @brief get the @ref GNRC_LWMAC_TX_CONTINUE flag of the device
 *
 * @param[in] netif        ptr to the network interface
 *
 * @return                 true if tx continue
 * @return                 false if tx will not continue
 */
static inline bool gnrc_lwmac_get_tx_continue(gnrc_netif_t *netif)
{
    return (netif->mac.mac_info & GNRC_LWMAC_TX_CONTINUE);
}

/**
 * @brief set the @ref GNRC_LWMAC_QUIT_TX flag of the device
 *
 * @param[in] netif        ptr to the network interface
 * @param[in] quit_tx      value for @ref GNRC_LWMAC_QUIT_TX flag
 *
 */
static inline void gnrc_lwmac_set_quit_tx(gnrc_netif_t *netif, bool quit_tx)
{
    if (quit_tx) {
        netif->mac.mac_info |= GNRC_LWMAC_QUIT_TX;
    }
    else {
        netif->mac.mac_info &= ~GNRC_LWMAC_QUIT_TX;
    }
}

/**
 * @brief get the @ref GNRC_LWMAC_QUIT_TX flag of the device
 *
 * @param[in] netif        ptr to the network interface
 *
 * @return                 true if quit tx
 * @return                 false if will not quit tx
 */
static inline bool gnrc_lwmac_get_quit_tx(gnrc_netif_t *netif)
{
    return (netif->mac.mac_info & GNRC_LWMAC_QUIT_TX);
}

/**
 * @brief set the @ref GNRC_LWMAC_PHASE_BACKOFF flag of the device
 *
 * @param[in] netif        ptr to the network interface
 * @param[in] backoff      value for LWMAC @ref GNRC_LWMAC_PHASE_BACKOFF flag
 *
 */
static inline void gnrc_lwmac_set_phase_backoff(gnrc_netif_t *netif, bool backoff)
{
    if (backoff) {
        netif->mac.mac_info |= GNRC_LWMAC_PHASE_BACKOFF;
    }
    else {
        netif->mac.mac_info &= ~GNRC_LWMAC_PHASE_BACKOFF;
    }
}

/**
 * @brief get the @ref GNRC_LWMAC_PHASE_BACKOFF flag of the device
 *
 * @param[in] netif        ptr to the network interface
 *
 * @return                 true if will run phase-backoff
 * @return                 false if will not run phase-backoff
 */
static inline bool gnrc_lwmac_get_phase_backoff(gnrc_netif_t *netif)
{
    return (netif->mac.mac_info & GNRC_LWMAC_PHASE_BACKOFF);
}

/**
 * @brief set the @ref GNRC_LWMAC_QUIT_RX flag of the device
 *
 * @param[in] netif        ptr to the network interface
 * @param[in] quit_rx      value for LWMAC @ref GNRC_LWMAC_QUIT_RX flag
 *
 */
static inline void gnrc_lwmac_set_quit_rx(gnrc_netif_t *netif, bool quit_rx)
{
    if (quit_rx) {
        netif->mac.mac_info |= GNRC_LWMAC_QUIT_RX;
    }
    else {
        netif->mac.mac_info &= ~GNRC_LWMAC_QUIT_RX;
    }
}

/**
 * @brief get the @ref GNRC_LWMAC_QUIT_RX flag of the device
 *
 * @param[in] netif        ptr to the network interface
 *
 * @return                 true if will quit rx
 * @return                 false if will not quit rx
 */
static inline bool gnrc_lwmac_get_quit_rx(gnrc_netif_t *netif)
{
    return (netif->mac.mac_info & GNRC_LWMAC_QUIT_RX);
}

/**
 * @brief set the @ref GNRC_LWMAC_DUTYCYCLE_ACTIVE flag of LWMAC
 *
 * @param[in] netif        ptr to the network interface
 * @param[in] active       value for LWMAC @ref GNRC_LWMAC_DUTYCYCLE_ACTIVE flag
 *
 */
static inline void gnrc_lwmac_set_dutycycle_active(gnrc_netif_t *netif, bool active)
{
    if (active) {
        netif->mac.prot.lwmac.lwmac_info |= GNRC_LWMAC_DUTYCYCLE_ACTIVE;
    }
    else {
        netif->mac.prot.lwmac.lwmac_info &= ~GNRC_LWMAC_DUTYCYCLE_ACTIVE;
    }
}

/**
 * @brief get the @ref GNRC_LWMAC_DUTYCYCLE_ACTIVE flag of LWMAC
 *
 * @param[in] netif        ptr to the network interface
 *
 * @return                 true if active
 * @return                 false if not active
 */
static inline bool gnrc_lwmac_get_dutycycle_active(gnrc_netif_t *netif)
{
    return (netif->mac.prot.lwmac.lwmac_info & GNRC_LWMAC_DUTYCYCLE_ACTIVE);
}

/**
 * @brief set the @ref GNRC_LWMAC_NEEDS_RESCHEDULE flag of LWMAC
 *
 * @param[in] netif        ptr to the network interface
 * @param[in] reschedule   value for @ref GNRC_LWMAC_NEEDS_RESCHEDULE flag
 *
 */
static inline void gnrc_lwmac_set_reschedule(gnrc_netif_t *netif, bool reschedule)
{
    if (reschedule) {
        netif->mac.prot.lwmac.lwmac_info |= GNRC_LWMAC_NEEDS_RESCHEDULE;
    }
    else {
        netif->mac.prot.lwmac.lwmac_info &= ~GNRC_LWMAC_NEEDS_RESCHEDULE;
    }
}

/**
 * @brief get the @ref GNRC_LWMAC_NEEDS_RESCHEDULE flag of LWMAC
 *
 * @param[in] netif        ptr to the network interface
 *
 * @return                 true if needs rescheduling
 * @return                 false if no need for rescheduling
 */
static inline bool gnrc_lwmac_get_reschedule(gnrc_netif_t *netif)
{
    return (netif->mac.prot.lwmac.lwmac_info & GNRC_LWMAC_NEEDS_RESCHEDULE);
}

/**
 * @brief send a @ref net_gnrc_pkt "packet" over the network interface in LWMAC
 *
 * @internal
 *
 * @pre `netif != NULL && pkt != NULL`
 *
 * @note The function re-formats the content of @p pkt to a format expected
 *       by the netdev_driver_t::send() method of gnrc_netif_t::dev and
 *       releases the packet before returning (so no additional release
 *       should be required after calling this method).
 *
 * @param[in] netif The network interface.
 * @param[in] pkt   A packet to send.
 *
 * @return  The number of bytes actually sent on success
 * @return  -EBADMSG, if the @ref net_gnrc_netif_hdr in @p pkt is missing
 *          or is in an unexpected format.
 * @return  -ENOTSUP, if sending @p pkt in the given format isn't supported
 *          (e.g. empty payload with Ethernet).
 * @return  Any negative error code reported by gnrc_netif_t::dev.
 */
int _gnrc_lwmac_transmit(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief Parse an incoming packet and extract important information.
 *
 *        Copies addresses into @p info, but header points inside @p pkt.
 *
 * @param[in]   pkt             packet that will be parsed
 * @param[out]  info            structure that will hold parsed information
 *
 * @return                      0 if correctly parsed
 * @return                      <0 on error
 */
int _gnrc_lwmac_parse_packet(gnrc_pktsnip_t *pkt, gnrc_lwmac_packet_info_t *info);

/**
 * @brief Shortcut to get the state of netdev.
 *
 * @param[in]   netif    ptr to the network interface
 *
 * @return               state of netdev
 */
netopt_state_t _gnrc_lwmac_get_netdev_state(gnrc_netif_t *netif);

/**
 * @brief Shortcut to set the state of netdev
 *
 * @param[in]   netif       ptr to the network interface
 * @param[in]   devstate    new state for netdev
 */
void _gnrc_lwmac_set_netdev_state(gnrc_netif_t *netif, netopt_state_t devstate);

/**
 * @brief Convert RTT ticks to device phase
 *
 * @param[in]   ticks    RTT ticks
 *
 * @return               device phase
 */
static inline uint32_t _gnrc_lwmac_ticks_to_phase(uint32_t ticks)
{
    assert(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US != 0);

    return (ticks % RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US));
}

/**
 * @brief Get device's current phase
 *
 * @return               device phase
 */
static inline uint32_t _gnrc_lwmac_phase_now(void)
{
    return _gnrc_lwmac_ticks_to_phase(rtt_get_counter());
}

/**
 * @brief Calculate how many ticks remaining to the targeted phase in the future
 *
 * @param[in]   phase    device phase
 *
 * @return               RTT ticks
 */
static inline uint32_t _gnrc_lwmac_ticks_until_phase(uint32_t phase)
{
    long int tmp = phase - _gnrc_lwmac_phase_now();

    if (tmp < 0) {
        /* Phase in next interval */
        tmp += RTT_US_TO_TICKS(CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US);
    }

    return (uint32_t)tmp;
}

/**
 * @brief Store the received packet to the dispatch buffer and remove possible
 *        duplicate packets.
 *
 * @param[in,out]   buffer      RX dispatch packet buffer
 * @param[in]       pkt         received packet
 *
 * @return                      0 if correctly stored
 * @return                      <0 on error
 */
int _gnrc_lwmac_dispatch_defer(gnrc_pktsnip_t * buffer[], gnrc_pktsnip_t * pkt);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LWMAC_INTERNAL_H */
