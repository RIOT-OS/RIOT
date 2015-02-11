/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
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
        case LPM_IDLE:                  /* STM Sleep mode */
            current_mode = LPM_IDLE;
            /* Reset SLEEPDEEP bit of system control block */
            SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
            /* Enter sleep mode */
            __WFI();
            break;
        case LPM_SLEEP:                 /* STM Stop mode */
            current_mode = LPM_SLEEP;
            /* Clear PDDS and LPDS bits to enter stop mode on */
            /* deepsleep with voltage regulator on */
            PWR->CR &= ~(PWR_CR_PDDS | PWR_CR_LPDS);
            /* Set SLEEPDEEP bit of system control block */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            /* Enter stop mode */
            __WFI();
            break;
        case LPM_POWERDOWN:             /* STM Standby mode */
            /* Fall-through */
        case LPM_OFF:                   /* STM Standby mode */
            current_mode = LPM_POWERDOWN;
            /* Set PDDS to enter standby mode on deepsleep and clear flags */
            PWR->CR |= (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF);
            /* Enable WKUP pin to use for wakeup from standby mode */
            PWR->CSR |= PWR_CSR_EWUP;
            /* Set SLEEPDEEP bit of system control block */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#if defined ( __CC_ARM   )
            /* Ensure that store operations are completed */
            __force_stores();
#endif
            /* Enter standby mode */
            __WFI();
            break;
        default:
            break;
    }

    return last_mode;
}

enum lpm_mode lpm_arch_get(void)
{
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

/** Not provided */
inline void lpm_arch_begin_awake(void) { }

/** Not provided */
inline void lpm_arch_end_awake(void) { }
