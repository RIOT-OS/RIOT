/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_lwmac LWMAC
 * @ingroup     net_gnrc
 * @brief       A Lightweight duty-cycling 802.15.4 MAC protocol
 * @deprecated  This module is deprecated and will be removed after the 2024.10 release.
 *              As an alternative MAC layer for IEEE 802.15.4, you can use @ref pkg_opendsme.
 *
 *
 * ## LWMAC implementation
 *
 * ## Radio duty cycling
 * LWMAC adopts the radio duty-cycle scheme to conserve power. Namely, in each
 * cycle period (MAC superframe), a node device wakes up for a short period of
 * time (called listen period or wake-up period) for receiving possible incoming
 * packets from other devices. Outside the listen period, the node device turns
 * off its radio to conserve power.
 *
 * ## Phase-lock scheme
 * LWMAC adopts the phase-lock scheme to further reduce power consumption. Each
 * node device in LWMAC will try to record/track its Tx-neighbor's wake-up phase.
 * This is called phase-lock. After phase-locking, the sender node will (likely)
 * spend less preamble packets (also called WR packet, i.e., wake-up-request, in
 * LWMAC) for initiating a hand-shaking procedure for transmitting a data packet,
 * compared to the first time it talks to the receiver.
 *
 * ## Burst transmission
 * LWMAC adopts pending-bit technique to enhance its throughput. Namely, in case
 * of having multi packets for the receiver, a sender uses the pending-bit flag
 * embedded in the MAC header to instruct this situation, and the buffered packets
 * will be transmitted in a continuous sequence, back to back, to the receiver in
 * one shot.
 *
 * ## Auto wake-up extension
 * LWMAC adopts auto wake-up extension scheme based on timeout (like T-MAC). In short,
 * when a packet is successfully received at the receiver side, the receiver will
 * reset the wake-up timeout to extend its wake-up period for receiving more potential
 * incoming packets. This is to be compatible with the pending-bit technique to allow
 * the receiver to absorb more packets when needed, thus boosts the throughput.
 *
 * ## Simple retransmission scheme
 * LWMAC adopts a simple retransmission scheme to enhance link reliability. The data
 * packet will only be dropped in case the retransmission counter gets larger than
 * @ref CONFIG_GNRC_LWMAC_MAX_DATA_TX_RETRIES.
 *
 * ## Automatic phase backoff scheme
 * LWMAC adopts an automatic phase backoff scheme to reduce WR (preamble) collision
 * probability. In multi-hop scenarios, let's say, nodes A <---B <----C (which is
 * common in multi-hop data collection networks), in which B has packets for A, and
 * C has packets for B. In case A and B's wake-up phases are too close (overlapping).
 * Then, especially in high traffic conditions, B and C may initiate transmissions
 * at the same time (B sends to A, and C sends to B), a link of either will be
 * definitely interfered, leading to collisions and link throughput reduction. To
 * this end, by using the automatic phase backoff scheme, if a sender finds its
 * receiver's phase is too close to its own phase, it will run a backoff scheme to
 * randomly reselect a new wake-up phase for itself.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef NET_GNRC_LWMAC_LWMAC_H
#define NET_GNRC_LWMAC_LWMAC_H

#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_gnrc_lwmac_conf    GNRC LWMAC compile configurations
 * @ingroup  net_gnrc_conf
 * @{
 */
/**
 * @brief Time between consecutive wake-ups.
 *
 * This macro governs power consumption, latency and throughput!
 * In LWMAC, devices adopt duty-cycle scheme to conserve power. That is,
 * time is divided into repeated cycles (or, superframes), and in each
 * cycle, a node only wakes up for a period of time for receiving potential
 * incoming packets for itself. This macro defines the wake-up interval, or,
 * in other words, defines the cycle duration used in LWMAC. If the wake-up
 * interval is short, nodes will wake up more frequently, which also increases
 * the chances for receiving packets from neighbors (i.e., leads to higher
 * throughput), but also results in higher power consumption.
 * In LWMAC, by default, we regard the wake-up period as the beginning of a
 * cycle.
 */
#ifndef CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US
#define CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US        (200LU *US_PER_MS)
#endif

/**
 * @brief The Maximum WR (preamble packet @ref gnrc_lwmac_frame_wr_t) duration
 *        time.
 *
 * Since LWMAC adopts duty-cycle scheme, a node only wakes up for a short
 * period in each cycle. Thus, to probe where is the wake-up period of the
 * receiver, a sender sends WR (preamble) packets to notice the receiver for
 * communication. To ensure that the receiver will catch at least one WR
 * packet in one cycle, the sender repeatedly broadcasts a stream of WR packets
 * with the broadcast duration (preamble duration) slightly longer period than
 * @ref CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US.
 */
#ifndef GNRC_LWMAC_PREAMBLE_DURATION_US
#define GNRC_LWMAC_PREAMBLE_DURATION_US  ((13LU * CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US) / 10)
#endif

/**
 * @brief Timeout to send the next WR in case no WA has been received during
 *        that time.
 *
 * In LWMAC, when a sender initiates a transmission to a receiver, it starts
 * with sending a stream of repeated WR packets with
 * @ref CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US interval between two consecutive
 * WRs. After sending one WR (preamble) packet, the sender turns to the listen
 * mode to receive the potential incoming WA (preamble-ACK) packet with a
 * timeout of @ref CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US. If no WA is received
 * during @ref CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US, the sender starts sending
 * the next WR. It is referenced to the beginning of both WRs, but due to
 * internal overhead, the exact spacing is slightly higher. The minimum
 * possible value depends on the time it takes to completely send a WR with the
 * given hardware (including processor) and data rate.
 */
#ifndef CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US
#define CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US        (5U *US_PER_MS)
#endif

/**
 * @brief How long a node in LWMAC should keep awake and listen on the channel
 *        in one cycle.
 *
 * LWMAC adopts the duty-cycle scheme that a node only wakes up for a short
 * period of @ref GNRC_LWMAC_WAKEUP_DURATION_US in each cycle. In the rest of
 * the cycle, the node turns off the radio to conserve power.
 * @ref GNRC_LWMAC_WAKEUP_DURATION_US is set to twice the duration of
 * @ref CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US, to guarantee that the wake-up
 * period is long enough that receiver will not miss the WR (preamble) packet.
 * Receiver needs to support @ref NETDEV_EVENT_RX_STARTED event in order to use
 * time-between-WR as a sensible default here. Otherwise the duration of WRs as
 * well as longest possible data broadcasts need to be taken into account.
 */
#ifndef GNRC_LWMAC_WAKEUP_DURATION_US
#define GNRC_LWMAC_WAKEUP_DURATION_US (CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US * 2)
#endif

/**
 * @brief How long broadcast packets @ref gnrc_lwmac_frame_broadcast_t will be
 *        sent to make sure every participant has received at least one copy.
 *
 * Since LWMAC adopts duty-cycle scheme, a node only wakes up for a short
 * period in each cycle. Thus, when a node wants to broadcast a packet, it
 * repeatedly broadcasts the packet for one
 * @ref GNRC_LWMAC_BROADCAST_DURATION_US duration which is slightly longer
 * than @ref CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US. This is to ensure that all
 * neighbors will not miss the broadcast procedure of the sender and catch at
 * least one copy of the broadcast packet.
 */
#ifndef GNRC_LWMAC_BROADCAST_DURATION_US
#define GNRC_LWMAC_BROADCAST_DURATION_US  ((CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US * 11) / 10)
#endif

/**
 * @brief Time to idle between two successive broadcast packets, referenced to
 *        the start of the packet.
 *
 * The same limitation as for @ref CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US apply
 * here. In LWMAC, when a sender initiates a broadcast, it starts with sending
 * a stream ofrepeated broadcast packets with
 * @ref GNRC_LWMAC_TIME_BETWEEN_BROADCAST_US interval between two consecutive
 * broadcast packets. After sending one broadcast packet, the sender turns to
 * the listen mode with a timeout of
 * @ref GNRC_LWMAC_TIME_BETWEEN_BROADCAST_US. When this timeout expires, the
 * sender sends the next broadcast packet until reaching the maximum broadcast
 * duration of @ref GNRC_LWMAC_BROADCAST_DURATION_US.
 */
#ifndef GNRC_LWMAC_TIME_BETWEEN_BROADCAST_US
#define GNRC_LWMAC_TIME_BETWEEN_BROADCAST_US  (CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US)
#endif

/**
 * @brief WR preparation overhead before it can be sent (higher with debugging
 *        output).
 *
 * In LWMAC, when a sender wants to send a data packet to the receiver, it
 * starts sending the WR stream a little bit earlier (advance) to the beginning
 * edge of destination's wake-up phase over time. The idea is not to miss the
 * wake-up period of the receiver, otherwise will lead to a long WR procedure.
 */
#ifndef CONFIG_GNRC_LWMAC_WR_PREPARATION_US
#define CONFIG_GNRC_LWMAC_WR_PREPARATION_US         ((3U *US_PER_MS))
#endif

/**
 * @brief How long to wait after a WA for data to come in.
 *
 * When a node in LWMAC gets a WR during its wake-up period, it immediately
 * replies a WA packet to the sender for acknowledging the sender's transmission
 * request. After sending the WA, the receiver waits for the data packet from
 * the sender, with a timeout of @ref CONFIG_GNRC_LWMAC_DATA_DELAY_US duration.
 * In case no data will be received in this period, the receiver regards
 * reception failed and go back to normal listen mode. However, in case the
 * receiver receives other unintended packets, like WR/WA packets from other
 * neighbor communication pairs, the receiver resets this timeout and continues
 * to wait for the data packet, with the consideration that the sender's data
 * transmission might be delayed due to other ongoing transmissions (the data
 * packet is transmitted with CSMA/CA). This data timeout is long enough to
 * catch the beginning of the packet if the transceiver supports
 * @ref NETDEV_EVENT_RX_STARTED event (this can be important for big packets).
 */
#ifndef CONFIG_GNRC_LWMAC_DATA_DELAY_US
#define CONFIG_GNRC_LWMAC_DATA_DELAY_US             (10U *US_PER_MS)
#endif

/**
 * @brief CSMA retries for DATA packet after WR->WA was successful.
 *
 * After receiving the WA packet @ref gnrc_lwmac_frame_wa_t from the receiver,
 * the sender starts sending the data packet using CSMA/CA. This macro defines
 * how many CSMA retries a sender will be allowed to execute for sending its
 * data, before the data is successfully sent (gets data ACK from the receiver).
 */
#ifndef CONFIG_GNRC_LWMAC_DATA_CSMA_RETRIES
#define CONFIG_GNRC_LWMAC_DATA_CSMA_RETRIES         (3U)
#endif

/**
 * @brief Maximum TX transmission retries for DATA packet in case of no
 *        response from the receiver.
 *
 * When a data packet is scheduled for transmission, i.e., pushed into TX for
 * sending, LWMAC defines a maximum of
 * @ref CONFIG_GNRC_LWMAC_MAX_DATA_TX_RETRIES retries for transmission of the
 * packet. That is, in case of transmission failure in TX due to no WA from the
 * receiver, the sender will not drop the packet, but keeps it and retries to
 * send the data packet in the following cycles, until the sender reaches the
 * maximum retries limit defined here. Then, the packet will be dropped.
 */
#ifndef CONFIG_GNRC_LWMAC_MAX_DATA_TX_RETRIES
#define CONFIG_GNRC_LWMAC_MAX_DATA_TX_RETRIES       (3U)
#endif

/**
 * @brief MAX burst transmission packet number in one shot.
 *
 * LWMAC supports burst transmission based on the pending-bit technique, and
 * this macro here defines the largest number of packets allowed to be sent in
 * one consecutive sequence. In case a sender has multi packets for one
 * receiver,the burst transmission procedure is as follows:
 *
 * 1. The sender first uses WR stream to locate the receiver's wake-up period
 *    (if the sender has already phase-locked the receiver's phase, normally
 *    the sender only cost one WR to get the first WA from the receiver) and
 *    then sends its first data.
 * 2. After the transmission of the first data, the sender immediately sends a
 *    WR to the receiver for starting the second round of transmission of the
 *    second data. The receiver should also immediately reply WA for continue
 *    receiving data packets. In case the sender doesn't receive WA during
 *    @ref CONFIG_GNRC_LWMAC_TIME_BETWEEN_WR_US, it regards the consecutive
 *    (burst) transmission failed and quits TX procedure (the data will be
 *    queued back to the transmission queue for normal transmission attempt in
 *    following cycles).
 * 3. In case the second transmission succeeds, the sender repeats step (2) to
 *    send all the following pending packets.
 *
 * In short, in burst transmission mode, the sender doesn't tolerate no-WA
 * event. All the pending data packets should be sent with only one WR cost for
 * leading the transmission.
 */
#ifndef GNRC_LWMAC_MAX_TX_BURST_PKT_NUM
#define GNRC_LWMAC_MAX_TX_BURST_PKT_NUM \
        (CONFIG_GNRC_LWMAC_WAKEUP_INTERVAL_US / GNRC_LWMAC_WAKEUP_DURATION_US)
#endif

/**
 * @brief MAX bad Listen period extensions a node can tolerate.
 *
 * In LWMAC, to allow burst transmissions, when in the wake-up period and by
 * default, a node will extend its wake-up period to another
 * @ref GNRC_LWMAC_WAKEUP_DURATION_US after each packet reception (except for
 * broadcast packet). However, in some cases, a receiver may overhear other
 * unintended packets, e.g., WR or WA packets for other nodes, these are called
 * bad extensions for the receiver. If a receiver reaches the maximum bad listen
 * extension limit defined here, it goes to sleep mode with the consideration
 * that the channel is currently unavailable/busy.
 */
#ifndef CONFIG_GNRC_LWMAC_MAX_RX_EXTENSION_NUM
#define CONFIG_GNRC_LWMAC_MAX_RX_EXTENSION_NUM      (3U)
#endif

/**
 * @brief CSMA retries for broadcast packet.
 *
 * Currently, each broadcast packet is sent with CSMA/CA for collision
 * avoidance.
 *
 * @note Too many CSMA retries may lead to running out of destinations
 * wake-up period.
 */
#ifndef CONFIG_GNRC_LWMAC_BROADCAST_CSMA_RETRIES
#define CONFIG_GNRC_LWMAC_BROADCAST_CSMA_RETRIES    (3U)
#endif
/** @} */

/**
* @brief Maximum preamble attempts before re-initialize radio in LWMAC.
*
* After a long period of run time, a radio may be in wrong condition which needs to be
* re-calibrated. This is indicated by having a series of continuous preamble failure (no WA)
* in LWMAC. In case we have @ref CONFIG_GNRC_LWMAC_RADIO_REINIT_THRESHOLD number of preamble
* failure, then we re-initialize the radio, trying to re-calibrate the radio for bringing it
* back to normal condition.
*/
#ifndef CONFIG_GNRC_LWMAC_RADIO_REINIT_THRESHOLD
#define CONFIG_GNRC_LWMAC_RADIO_REINIT_THRESHOLD     (10U)
#endif

/**
 * @brief   Creates an IEEE 802.15.4 LWMAC network interface
 *
 * @param[out] netif    The interface. May not be `NULL`.
 * @param[in] stack     The stack for the LWMAC network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the LWMAC network interface's thread.
 * @param[in] name      Name for the LWMAC network interface. May be NULL.
 * @param[in] dev       Device for the interface
 *
 * @see @ref gnrc_netif_create()
 *
 * @return  0 on success
 * @return  negative number on error
 */
int gnrc_netif_lwmac_create(gnrc_netif_t *netif, char *stack, int stacksize,
                            char priority, const char *name, netdev_t *dev);
#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_LWMAC_LWMAC_H */
/** @} */
