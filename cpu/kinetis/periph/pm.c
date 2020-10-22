/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Eistec AB
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "periph/pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Set to 1 to use the LEDx macros to show which sleep mode is entered */
#define ENABLE_LEDS (0)

#if ENABLE_LEDS
#include "board.h"
#define PM_LED(x, state) LED ## x ## _ ## state
#else
#define PM_LED(x, state)
#endif

/* SMC_PMCTRL_STOPM masks */
enum {
    SMC_PMCTRL_STOPM_STOP = 0,
    /* 1 is reserved */
    SMC_PMCTRL_STOPM_VLPS = 2,
    SMC_PMCTRL_STOPM_LLS  = 3,
    /* VLLS is not supported */
};

/** Configure which stop mode will be entered when cortexm_sleep(1) is called */
static inline void pm_stopm(uint8_t stopm)
{
    SMC->PMCTRL = (SMC->PMCTRL & ~(SMC_PMCTRL_STOPM_MASK)) | SMC_PMCTRL_STOPM(stopm);
}

void pm_set(unsigned mode)
{
    unsigned deep = 1;
    switch (mode) {
        /* Only the CPU stops in this mode. No clocks or peripherals are powered
         * off. This state should always be safe to use, as any interrupt will
         * wake it. Power consumption in this state is reduced by about 50%. */
        case KINETIS_PM_WAIT: /* 3 - ARM Sleep */
            /* WAIT */
            deep = 0;
            PM_LED(0, ON);
            break;

        /* Some system clocks are stopped in this mode. Usage, means of waking,
         * and power consumption depend heavily on board-level clock and
         * peripheral configuration.*/
        /* If the debug hardware has been used since the last POR, the debug
         * hardware as well as the clock needed to support it will stay on in
         * this mode. Thus, after flashing, issue a POR before testing it. */
        case KINETIS_PM_STOP: /* 2 - ARM Deep Sleep */
            /* STOP */
            pm_stopm(SMC_PMCTRL_STOPM_STOP);
            PM_LED(1, ON);
            break;

        /* Some system clocks are stopped in this mode. Usage, means of waking,
         * and power consumption depend heavily on board-level clock and
         * peripheral configuration.*/
        /* If the debug hardware has been used since the last POR, the debug
         * hardware as well as the clock needed to support it will stay on in
         * this mode. Thus, after flashing, issue a POR before testing it. */
        case KINETIS_PM_VLPS: /* 1 - ARM Deep Sleep */
            /* VLPS */
            pm_stopm(SMC_PMCTRL_STOPM_VLPS);
            PM_LED(2, ON);
            break;

        /* All clocks except 32kHz are stopped. Wakeup can occur from
         * interrupts, several (LLWU-enabled) GPIOs, RTC, or (if the board
         * configuration uses the 32kHz clock for xtimer) xtimer. Debug hardware
         * is powered off. Power consumption in this state should be on the
         * order of 5 uA for the MCU alone, with most boards having additional
         * parts that will raise this amount. */
        case KINETIS_PM_LLS: /* 0 - ARM Deep Sleep */
            /* LLSx */
            pm_stopm(SMC_PMCTRL_STOPM_LLS);
            PM_LED(0, ON);
            PM_LED(2, ON);
            /* Enable LLWU interrupt, or else we can never resume from LLS */
            /* Clear pending flag first, the LLWU has no purpose in RUN mode, so
             * if the flag is set then it must be a remainder from handling the
             * previous wakeup. */
            LLWU->F1 = 0xffu;
            LLWU->F2 = 0xffu;
            NVIC_ClearPendingIRQ(LLWU_IRQn);
            NVIC_EnableIRQ(LLWU_IRQn);
            break;
    }
    DEBUG("pm_set(%u)\n", mode);
    cortexm_sleep(deep);
    PM_LED(0, OFF);
    PM_LED(1, OFF);
    PM_LED(2, OFF);
}
