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
#include "board.h"

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
    while (0 == PM->INTFLAG.bit.PLRDY) {}

    MCLK->APBBMASK.reg |= MCLK_APBBMASK_NVMCTRL;
    _NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_RWS(1);
    MCLK->APBBMASK.reg &= ~MCLK_APBBMASK_NVMCTRL;

    /* set OSC16M to 16MHz */
    OSCCTRL->OSC16MCTRL.bit.FSEL = 3;
    OSCCTRL->OSC16MCTRL.bit.ONDEMAND = 0;
    OSCCTRL->OSC16MCTRL.bit.RUNSTDBY = 0;

    /* Setup GCLK generators */
    _gclk_setup(0, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC16M);

    /* trigger static peripheral initialization */
    periph_init();
}
