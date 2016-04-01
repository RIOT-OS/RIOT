/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lwmac Simplest possible MAC layer
 * @ingroup     net
 * @brief       Lightweight MAC protocol that allows for duty cycling to save
 *              energy.
 * @{
 *
 * @file
 * @brief       Interface definition for the LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 */

#ifndef GNRC_LWMAC_H
#define GNRC_LWMAC_H

#include <kernel_types.h>
#include <net/gnrc/netdev.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Time between consecutive wakeups. This parameter governs power consumption,
 * latency and throughput! */
#ifndef LWMAC_WAKEUP_INTERVAL_US
#define LWMAC_WAKEUP_INTERVAL_US        (100U * 1000)
#endif

/* Timeout to send the next WR in case no WA has been received during that
 * time. It is referenced to the beginning of both WRs, but due to internal
 * overhead, the exact spacing is slightly higher.
 * The minimum possible value depends on the time it takes to completely
 * send a WR with the given hardware (including processor) and data rate. */
#ifndef LWMAC_TIME_BETWEEN_WR_US
#define LWMAC_TIME_BETWEEN_WR_US        (7000U)
#endif

/* Time to idle between two successive BROADCAST packets, referenced to the
 * start of the packet. The same limitation as for time-between-WR apply here. */
#ifndef LWMAC_TIME_BETWEEN_BROADCAST_US
#define LWMAC_TIME_BETWEEN_BROADCAST_US (LWMAC_TIME_BETWEEN_WR_US)
#endif

/* Receiver needs to support RX_START event in order to use time-between-WR
 * as a sensible default here. Otherwise the duration of WRs as well as longest
 * possible data broadcasts need to be taken into account. */
#ifndef LWMAC_WAKEUP_DURATION_US
#define LWMAC_WAKEUP_DURATION_US        (LWMAC_TIME_BETWEEN_WR_US * 2)
#endif

/* Start sending earlier then known phase. Therefore advance to beginning edge
 * of destinations wakeup phase over time.
 * Note: * RTT tick is ~30us @ 32 kHz timer
 *       * there is a certain overhead from dispatching driver call until WR
 *         will be really sent that may depend on hardware and driver
 *         implementation
 */
#ifndef LWMAC_WR_BEFORE_PHASE_US
#define LWMAC_WR_BEFORE_PHASE_US        (500U)
#endif

/* WR preparation overhead before it can be sent (higher with debugging output).
 * LwMAC will wakeup earlier to prepare for synced WR sending. When preparation
 * is done, it will busy wait (block the whole system) until the WR has been
 * dispatched to the driver. */
#ifndef LWMAC_WR_PREPARATION_US
#define LWMAC_WR_PREPARATION_US         (7000U + LWMAC_WR_BEFORE_PHASE_US)
#endif

/* How long to wait after a WA for data to come in. It's enough to catch the
 * beginning of the packet if the transceiver supports RX_STARTED event (this
 * can be important for big packets). */
#ifndef LWMAC_DATA_DELAY_US
#define LWMAC_DATA_DELAY_US             (5000U)
#endif

/* How long BROADCAST packets will be sent to make sure every participant has
 * received at least one packet. */
#ifndef LWMAC_BROADCAST_DURATION_US
#define LWMAC_BROADCAST_DURATION_US     ((LWMAC_WAKEUP_INTERVAL_US * 1100) / 1000)
#endif

/* CSMA retries for DATA packet after WR->WA was successful. Too many retries
 * may timeout the receiver, refer LWMAC_DATA_DELAY_US */
#ifndef LWMAC_DATA_CSMA_RETRIES
#define LWMAC_DATA_CSMA_RETRIES         (3U)
#endif

/* CSMA retries for BROADCAST packet, too many may lead to running out of
 * destinations wakup period. */
#ifndef LWMAC_BROADCAST_CSMA_RETRIES
#define LWMAC_BROADCAST_CSMA_RETRIES    (3U)
#endif

/* Store incoming BROADCAST packets until unicast transaction has finished.
 * This buffer will also store the received DATA packets at the same time. */
#ifndef LWMAC_DISPATCH_BUFFER_SIZE
#define LWMAC_DISPATCH_BUFFER_SIZE      (4U)
#endif


/**
 * @brief   Set the default message queue size for LWMAC layer
 */
#ifndef LWMAC_IPC_MSG_QUEUE_SIZE
#define LWMAC_IPC_MSG_QUEUE_SIZE        (8U)
#endif

/**
 * @brief   Count of parallel timeouts. Shouldn't needed to be changed.
 */
#ifndef LWMAC_TIMEOUT_COUNT
#define LWMAC_TIMEOUT_COUNT             (3U)
#endif

/**
 * @brief   Count of nodes in one-hop distance whose wakeup phase is tracked
 */
#ifndef LWMAC_NEIGHBOUR_COUNT
#define LWMAC_NEIGHBOUR_COUNT           (8U)
#endif

/**
 * @brief   Set the default queue size for packets coming from higher layers
 */
#ifndef LWMAC_TX_QUEUE_SIZE
#define LWMAC_TX_QUEUE_SIZE             (8U)
#endif

/**
 * @brief   Set the default queue size for incoming packets
 */
#ifndef LWMAC_RX_QUEUE_SIZE
#define LWMAC_RX_QUEUE_SIZE             (8U)
#endif

/* Max link layer address length in bytes */
#ifndef LWMAC_MAX_L2_ADDR_LEN
#define LWMAC_MAX_L2_ADDR_LEN           (2U)
#endif


#define LWMAC_LPM_MASK					(1 << 17)

/**
 * @brief   Initialize an instance of the LWMAC layer
 *
 * The initialization starts a new thread that connects to the given netdev
 * device and starts a link layer event loop.
 *
 * @param[in] stack         stack for the control thread
 * @param[in] stacksize     size of *stack*
 * @param[in] priority      priority for the thread housing the LWMAC instance
 * @param[in] name          name of the thread housing the LWMAC instance
 * @param[in] dev           netdev device, needs to be already initialized
 *
 * @return                  PID of LWMAC thread on success
 * @return                  -EINVAL if creation of thread fails
 * @return                  -ENODEV if *dev* is invalid
 */
kernel_pid_t gnrc_lwmac_init(char *stack, int stacksize, char priority,
                           const char *name, gnrc_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_LWMAC_H */
/** @} */
