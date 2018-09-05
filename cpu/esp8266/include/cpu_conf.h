/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
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
 * @brief   Stack size configuration
 * @{
 */
#ifdef MODULE_ESP_SDK_INT_HANDLING
#define THREAD_EXTRA_STACKSIZE_PRINTF (0)
#define THREAD_STACKSIZE_DEFAULT      (2048)
#define THREAD_STACKSIZE_IDLE         (2048)
#else
#define THREAD_EXTRA_STACKSIZE_PRINTF (0)
#define THREAD_STACKSIZE_DEFAULT      (2048)
#define THREAD_STACKSIZE_IDLE         (2048)
#endif
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
#endif /* CPU_CONF_H */

#endif /* CPU_CONF_H */
/** @} */
