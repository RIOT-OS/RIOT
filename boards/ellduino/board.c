/*
 * Copyright (C) 2014 ELL-i co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_ellduino
 * @{
 *
 * @file
 * @brief       Board specific implementations for the ELL-i ellduino board
 *
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
 *
 * @}
 */

#include "board.h"
#include "periph/uart.h"

static void clock_init(void);

void board_init(void)
{

    /* initialize the CPU */
    cpu_init();

    /* Initialize the HSI+PLL clocks as the board does not have external
     * clock source.
     */
    clock_init();
}


/**
 * @brief Configure the controllers clock system
 *
 * The clock initialization makes the following assumptions:
 * - the internal HSI clock is used as base clock
 * - the internal PLL circuit is used for clock refinement
 *
 * Use the following formulas to calculate the needed values:
 *
 * SYSCLK = ((HSI_VALUE / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_P
 * SDIO and RNG Clock =  ((HSI_VALUE / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_Q
 *
 * The actual used values are specified in the board's `periph_conf.h` file.
 *
 * NOTE: currently there is no timeout for initialization of PLL and other locks
 *       -> when wrong values are chosen, the initialization could stall
 */
void clock_init(void) {
    /* set PLL configuration */
	RCC->CFGR &= (~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL));
	RCC->CFGR |= (RCC_CFGR_PLLSRC_HSI_DIV2 |
                    RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV2 |
                    RCC_CFGR_PLLMUL12);

    /* enable PLL again */
    RCC->CR |= RCC_CR_PLLON;

    /* wait until PLL is stable */
    while(!(RCC->CR & RCC_CR_PLLRDY));


    /* configure the sysclock and the peripheral clocks */

    /* set sysclock to be driven by the PLL clock */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* wait for sysclock to be stable */
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}
