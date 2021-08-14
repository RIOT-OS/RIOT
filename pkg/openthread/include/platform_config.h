/*
 * Copyright (C) Baptiste Clenet
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread platform config
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @}
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

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

#endif /* PLATFORM_CONFIG_H */
