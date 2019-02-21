/*
 * Copyright (C) 2018 Gunar Schorcht
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

#include "xtensa_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Stack size configuration
 * @{
 */
#ifdef MODULE_ESP_SDK_INT_HANDLING

#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF (0)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT      (1536)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE         (1536)
#endif
#ifndef THREAD_STACKSIZE_MAIN
#define THREAD_STACKSIZE_MAIN         (3072)
#endif

#ifndef GNRC_IPV6_STACK_SIZE
#define GNRC_IPV6_STACK_SIZE          (1536)
#endif
#ifndef GNRC_PKTDUMP_STACKSIZE
#define GNRC_PKTDUMP_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
#endif

#ifndef ESP_NOW_STACKSIZE
#define ESP_NOW_STACKSIZE             (2560)
#endif

#ifndef ETS_THREAD_STACKSIZE
#define ETS_THREAD_STACKSIZE          (2048)
#endif

#else /* MODULE_ESP_SDK_INT_HANDLING */

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
#define GNRC_PKTDUMP_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
#endif

#ifndef ESP_NOW_STACKSIZE
#define ESP_NOW_STACKSIZE             (2560)
#endif

#ifndef ETS_THREAD_STACKSIZE
#define ETS_THREAD_STACKSIZE          (1536)
#endif

#endif /* MODULE_ESP_SDK_INT_HANDLING */
/** @} */

/**
 * Buffer size used for printf functions (maximum length of formatted output).
 */
#define PRINTF_BUFSIZ 256

/* Following include is neccessary to overwrite newlib's PRI*8 and PRI*32. */
/* PLEASE NOTE: ets_vprintf does not understand %i %li, or %hi */
#ifndef DOXYGEN
#include <inttypes.h>
#undef  __INT8
#define __INT8
#undef  __INT32
#define __INT32
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
