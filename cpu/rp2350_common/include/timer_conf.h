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
    TIMER0_Type *dev;   /**< Base address of the I/O registers of the device */
    IRQn_Type irqn_base; /**< IRQ number of alarm 0 */
    uint8_t ch_numof;   /**< Number of alarm channels to use */
} timer_conf_t;

#ifdef __cplusplus
}
#endif

/** @} */
