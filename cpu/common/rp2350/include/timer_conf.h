/*
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Timer configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "RP2350.h"
#include "periph_cpu.h"
#include "regs/ticks.h"
#include "regs/resets.h"

/** Number of alarm channels per timer block */
#define TIMER_CHANNEL_NUMOF 4u

#define TIMER_0_ISRA    isr_timer0_0
#define TIMER_0_ISRB    isr_timer0_1
#define TIMER_0_ISRC    isr_timer0_2
#define TIMER_0_ISRD    isr_timer0_3

#define TIMER_1_ISRA    isr_timer1_0
#define TIMER_1_ISRB    isr_timer1_1
#define TIMER_1_ISRC    isr_timer1_2
#define TIMER_1_ISRD    isr_timer1_3

/**
 * @brief   A low-level timer_set() implementation is provided
 */
#define PERIPH_TIMER_PROVIDES_SET

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configuration details for a timer device
 */
typedef struct {
    TIMER0_Type *dev;       /**< Base address of the I/O registers of the device */
    IRQn_Type irqn_base;    /**< IRQ number of alarm 0 */
    uint8_t ch_numof;       /**< Number of alarm channels to use */
} timer_conf_t;

#ifdef __cplusplus
}
#endif

/** @} */
