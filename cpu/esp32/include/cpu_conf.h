/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_esp32_conf ESP32 compile configurations
 * @ingroup     cpu_esp32
 * @ingroup     config
 * @brief       Compile-time configuration macros for ESP32 modules
 * @{
 *
 * @file
 * @brief       CPU specific configuration options
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include <stdint.h>
#include "esp_common_log.h"
#include "xtensa_conf.h"
#include "xtensa/xtensa_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Declare the heap_stats function as available
 */
#define HAVE_HEAP_STATS

/**
 * @name   Stack size configuration
 * @{
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF (1024)
#define THREAD_STACKSIZE_DEFAULT      (2048)
#define THREAD_STACKSIZE_IDLE         (2048)
/** @} */

/**
 * Buffer size used for printf functions (maximum length of formatted output).
 */
#define PRINTF_BUFSIZ 256

#ifdef __cplusplus
}
#endif /* CPU_CONF_H */

#endif /* CPU_CONF_H */
/** @} */
