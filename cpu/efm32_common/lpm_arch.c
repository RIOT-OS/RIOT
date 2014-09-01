/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ryan Kurte <ryankurte@gmail.com>
 *
 * @}
 */

#include "arch/lpm_arch.h"


void lpm_arch_init(void)
{
    /* TODO */
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    /* TODO */
    switch (target) {
    case LPM_ON:             /**< MCU is active */

        break;
    case LPM_IDLE:           /**< MCU is idle */

        break;
    case LPM_SLEEP:          /**< MCU in sleep mode */

        break;
    case LPM_POWERDOWN:      /**< MCU is powered down */

        break;
    case LPM_OFF:            /**< MCU is off */

        break;
    case LPM_UNKNOWN:         /**< status unknown/unavailable */

        break;
    }

    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    /* TODO */
    return 0;
}

void lpm_arch_awake(void)
{
    /* TODO*/
}

void lpm_arch_begin_awake(void)
{
    /* TODO */
}

void lpm_arch_end_awake(void)
{
    /* TODO */
}
