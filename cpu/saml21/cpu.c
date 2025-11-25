/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml21
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for Atmel SAML21 MCUs
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "kernel_init.h"
#include "periph/init.h"
#include "periph_conf.h"
#include "stdio_base.h"

/* As long as DFLL & DPLL are not used, we can default to
 * always use the buck converter when available.
 *
 * An external inductor needs to be present on the board,
 * so the feature can only be enabled by the board configuration.
 */
#ifndef USE_VREG_BUCK
#define USE_VREG_BUCK   (0)
#endif

#if (CLOCK_CORECLOCK == 48000000U) || defined (MODULE_PERIPH_USBDEV)
#define USE_DFLL        (1)
#define GCLK_GENCTRL_SRC_MAIN GCLK_GENCTRL_SRC_DFLL48M
#else
#define USE_DFLL        (0)
#define GCLK_GENCTRL_SRC_MAIN GCLK_GENCTRL_SRC_OSC16M
#endif

static void _gclk_setup(int gclk, uint32_t reg)
{
    GCLK->GENCTRL[gclk].reg = reg;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(gclk)) {}
}

static void _osc32k_setup(void)
{
#if INTERNAL_OSC32_SOURCE
    uint32_t * pCalibrationArea;
    uint32_t osc32kcal;

    /* Read OSC32KCAL, calibration data for OSC32 !!! */
    pCalibrationArea = (uint32_t*) NVMCTRL_OTP5;
    osc32kcal = ( (*pCalibrationArea) & 0x1FC0 ) >> 6;

    /* RTC use Low Power Internal Oscillator at 32kHz */
    OSC32KCTRL->OSC32K.reg = OSC32KCTRL_OSC32K_RUNSTDBY
                           | OSC32KCTRL_OSC32K_EN32K
                           | OSC32KCTRL_OSC32K_CALIB(osc32kcal)
                           | OSC32KCTRL_OSC32K_ENABLE;

    /* Wait OSC32K Ready */
    while (!(OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_OSC32KRDY)) {}
#endif /* INTERNAL_OSC32_SOURCE */
}

static void _xosc32k_setup(void)
{
#if EXTERNAL_OSC32_SOURCE
    /* RTC uses External 32,768KHz Oscillator */
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_XTALEN
                            | OSC32KCTRL_XOSC32K_RUNSTDBY
                            | OSC32KCTRL_XOSC32K_EN32K
                            | OSC32KCTRL_XOSC32K_ENABLE;

    /* Wait XOSC32K Ready */
    while (!(OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY)) {}
#endif
}

void sam0_gclk_enable(uint8_t id)
{
    switch(id) {
        case SAM0_GCLK_48MHZ:
            _gclk_setup(SAM0_GCLK_48MHZ, GCLK_GENCTRL_GENEN |
                        GCLK_GENCTRL_SRC_DFLL48M);
            break;
        default:
            break;
    }
}

uint32_t sam0_gclk_freq(uint8_t id)
{
    switch (id) {
    case SAM0_GCLK_MAIN:
        return CLOCK_CORECLOCK;
    case SAM0_GCLK_TIMER:
#if (CLOCK_CORECLOCK == 48000000U) || (CLOCK_CORECLOCK == 16000000U) || (CLOCK_CORECLOCK == 8000000U)
        return 8000000;
#else
        return 4000000;
#endif
    case SAM0_GCLK_32KHZ:
        return 32768;
    case SAM0_GCLK_48MHZ:
        return 48000000;
    default:
        return 0;
    }
}

static void _dfll_setup(void)
{
    if (!USE_DFLL) {
        return;
    }

    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg = GCLK_PCHCTRL_CHEN |
                                                GCLK_PCHCTRL_GEN_GCLK2;

    /* wait for sync */
    while (!(GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg & GCLK_PCHCTRL_CHEN)) {}

    OSCCTRL->DFLLCTRL.reg = OSCCTRL_DFLLCTRL_ENABLE;
    /* Wait for write synchronization */
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {}
    OSCCTRL->DFLLVAL.reg = OSCCTRL_DFLLVAL_COARSE((*(uint32_t*)NVMCTRL_OTP5)
                           >> 26) |  OSCCTRL_DFLLVAL_FINE(512);

    /* Wait for write synchronization */
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {}
    /* Generate a 48 Mhz clock from the 32KHz */
    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP(0x08) |
                           OSCCTRL_DFLLMUL_FSTEP(0x08) |
                           OSCCTRL_DFLLMUL_MUL((48000000U/32768));

    /* Disable DFLL before setting its configuration */
    OSCCTRL->DFLLCTRL.reg = 0;
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {}
    /* Write full configuration to DFLL control register */
    OSCCTRL->DFLLCTRL.reg =  OSCCTRL_DFLLCTRL_WAITLOCK |
                             OSCCTRL_DFLLCTRL_MODE |
                             OSCCTRL_DFLLCTRL_CCDIS |
                             OSCCTRL_DFLLCTRL_BPLCKC |
                             OSCCTRL_DFLLCTRL_ENABLE;

    /* Ensure COARSE and FINE are locked */
    while ((!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLLCKC)) &&
           (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLLCKF))) {}
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {}

    /* Enable NVMCTRL */
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_NVMCTRL;
    /* Set Wait State to meet requirements */
    NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_RWS(3);
}

static void _set_active_mode_vreg(void)
{
    if (!USE_VREG_BUCK) {
        return;
    }

    /* not compatible with 48 MHz DFLL & 96 MHz FDPLL */
    if (USE_DFLL) {
        sam0_set_voltage_regulator(SAM0_VREG_LDO);
    } else {
        sam0_set_voltage_regulator(SAM0_VREG_BUCK);
    }
}

void cpu_pm_cb_enter(int deep)
{
    if (!deep) {
        return;
    }

    /* If you are using saml21 rev. B, switch Main Clock to OSCULP32 during standby
       to work around errata 1.2.1.
       See discussion in #13441  */
    assert(((DSU->DID.reg & DSU_DID_REVISION_Msk) > 1) ||
           ((PM->STDBYCFG.reg & 0x80) == 0));

    /* errata 51.1.5 – When VDDCORE is supplied by the BUCK converter in performance
                       level 0, the chip cannot wake-up from standby mode because the
                       VCORERDY status is stuck at 0. */
    if (USE_VREG_BUCK && !(PM->PLCFG.reg & PM_PLCFG_PLSEL_Msk)) {
        sam0_set_voltage_regulator(SAM0_VREG_LDO);
    }

    /* TODO: If we source Main Clock from OSCULP32 during standby and are not in
             performance level 0, we should always be able to use the BUCK converter
             during standby */
}

void cpu_pm_cb_leave(int deep)
{
    if (!deep) {
        return;
    }

    _set_active_mode_vreg();
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    uint32_t reg = 0;
    /* disable the watchdog timer */
    WDT->CTRLA.reg = 0;

    /* Disable the RTC module to prevent synchronization issues during CPU init
       if the RTC was running from a previous boot (e.g wakeup from backup) */
    if (RTC->MODE2.CTRLA.reg & RTC_MODE2_CTRLA_ENABLE) {
        while (RTC->MODE2.SYNCBUSY.reg) {}
        RTC->MODE2.CTRLA.reg &= ~RTC_MODE2_CTRLA_ENABLE;
        while (RTC->MODE2.SYNCBUSY.reg) {}
    }

    /* initialize the Cortex-M core */
    cortexm_init();

    /* select the right voltage regulator config for active mode */
    _set_active_mode_vreg();

    /* turn on only needed APB peripherals */
    MCLK->APBAMASK.reg =
        MCLK_APBAMASK_PM
        |MCLK_APBAMASK_MCLK
        |MCLK_APBAMASK_RSTC
        |MCLK_APBAMASK_OSCCTRL
        |MCLK_APBAMASK_OSC32KCTRL
        |MCLK_APBAMASK_SUPC
        |MCLK_APBAMASK_GCLK
        |MCLK_APBAMASK_WDT
        |MCLK_APBAMASK_RTC
        |MCLK_APBAMASK_EIC
        |MCLK_APBAMASK_PORT
        //|MCLK_APBAMASK_TAL
        ;

    /* Software reset the GCLK module to ensure it is re-initialized correctly */
    GCLK->CTRLA.reg = GCLK_CTRLA_SWRST;
    while (GCLK->CTRLA.reg & GCLK_CTRLA_SWRST) {}
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST) {}

#if (CLOCK_CORECLOCK > 12000000U)
    PM->PLCFG.reg = PM_PLCFG_PLSEL_PL2;
    while (!(PM->INTFLAG.reg & PM_INTFLAG_PLRDY)) {}
#endif

    /* set OSC16M according to CLOCK_CORECLOCK */
#if (CLOCK_CORECLOCK == 48000000U) || (CLOCK_CORECLOCK == 16000000U)
    reg = OSCCTRL_OSC16MCTRL_FSEL_16;
#elif (CLOCK_CORECLOCK == 12000000U)
    reg = OSCCTRL_OSC16MCTRL_FSEL_12;
#elif (CLOCK_CORECLOCK == 8000000U)
    reg = OSCCTRL_OSC16MCTRL_FSEL_8;
#elif (CLOCK_CORECLOCK == 4000000U)
    reg = OSCCTRL_OSC16MCTRL_FSEL_4;
#else
#error "Please select a valid CPU frequency"
#endif

    reg |= OSCCTRL_OSC16MCTRL_ONDEMAND;
    reg |= OSCCTRL_OSC16MCTRL_ENABLE;
    OSCCTRL->OSC16MCTRL.reg = reg;

    _osc32k_setup();
    _xosc32k_setup();

#if EXTERNAL_OSC32_SOURCE
    _gclk_setup(SAM0_GCLK_32KHZ, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K);
#else
    _gclk_setup(SAM0_GCLK_32KHZ, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K);
#endif

    _dfll_setup();

    /* Setup GCLK generators */
    _gclk_setup(SAM0_GCLK_MAIN, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_MAIN);

    /* Ensure APB Backup domain clock is within the 6MHZ limit, BUPDIV value
       must be a power of 2 and between 1(2^0) and 128(2^7) */
    for (unsigned i = 0; i < 8; i++) {
        if (CLOCK_CORECLOCK / (1 << i) <= 6000000) {
            MCLK->BUPDIV.reg = (1 << i);
            while (!(MCLK->INTFLAG.reg & MCLK_INTFLAG_CKRDY)) {}
            break;
        }
    }
    /* clock used by timers */
    _gclk_setup(SAM0_GCLK_TIMER, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_MAIN
                | GCLK_GENCTRL_DIV(CLOCK_CORECLOCK/sam0_gclk_freq(SAM0_GCLK_TIMER)));

#ifdef MODULE_PERIPH_PM
    PM->CTRLA.reg = PM_CTRLA_MASK & (~PM_CTRLA_IORET);

    /* disable brownout detection
     * (Caused unexplicable reboots from sleep on saml21. /KS)
     */
    SUPC->BOD33.reg &= ~SUPC_BOD33_ENABLE;
#endif

#ifdef MODULE_PERIPH_DMA
    /*  initialize DMA streams */
    dma_init();
#endif
    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
