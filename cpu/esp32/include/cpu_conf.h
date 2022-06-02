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

#include "cpu_conf_common.h"
#include "esp_common_log.h"
#include "xtensa_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Stack size configuration
 * @{
 */
/** Extra thread stack size required if newlib-nano is not used */
#ifdef CONFIG_NEWLIB_NANO_FORMAT
#define THREAD_EXTRA_STACKSIZE          (0)
#else
#define THREAD_EXTRA_STACKSIZE          (512)
#endif

/** Extra thread stack size if `printf` is used */
#define THREAD_EXTRA_STACKSIZE_PRINTF   (1536)

#ifndef THREAD_STACKSIZE_DEFAULT
/** Default thread stack size */
#define THREAD_STACKSIZE_DEFAULT        (2048)
#endif

#ifndef THREAD_STACKSIZE_IDLE
/** Stack size for the idle thread */
#define THREAD_STACKSIZE_IDLE           (2048)
#endif

#ifndef ESP_WIFI_STACKSIZE
/** Stack size for the WiFi thread */
#define ESP_WIFI_STACKSIZE              (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE)
#endif

/** @} */

/**
 * Buffer size used for printf functions (maximum length of formatted output).
 */
#define PRINTF_BUFSIZ 256

/**
 * @brief   Remaining parts of the various DRAM sections can be used as heap.
 */
#define NUM_HEAPS (4)

/**
 * @brief   Attribute for memory sections required by SRAM PUF
 */
#define PUF_SRAM_ATTRIBUTES __attribute__((used, section(".noinit")))

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
