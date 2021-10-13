/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#ifndef IRQ_ARCH_H
#define IRQ_ARCH_H

#include "irq_arch_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   CPU interrupt numbers
 *
 * All interrupts that are used for RIOT-OS are preallocated and fix.
 * The allocated interrupts are all level interrupts, most of them with
 * low priority.
 *
 * @{
 */
#define CPU_INUM_GPIO       2   /**< Level interrupt with low priority 1 */
#define CPU_INUM_CAN        3   /**< Level interrupt with low priority 1 */
#define CPU_INUM_UART       5   /**< Level interrupt with low priority 1 */
#define CPU_INUM_RTC        9   /**< Level interrupt with low priority 1 */
#define CPU_INUM_I2C        12  /**< Level interrupt with low priority 1 */
#define CPU_INUM_WDT        13  /**< Level interrupt with low priority 1 */
#define CPU_INUM_SOFTWARE   17  /**< Level interrupt with low priority 1 */
#define CPU_INUM_ETH        18  /**< Level interrupt with low priority 1 */
#define CPU_INUM_TIMER      19  /**< Level interrupt with medium priority 2 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_H */
