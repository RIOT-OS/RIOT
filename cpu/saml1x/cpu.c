/*
 * Copyright (C) 2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml1x
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for Microchip
 *              SAML10/SAML11 MCUs
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"
#include "periph_conf.h"
#include "board.h"
#include "stdio_base.h"

#ifdef CPU_FAM_SAML11
#define _NVMCTRL NVMCTRL_SEC
#else
#define _NVMCTRL NVMCTRL
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
    while (!OSC32KCTRL->STATUS.bit.OSC32KRDY) {}
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
    while (!OSC32KCTRL->STATUS.bit.XOSC32KRDY) {}
#endif
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
                         | MCLK_APBAMASK_PM
#ifdef MODULE_PERIPH_GPIO_IRQ
                         | MCLK_APBAMASK_EIC
#endif
#ifdef MODULE_PERIPH_GPIO
                         | MCLK_APBAMASK_PORT
#endif
                         ;

    /* Software reset the GCLK module to ensure it is re-initialized correctly */
    GCLK->CTRLA.reg = GCLK_CTRLA_SWRST;
    while (GCLK->CTRLA.reg & GCLK_CTRLA_SWRST) {}
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST) {}

    PM->PLCFG.reg = PM_PLCFG_PLSEL_PL2;
    while (!PM->INTFLAG.bit.PLRDY) {}

    MCLK->APBBMASK.reg |= MCLK_APBBMASK_NVMCTRL;
    _NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_RWS(1);
    MCLK->APBBMASK.reg &= ~MCLK_APBBMASK_NVMCTRL;

    /* set OSC16M to 16MHz */
    OSCCTRL->OSC16MCTRL.bit.FSEL = 3;
    OSCCTRL->OSC16MCTRL.bit.ONDEMAND = 0;
    OSCCTRL->OSC16MCTRL.bit.RUNSTDBY = 0;

    _osc32k_setup();
    _xosc32k_setup();

    /* Setup GCLK generators */
    _gclk_setup(0, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC16M);

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
}
