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


/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust for the cortex-m0
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (256/2)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (2048) //1024
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (256)
/** @} */

/**
 * @name UART0 buffer size definition for compatibility reasons
 *
 * TODO: remove once the remodeling of the uart0 driver is done
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (256)
#endif
/** @} */

#define TRANSCEIVER_BUFFER_SIZE (5)


/**
 * @name Definition of different panic modes
 */
typedef enum {
    HARD_FAULT,
    WATCHDOG,
    BUS_FAULT,
    USAGE_FAULT,
    DUMMY_HANDLER
} panic_t;

/**
 * @def CPUID_ID_LEN
 *
 * @brief   The length in byte of the CPU's serial number.
 *
 * @note    Must be defined in the CPU's @ref cpu-conf.h
 */

#define CPUID_ID_LEN (128/8) //128 bits long, 16 bytes long

#endif /* __CPU_CONF_H */
/** @} */
