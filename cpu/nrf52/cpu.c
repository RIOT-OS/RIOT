/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf52832
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"

#define ENABLE_SWO   (0)
#define ENABLE_TRACE (0)

/* FTPAN helper functions */
static bool ftpan_32(void);
static bool ftpan_37(void);
static bool ftpan_36(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);

    /* Workaround for FTPAN-32 "DIF: Debug session automatically enables TracePort pins." */
    if (ftpan_32()) {
        CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    }

    /* Workaround for FTPAN-37 "AMLI: EasyDMA is slow with Radio, ECB, AAR and CCM." */
    if (ftpan_37()) {
        *(volatile uint32_t *)0x400005A0 = 0x3;
    }

    /* Workaround for FTPAN-36 "CLOCK: Some registers are not reset when expected." */
    if (ftpan_36()) {
        NRF_CLOCK->EVENTS_DONE = 0;
        NRF_CLOCK->EVENTS_CTTO = 0;
    }

    /* Enable the FPU if the compiler used floating point unit instructions. __FPU_USED is a MACRO defined by the
     * compiler. Since the FPU consumes energy, remember to disable FPU use in the compiler if floating point unit
     * operations are not used in your code. */
#if (__FPU_USED == 1)
    SCB->CPACR |= (3UL << 20) | (3UL << 22);
    __DSB();
    __ISB();
#endif

    /* Enable SWO trace functionality. If ENABLE_SWO is not defined, SWO pin will be used as GPIO */
#if (ENABLE_SWO == 1)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    NRF_CLOCK->TRACECONFIG |= CLOCK_TRACECONFIG_TRACEMUX_Serial << CLOCK_TRACECONFIG_TRACEMUX_Pos;
#endif

    /* Enable Trace functionality. If ENABLE_TRACE is not defined, TRACE pins will be used as GPIOs */
#if (ENABLE_TRACE == 1)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    NRF_CLOCK->TRACECONFIG |= CLOCK_TRACECONFIG_TRACEMUX_Parallel << CLOCK_TRACECONFIG_TRACEMUX_Pos;
#endif

    /* set the correct clock source for HFCLK */
#if (CLOCK_CRYSTAL == 32)
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);

    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

#endif
}

/**
 * @brief Check workaround for FTPAN-32
 */
static bool ftpan_32(void)
{
    if ((((*(uint32_t *)0xF0000FE0) & 0x000000FF) == 0x6)
        && (((*(uint32_t *)0xF0000FE4) & 0x0000000F) == 0x0)) {
        if ((((*(uint32_t *)0xF0000FE8) & 0x000000F0) == 0x30)
            && (((*(uint32_t *)0xF0000FEC) & 0x000000F0) == 0x0)) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Check workaround for FTPAN-36
 */
static bool ftpan_36(void)
{
    if ((((*(uint32_t *)0xF0000FE0) & 0x000000FF) == 0x6)
        && (((*(uint32_t *)0xF0000FE4) & 0x0000000F) == 0x0)) {
        if ((((*(uint32_t *)0xF0000FE8) & 0x000000F0) == 0x30)
            && (((*(uint32_t *)0xF0000FEC) & 0x000000F0) == 0x0)) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Check workaround for FTPAN-37
 */
static bool ftpan_37(void)
{
    if ((((*(uint32_t *)0xF0000FE0) & 0x000000FF) == 0x6)
        && (((*(uint32_t *)0xF0000FE4) & 0x0000000F) == 0x0)) {
        if ((((*(uint32_t *)0xF0000FE8) & 0x000000F0) == 0x30)
            && (((*(uint32_t *)0xF0000FEC) & 0x000000F0) == 0x0)) {
            return true;
        }
    }

    return false;
}
