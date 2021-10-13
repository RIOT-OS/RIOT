/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef CPU_PM_H
#define CPU_PM_H

#ifdef __cplusplus
extern "C" {
#endif

void pm_periph_enable(pwr_reduction_t pwr);
void pm_periph_disable(pwr_reduction_t pwr);
void pm_periph_power_off(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_PM_H */
/** @} */
