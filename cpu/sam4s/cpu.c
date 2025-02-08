/*
 * Copyright (C) 2025 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam4s
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "kernel_init.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

#define XTAL_STARTUP        (8U)
#define PLL_CNT             (64U)

#if ((CLOCK_PLL_MUL < 7) || (CLOCK_PLL_MUL > 62))
#error "CLOCK_PLL_MUL has an incorrect value"
#endif

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* SAM4s MCUs requires WPKEY for enabling peripheral */
    PMC->PMC_WPMR = PMC_WPMR_WPKEY_PASSWD;

    /* disable the watchdog timer */
    WDT->WDT_MR |= WDT_MR_WDDIS;

    /* initialize the Cortex-M core */
    cortexm_init();

    /* setup the flash wait states */
    EFC0->EEFC_FMR = EEFC_FMR_FWS(CLOCK_FWS);
    EFC1->EEFC_FMR = EEFC_FMR_FWS(CLOCK_FWS);

    /* enable the Cortex-M Cache Controller */
    CMCC->CMCC_CTRL |= CMCC_CTRL_CEN;

    /* unlock write protect register for PMC module */
    PMC->PMC_WPMR = PMC_WPMR_WPKEY_PASSWD;

    /* activate the external crystal */
    PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD
                  | CKGR_MOR_MOSCXTST(XTAL_STARTUP)
                  | CKGR_MOR_MOSCXTEN
                  | CKGR_MOR_MOSCRCEN;

    /* wait for crystal to be stable */
    while (!(PMC->PMC_SR & PMC_SR_MOSCXTS)) {}

    /* select crystal to clock the main clock */
    PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD
                  | CKGR_MOR_MOSCXTST(XTAL_STARTUP)
                  | CKGR_MOR_MOSCXTEN
                  | CKGR_MOR_MOSCRCEN
                  | CKGR_MOR_MOSCSEL;

    /* wait for main oscillator selection to be complete */
    while (!(PMC->PMC_SR & PMC_SR_MOSCSELS)) {}

    /* setup PLLA */
    PMC->CKGR_PLLAR = CKGR_PLLAR_ONE
                    | CKGR_PLLAR_PLLACOUNT(PLL_CNT)
                    | CKGR_PLLAR_MULA(CLOCK_PLL_MUL)
                    | CKGR_PLLAR_DIVA(CLOCK_PLL_DIV);

    /* wait for PLL to lock */
    while (!(PMC->PMC_SR & PMC_SR_LOCKA)) {}

    /* before switching to PLLA, we need to switch to main clock */
    PMC->PMC_MCKR = PMC_MCKR_CSS_MAIN_CLK;
    while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}

    /* use PLLA as main clock source */
    PMC->PMC_MCKR = PMC_MCKR_CSS_PLLA_CLK;
    /* wait for master clock to be ready */
    while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}

    /* setup the SCLK: switch to external oscillator if applicable */
#if CLOCK_SCLK_XTAL
    /* enable external oscillator */
    SUPC->SUPC_CR = SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL;
    while (!(SUPC->SUPC_SR & SUPC_SR_OSCSEL_CRYST)) {}
#endif

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
