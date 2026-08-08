/*
 * SPDX-FileCopyrightText: Baptiste Clenet
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread platform config
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_ACK_TIMEOUT_ENABLE
 *
 * Enables software ack time out in OpenThread MAC layer
 * Only enable if board doesn't support hardware acks
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_ACK_TIMEOUT_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
 *
 * Enable software csma in OpenThread MAC layer
 * Only enable if board doesn't support hardware csma
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
 *
 * Enable software retransmission logic in Openthread MAC layer
 * Only enable if board doesnt support hardware retransmissions
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
 *
 * If set to 1 Flash API is used else Settings API
 * Only one of them has to be implemented as non-volatile storage requirement
 * currently only Settings API is supported by RIOT
 *
 * @see https://openthread.io/guides/porting/implement-platform-abstraction-layer-apis
 */
#define OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE 0

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
 *
 * If set to 1 allows usage of usec. Currently has to be set to 1.
 */
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_TCP_ENABLE
 *
 * If set to 1 enables TCP in OpenThread. Currently not supported
 */
#define OPENTHREAD_CONFIG_TCP_ENABLE 0

/**
 * @def OPENTHREAD_CONFIG_THREAD_VERSION
 *
 * Set the version of the thread protocol specification.
 */
#define OPENTHREAD_CONFIG_THREAD_VERSION OT_THREAD_VERSION_1_4

#ifdef __cplusplus
}
#endif
