/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_efm32
 * @brief           EFM32 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *                  Based on STM32F0 example
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "em_device.h"

/**
 * @name Kernel configuration
 *
 * The absolute minimum stack size is 140 byte (68 byte for the tcb + 72 byte
 * for a complete context save).
 *
 * TODO: set this to change per device
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (512)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (512)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (192)
/** @} */

#endif /* __CPU_CONF_H */
/** @} */
