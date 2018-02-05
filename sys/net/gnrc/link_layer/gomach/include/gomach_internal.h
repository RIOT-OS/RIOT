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
 * @brief       GoMacH's internal functions.
 * @internal
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef GOMACH_INTERNAL_H
#define GOMACH_INTERNAL_H

#include <stdint.h>

#include "periph/rtt.h"
#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flag to track if the transmission has finished.
 */
#define GNRC_GOMACH_INFO_TX_FINISHED                     (0x0008U)

/**
 * @brief Flag to track if a packet has been successfully received.
 */
#define GNRC_GOMACH_INFO_PKT_RECEIVED                    (0x0010U)

/**
 * @brief Flag to track if need to update GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_ND_UPDATE              (0x0001U)

/**
 * @brief Flag to track if need to quit the current cycle in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_QUIT_CYCLE             (0x0002U)

/**
 * @brief Flag to track if CP period has ended in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_CP_END                 (0x0004U)

/**
 * @brief Flag to track if vTDMA has ended in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_VTDMA_END              (0x0008U)

/**
 * @brief Flag to track if the node has received unintended preamble.
 */
#define GNRC_GOMACH_INTERNAL_INFO_UNINTD_PREAMBLE        (0x0010U)

/**
 * @brief Flag to track if need to quit the current cycle in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLE           (0x0020U)

/**
 * @brief Flag to track if node's duty-cycle has started in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_DUTY_CYCLE_START       (0x0040U)

/**
 * @brief Flag to track if node need to backoff its phase in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_PHASE_BACKOFF          (0x0080U)

/**
 * @brief Flag to track if beacon transmission fail in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_BEACON_FAIL            (0x0200U)

/**
 * @brief Flag to track if node's packet buffer is full in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_BUFFER_FULL            (0x0400U)

/**
 * @brief Flag to track if node has entered a new cycle in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_ENTER_NEW_CYCLE        (0x0800U)

/**
 * @brief Flag to track if node has got preamble-ACK in GoMacH.
 */
#define GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLEACK        (0x1000U)

/**
 * @brief Flag to track if node's radio is on public-channel-1.
 */
#define GNRC_GOMACH_INTERNAL_INFO_ON_PUBCHAN_1           (0x2000U)

/**
 * @brief Flag to track if node has reached maximum preamble interval.
 */
#define GNRC_GOMACH_INTERNAL_INFO_MAX_PREAM_INTERV       (0x4000U)

/**
 * @brief Flag to track if node has turned on its radio.
 */
#define GNRC_GOMACH_INTERNAL_INFO_RADIO_IS_ON            (0x8000U)

/**
 * @brief Set the @ref GNRC_GOMACH_INFO_TX_FINISHED flag of the device.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] tx_finish    value for GoMacH's
 *                         @ref GNRC_GOMACH_INFO_TX_FINISHED flag.
 *
 */
static inline void gnrc_gomach_set_tx_finish(gnrc_netif_t *netif, bool tx_finish)
{
    if (tx_finish) {
        netif->mac.mac_info |= GNRC_GOMACH_INFO_TX_FINISHED;
    }
    else {
        netif->mac.mac_info &= ~GNRC_GOMACH_INFO_TX_FINISHED;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INFO_TX_FINISHED flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if TX has finished.
 * @return             false if TX hasn't finished yet.
 */
static inline bool gnrc_gomach_get_tx_finish(gnrc_netif_t *netif)
{
    return (netif->mac.mac_info & GNRC_GOMACH_INFO_TX_FINISHED);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INFO_PKT_RECEIVED flag of the device.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] received     value for GoMacH's
 *                         @ref GNRC_GOMACH_INFO_PKT_RECEIVED flag.
 *
 */
static inline void gnrc_gomach_set_pkt_received(gnrc_netif_t *netif, bool received)
{
    if (received) {
        netif->mac.mac_info |= GNRC_GOMACH_INFO_PKT_RECEIVED;
    }
    else {
        netif->mac.mac_info &= ~GNRC_GOMACH_INFO_PKT_RECEIVED;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INFO_PKT_RECEIVED flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if radio has successfully received a packet.
 * @return             false if radio hasn't received a packet yet.
 */
static inline bool gnrc_gomach_get_pkt_received(gnrc_netif_t *netif)
{
    return (netif->mac.mac_info & GNRC_GOMACH_INFO_PKT_RECEIVED);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_QUIT_CYCLE flag of the device.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] quit         value for GoMacH's
 *                         @ref GNRC_GOMACH_INTERNAL_INFO_QUIT_CYCLE flag.
 *
 */
static inline void gnrc_gomach_set_quit_cycle(gnrc_netif_t *netif, bool quit)
{
    if (quit) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_QUIT_CYCLE;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_QUIT_CYCLE;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_QUIT_CYCLE flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if need to quit cycle.
 * @return             false if no need to quit cycle.
 */
static inline bool gnrc_gomach_get_quit_cycle(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_QUIT_CYCLE);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLE flag of the device.
 *
 * @param[in,out] netif     the network interface.
 * @param[in] got_preamble  value for GoMacH's
 *                          @ref GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLE flag.
 *
 */
static inline void gnrc_gomach_set_got_preamble(gnrc_netif_t *netif, bool got_preamble)
{
    if (got_preamble) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLE;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLE;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLE flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if get preamble packet.
 * @return             false if not get preamble packet yet.
 */
static inline bool gnrc_gomach_get_got_preamble(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLE);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_CP_END flag of the device.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] cp_end       value for GoMacH's
 *                         @ref GNRC_GOMACH_INTERNAL_INFO_CP_END flag.
 *
 */
static inline void gnrc_gomach_set_cp_end(gnrc_netif_t *netif, bool cp_end)
{
    if (cp_end) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_CP_END;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_CP_END;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_CP_END flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if cp has ended.
 * @return             false if cp hasn't ended yet.
 */
static inline bool gnrc_gomach_get_cp_end(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_CP_END);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_VTDMA_END flag of the device.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] vtdma_end    value for GoMacH's
 *                         @ref GNRC_GOMACH_INTERNAL_INFO_VTDMA_END flag.
 *
 */
static inline void gnrc_gomach_set_vTDMA_end(gnrc_netif_t *netif, bool vtdma_end)
{
    if (vtdma_end) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_VTDMA_END;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_VTDMA_END;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_VTDMA_END flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if vTDMA has ended.
 * @return             false if vTDMA hasn't ended yet.
 */
static inline bool gnrc_gomach_get_vTDMA_end(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_VTDMA_END);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_UNINTD_PREAMBLE flag of the device.
 *
 * @param[in,out] netif        the network interface.
 * @param[in] uintd_preamble   value for GoMacH's
 *                             @ref GNRC_GOMACH_INTERNAL_INFO_UNINTD_PREAMBLE flag.
 *
 */
static inline void gnrc_gomach_set_unintd_preamble(gnrc_netif_t *netif, bool uintd_preamble)
{
    if (uintd_preamble) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_UNINTD_PREAMBLE;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_UNINTD_PREAMBLE;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_UNINTD_PREAMBLE flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if has received unintended-preamble.
 * @return             false if hasn't received unintended-preamble yet.
 */
static inline bool gnrc_gomach_get_unintd_preamble(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_UNINTD_PREAMBLE);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_ND_UPDATE flag of the device.
 *
 * @param[in,out] netif  the network interface.
 * @param[in] update     value for GoMacH's
 *                       @ref GNRC_GOMACH_INTERNAL_INFO_ND_UPDATE flag.
 *
 */
static inline void gnrc_gomach_set_update(gnrc_netif_t *netif, bool update)
{
    if (update) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_ND_UPDATE;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_ND_UPDATE;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_ND_UPDATE flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if need update GoMacH.
 * @return             false if no need to update GoMacH.
 */
static inline bool gnrc_gomach_get_update(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_ND_UPDATE);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_DUTY_CYCLE_START flag of the device.
 *
 * @param[in,out] netif  the network interface.
 * @param[in] start      value for GoMacH's
 *                       @ref GNRC_GOMACH_INTERNAL_INFO_DUTY_CYCLE_START flag.
 *
 */
static inline void gnrc_gomach_set_duty_cycle_start(gnrc_netif_t *netif, bool start)
{
    if (start) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_DUTY_CYCLE_START;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_DUTY_CYCLE_START;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_DUTY_CYCLE_START flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if duty-cycle has started.
 * @return             false if duty-cycle hasn't started yet.
 */
static inline bool gnrc_gomach_get_duty_cycle_start(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_DUTY_CYCLE_START);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_PHASE_BACKOFF flag of the device.
 *
 * @param[in,out] netif   the network interface.
 * @param[in] backoff     value for GoMacH's
 *                        @ref GNRC_GOMACH_INTERNAL_INFO_PHASE_BACKOFF flag.
 *
 */
static inline void gnrc_gomach_set_phase_backoff(gnrc_netif_t *netif, bool backoff)
{
    if (backoff) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_PHASE_BACKOFF;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_PHASE_BACKOFF;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_PHASE_BACKOFF flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if need to run phase backoff.
 * @return             false if no need to run phase backoff.
 */
static inline bool gnrc_gomach_get_phase_backoff(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_PHASE_BACKOFF);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_BEACON_FAIL flag of the device.
 *
 * @param[in,out] netif  the network interface.
 * @param[in] fail       value for GoMacH's
 *                       @ref GNRC_GOMACH_INTERNAL_INFO_BEACON_FAIL flag.
 *
 */
static inline void gnrc_gomach_set_beacon_fail(gnrc_netif_t *netif, bool fail)
{
    if (fail) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_BEACON_FAIL;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_BEACON_FAIL;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_BEACON_FAIL flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if send beacon fail.
 * @return             false upon beacon transmission success.
 */
static inline bool gnrc_gomach_get_beacon_fail(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_BEACON_FAIL);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_BUFFER_FULL flag of the device.
 *
 * @param[in,out] netif  the network interface.
 * @param[in] full       value for GoMacH's
 *                       @ref GNRC_GOMACH_INTERNAL_INFO_BUFFER_FULL flag.
 *
 */
static inline void gnrc_gomach_set_buffer_full(gnrc_netif_t *netif, bool full)
{
    if (full) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_BUFFER_FULL;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_BUFFER_FULL;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_BUFFER_FULL flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if node's packet buffer is full.
 * @return             false if node's packet buffer is not full.
 */
static inline bool gnrc_gomach_get_buffer_full(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_BUFFER_FULL);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_ENTER_NEW_CYCLE flag of the device.
 *
 * @param[in,out] netif  the network interface.
 * @param[in] enter      value for GoMacH's
 *                       @ref GNRC_GOMACH_INTERNAL_INFO_ENTER_NEW_CYCLE flag.
 *
 */
static inline void gnrc_gomach_set_enter_new_cycle(gnrc_netif_t *netif, bool enter)
{
    if (enter) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_ENTER_NEW_CYCLE;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_ENTER_NEW_CYCLE;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_ENTER_NEW_CYCLE flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if node has entered a new cycle.
 * @return             false if node hasn't entered a new cycle yet.
 */
static inline bool gnrc_gomach_get_enter_new_cycle(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_ENTER_NEW_CYCLE);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLEACK flag of the device.
 *
 * @param[in,out] netif  the network interface.
 * @param[in] got        value for GoMacH's
 *                       @ref GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLEACK flag.
 *
 */
static inline void gnrc_gomach_set_got_preamble_ack(gnrc_netif_t *netif, bool got)
{
    if (got) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLEACK;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLEACK;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLEACK flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if node has got preamble-ACK.
 * @return             false if node hasn't got preamble-ACK yet.
 */
static inline bool gnrc_gomach_get_got_preamble_ack(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_GOT_PREAMBLEACK);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_ON_PUBCHAN_1 flag of the device.
 *
 * @param[in,out] netif      the network interface.
 * @param[in] on_pubchan_1   value for GoMacH's
 *                           @ref GNRC_GOMACH_INTERNAL_INFO_ON_PUBCHAN_1 flag.
 *
 */
static inline void gnrc_gomach_set_on_pubchan_1(gnrc_netif_t *netif, bool on_pubchan_1)
{
    if (on_pubchan_1) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_ON_PUBCHAN_1;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_ON_PUBCHAN_1;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_ON_PUBCHAN_1 flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if node is on public channel 1.
 * @return             false if node is not on public channel 1.
 */
static inline bool gnrc_gomach_get_on_pubchan_1(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_ON_PUBCHAN_1);
}

/**
 * @brief Set the @ref GNRC_GOMACH_INTERNAL_INFO_MAX_PREAM_INTERV flag of the device.
 *
 * @param[in,out] netif  the network interface.
 * @param[in] max        value for GoMacH's @ref
 *                       GNRC_GOMACH_INTERNAL_INFO_MAX_PREAM_INTERV flag.
 *
 */
static inline void gnrc_gomach_set_max_pream_interv(gnrc_netif_t *netif, bool max)
{
    if (max) {
        netif->mac.prot.gomach.gomach_info |= GNRC_GOMACH_INTERNAL_INFO_MAX_PREAM_INTERV;
    }
    else {
        netif->mac.prot.gomach.gomach_info &= ~GNRC_GOMACH_INTERNAL_INFO_MAX_PREAM_INTERV;
    }
}

/**
 * @brief Get the @ref GNRC_GOMACH_INTERNAL_INFO_MAX_PREAM_INTERV flag of the device.
 *
 * @param[in] netif    the network interface.
 *
 * @return             true if node has reached maximum preamble interval.
 * @return             false if node hasn't reached maximum preamble interval yet.
 */
static inline bool gnrc_gomach_get_max_pream_interv(gnrc_netif_t *netif)
{
    return (netif->mac.prot.gomach.gomach_info & GNRC_GOMACH_INTERNAL_INFO_MAX_PREAM_INTERV);
}

/**
 * @brief Get device's current phase.
 *
 * @param[in] netif    the network interface.
 *
 * @return             device's current phase.
 */
uint64_t gnrc_gomach_phase_now(gnrc_netif_t *netif);

/**
 * @brief Shortcut to set the state of netdev
 *
 * @param[in]   netif       ptr to the network interface
 * @param[in]   devstate    new state for netdev
 */
void gnrc_gomach_set_netdev_state(gnrc_netif_t *netif, netopt_state_t devstate);

/**
 * @brief Set the auto-ACK parameter of the device.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] autoack      value for the auto-ACK parameter.
 *
 */
static inline void gnrc_gomach_set_autoack(gnrc_netif_t *netif, netopt_enable_t autoack)
{
    assert(netif != NULL);

    netif->dev->driver->set(netif->dev,
                            NETOPT_AUTOACK,
                            &autoack,
                            sizeof(autoack));
}

/**
 * @brief Set the ACK-require parameter of the device.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] ack_req      value for the ACK-require parameter.
 *
 */
static inline void gnrc_gomach_set_ack_req(gnrc_netif_t *netif, netopt_enable_t ack_req)
{
    assert(netif != NULL);

    netif->dev->driver->set(netif->dev,
                            NETOPT_ACK_REQ,
                            &ack_req,
                            sizeof(ack_req));
}

/**
 * @brief Shortcut to get the state of netdev.
 *
 * @param[in] netif    the network interface.
 *
 * @return             state of netdev upon success.
 * @return             -ENOSYS, upon failure.
 */
static inline netopt_state_t gnrc_gomach_get_netdev_state(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    netopt_state_t state;

    if (0 < netif->dev->driver->get(netif->dev,
                                    NETOPT_STATE,
                                    &state,
                                    sizeof(state))) {
        return state;
    }
    return -ENOSYS;
}

/**
 * @brief Turn the radio to a specific channel.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] channel_num  targeted channel number to turn to.
 *
 */
static inline void gnrc_gomach_turn_channel(gnrc_netif_t *netif, uint16_t channel_num)
{
    assert(netif != NULL);

    netif->dev->driver->set(netif->dev,
                            NETOPT_CHANNEL,
                            &channel_num,
                            sizeof(channel_num));
}

/**
 * @brief send a @ref net_gnrc_pkt "packet" over the network interface in GoMacH
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
int _gnrc_gomach_transmit(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief Check if the received packet is a duplicate packet.
 *
 * @param[in] netif    the network interface.
 * @param[in] pa_info  ptr to received packet's parsed information.
 *
 * @return             true if the received packet is a duplicate packet.
 * @return             false if the received packet is not a duplicate packet.
 */
bool gnrc_gomach_check_duplicate(gnrc_netif_t *netif, gnrc_gomach_packet_info_t *pa_info);

/**
 * @brief Send a pktsnip in GoMacH.
 *
 * @param[in] netif        the network interface.
 * @param[in] pkt          ptr to the packet for sending.
 * @param[in] csma_enable  value of csma-enable parameter.
 *
 * @return                 >0 upon sending success.
 * @return                 0< upon sending failure.
 */
int gnrc_gomach_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt, netopt_enable_t csma_enable);

/**
 * @brief Reply a preamble-ACK packet in GoMacH.
 *
 * @param[in] netif        the network interface.
 * @param[in] info         ptr to the info of the preamble packet.
 *
 * @return                 >0 upon sending success.
 * @return                 0< upon sending failure.
 */
int gnrc_gomach_send_preamble_ack(gnrc_netif_t *netif, gnrc_gomach_packet_info_t *info);

/**
 * @brief Broadcast a beacon packet in GoMacH.
 *
 * @param[in] netif        the network interface.
 *
 * @return                 >0 upon sending success.
 * @return                 0< upon sending failure.
 */
int gnrc_gomach_send_beacon(gnrc_netif_t *netif);

/**
 * @brief Store the received packet to the dispatch buffer.
 *
 * @param[in,out]   buffer      RX dispatch packet buffer
 * @param[in]       pkt         received packet
 *
 * @return                      0 if correctly stored
 * @return                      <0 on error
 */
int gnrc_gomach_dispatch_defer(gnrc_pktsnip_t * buffer[], gnrc_pktsnip_t * pkt);

/**
 * @brief Update the queue-length indicator of the packet sender.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] pkt          received packet
 * @param[in] pa_info      ptr to the info of the received packet.
 *
 */
void gnrc_gomach_indicator_update(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt,
                                  gnrc_gomach_packet_info_t *pa_info);

/**
 * @brief Process packets received during the CP (wake-up) period of GoMacH.
 *
 * @param[in,out] netif    the network interface.
 *
 */
void gnrc_gomach_cp_packet_process(gnrc_netif_t *netif);

/**
 * @brief Choose a sub-channel for a device running GoMacH.
 *
 * @param[in,out] netif    the network interface.
 *
 */
void gnrc_gomach_init_choose_subchannel(gnrc_netif_t *netif);

/**
 * @brief Broadcast the chosen sub-channel sequence to the device's neighbors.
 *
 * @param[in] netif        the network interface.
 * @param[in] use_csma     value of csma-enable parameter.
 *
 * @return                 >0 upon sending success.
 * @return                 0< upon sending failure.
 */
int gnrc_gomach_bcast_subchann_seq(gnrc_netif_t *netif, netopt_enable_t use_csma);

/**
 * @brief Send a preamble packet to the targeted neighbor.
 *
 * @param[in] netif        the network interface.
 * @param[in] csma_enable  value of csma-enable parameter.
 *
 * @return                 >0 upon sending success.
 * @return                 0< upon sending failure.
 */
int gnrc_gomach_send_preamble(gnrc_netif_t *netif, netopt_enable_t csma_enable);

/**
 * @brief Process the received preamble-ACK packet to get phase-locked with the sender.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] pkt          ptr to the received preamble-ACK.
 *
 */
void gnrc_gomach_process_preamble_ack(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief Process the received packets to when waiting for the preamble-ACK packet.
 *
 * @param[in,out] netif    the network interface.
 *
 */
void gnrc_gomach_process_pkt_in_wait_preamble_ack(gnrc_netif_t *netif);

/**
 * @brief Send a data packet to the targeted neighbor.
 *
 * @param[in,out] netif    the network interface.
 * @param[in] csma_enable  value of csma-enable parameter.
 *
 * @return                 >0 upon sending success.
 * @return                 0< upon sending failure.
 */
int gnrc_gomach_send_data(gnrc_netif_t *netif, netopt_enable_t csma_enable);

/**
 * @brief Find a neighbor that is next to send packet to.
 *
 * @param[in,out] netif    the network interface.
 *
 * @return                 true, if found next TX neighbor.
 * @return                 false, if not found next TX neighbor.
 *
 */
bool gnrc_gomach_find_next_tx_neighbor(gnrc_netif_t *netif);

/**
 * @brief Process the received beacon packet.
 *
 * @param[in,out] netif    the network interface.
 * @param[in]     pkt          ptr to the received beacon.
 *
 */
void gnrc_gomach_beacon_process(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief Process the received packets when waiting for the beacon during
 *        t2k procedure in GoMacH.
 *
 * @param[in,out] netif    the network interface.
 *
 */
void gnrc_gomach_packet_process_in_wait_beacon(gnrc_netif_t *netif);

/**
 * @brief Process the received packets in the vTDMA period in GoMacH.
 *
 * @param[in,out] netif    the network interface.
 *
 */
void gnrc_gomach_packet_process_in_vtdma(gnrc_netif_t *netif);

/**
 * @brief Update the TX neighbors' phases in GoMacH.
 *
 * @param[in,out] netif    the network interface.
 *
 */
void gnrc_gomach_update_neighbor_phase(gnrc_netif_t *netif);

/**
 * @brief Update the TX neighbors' public channel phase in GoMacH.
 *
 * @param[in,out] netif    the network interface.
 *
 */
void gnrc_gomach_update_neighbor_pubchan(gnrc_netif_t *netif);

#ifdef __cplusplus
}
#endif

#endif /* GOMACH_INTERNAL_H */
/** @} */
