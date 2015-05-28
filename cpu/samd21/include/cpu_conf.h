/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "samd21.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust for the cortex-m0
 * @{
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF   (512)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (1024)
#endif

#define THREAD_STACKSIZE_IDLE           (256)
/** @} */

/**
 * @name UART0 buffer size definition for compatibility reasons
 *
 * TODO: remove once the remodeling of the uart0 driver is done
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif
/** @} */

/**
 * @brief Buffer size to use by the transceiver
 */
#define TRANSCEIVER_BUFFER_SIZE         (3)

/**
 * @brief CPUID_ID_LEN length of cpuid in bytes
 */
#define CPUID_ID_LEN                    (16) /* 128 bits long, 16 bytes long */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
