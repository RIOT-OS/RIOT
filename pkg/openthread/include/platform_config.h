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
#if OPENTHREAD_ENABLE_NCP_UART
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS       (30U)
#elif OPENTHREAD_MTD
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS       (20U)
#elif OPENTHREAD_FTD
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS       (10U)
#endif

/**
  * @def OPENTHREAD_CONFIG_LEGACY_TRANSMIT_DONE
  *
  * Define to 1 if you want use legacy transmit done.
  *
  */
#define OPENTHREAD_CONFIG_LEGACY_TRANSMIT_DONE      (1U)

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_CONFIG_H */
