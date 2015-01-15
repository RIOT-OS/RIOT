/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Saurabh Singh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file        lpm_arch.c
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Saurabh Singh <email-missing@example.com>
 *
 * @}
 */
#include "cpu.h"
#include "arch/lpm_arch.h"

enum system_sleepmode {
    /**
     *  Idle 0 mode.
     *  Potential Wake Up sources: Synchronous(APB, AHB), asynchronous.
     */
    SYSTEM_SLEEPMODE_IDLE_0,
    /**
     *  Idle 1 mode.
     *  Potential Wake Up sources: Synchronous (APB), asynchronous
     */
    SYSTEM_SLEEPMODE_IDLE_1,
    /**
     *  Idle 2 mode.
     *  Potential Wake Up sources: Asynchronous
     */
    SYSTEM_SLEEPMODE_IDLE_2,
    /**
     * Standby mode.
     * Potential Wake Up sources: Asynchronous
     */
    SYSTEM_SLEEPMODE_STANDBY,
};

static enum lpm_mode current_mode;

static void start_lpm(void);


void lpm_arch_init(void)
{
    current_mode = LPM_ON;
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
enum lpm_mode last_mode = current_mode;

    switch (target) {
        case LPM_ON:                    /* Run mode */
            current_mode = LPM_ON;
            break;
        case LPM_IDLE:                  /* Sleep mode Idle 0 */
            current_mode = LPM_IDLE;
            /* Idle Mode 0 */
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_0;
            start_lpm();
            break;
        case LPM_SLEEP:                 /* Sleep mode Idle 1 */
            current_mode = LPM_SLEEP;
             /* Idle Mode 1 */
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_1;
            start_lpm();
            break;
        case LPM_POWERDOWN:             /* Sleep mode Idle 2 */
            /* Idle Mode 2 */
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_2;
            start_lpm();
            break;
        case LPM_OFF:                   /* Standby Mode - Potential Wake Up sources: Asynchronous */
            current_mode = LPM_POWERDOWN;
            /* Standby Mode */
            SCB->SCR |=  SCB_SCR_SLEEPDEEP_Msk;
            start_lpm();
            break;
        default:
            break;
    }

    return last_mode;
}

static void start_lpm(void)
{
    /* Executes a device DSB (Data Synchronization Barrier) */
    __DSB();
    /* Enter standby mode */
    __WFI();
}

enum lpm_mode lpm_arch_get(void)
{
    return current_mode;
}

void lpm_arch_awake(void)
{
    if (current_mode == LPM_SLEEP) {
        /* Re-init */
        cpu_init();
    }
    current_mode = LPM_ON;
}

/** Not needed */
void lpm_arch_begin_awake(void){ }

/** Not needed */
void lpm_arch_end_awake(void){ }
