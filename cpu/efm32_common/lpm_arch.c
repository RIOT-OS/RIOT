/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Hauke Petersen <hauke.peterse@fu-berlin.de>
 *
 * @}
 */

#include "arch/lpm_arch.h"

#include "em_emu.h"

void lpm_arch_init(void)
{
    /* initialize deep sleep/stop mode */
    EMU_EM23Init_TypeDef init = EMU_EM23INIT_DEFAULT;

    EMU_EM23Init(&init);
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    switch (target) {
        /* wait for next event or interrupt */
        case LPM_IDLE:
            EMU_EnterEM1();
            break;
        case LPM_SLEEP:
        case LPM_POWERDOWN:
            EMU_EnterEM2(true);
            break;

        /* do nothing here */
        case LPM_OFF:
        case LPM_UNKNOWN:
        case LPM_ON:
        default:
            break;
    }

    /* no matter which case was selected, instructions executed in EM0 only */
    return LPM_ON;
}

enum lpm_mode lpm_arch_get(void)
{
    /* instructions are executed in EM0 only */
    return LPM_ON;
}

void lpm_arch_awake(void)
{
    return;
}

void lpm_arch_begin_awake(void)
{
    return;
}

void lpm_arch_end_awake(void)
{
    return;
}
