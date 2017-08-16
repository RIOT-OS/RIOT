/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samr30
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for Atmel SAMR30 MCUs
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Pierre Godicheau <pierre.godicheau@gmail.com>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"

//#define USE_EXTERNAL_16MHZ_CRYSTAL

static void _gclk_setup(int gclk, uint32_t reg)
{
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(gclk)) {}
    GCLK->GENCTRL[gclk].reg = reg;
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    /* disable the watchdog timer */
    WDT->CTRLA.bit.ENABLE = 0;

    /* initialize the Cortex-M core */
    cortexm_init();

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

#if !defined(USE_EXTERNAL_16MHZ_CRYSTAL)
    /* set OSC16M to 16MHz */
    OSCCTRL->OSC16MCTRL.bit.FSEL = 3;
    OSCCTRL->OSC16MCTRL.bit.ONDEMAND = 0;
    OSCCTRL->OSC16MCTRL.bit.RUNSTDBY = 0;
    OSCCTRL->OSC16MCTRL.bit.ENABLE = 1;

    /* Setup GCLK generators */
    _gclk_setup(0, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC16M);
    _gclk_setup(1, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K);

#else
    OSCCTRL->XOSCCTRL.bit.STARTUP = 0xF;
    OSCCTRL->XOSCCTRL.bit.GAIN = 0x3;
    OSCCTRL->XOSCCTRL.bit.XTALEN = 0x1;
    OSCCTRL->XOSCCTRL.bit.ENABLE = 0x1;
    while (OSCCTRL->STATUS.bit.XOSCRDY) {};
    OSCCTRL->XOSCCTRL.bit.AMPGC = 0x1;

    OSC32KCTRL->XOSC32K.bit.STARTUP = 0x6;
    OSC32KCTRL->XOSC32K.bit.XTALEN = 0x1;

    /* Setup GCLK generators */
    _gclk_setup(0, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC16M);
    _gclk_setup(1, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K);
#endif 


#ifdef FEATURE_PERIPH_PM
    /* enable power managemet module */
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_PM;
    PM->CTRLA.reg = PM_CTRLA_MASK & (~PM_CTRLA_IORET);

    /* disable brownout detection
     * (Caused unexplicable reboots from sleep on samr30. /KS)
     */
    SUPC->BOD33.bit.ENABLE=0;
#endif

    /* trigger static peripheral initialization */
    periph_init();
}
