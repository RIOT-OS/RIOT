/*
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_atxmega
 * @{
 *
 * @file
 * @brief           Power Management and Power Reduction API
 *
 * This help to save power disabling all non used peripherals.  It can help to
 * save power when in active or sleep modes.  For any other low power modes
 * xmega will freeze all peripherals clock.
 *
 * @author          Gerson Fernando Budke <nandojve@gmail.com>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

void pm_periph_enable(pwr_reduction_t pwr);
void pm_periph_disable(pwr_reduction_t pwr);
void pm_periph_power_off(void);

#ifdef __cplusplus
}
#endif

/** @} */
