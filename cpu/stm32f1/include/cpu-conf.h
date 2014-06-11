/*
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f1 STM32F1
 * @addtogroup      cpu
 * @brief           CPU specific implementations for the STM32F1
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Alaeddine Weslati <alaeddine.weslati@intia.fr>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CPUCONF_H_
#define CPUCONF_H_

#include "stm32f10x.h"

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

/**
 * @name Macro for reading CPU_ID
 */
#define GET_CPU_ID(id)          memcpy(&id, (void *)(0x1ffff7e8), CPU_ID_LEN)

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

void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale);

#endif /* __CPU_CONF_H */
/** @} */
