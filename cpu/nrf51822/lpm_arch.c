/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file        lpm_arch.c
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 *
 * @}
 */

#include "cpu.h"
#include "arch/lpm_arch.h"

void lpm_arch_init(void)
{
    /* TODO: needs to be implemented */
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    switch (target) {
        /* wait for next interrupt */
        case LPM_IDLE:
        case LPM_SLEEP:
        case LPM_POWERDOWN:
            __DSB();
            __WFI();
            break;
        case LPM_OFF:
            /* Switch of RAM and power off */
            NRF_POWER->RAMON = 0;
            NRF_POWER->SYSTEMOFF = 1;
            break;

        /* do nothing here */
        case LPM_UNKNOWN:
        case LPM_ON:
        default:
            break;
    }
    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    /* TODO: needs to be implemented */
    return 0;
}

void lpm_arch_awake(void)
{
    /* TODO: needs to be implemented */
}

void lpm_arch_begin_awake(void)
{
    /* TODO: needs to be implemented */
}

void lpm_arch_end_awake(void)
{
    /* TODO: needs to be implemented */
}
