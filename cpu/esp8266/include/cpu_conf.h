/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_esp8266_conf ESP8266 compile configurations
 * @ingroup     cpu_esp8266
 * @ingroup     config
 * @brief       Compile-time configuration macros for ESP8266 modules
 * @{
 *
 * @file
 * @brief       CPU specific configuration options
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"
#include "xtensa_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @name    Clock configuration
* @{
*/

#ifndef DOXYGEN
/* Mapping of Kconfig defines to the respective enumeration values */
#if CONFIG_ESP8266_CPU_FREQUENCY_80
#define ESP8266_CPU_FREQUENCY   80
#elif CONFIG_ESP8266_CPU_FREQUENCY_160
#define ESP8266_CPU_FREQUENCY   160
#endif
#endif

/**
 * @brief   Defines the CPU frequency in MHz
 *
 * Possible values are 80 and 160 MHz.
 */
#ifndef ESP8266_CPU_FREQUENCY
#define ESP8266_CPU_FREQUENCY   (80)
#endif

/**
 * @brief   Mapping configured ESP8266 default clock to CLOCK_CORECLOCK define
 */
#define CLOCK_CORECLOCK         (1000000UL * ESP8266_CPU_FREQUENCY)
/** @} */

/**
 * @name   Stack size configurations
 * @{
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF (0)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT      (1024)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE         (1024)
#endif
#ifndef THREAD_STACKSIZE_MAIN
#define THREAD_STACKSIZE_MAIN         (3072)
#endif

#ifndef GNRC_IPV6_STACK_SIZE
#define GNRC_IPV6_STACK_SIZE          (1536)
#endif
#ifndef GNRC_PKTDUMP_STACKSIZE
#define GNRC_PKTDUMP_STACKSIZE        (THREAD_STACKSIZE_DEFAULT << 1)
#endif

#ifndef ESP_NOW_STACKSIZE
#define ESP_NOW_STACKSIZE             (2560)
#endif

#ifndef TCPIP_THREAD_STACKSIZE
#define TCPIP_THREAD_STACKSIZE        (3072)
#endif
/** @} */

/**
 * Buffer size used for printf functions (maximum length of formatted output).
 */
#define PRINTF_BUFSIZ 256

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
