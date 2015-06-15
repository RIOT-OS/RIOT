/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_csma_mac basic CSMA MAC layer
 * @ingroup     net
 * @brief       Basic CSMA MAC protocol that sends in unslotted CSMA Mode
 * @{
 *
 * @file
 * @brief       Interface definition for the CSMA MAC layer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 */

#ifndef NG_CSMA_MAC_H_
#define NG_CSMA_MAC_H_

#include "kernel.h"
#include "net/ng_netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MAC sublayer constants, referring to IEEE 802.15.4-2011 Table 51, 52 */

/**
 * @brief    The maximum value of the backoff exponent BE.
 * @details  The IEEE 802.15.4 Standard allows values from 3-8.
 */
#define CSMA_MAC_MAX_BE                  5

/**
 * @brief    The minimum value of the backoff exponent BE.
 * @details  The IEEE 802.15.4 Standard allows values from 0-CSMA_MAC_MAX_BE.
 */
#define CSMA_MAC_MIN_BE                  3

/**
 * @brief    The maximum number of retries allowed after a transmission failure.
 * @details  The IEEE 802.15.4 Standard allows values from 0-7.
 */
#define CSMA_MAC_MAX_FRAME_RETRIES       3

/**
 * @brief   The physical timer that is used for the MAC-layer, (periph/timer - IF).
 */
#define CSMA_MAC_TIMER               TIMER_0

/**
 * @brief   The physical timer that is used for the MAC-layer, (periph/timer - IF).
 */
#define CSMA_MAC_TIMER_CH              0

/**
 * @brief   Set the default message queue size for CSMA_MAC layers
 */
#ifndef CSMA_MAC_MSG_QUEUE_SIZE
#define CSMA_MAC_MSG_QUEUE_SIZE         (8U)
#endif

/******************************************************************************
 * TODO: The timer definition will move to the board.h file. For testing purposes
 * it is temporarily defined at this place.
 */
/**
 * @brief   The physical timer that is used for the MAC-layer, (periph/timer - IF).
 */
#define CSMA_MAC_TIMER               TIMER_0

/**
 * @brief   The physical timer that is used for the MAC-layer, (periph/timer - IF).
 */
#define CSMA_MAC_TIMER_CH              0

/******************************************************************************
 * TODO: The following defines are Phy dependent. We have to decide where to put them.
 * They are directly related to the netdev dev, so what possibilities do we have
 * to link them to the network dev? Do it in a similar way like the linux kernel.
 * and store them in a device stuct.
 */

/**
 * @brief   Length of a single simbol in us. (PHY dependant)
 * @details Is calculated as follows:
 */
#define CSMA_MAC_SYMBOL_LENGTH           16
#define BIT_PER_SYMBOL                   4
#define N_SFD_DURATION                   (8 / BIT_PER_SYMBOL)
#define N_PREAMBLE_DURATION              (32 / BIT_PER_SYMBOL)
#define SHR_DURATION                     (N_PREAMBLE_DURATION + N_SFD_DURATION) * CSMA_MAC_SYMBOL_LENGTH
#define TURNAROUND_TIME                  12 * CSMA_MAC_SYMBOL_LENGTH

/**
 * @brief   The number of symbols forming the basic time period
 *          used by the CSMA-CA algorithm.
 */
#define CSMA_MAC_UNIT_BACKOFF_PERIOD     20 * CSMA_MAC_SYMBOL_LENGTH

/**
 * @brief    Max time to wait for ack [us].
 * @details  The maximum amount of time to
 *           wait for an acknowledgment frame to arrive after TX process is done.
 *           This value is dependent on the supported PHY.
 */
#define CSMA_MAC_MAX_ACK_WAIT_DURATION   CSMA_MAC_UNIT_BACKOFF_PERIOD\
                        + TURNAROUND_TIME + SHR_DURATION + 6*(8/BIT_PER_SYMBOL) * CSMA_MAC_SYMBOL_LENGTH

/**
 * @brief   Device specific option: Automatic CCA before TX supported
 */
#define CSMA_MAC_DEV_SUPPORTS_AUTOCCA  (1 << 0)

/**
 * @brief   Device specific option: Automatic send of ACK frame after receive
 */
#define CSMA_MAC_DEV_SUPPORTS_AUTOACK  (1 << 1)

/**
 * @brief   Device specific option: PRELOADING
 */
#define CSMA_MAC_DEV_SUPPORTS_PRELOADING  (1 << 1)

/**
 * @brief   Size of the buffer that stores pointer to
 *          queued NG_NETAPI_MSG_TYPE_SND-Messages
 */
#define CSMA_MAC_BUFSIZE 100

/******************************************************************************/


/**
 * @brief   Initialize an instance of the CSMA_MAC layer
 *
 * The initialization starts a new thread that connects to the given netdev
 * device and starts a link layer event loop.
 *
 * @param[in] stack         stack for the control thread
 * @param[in] stacksize     size of *stack*
 * @param[in] priority      priority for the thread housing the CSMA_MAC instance
 * @param[in] name          name of the thread housing the CSMA_MAC instance
 * @param[in] dev           netdev device, needs to be already initialized
 *
 * @return                  PID of CSMA_MAC thread on success
 * @return                  -EINVAL if creation of thread fails
 * @return                  -ENODEV if *dev* is invalid
 */
kernel_pid_t csma_mac_init(char *stack, int stacksize, char priority,
                        const char *name, ng_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __CSMA_MAC_H_ */
/** @} */
