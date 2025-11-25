/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd5x
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for Microchip SAMD5x/SAME5x MCUs
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "kernel_init.h"
#include "macros/units.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

/*
 * An external inductor needs to be present on the board,
 * so the feature can only be enabled by the board configuration.
 */
#ifndef USE_VREG_BUCK
#define USE_VREG_BUCK   (0)
#endif

#if CLOCK_CORECLOCK == 0
#error Please select CLOCK_CORECLOCK
#endif

#if EXTERNAL_OSC32_SOURCE && ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE
#error Select EITHER external 32kHz oscillator OR internal 32kHz Oscillator
#endif

#ifndef XOSC0_FREQUENCY
#define XOSC0_FREQUENCY (0)
#endif

#ifndef XOSC1_FREQUENCY
#define XOSC1_FREQUENCY (0)
#endif

#define GCLK_SOURCE_ACTIVE_XOSC (XOSC0_FREQUENCY ? GCLK_SOURCE_XOSC0 : GCLK_SOURCE_XOSC1)

#if USE_XOSC_ONLY /* don't use fast internal oscillators */

#if (XOSC0_FREQUENCY == 0) && (XOSC1_FREQUENCY == 0)
#error Configuration error: no external oscillator frequency defined
#endif

#if (CLOCK_CORECLOCK > SAM0_XOSC_FREQ_HZ)
#error When using an external oscillator for the main clock, the CPU frequency can't exceed it's frequency.
#endif

#define USE_DPLL 0
#define USE_DFLL 0
#define USE_XOSC 1

#ifndef GCLK_TIMER_HZ
#define GCLK_TIMER_HZ   MHZ(4)
#endif

#else /* !USE_XOSC_ONLY */

/* Main clock > 48 MHz -> use DPLL, otherwise use DFLL */
#define USE_DPLL (CLOCK_CORECLOCK > SAM0_DFLL_FREQ_HZ)
#define USE_DFLL 1
#define USE_XOSC 0

#ifndef GCLK_TIMER_HZ
#define GCLK_TIMER_HZ   MHZ(8)
#endif

#endif /* USE_XOSC_ONLY */

#if (CLOCK_CORECLOCK <= SAM0_DFLL_FREQ_HZ) && (SAM0_DFLL_FREQ_HZ % CLOCK_CORECLOCK)
#error For frequencies <= 48 MHz, CLOCK_CORECLOCK must be a divider of 48 MHz
#endif

/* If the CPU clock is lower than the minimal DPLL Freq
   set fDPLL = 2 * CLOCK_CORECLOCK */
#if USE_DPLL && (CLOCK_CORECLOCK < SAM0_DPLL_FREQ_MIN_HZ)
#define DPLL_DIV 2
#else
#define DPLL_DIV 1
#endif

static void xosc32k_init(void)
{
    if (!EXTERNAL_OSC32_SOURCE) {
        OSC32KCTRL->XOSC32K.reg = 0;
        return;
    }

    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_ENABLE
                            | OSC32KCTRL_XOSC32K_EN1K
                            | OSC32KCTRL_XOSC32K_EN32K
                            | OSC32KCTRL_XOSC32K_RUNSTDBY
                            | OSC32KCTRL_XOSC32K_XTALEN
                            | OSC32KCTRL_XOSC32K_STARTUP(7);

    while (!(OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY)) {}
}

static void xosc_init(uint8_t idx)
{
    uint32_t freq;

    if (!USE_XOSC ||
        (idx == 0 && XOSC0_FREQUENCY == 0) ||
        (idx == 1 && XOSC1_FREQUENCY == 0)) {
        OSCCTRL->XOSCCTRL[idx].reg = 0;
        return;
    }

    assert(idx == 0 || idx == 1);

    if (idx == 0) {
        freq = XOSC0_FREQUENCY;
    } else if (idx == 1) {
        freq = XOSC1_FREQUENCY;
    }

    uint32_t reg = OSCCTRL_XOSCCTRL_XTALEN
                 | OSCCTRL_XOSCCTRL_ENALC
                 | OSCCTRL_XOSCCTRL_ENABLE;

    /* SAM D5x/E5x Manual 54.12.1 (Crystal oscillator characteristics) &
     * 28.8.6 (External Multipurpose Crystal Oscillator Control)
     */
    if (freq <= MHZ(8)) {
        /* 72200 cycles @ 8MHz = 9025 µs */
        reg |= OSCCTRL_XOSCCTRL_STARTUP(9)
            |  OSCCTRL_XOSCCTRL_IMULT(3)
            |  OSCCTRL_XOSCCTRL_IPTAT(2);
    } else if (freq <= MHZ(16)) {
        /* 62000 cycles @ 16MHz = 3875 µs */
        reg |= OSCCTRL_XOSCCTRL_STARTUP(7)
            |  OSCCTRL_XOSCCTRL_IMULT(4)
            |  OSCCTRL_XOSCCTRL_IPTAT(3);
    } else if (freq <= MHZ(24)) {
        /* 68500 cycles @ 24MHz = 2854 µs */
        reg |= OSCCTRL_XOSCCTRL_STARTUP(7)
            |  OSCCTRL_XOSCCTRL_IMULT(5)
            |  OSCCTRL_XOSCCTRL_IPTAT(3);
    } else {
        /* 38500 cycles @ 48MHz = 802 µs */
        reg |= OSCCTRL_XOSCCTRL_STARTUP(5)
            |  OSCCTRL_XOSCCTRL_IMULT(6)
            |  OSCCTRL_XOSCCTRL_IPTAT(3);
    }

    OSCCTRL->XOSCCTRL[idx].reg = reg;
    while (!(OSCCTRL->STATUS.vec.XOSCRDY & (idx + 1))) {}
}

static void dfll_init(void)
{
    uint32_t reg = OSCCTRL_DFLLCTRLB_QLDIS
#ifdef OSCCTRL_DFLLCTRLB_WAITLOCK
          | OSCCTRL_DFLLCTRLB_WAITLOCK
#endif
    ;

    /* workaround for Errata 2.8.3 DFLLVAL.FINE Value When DFLL48M Re-enabled */
    OSCCTRL->DFLLMUL.reg   = 0;   /* Write new DFLLMULL configuration */
    OSCCTRL->DFLLCTRLB.reg = 0;   /* Select Open loop configuration */
    OSCCTRL->DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_ENABLE; /* Enable DFLL */
    OSCCTRL->DFLLVAL.reg   = OSCCTRL->DFLLVAL.reg; /* Reload DFLLVAL register */
    OSCCTRL->DFLLCTRLB.reg = reg; /* Write final DFLL configuration */

    OSCCTRL->DFLLCTRLA.reg = OSCCTRL_DFLLCTRLA_ENABLE;
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {}
}

static void fdpll_init_nolock(uint8_t idx, uint32_t f_cpu, uint8_t flags)
{
    /* Trigger assertion if not using FDPLL0 or FDPLL1 */
    assert(idx == 0 || idx == 1);

    if (!USE_DPLL) {
        OSCCTRL->Dpll[idx].DPLLCTRLA.reg = 0;
        return;
    }

    /* Source the DPLL from 32kHz GCLK1 ( equivalent to ((f_cpu << 5) / 32768) ) */
    const uint32_t LDR = (f_cpu >> 10);

    /* disable the DPLL before changing the configuration */
    OSCCTRL->Dpll[idx].DPLLCTRLA.reg &= ~OSCCTRL_DPLLCTRLA_ENABLE;
    while (OSCCTRL->Dpll[idx].DPLLSYNCBUSY.reg) {}

    /* set DPLL clock source */
    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0 + idx].reg = GCLK_PCHCTRL_GEN(1) | GCLK_PCHCTRL_CHEN;
    while (!(GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0 + idx].reg & GCLK_PCHCTRL_CHEN)) {}

    OSCCTRL->Dpll[idx].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(LDR & 0x1F)
                                     | OSCCTRL_DPLLRATIO_LDR((LDR >> 5) - 1);

    /* Without LBYPASS, startup takes very long, see errata section 2.13. */
    OSCCTRL->Dpll[idx].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK_GCLK
                                     | OSCCTRL_DPLLCTRLB_WUF
                                     | OSCCTRL_DPLLCTRLB_LBYPASS;

    OSCCTRL->Dpll[idx].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE | flags;

    while (OSCCTRL->Dpll[idx].DPLLSYNCBUSY.reg) {}
}

static void fdpll_lock(uint8_t idx) {
    const uint32_t flags = (OSCCTRL_DPLLSTATUS_CLKRDY | OSCCTRL_DPLLSTATUS_LOCK);
    while (!((OSCCTRL->Dpll[idx].DPLLSTATUS.reg & flags) == flags)) {}
}

static void gclk_connect(uint8_t id, uint8_t src, uint32_t flags) {
    GCLK->GENCTRL[id].reg = GCLK_GENCTRL_SRC(src) | GCLK_GENCTRL_GENEN | flags | GCLK_GENCTRL_IDC;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(id)) {}
}

void sam0_gclk_enable(uint8_t id)
{
    /* clocks 0 & 1 are always running */

    switch (id) {
    case SAM0_GCLK_TIMER:
        /* 8 MHz clock used by xtimer */
        if (USE_DPLL) {
            gclk_connect(SAM0_GCLK_TIMER,
                         GCLK_SOURCE_DPLL0,
                         GCLK_GENCTRL_DIV(DPLL_DIV * CLOCK_CORECLOCK / GCLK_TIMER_HZ));
        } else if (USE_DFLL) {
            gclk_connect(SAM0_GCLK_TIMER,
                         GCLK_SOURCE_DFLL,
                         GCLK_GENCTRL_DIV(SAM0_DFLL_FREQ_HZ / GCLK_TIMER_HZ));
        } else if (USE_XOSC) {
            gclk_connect(SAM0_GCLK_TIMER,
                         GCLK_SOURCE_ACTIVE_XOSC,
                         GCLK_GENCTRL_DIV(SAM0_XOSC_FREQ_HZ / GCLK_TIMER_HZ));
        }
        break;
    case SAM0_GCLK_PERIPH:
        if (USE_DFLL) {
            gclk_connect(SAM0_GCLK_PERIPH, GCLK_SOURCE_DFLL, 0);
        } else if (USE_XOSC) {
            gclk_connect(SAM0_GCLK_PERIPH, GCLK_SOURCE_ACTIVE_XOSC, 0);
        }
        break;
    case SAM0_GCLK_100MHZ:
        fdpll_init_nolock(1, MHZ(100), 0 /* OSCCTRL_DPLLCTRLA_ONDEMAND */);
        gclk_connect(SAM0_GCLK_100MHZ, GCLK_SOURCE_DPLL1, 0);
        fdpll_lock(1);
        break;
    }
}

uint32_t sam0_gclk_freq(uint8_t id)
{
    switch (id) {
    case SAM0_GCLK_MAIN:
        return CLOCK_CORECLOCK;
    case SAM0_GCLK_32KHZ:
        return 32768;
    case SAM0_GCLK_TIMER:
        return GCLK_TIMER_HZ;
    case SAM0_GCLK_PERIPH:
        if (USE_DFLL) {
            return SAM0_DFLL_FREQ_HZ;
        } else if (USE_XOSC) {
            return SAM0_XOSC_FREQ_HZ;
        } else {
            assert(0);
            return 0;
        }
    case SAM0_GCLK_100MHZ:
        return MHZ(100);
    default:
        return 0;
    }
}

void cpu_pm_cb_enter(int deep)
{
    (void) deep;
    /* will be called before entering sleep */
}

void cpu_pm_cb_leave(int deep)
{
    /* will be called after wake-up */

    if (deep) {
        /* DFLL needs to be re-initialized to work around errata 2.8.3 */
        dfll_init();
    }
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    /* CPU starts with DFLL48 as clock source, so we must use the LDO */
    sam0_set_voltage_regulator(SAM0_VREG_LDO);

    /* initialize the Cortex-M core */
    cortexm_init();

    /* turn on only needed APB peripherals */
    MCLK->APBAMASK.reg = MCLK_APBAMASK_MCLK
                       | MCLK_APBAMASK_OSCCTRL
                       | MCLK_APBAMASK_OSC32KCTRL
                       | MCLK_APBAMASK_GCLK
                       | MCLK_APBAMASK_SUPC
                       | MCLK_APBAMASK_PAC
#ifdef MODULE_PERIPH_PM
                       | MCLK_APBAMASK_PM
#endif
#ifdef MODULE_PERIPH_FREQM
                       | MCLK_APBAMASK_FREQM
#endif
#ifdef MODULE_PERIPH_GPIO_IRQ
                       | MCLK_APBAMASK_EIC
#endif
                       ;

    MCLK->APBBMASK.reg = 0
#ifdef MODULE_PERIPH_FLASHPAGE
                       | MCLK_APBBMASK_NVMCTRL
#endif
#ifdef MODULE_PERIPH_GPIO
                       | MCLK_APBBMASK_PORT
#endif
                       ;

    MCLK->APBCMASK.reg = 0;
    MCLK->APBDMASK.reg = 0;

    /* enable the Cortex M Cache Controller */
    CMCC->CTRL.reg |= CMCC_CTRL_CEN;

    /* make sure main clock is not sourced from DPLL */
    dfll_init();
    gclk_connect(SAM0_GCLK_MAIN, GCLK_SOURCE_DFLL, 0);

    xosc32k_init();
    if (EXTERNAL_OSC32_SOURCE) {
        gclk_connect(SAM0_GCLK_32KHZ, GCLK_SOURCE_XOSC32K, 0);
    } else if (ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE) {
        gclk_connect(SAM0_GCLK_32KHZ, GCLK_SOURCE_OSCULP32K, 0);
    }

    xosc_init(0);
    xosc_init(1);

    /* select the source of the main clock */
    if (USE_DPLL) {
        fdpll_init_nolock(0, CLOCK_CORECLOCK * DPLL_DIV, OSCCTRL_DPLLCTRLA_ONDEMAND);
        gclk_connect(SAM0_GCLK_MAIN, GCLK_SOURCE_DPLL0,
                     GCLK_GENCTRL_DIV(DPLL_DIV));
        fdpll_lock(0);
    } else if (USE_DFLL) {
        gclk_connect(SAM0_GCLK_MAIN, GCLK_SOURCE_DFLL,
                     GCLK_GENCTRL_DIV(SAM0_DFLL_FREQ_HZ / CLOCK_CORECLOCK));
    } else if (USE_XOSC) {
        gclk_connect(SAM0_GCLK_MAIN, GCLK_SOURCE_ACTIVE_XOSC,
                     GCLK_GENCTRL_DIV(SAM0_XOSC_FREQ_HZ / CLOCK_CORECLOCK));
    }

    /* make sure fast clocks are off */
    if (!USE_DFLL) {
        OSCCTRL->DFLLCTRLA.reg = 0;
    }

    /* when fast internal oscillators are not used, we can turn on the buck converter */
    if (!USE_DFLL && !USE_DPLL && USE_VREG_BUCK) {
        sam0_set_voltage_regulator(SAM0_VREG_BUCK);
    }

#ifdef MODULE_PERIPH_DMA
    /*  initialize DMA streams */
    dma_init();
#endif

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();

    /* set ONDEMAND bit after all clocks have been configured */
    /* This is to avoid setting the source for the main clock to ONDEMAND before using it. */
    OSCCTRL->Dpll[0].DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_ONDEMAND;
}
