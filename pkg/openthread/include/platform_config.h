/*
 * Copyright (C) Baptiste Clenet
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
