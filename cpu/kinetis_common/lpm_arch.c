/*
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_lpm
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's power management interface
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "cpu.h"
#include "arch/lpm_arch.h"
#include "kinetis_lpm.h"
#include "board.h"
#include "periph/gpio.h"

#ifndef LPM_TRACE_LPM_ENTRY
#define LPM_TRACE_LPM_ENTRY()
#endif
#ifndef LPM_TRACE_LPM_EXIT
#define LPM_TRACE_LPM_EXIT()
#endif
#ifndef LPM_TRACE_WAIT
#define LPM_TRACE_WAIT()
#endif
#ifndef LPM_TRACE_STOP
#define LPM_TRACE_STOP()
#endif
#ifndef LPM_TRACE_VLPS
#define LPM_TRACE_VLPS()
#endif
#ifndef LPM_TRACE_LLS
#define LPM_TRACE_LLS()
#endif

/*
 * Counting semaphores for inhibiting undesired power modes.
 */
atomic_int_t kinetis_lpm_inhibit_stop_sema = ATOMIC_INIT(0);
atomic_int_t kinetis_lpm_inhibit_vlps_sema = ATOMIC_INIT(0);
atomic_int_t kinetis_lpm_inhibit_lls_sema  = ATOMIC_INIT(0);

static inline void wait(void)
{
    /* Clear the SLEEPDEEP bit to make sure we go into WAIT (sleep) mode instead
     * of deep sleep.
     */
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    /* Data synchronization barrier, wait for any outstanding writes to
     * complete before entering low power mode. */
    __DSB();
    /* WFI instruction will start entry into WAIT mode */
    __WFI();
}

/**
 * @brief Switch the CPU into a STOP mode
 *
 * @param[in] stopmode     The chosen STOP mode
 *
 * @see Kinetis CPU reference manual, chapter System Mode Controller, Power Mode
 * Control register (SMC_PMCTRL), field STOPM
 */
static inline void stop(uint8_t stopmode)
{
    volatile uint8_t dummy;
    /* Set the SLEEPDEEP bit to enable deep sleep modes (STOP) */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Write new mode to PMCTRL register */
    KINETIS_PMCTRL_SET_MODE(stopmode);

    /* Wait for the write to complete before stopping core */
    dummy = KINETIS_PMCTRL;
    (void)dummy; /* Avoid warnings about set but not used variable [-Wunused-but-set-variable] */

    /* Data synchronization barrier, wait for any outstanding writes to
     * complete before entering low power mode. */
    __DSB();
    /* WFI instruction will start entry into STOP mode */
    __WFI();
}

/** @todo Implement Kinetis power modes: VLPR, VLPW, VLLSx */

static void kinetis_low_power_mode(void)
{
    unsigned int mask = disableIRQ();
    LPM_TRACE_LPM_ENTRY()
    if (ATOMIC_VALUE(kinetis_lpm_inhibit_stop_sema) != 0) {
        /* STOP inhibited, go to WAIT mode */
        LPM_TRACE_WAIT()
        wait();
    }
    else if (ATOMIC_VALUE(kinetis_lpm_inhibit_vlps_sema) != 0) {
        /* VLPS inhibited, go to normal STOP mode */
        LPM_TRACE_STOP()
        stop(KINETIS_POWER_MODE_NORMAL);
    }
    else if (ATOMIC_VALUE(kinetis_lpm_inhibit_lls_sema) != 0) {
        /* LLS inhibited, go to VLPS mode */
        LPM_TRACE_VLPS()
        stop(KINETIS_POWER_MODE_VLPS);
    }
    else {
        /* go to LLS mode */
        LPM_TRACE_LLS()
        stop(KINETIS_POWER_MODE_LLS);
    }
    LPM_TRACE_LPM_EXIT()
    restoreIRQ(mask);
}

void lpm_arch_init(void)
{
    /* Setup Low Leakage Wake-up Unit (LLWU) */
    LLWU_UNLOCK();           /* Enable LLWU clock gate */
    KINETIS_PMPROT_UNLOCK(); /* Enable all available power modes */

    /* Enables wake on LPTMR, to let the hwtimer take the chip out of sleep */
    kinetis_lpm_enable_wakeup_module(KINETIS_LPM_WAKEUP_MODULE_LPTMR);

    NVIC_EnableIRQ(KINETIS_LLWU_IRQ); /* Enable LLWU interrupt */
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    switch (target) {
        case LPM_ON:
            /* MCU is active, do not go to low power */
            break;

        case LPM_IDLE:
        case LPM_SLEEP:
        case LPM_POWERDOWN:
        case LPM_OFF:
            /* Go to the lowest power mode the currently active peripherals will
             * allow for. */
            kinetis_low_power_mode();
            break;

        case LPM_UNKNOWN:
        default:
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
    /* TODO */
}

void lpm_arch_begin_awake(void)
{
    /* TODO */
}

void lpm_arch_end_awake(void)
{
    /* TODO */
}

void kinetis_lpm_enable_wakeup_module(const uint8_t module)
{
    /* Use bit-banding for thread-safety instead of disabling interrupts */
    BITBAND_REG8(LLWU->ME, module) = 1;
}

void kinetis_lpm_disable_wakeup_module(const uint8_t module)
{
    /* Use bit-banding for thread-safety instead of disabling interrupts */
    BITBAND_REG8(LLWU->ME, module) = 0;
}

void isr_llwu(void)
{
    /* Clear all module wake up flags */
    LLWU->F1 = 0xFF;
    LLWU->F2 = 0xFF;
    /* LLWU->F3 flags are cleared automatically when the modules' interrupt
     * flags are cleared. */
}
