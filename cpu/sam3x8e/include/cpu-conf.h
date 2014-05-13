/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_sam3x8e Atmel SAM3X8E
 * @ingroup         cpu
 * @brief           Implementation of Atmel's SAM3X8E Cortex-M3 MCU
 * @{
 *
 * @file            cpu-conf.h
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "sam3x8e.h"


/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust for the cortex-m3
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (2500)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (2500)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (512)
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
