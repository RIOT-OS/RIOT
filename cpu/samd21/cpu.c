/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

#ifndef CLOCK_8MHZ
#define CLOCK_8MHZ          1
#endif

#ifndef GEN2_ULP32K
#define GEN2_ULP32K         1
#endif

#ifndef GEN3_ULP32K
#define GEN3_ULP32K         GEN2_ULP32K
#endif

#ifndef XOSC32_STARTUP_TIME
/**
 * @brief   XOSC32 start up time
 *
 * @note    Override this value in your boards periph_conf.h file
 *          if a different start up time is to be used.
 */
#define XOSC32_STARTUP_TIME      6
#endif

#ifndef VDD
/**
 * @brief   Set system voltage level in mV (determines flash wait states)
 *
 * @note    Override this value in your boards periph_conf.h file
 *          if a different system voltage is used.
 */
#define VDD                 (3300U)
#endif

/* determine the needed flash wait states based on the system voltage (Vdd)
 * see SAMD21 datasheet Rev A (2017) table 37-40 , page 816 */
#if (VDD > 2700)
#define WAITSTATES          ((CLOCK_CORECLOCK - 1) / 24000000)
#else
#define WAITSTATES          ((CLOCK_CORECLOCK - 1) / 14000000)
#endif

#ifndef GCLK_GENCTRL_SRC_FDPLL
#define GCLK_GENCTRL_SRC_FDPLL_Val  _U_(0x8)
#define GCLK_GENCTRL_SRC_FDPLL      (GCLK_GENCTRL_SRC_FDPLL_Val    << GCLK_GENCTRL_SRC_Pos)
#endif

void sam0_gclk_enable(uint8_t id)
{
    (void) id;
    /* clocks are always running */
}

uint32_t sam0_gclk_freq(uint8_t id)
{
    switch (id) {
    case SAM0_GCLK_MAIN:
        return CLOCK_CORECLOCK;
    case SAM0_GCLK_1MHZ:
        return 1000000;
    case SAM0_GCLK_32KHZ:
        return 32768;
    case SAM0_GCLK_1KHZ:
        return 1024;
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
    (void) deep;
    /* will be called after wake-up */
}

/**
 * @brief   Configure clock sources and the cpu frequency
 */
static void clk_init(void)
{
    /* enable clocks for the power, sysctrl and gclk modules */
    PM->APBAMASK.reg = (PM_APBAMASK_PM | PM_APBAMASK_SYSCTRL |
                        PM_APBAMASK_GCLK);

    /* adjust NVM wait states */
    PM->APBBMASK.reg |= PM_APBBMASK_NVMCTRL;
    NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_RWS(WAITSTATES);
    PM->APBBMASK.reg &= ~PM_APBBMASK_NVMCTRL;

#if CLOCK_8MHZ
    /* configure internal 8MHz oscillator to run without prescaler */
    SYSCTRL->OSC8M.bit.PRESC = 0;
    SYSCTRL->OSC8M.bit.ONDEMAND = 1;
    SYSCTRL->OSC8M.bit.RUNSTDBY = 0;
    SYSCTRL->OSC8M.bit.ENABLE = 1;
    while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC8MRDY)) {}
#endif

#if CLOCK_USE_XOSC32_DFLL || !GEN2_ULP32K || !GEN3_ULP32K
    /* Use External 32.768KHz Oscillator */
    SYSCTRL->XOSC32K.reg =  SYSCTRL_XOSC32K_EN32K |
                            SYSCTRL_XOSC32K_XTALEN |
                            SYSCTRL_XOSC32K_STARTUP(XOSC32_STARTUP_TIME) |
                            SYSCTRL_XOSC32K_RUNSTDBY;
    /* Enable XOSC32 with Separate Call */
    SYSCTRL->XOSC32K.bit.ENABLE = 1;
#endif

    /* reset the GCLK module so it is in a known state */
    GCLK->CTRL.reg = GCLK_CTRL_SWRST;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

    /* Setup GCLK2 with divider 1 (32.768kHz) */
    GCLK->GENDIV.reg  = (GCLK_GENDIV_ID(SAM0_GCLK_32KHZ)  | GCLK_GENDIV_DIV(0));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_ID(SAM0_GCLK_32KHZ) | GCLK_GENCTRL_GENEN
                      | GCLK_GENCTRL_RUNSTDBY
#if GEN2_ULP32K
                      | GCLK_GENCTRL_SRC_OSCULP32K);
#else
                      | GCLK_GENCTRL_SRC_XOSC32K);
#endif


#if CLOCK_USE_PLL
    /* setup generic clock 1 to feed DPLL with 1MHz */
    GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(8) |
                        GCLK_GENDIV_ID(SAM0_GCLK_1MHZ));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN |
                         GCLK_GENCTRL_SRC_OSC8M |
                         GCLK_GENCTRL_ID(SAM0_GCLK_1MHZ));
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_GEN(SAM0_GCLK_1MHZ) |
                         GCLK_CLKCTRL_ID_FDPLL |
                         GCLK_CLKCTRL_CLKEN);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

    /* enable PLL */
    SYSCTRL->DPLLRATIO.reg = (SYSCTRL_DPLLRATIO_LDR(CLOCK_PLL_MUL));
    SYSCTRL->DPLLCTRLB.reg = (SYSCTRL_DPLLCTRLB_REFCLK_GCLK);
    SYSCTRL->DPLLCTRLA.reg = (SYSCTRL_DPLLCTRLA_ENABLE);
    while(!(SYSCTRL->DPLLSTATUS.reg &
           (SYSCTRL_DPLLSTATUS_CLKRDY | SYSCTRL_DPLLSTATUS_LOCK))) {}

    /* select the PLL as source for clock generator 0 (CPU core clock) */
    GCLK->GENDIV.reg =  (GCLK_GENDIV_DIV(CLOCK_PLL_DIV) |
                        GCLK_GENDIV_ID(SAM0_GCLK_MAIN));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN |
                         GCLK_GENCTRL_SRC_FDPLL |
                         GCLK_GENCTRL_ID(SAM0_GCLK_MAIN));
#elif CLOCK_USE_XOSC32_DFLL
    /* setup generic clock 1 as 1MHz for timer.c */
    GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(8) |
                        GCLK_GENDIV_ID(SAM0_GCLK_1MHZ));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN |
                         GCLK_GENCTRL_SRC_OSC8M |
                         GCLK_GENCTRL_ID(SAM0_GCLK_1MHZ));
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

    /* set GCLK2 as source for DFLL */
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_GEN(SAM0_GCLK_32KHZ) |
                         GCLK_CLKCTRL_ID_DFLL48 |
                         GCLK_CLKCTRL_CLKEN);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

    /* Disable ONDEMAND mode while writing configurations */
    SYSCTRL->DFLLCTRL.bit.ONDEMAND = 0;
    while ((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0) {
        /* Wait for DFLL sync */
    }

    /* get the coarse and fine values stored in NVM (Section 9.3) */
    uint32_t coarse = (*(uint32_t *)(0x806024) >> 26);  /* Bits 63:58 */
    uint32_t fine = (*(uint32_t *)(0x806028) & 0x3FF);  /* Bits 73:64 */

    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP(coarse >> 1) |
                           SYSCTRL_DFLLMUL_FSTEP(fine >> 1) |
                           SYSCTRL_DFLLMUL_MUL(CLOCK_CORECLOCK / CLOCK_XOSC32K);
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) |
                           SYSCTRL_DFLLVAL_FINE(fine);
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_MODE;
    while ((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0) {
        /* Wait for DFLL sync */
    }

    SYSCTRL->DFLLCTRL.bit.ENABLE = 1;
    uint32_t mask = SYSCTRL_PCLKSR_DFLLRDY |
                    SYSCTRL_PCLKSR_DFLLLCKF |
                    SYSCTRL_PCLKSR_DFLLLCKC;
    while ((SYSCTRL->PCLKSR.reg & mask) != mask) { } /* Wait for DFLL lock */

    /* select the DFLL as source for clock generator 0 (CPU core clock) */
    GCLK->GENDIV.reg =  (GCLK_GENDIV_DIV(1U) | GCLK_GENDIV_ID(SAM0_GCLK_MAIN));
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN
                      | GCLK_GENCTRL_SRC_DFLL48M
                      | GCLK_GENCTRL_ID(SAM0_GCLK_MAIN);
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_ID_DFLL48
                      | GCLK_CLKCTRL_GEN(SAM0_GCLK_MAIN);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

    SYSCTRL->DFLLCTRL.bit.ONDEMAND = 1;
    while ((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0) {
        /* Wait for DFLL sync */
    }
#else /* do not use PLL, use internal 8MHz oscillator directly */
    GCLK->GENDIV.reg =  (GCLK_GENDIV_DIV(CLOCK_DIV) |
                        GCLK_GENDIV_ID(SAM0_GCLK_MAIN));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN |
                         GCLK_GENCTRL_SRC_OSC8M |
                         GCLK_GENCTRL_ID(SAM0_GCLK_MAIN));
#endif

    /* make sure we synchronize clock generator 0 before we go on */
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

    /* Setup GCLK3 with divider 32 (1024 Hz) */
    GCLK->GENDIV.reg  = (GCLK_GENDIV_ID(SAM0_GCLK_1KHZ)  | GCLK_GENDIV_DIV(4));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_ID(SAM0_GCLK_1KHZ) | GCLK_GENCTRL_GENEN
                      | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_DIVSEL
#if GEN3_ULP32K
                      | GCLK_GENCTRL_SRC_OSCULP32K);
#else
                      | GCLK_GENCTRL_SRC_XOSC32K);
#endif

    /* redirect all peripherals to a disabled clock generator (7) by default */
    for (int i = 0x3; i <= 0x22; i++) {
        GCLK->CLKCTRL.reg = ( GCLK_CLKCTRL_ID(i) | GCLK_CLKCTRL_GEN_GCLK7 );
        while (GCLK->STATUS.bit.SYNCBUSY) {}
    }
}

void cpu_init(void)
{
    /* disable the watchdog timer */
    WDT->CTRL.bit.ENABLE = 0;
    /* initialize the Cortex-M core */
    cortexm_init();
    /* Initialise clock sources and generic clocks */
    clk_init();
#ifdef MODULE_PERIPH_DMA
    /*  initialize DMA streams */
    dma_init();
#endif
    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();
    /* trigger static peripheral initialization */
    periph_init();
}
