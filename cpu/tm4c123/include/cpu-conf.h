/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_tm4c123 TM4C123
 * @ingroup         cpu
 * @brief           CPU specific implementations for the TM4C123
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "tm4c123gh6pm.h"

/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust for the Cortex-M4f
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF_FLOAT  (4096)
#define KERNEL_CONF_STACKSIZE_PRINTF        (2048)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (256)
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
#define UART0_BUFSIZE                   (128)
#endif
/** @} */

#endif /* __CPU_CONF_H */
/** @} */
