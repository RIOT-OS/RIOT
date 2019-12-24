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

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

#if CLOCK_CORECLOCK == 0
#error Please select CLOCK_CORECLOCK
#endif

/* use DFLL for low frequency operation */
#if CLOCK_CORECLOCK > SAM0_DFLL_FREQ_HZ
#define USE_DPLL 1
#else
#define USE_DPLL 0
#if (SAM0_DFLL_FREQ_HZ % CLOCK_CORECLOCK)
#error For frequencies < 48 MHz, CLOCK_CORECLOCK must be a divider of 48 MHz
#endif
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
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_ENABLE
                            | OSC32KCTRL_XOSC32K_EN1K
                            | OSC32KCTRL_XOSC32K_EN32K
                            | OSC32KCTRL_XOSC32K_RUNSTDBY
                            | OSC32KCTRL_XOSC32K_XTALEN
                            | OSC32KCTRL_XOSC32K_STARTUP(7);

    while (!OSC32KCTRL->STATUS.bit.XOSC32KRDY) {}
}

static void dfll_init(void)
{
    uint32_t reg = OSCCTRL_DFLLCTRLB_QLDIS
#ifdef OSCCTRL_DFLLCTRLB_WAITLOCK
          | OSCCTRL_DFLLCTRLB_WAITLOCK
#endif
    ;

    OSCCTRL->DFLLCTRLB.reg = reg;
    OSCCTRL->DFLLCTRLA.reg = OSCCTRL_DFLLCTRLA_ENABLE;

    while (!OSCCTRL->STATUS.bit.DFLLRDY) {}
}

#if USE_DPLL
static void fdpll0_init(uint32_t f_cpu)
{
    /* We source the DPLL from 32kHz GCLK1 */
    const uint32_t LDR = ((f_cpu << 5) / 32768);

    /* disable the DPLL before changing the configuration */
    OSCCTRL->Dpll[0].DPLLCTRLA.bit.ENABLE = 0;
    while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.reg) {}

    /* set DPLL clock source */
    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0].reg = GCLK_PCHCTRL_GEN(1) | GCLK_PCHCTRL_CHEN;
    while (!(GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0].reg & GCLK_PCHCTRL_CHEN)) {}

    OSCCTRL->Dpll[0].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(LDR & 0x1F)
                                   | OSCCTRL_DPLLRATIO_LDR((LDR >> 5) - 1);

    /* Without LBYPASS, startup takes very long, see errata section 2.13. */
    OSCCTRL->Dpll[0].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK_GCLK
                                   | OSCCTRL_DPLLCTRLB_WUF
                                   | OSCCTRL_DPLLCTRLB_LBYPASS;

    OSCCTRL->Dpll[0].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE;

    while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.reg) {}
    while (!(OSCCTRL->Dpll[0].DPLLSTATUS.bit.CLKRDY &&
             OSCCTRL->Dpll[0].DPLLSTATUS.bit.LOCK)) {}
}
#endif

static void gclk_connect(uint8_t id, uint8_t src, uint32_t flags) {
    GCLK->GENCTRL[id].reg = GCLK_GENCTRL_SRC(src) | GCLK_GENCTRL_GENEN | flags | GCLK_GENCTRL_IDC;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(id)) {}
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
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
    CMCC->CTRL.bit.CEN = 1;

    xosc32k_init();
    gclk_connect(1, GCLK_SOURCE_XOSC32K, 0);

    /* make sure main clock is not sourced from DPLL */
    dfll_init();
    gclk_connect(0, GCLK_SOURCE_DFLL, 0);

#if USE_DPLL
    fdpll0_init(CLOCK_CORECLOCK * DPLL_DIV);

    /* source main clock from DPLL */
    gclk_connect(0, GCLK_SOURCE_DPLL0, GCLK_GENCTRL_DIV(DPLL_DIV));

    /* clock used by xtimer */
    gclk_connect(5, GCLK_SOURCE_DPLL0, GCLK_GENCTRL_DIV(DPLL_DIV * CLOCK_CORECLOCK / 8000000));
#else
    gclk_connect(0, GCLK_SOURCE_DFLL, GCLK_GENCTRL_DIV(SAM0_DFLL_FREQ_HZ / CLOCK_CORECLOCK));

    /* clock used by xtimer */
    gclk_connect(5, GCLK_SOURCE_DFLL, GCLK_GENCTRL_DIV(SAM0_DFLL_FREQ_HZ / 8000000));
#endif

#ifdef MODULE_PERIPH_USBDEV
    gclk_connect(6, GCLK_SOURCE_DFLL, 0);
#endif

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
}
