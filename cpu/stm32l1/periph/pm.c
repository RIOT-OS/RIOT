/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Nick v. IJzendoorn <nijzndoorn@engineering-spirit.nl>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph/pm.h"
#include "periph/uart.h"
#include "uart_stdio.h"

#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @name Available power modes
 */
enum pm_mode {
    PM_OFF = 0,        /**< MCU is off */
    PM_POWERDOWN,      /**< MCU is powered down */
    PM_SLEEP,          /**< MCU in sleep mode */
};

#ifndef PM_STOP_CONFIG
/**
 * @brief Define config flags for stop mode
 *
 * Available values can be found in reference manual, PWR section, register CR.
 */
#define PM_STOP_CONFIG (PWR_CR_LPDS | PWR_CR_FPDS)
#endif

/* Select CPU clocking between default (PM_ON) and medium-speed (PM_IDLE) */
static void pm_run_mode(void) {
    stmclk_init_sysclk();

#ifdef MODULE_XTIMER
    /* Recalculate xtimer frequency */
    /* NB: default XTIMER_HZ clock is 1 MHz, so CPU clock must be at least 1 MHz for xtimer to work properly */
    xtimer_init();
#endif

    /* Recalculate stdio UART baudrate */
    uart_set_baudrate(UART_STDIO_DEV, UART_STDIO_BAUDRATE);
}

void pm_set(unsigned mode)
{
    unsigned state;

    switch (mode) {
        case PM_OFF:             /* Stand by mode */
            /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;

            /* Select STANDBY mode */
            PWR->CR |= PWR_CR_PDDS;

            /* Set SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR |= (uint32_t)SCB_SCR_SLEEPDEEP;

            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;

            /* This option is used to ensure that store operations are completed */
            #if defined (__CC_ARM)
            __force_stores();
            #endif

            irq_disable();

            /* Request Wait For Interrupt */
            __DSB();
            __WFI();
            break;

        case PM_POWERDOWN:             /* Stop mode */
            /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;
            /* Regulator in LP mode */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;

            /* Set SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR |= (uint32_t)SCB_SCR_SLEEPDEEP;

            state = irq_disable();

            /* Request Wait For Interrupt */
            __DSB();
            __WFI();

            /* Clear SLEEPDEEP bit */
            SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP);

            /* Restore clocks and PLL */
            pm_run_mode();

            irq_restore(state);
            break;

        case PM_SLEEP:              /* Low-power sleep mode */
            /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;
            /* Enable low-power mode of the voltage regulator */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Clear SLEEPDEEP bit */
            SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP);

            state = irq_disable();

            /* Switch to 65kHz clock */
            stmclk_switch_msi(RCC_ICSCR_MSIRANGE_0, RCC_CFGR_HPRE_DIV1);

            /* Request Wait For Interrupt */
            __DSB();
            __WFI();

            /* Switch back to default speed */
            pm_run_mode();

            irq_restore(state);
            break;

        default:
            break;
    }

}

void pm_off(void)
{
    irq_disable();
    pm_set(0);
}
