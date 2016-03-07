/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2016 Laksh Bhatia
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's lpm interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Laksh Bhatia <bhatialaksh3@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "arch/lpm_arch.h"

static enum lpm_mode current_mode = LPM_UNKNOWN;

void lpm_arch_init(void)
{
    current_mode = LPM_ON;
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    enum lpm_mode last_mode = current_mode;
    switch (target) {
        case LPM_ON:                    /* STM Run mode */
            current_mode = LPM_ON;
            break;
        case LPM_IDLE:                  /* STM Low power run mode */
            current_mode = LPM_IDLE;
            PWR->CR |= (PWR_CR_LPSDSR | PWR_CR_LPRUN);
            /* Enter low power run mode */
            __WFE();
            break;
        case LPM_SLEEP:                 /* STM Low power sleep mode */
            current_mode = LPM_SLEEP;
            PWR->CR |= (PWR_CR_LPSDSR);
            /* Enter low power sleep mode */
            __WFI();  /* Option between WFI and WFE*/
            // __WFE();
            break;
        case LPM_POWERDOWN:             /* STM Standby mode */
            current_mode = LPM_POWERDOWN;
            /* Set PDDS to enter standby mode on deepsleep and clear flags */
            PWR->CR |= (PWR_CR_PDDS | PWR_CR_CSBF | PWR_CR_CWUF);
            /* Enable WKUP pin to use for wakeup from standby mode in pin1 */
            //PWR->CSR |= PWR_CSR_EWUP1;
            /* Set SLEEPDEEP bit of system control block */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    #if defined ( __CC_ARM   )
            /* Ensure that store operations are completed */
            __force_stores();
    #endif
            /* Enter standby mode */
            __WFI();
            __WFE();
            break;
        case LPM_OFF:                   /* STM Stop mode */
            current_mode = LPM_OFF;
            /* Set PDDS to enter standby mode on deepsleep and clear flags */
            PWR->CR |= (PWR_CR_PDDS | PWR_CR_LPSDSR | PWR_CR_CWUF );
            /* Enable WKUP pin to use for wakeup from standby mode in pin1 */
            //PWR->CSR |= PWR_CSR_EWUP1;
            /* Set SLEEPDEEP bit of system control block */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    #if defined ( __CC_ARM   )
            /* Ensure that store operations are completed */
            __force_stores();
    #endif
            /* Enter standby mode */
            __WFI();
            __WFE();
            break;
        default:
            break;
    }
    return last_mode;
}

enum lpm_mode lpm_arch_get(void)
{
    /* TODO */
    return current_mode;
}

void lpm_arch_awake(void)
{
    if (current_mode == LPM_SLEEP) {
        /* After stop mode, the clock system needs to be reconfigured */
        cpu_init();
    }
    current_mode = LPM_ON;
}

void lpm_arch_begin_awake(void)
{
    /* TODO */
}

void lpm_arch_end_awake(void)
{
    /* TODO */
}
