/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_gomach GoMacH
 * @ingroup     net_gnrc
 * @brief       A traffic-adaptive multi-channel MAC
 * @deprecated  This module is deprecated and will be removed after the 2024.10 release.
 *              As an alternative MAC layer for IEEE 802.15.4, you can use @ref pkg_opendsme.
 *
 *
 * GoMacH is, "a General, nearly Optimal MAC protocol for multi-Hop communications",
 * for IEEE 802.15.4 IoT/WSNs networks. It is designed to be a traffic adaptive MAC
 * protocol that provides high traffic adaptability, high energy efficiency and high
 * robustness.
 *
 * ## GoMacH's main features
 * - doesn't rely on global synchronization.
 * - supports for multi-hop and mesh network.
 * - adopts a duty-cycle scheme to conserve power.
 * - provides high traffic adaptation for handling burst or dynamic traffics.
 *   It achieves this by dynamically allocating transmission slots to intensive
 *   senders (that have pending packets), which enables one (or more) sender(s)
 *   to burst transmit all of its (their) buffered packets (for the same destination) to
 *   the receiver in one shot (or during a shot notice). The burst slotted-transmissions
 *   will be ordered by the receiver device in a TDMA period.
 * - adopts a multi-channel scheme for avoiding/reducing wireless interference jam.
 *
 * @{
 *
 * @file
 * @brief       Implementation of GoMacH protocol
 *
 * @author      Shuguo Zhuo <shuguo.zhuo@inria.fr>
 */


#include "periph/rtt.h"
#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup   net_gnrc_gomach_conf     GNRC GoMacH compile configuration
 * @ingroup    net_gnrc_conf
 * @{
 */
/**
 * @brief The default duration of GoMacH's wake-up period (WP).
 *
 * GoMacH adopts the duty-cycle scheme that, by default, a node only wakes up
 * for a short period of @ref CONFIG_GNRC_GOMACH_CP_DURATION_US in each cycle.
 * In the rest of the cycle (except vTDMA), the node turns off the radio  to
 * conserve power. @ref CONFIG_GNRC_GOMACH_CP_DURATION_US should be at least
 * longer than @ref CONFIG_GNRC_GOMACH_MAX_PREAM_INTERVAL_US, thus to guarantee
 * that the receiver will not miss the preamble packet.
 */
#ifndef CONFIG_GNRC_GOMACH_CP_DURATION_US
#define CONFIG_GNRC_GOMACH_CP_DURATION_US        (10U * US_PER_MS)
#endif

/**
 * @brief GoMacH's superframe duration, i.e., time between two consecutive wake-ups.
 *
 * This macro governs power consumption and GoMacH's reactiveness to traffic
 * loads. In GoMacH, nodes adopt duty-cycle scheme to conserve power. That is,
 * time is divided into repeated cycles (superframes), and in each cycle, a
 * node only wakes up for a short period of time for receiving potential
 * incoming packets for itself. This macro defines the wake-up interval, or,
 * in other words, defines the cycle duration used in GoMacH. If the wake-up
 * interval is short, nodes will wake up more frequently, which leads to quicker
 * reactiveness of the MAC protocol for handling packet reception and
 * transmission, but also results in higher power consumption due to more idle
 * listening. In GoMacH, by default, we regard the wake-up period (WP) as the
 * beginning of a cycle.
 *
 * @note GoMacH's superframe duration
 * @ref CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US should not be shorter than 10
 * times of @ref CONFIG_GNRC_GOMACH_CP_DURATION_US and not shorter than the RTT
 * tickle interval.
 */
#ifndef CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US
#define CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US        (300LU * US_PER_MS)
#endif

#ifndef RTT_FREQUENCY
#error "RTT_FREQUENCY undefined."
#else
#if ((CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US < ((1000LU *US_PER_MS) / RTT_FREQUENCY)) || \
     (CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US < (10 * CONFIG_GNRC_GOMACH_CP_DURATION_US)))
#undef CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US
#if (((1000LU *US_PER_MS) / RTT_FREQUENCY) > (10 * CONFIG_GNRC_GOMACH_CP_DURATION_US))
#define CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US   ((1000LU * US_PER_MS) / RTT_FREQUENCY)
#else
#define CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US   (10 * CONFIG_GNRC_GOMACH_CP_DURATION_US)
#endif
#endif
#endif

/**
 * @brief The maximum duration of the random period at the end of GoMacH's
 *        wake-up period (WP).
 *
 * Currently, GoMacH's WP is actually composed of
 * @ref CONFIG_GNRC_GOMACH_CP_DURATION_US and (+)
 * @ref CONFIG_GNRC_GOMACH_CP_RANDOM_END_US. We currently introduced this random
 * period to avoid beacon collision among neighbor nodes. This macro may be
 * removed in the future.
 */
#ifndef CONFIG_GNRC_GOMACH_CP_RANDOM_END_US
#define CONFIG_GNRC_GOMACH_CP_RANDOM_END_US        (1U * US_PER_MS)
#endif

/**
 * @brief The maximum duration of GoMacH's wake-up period (WP).
 *
 * @ref GNRC_GOMACH_CP_DURATION_MAX_US defines the allowed maximum duration
 * of GoMacH's WP period. A node will quit WP once it reaches this maximum
 * duration.
 * @note In GoMacH's WP, after each normal packet reception (except broadcast
 * packet), a receiver will automatically extends the WP period (reset WP
 * timeout), to receiver more potential incoming packets, before WP
 * reaches this @ref GNRC_GOMACH_CP_DURATION_MAX_US duration.
 */
#ifndef GNRC_GOMACH_CP_DURATION_MAX_US
#define GNRC_GOMACH_CP_DURATION_MAX_US (5LU * CONFIG_GNRC_GOMACH_CP_DURATION_US)
#endif

/**
 * @brief The maximum time for waiting the receiver's beacon in GoMacH.
 *
 * After transmissions in the WP, if the sender still has pending packets for
 * the receiver, it will wait for the receiver's incoming beacon that allocates
 * dynamic transmission slots to it. @ref GNRC_GOMACH_WAIT_BEACON_TIME_US
 * defines the maximum waiting time for the beacon. Once the beacon-waiting
 * timeout expires, the sender will quit the vTMDA (slotted transmission)
 * procedure, and restarts transmissions (started with normal CSMA attempts
 * in the receiver's WP) in the next cycle for sending the pending  packets
 * to the same receiver.
 */
#ifndef GNRC_GOMACH_WAIT_BEACON_TIME_US
#define GNRC_GOMACH_WAIT_BEACON_TIME_US        (GNRC_GOMACH_CP_DURATION_MAX_US)
#endif

/**
 * @brief The minimum gap between neighbor nodes' wake-up phases in GoMacH.
 *
 * To reduce beacon collisions and transmission collisions, GoMacH intends to
 * avoid neighbor nodes' phases being too close to each other. This macro
 * defines the minimum gap between two nodes's wake-up phases. If the sender
 * finds its wake-up phase too closed to its receiver's, it will randomly
 * select a new phase for itself.
 */
#ifndef CONFIG_GNRC_GOMACH_CP_MIN_GAP_US
#define CONFIG_GNRC_GOMACH_CP_MIN_GAP_US        (25U * US_PER_MS)
#endif

/**
 * @brief Timeout duration for waiting @ref NETDEV_EVENT_RX_COMPLETE in GoMacH.
 *
 * Sometimes in GoMacH, if a node finds RX ongoing when it is just about to
 * enter the next MAC state, it will set up a timeout for waiting this packet
 * reception complete with a timeout of this
 * @ref CONFIG_GNRC_GOMACH_WAIT_RX_END_US duration.
 */
#ifndef CONFIG_GNRC_GOMACH_WAIT_RX_END_US
#define CONFIG_GNRC_GOMACH_WAIT_RX_END_US        (6U * US_PER_MS)
#endif

/**
 * @brief Timeout duration for confirming TX-No-ISR event in GoMacH.
 *
 * This macro is used to confirm/catch a case that a transmission doesn't have
 * its @ref NETDEV_EVENT_TX_COMPLETE interrupt event, which is considered as a
 * hardware abnormal event. Upon this timeout expiration, GoMach will
 * accordingly take actions to maintain its state-machine.
 */
#ifndef CONFIG_GNRC_GOMACH_NO_TX_ISR_US
#define CONFIG_GNRC_GOMACH_NO_TX_ISR_US          (50U * US_PER_MS)
#endif

/**
 * @brief Maximum time interval between two consecutive preamble packets in GoMacH.
 *
 * In GoMacH, a sender first uses preamble stream to track the receiver's
 * wake-up phase (WP), if the receiver's WP is unknown. This macro defines the
 * maximum time interval between twoconsecutive preamble packets.
 */
#ifndef CONFIG_GNRC_GOMACH_MAX_PREAM_INTERVAL_US
#define CONFIG_GNRC_GOMACH_MAX_PREAM_INTERVAL_US        (6U * US_PER_MS)
#endif

/**
 * @brief Time interval between two consecutive preamble packets in GoMacH.
 *
 * In GoMacH, after a preamble is sent, the sender sets a timeout with
 * @ref CONFIG_GNRC_GOMACH_PREAMBLE_INTERVAL_US duration for waiting to send
 * the next preamble. Notably, this macro is with a very small value. In
 * GoMacH, for receiving the preamble-ACK packet, the sender doesn't wait for
 * the whole reception of the preamble-ACK. Instead, it only waits for the
 * @ref NETDEV_EVENT_RX_STARTED event which leads to shorter time interval
 * between two consecutive preamble transmissions.
 */
#ifndef CONFIG_GNRC_GOMACH_PREAMBLE_INTERVAL_US
#define CONFIG_GNRC_GOMACH_PREAMBLE_INTERVAL_US        (2U * US_PER_MS)
#endif

/**
 * @brief Time interval between two consecutive broadcast packets in GoMacH.
 *
 * In GoMacH, when sending a broadcast packet, the sender broadcasts the same
 * packet frame on its two public channels simultaneously, with a total duration
 * of @ref CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US to guarantee that all
 * neighbors will get a copy. This macro defines the time interval between
 * ending two consecutive broadcast copies.
 */
#ifndef CONFIG_GNRC_GOMACH_BCAST_INTERVAL_US
#define CONFIG_GNRC_GOMACH_BCAST_INTERVAL_US        (1U * US_PER_MS)
#endif

/**
 * @brief The Maximum preamble duration time of GoMacH.
 *
 * Since GoMacH adopts duty-cycle scheme, to probe receiver's wake-up period
 * a sender sends preamble streams to notice the receiver for communication.
 * To ensure that the receiver will catch at least one preamble packet
 * in a critical case that one public channel is jammed, the sender repeatedly
 * broadcasts a stream of preamble packets with the broadcast duration
 * (preamble duration) slightly longer than twice of
 * @ref CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US.
 */
#ifndef GNRC_GOMACH_PREAMBLE_DURATION_US
#define GNRC_GOMACH_PREAMBLE_DURATION_US \
     (21LU * CONFIG_GNRC_GOMACH_SUPERFRAME_DURATION_US / 10)
#endif

/**
 * @brief The transmission slot size in GoMacH.
 *
 * GoMacH adopts dynamic slots allocation scheme to allocate transmission slots
 * to senders that have pending packets. Each slot is for one data packet with
 * ACK transmission. @ref CONFIG_GNRC_GOMACH_VTDMA_SLOT_SIZE_US is right
 * sufficient for the transmission of the longest packet in IEEE 802.15.4 with
 * ACK. Should not be changed.
 */
#ifndef CONFIG_GNRC_GOMACH_VTDMA_SLOT_SIZE_US
#define CONFIG_GNRC_GOMACH_VTDMA_SLOT_SIZE_US        (5U * US_PER_MS)
#endif

/**
 * @brief Maximum times of CSMA TX attempts under busy-indication in the WP
 *        period of the receiver.
 *
 * Senders in GoMacH adopt CSMA scheme to send data packets in the WP period of
 * the receiver. In case of having medium-busy feedback in WP and the TX failure
 * count (due to busy) is below @ref CONFIG_GNRC_GOMACH_TX_BUSY_THRESHOLD, the
 * sender continue to send the packet with CSMAin the receiver's WP, with the
 * consideration that there may be multi-senders simultaneously competing in WP
 * and the WP will be continuously extended (thus the packet can be received).
 */
#ifndef CONFIG_GNRC_GOMACH_TX_BUSY_THRESHOLD
#define CONFIG_GNRC_GOMACH_TX_BUSY_THRESHOLD      (5U)
#endif

/**
 * @brief Maximum WP period extension number in GoMacH.
 *
 * In GoMacH, the WP period of a receiver will be extended upon each successful
 * packet reception (except receiving broadcast or preamble packet) to receive
 * more potential incoming packets. This macro defines the maximum WP period
 * extension number allowed in GoMacH.
 */
#ifndef CONFIG_GNRC_GOMACH_CP_EXTEND_THRESHOLD
#define CONFIG_GNRC_GOMACH_CP_EXTEND_THRESHOLD      (5U)
#endif

/**
 * @brief GoMacH's check-duplicate-packet unit life time in cycle count.
 *
 * In GoMacH, to avoid receiving duplicate-packet, we currently introduce a
 * data type of @ref gnrc_gomach_dupchk_unit_t to record the recent senders'
 * information (especially MAC TX sequence). This macro defines the
 * check-duplicate-packet data unit's life time in cycle count. Once expired,
 * the related data unit will be reset. This macro maybe removed in the future.
 */
#ifndef CONFIG_GNRC_GOMACH_RX_DUPCHK_UNIT_LIFE
#define CONFIG_GNRC_GOMACH_RX_DUPCHK_UNIT_LIFE            (30U)
#endif

/**
 * @brief Maximum number of senders allowed to be allocated slots in one cycle.
 *
 * Exclude the static GoMacH MAC header payload in the beacon, which is 20
 * bytes, we have 107 bytes left for constructing the sender-ID list and the
 * related slots-number list. A combined slots allocation information pair
 * (sender ID with its corresponded allocate slots number) will cost 9 (8+1)
 * bytes, thus we can hold a maximum of 11 i.e., ((127 - 20) / 9), sender IDs
 * in the beacon.
 */
#ifndef CONFIG_GNRC_GOMACH_MAX_ALLOC_SENDER_NUM
#define CONFIG_GNRC_GOMACH_MAX_ALLOC_SENDER_NUM          (11U)
#endif

/**
 * @brief Maximum t2k attempts before going to t2u in GoMacH.
 *
 * After phase-locked with the receiver, a sender runs a t2k (transmit-to-known)
 * procedure to transmit packet to the phase-known device. However, due to
 * factors like timer driftor busy-channel, a transmission attempt may fail
 * in t2k. If the t2k failure count has reached this
 * @ref CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD, the sender regards
 * phase-locked failed due to timer drift. In this case, it will adopt t2u
 * (transmit-to-unknown) procedure to get re-phase-locked with the receiver.
 */
#ifndef CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD
#define CONFIG_GNRC_GOMACH_REPHASELOCK_THRESHOLD      (4U)
#endif

/**
 * @brief Maximum t2u attempts before dropping data packet in GoMacH.
 *
 * In case the receiver's phase is unknown to the sender, the sender adopts the
 * t2u (transmit-to-unknown) procedure to get phase-locked with the receiver.
 * This macro defines the maximum t2u attempts before dropping the data packet
 * in GoMacH.
 */
#ifndef CONFIG_GNRC_GOMACH_T2U_RETYR_THRESHOLD
#define CONFIG_GNRC_GOMACH_T2U_RETYR_THRESHOLD      (2U)
#endif

/**
 * @brief Maximum t2u attempts before re-initiate radio in GoMacH.
 *
 * After a long period of run time, a radio may be in wrong condition which
 * needs to be re-calibrated. This is indicated by having a series of
 * continuous t2u failures (no preambleACK) in GoMacH. In case we have
 * @ref CONFIG_GNRC_GOMACH_MAX_T2U_RETYR_THRESHOLD number of t2u failures, then
 * we re-initiate the radio, trying to re-calibrate the radio for bringing it
 * back to normal condition.
 */
#ifndef CONFIG_GNRC_GOMACH_MAX_T2U_RETYR_THRESHOLD
#define CONFIG_GNRC_GOMACH_MAX_T2U_RETYR_THRESHOLD          (10U)
#endif
/** @} */

/**
 * @brief   Creates an IEEE 802.15.4 GoMacH network interface
 *
 * @param[out] netif    The interface. May not be `NULL`.
 * @param[in] stack     The stack for the GoMacH network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the GoMacH network interface's thread.
 * @param[in] name      Name for the GoMacH network interface. May be NULL.
 * @param[in] dev       Device for the interface
 *
 * @see @ref gnrc_netif_create()
 *
 * @return  0 on success
 * @return  negative number on error
 */
int gnrc_netif_gomach_create(gnrc_netif_t *netif, char *stack, int stacksize,
                             char priority, const char *name, netdev_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
