/*
 * SPDX-FileCopyrightText: 2017-2021 Baptiste Clenet
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
 * @def OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS
 *
 * The number of message buffers in buffer pool
 */
#if OPENTHREAD_MTD
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS       (20U)
#else
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS       (10U)
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL
 *
 * Set OpenThread log level
 *
 * @see https://openthread.io/releases/thread-reference-20170716/group/plat-logging
 */
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_NONE

#ifdef __cplusplus
}
#endif
