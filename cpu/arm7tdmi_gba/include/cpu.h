/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_arm7tdmi_gba
 * @{
 */
#include "arm_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The interrupt service routine for the timer interrupts
 */
void gba_timer_isr(uint16_t interrupt_flags_timer);

/**
 * @brief   The interrupt service routine for the uart interrupts
 */
void gba_uart_isr(void);

#ifdef __cplusplus
}
#endif

/** @} */
