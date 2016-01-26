/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml21
 * @{
 *
 * @file        lpm_arch.c
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "lpm.h"
#include "arch/lpm_arch.h"
#include "cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void lpm_arch_init(void)
{
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_PM;
    PM->CTRLA.reg = PM_CTRLA_MASK & (~PM_CTRLA_IORET);

    SUPC->BOD33.bit.ENABLE=0;

    lpm_prevent_sleep = 1;
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    uint32_t mode;
    switch(target) {
        case LPM_IDLE:
            DEBUG("lpm_arch_set(): setting IDLE mode.\n");
            mode = PM_SLEEPCFG_SLEEPMODE_IDLE2;
            break;
        case LPM_SLEEP:
            DEBUG("lpm_arch_set(): setting STANDBY mode.\n");
            mode = PM_SLEEPCFG_SLEEPMODE_STANDBY;
            break;
        case LPM_POWERDOWN:
            DEBUG("lpm_arch_set(): setting BACKUP mode.\n");
            mode = PM_SLEEPCFG_SLEEPMODE_BACKUP;
            break;
        default:
            DEBUG("lpm_arch_set(): unhandled low-power mode.\n");
            return 0;
    }

    /* write sleep configuration */
    PM->SLEEPCFG.bit.SLEEPMODE = mode;

    /* make sure value has been set */
    while (PM->SLEEPCFG.bit.SLEEPMODE != mode) {}

    /* ensure all memory accesses have completed */
    __DSB();

    /* go to sleep mode (issue wait-for-interrupt instruction) */
    __WFI();

    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    // TODO
    return 0;
}

void lpm_arch_awake(void)
{
    // TODO
}

void lpm_arch_begin_awake(void)
{
    // TODO
}

void lpm_arch_end_awake(void)
{
    // TODO
}
