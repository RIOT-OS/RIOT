/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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
#define CPU_INUM_RMT             1  /**< Level interrupt with low priority 1 */
#define CPU_INUM_GPIO            2  /**< Level interrupt with low priority 1 */
#define CPU_INUM_CAN             3  /**< Level interrupt with low priority 1 */
#define CPU_INUM_UART            4  /**< Level interrupt with low priority 1 */
#define CPU_INUM_USB             8  /**< Level interrupt with low priority 1 */
#define CPU_INUM_RTT             9  /**< Level interrupt with low priority 1 */
#define CPU_INUM_SERIAL_JTAG    10  /**< Level interrupt with low priority 1 */
#define CPU_INUM_I2C            12  /**< Level interrupt with low priority 1 */
#define CPU_INUM_WDT            13  /**< Level interrupt with low priority 1 */
#define CPU_INUM_SOFTWARE       17  /**< Level interrupt with low priority 1 */
#define CPU_INUM_ETH            18  /**< Level interrupt with low priority 1 */
#define CPU_INUM_LCD            18  /**< Level interrupt with low priority 1 */
#define CPU_INUM_TIMER          19  /**< Level interrupt with medium priority 2 */
#define CPU_INUM_FRC2           20  /**< Level interrupt with medium priority 2 */
#define CPU_INUM_SYSTIMER       20  /**< Level interrupt with medium priority 2 */
#define CPU_INUM_BLE            21  /**< Level interrupt with medium priority 2 */
#define CPU_INUM_SDMMC          23  /**< Level interrupt with medium priority 2 */
#define CPU_INUM_CACHEERR       25  /**< Level interrupt with high priority 4   */
/** @} */

/**
 * @brief   Initialize architecture specific interrupt handling
 */
void esp_irq_init(void);

#ifdef __cplusplus
}
#endif
