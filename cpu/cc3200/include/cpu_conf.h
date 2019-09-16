/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc3200
 * @ingroup         cpu
 * @brief           CPU specific implementations for CC3200
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */
#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cc3200.h"
#include "cc3200_gpio.h"
#include "cc3200_prcm.h"
#include "cc3200_spi.h"
#include "cc3200_timer.h"
#include "cc3200_uart.h"
#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief reset a peripheral clock
 *
 * @param reg arcm register pointer
 * @{
 */
extern void reset_periph_clk(cc3200_arcm_reg_t *reg);
/** @} */

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */

/**
 * @brief Default CPU IRQ priority: The default priority is 1 for every
 * interrupt, 0 is the highest possible priority.
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO (1U)
/** @} */

/**
 * @brief Total number of cpu interrupts
 *
 */
#define CPU_IRQ_NUMOF IRQN_COUNT
#define CPU_FLASH_BASE FLASH_BASE /**< number of interrupt sources */
/** @} */

/**
 * @brief interrupt priority levels
 * @{
 */
#define INT_PRIORITY_LVL_0 0x00
#define INT_PRIORITY_LVL_1 0x20
#define INT_PRIORITY_LVL_2 0x40
#define INT_PRIORITY_LVL_3 0x60
#define INT_PRIORITY_LVL_4 0x80
#define INT_PRIORITY_LVL_5 0xA0
#define INT_PRIORITY_LVL_6 0xC0
#define INT_PRIORITY_LVL_7 0xE0
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
