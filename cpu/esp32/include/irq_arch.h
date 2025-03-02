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

/* While interrupt 0 is reserved for WiFi on Xtensa-based ESP32x SoCs and
 * interrupt 1 is available, interrupt 1 is reserved for WiFi on RISC-V-based
 * ESP32x SoCs and interrupt 0 is available. */
#if defined(__riscv)
#define CPU_INUM_RMT             0  /**< Level interrupt with low priority 1 */
#else
#define CPU_INUM_RMT             1  /**< Level interrupt with low priority 1 */
#endif
#define CPU_INUM_GPIO            2  /**< Level interrupt with low priority 1 */
#define CPU_INUM_BLE             5  /**< Level interrupt with low priority 1 */
#define CPU_INUM_RTT             9  /**< Level interrupt with low priority 1 */
#define CPU_INUM_SERIAL_JTAG    10  /**< Edge  interrupt with low priority 1 */
#define CPU_INUM_I2C            12  /**< Level interrupt with low priority 1 */
#define CPU_INUM_UART           13  /**< Level interrupt with low priority 1 */
#define CPU_INUM_CAN            17  /**< Level interrupt with low priority 1 */
#define CPU_INUM_ETH            18  /**< Level interrupt with low priority 1 */
#define CPU_INUM_USB            18  /**< Level interrupt with low priority 1 */
#define CPU_INUM_FRC2           20  /**< Level interrupt with medium priority 2 */
#define CPU_INUM_SYSTIMER       20  /**< Level interrupt with medium priority 2 */
#define CPU_INUM_SDMMC          21  /**< Level interrupt with medium priority 2 */
#define CPU_INUM_TIMER          22  /**< Edge  interrupt with medium priority 2 */
#define CPU_INUM_WDT            23  /**< Level interrupt with medium priority 3 */
#define CPU_INUM_SOFTWARE       29  /**< Software interrupt with medium priority 3 */
/** @} */

/**
 * @brief   Initialize architecture specific interrupt handling
 */
void esp_irq_init(void);

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_H */
