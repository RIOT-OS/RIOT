/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f0discovery
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F0Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/uart.h"


static void leds_init(void);
static void hse_init(void);


void board_init(void)
{
    /* initialize the boards LEDs */
    leds_init();

    /* initialize the CPU */
    cpu_init();

    /* initialize the HSE clock */
    hse_init();
}

/**
 * @brief Initialize the boards on-board LEDs (LD3 and LD4)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LD3: PC8
 * - LD4: PC9
 */
void leds_init(void)
{
    /* enable clock for port GPIOC */
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    /* set output speed to 50MHz */
    LED_PORT->OSPEEDR |= 0x000f0000;
    /* set output type to push-pull */
    LED_PORT->OTYPER &= ~(0x00000300);
    /* configure pins as general outputs */
    LED_PORT->MODER &= ~(0x000f0000);
    LED_PORT->MODER |= 0x00050000;
    /* disable pull resistors */
    LED_PORT->PUPDR &= ~(0x000f0000);

    /* turn all LEDs off */
    LED_PORT->BRR = 0x0300;
}

/*
 * @brief Configure the controllers HSE clock
 *
 * The clock initialization makes the following assumptions:
 * - the external HSE clock from an external oscillator is used as base clock
 * - the internal PLL circuit is used for clock refinement
 *
 * Use the following formulas to calculate the needed values:
 *
 * SYSCLK = ((HSE_VALUE / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_P
 * USB, SDIO and RNG Clock =  ((HSE_VALUE / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_Q
 *
 * The actual used values are specified in the board's `periph_conf.h` file.
 *
 * NOTE: currently there is no timeout for initialization of PLL and other locks
 *       -> when wrong values are chosen, the initialization could stall
 */
static void hse_init(void)
{
    /* configure the HSE clock */

    /* enable the HSE clock */
    RCC->CR |= RCC_CR_HSEON;

    /* wait for HSE to be ready */
    while (!(RCC->CR & RCC_CR_HSERDY));


    /* configure the PLL */

    /* reset PLL configuration bits */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
    /* set PLL configuration */
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1 |
                 (((CLOCK_PLL_MUL - 2) & 0xf) << 18);

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
