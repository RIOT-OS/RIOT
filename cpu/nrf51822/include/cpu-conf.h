/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_nrf51822
 * @{
 *
 * @file            cpu-conf.h
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "nrf51.h"
#include "nrf51_bitfields.h"


/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust for the cortex-m0
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (1024)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (1024)
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
