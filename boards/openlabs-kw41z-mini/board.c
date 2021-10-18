/*
 * Copyright (C) 2017 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_openlabs-kw41z-mini
 * @{
 *
 * @file
 * @brief       Board specific initialization for openlabs-kw41z-mini
 *
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 *
 * @}
 */

#include "board.h"
#include "periph/rtc.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize and turn off LEDs */
    LED0_OFF;
    gpio_init(LED0_PIN, GPIO_OUT);

    /* enable OSCERCLK output on PTB3 */
#if PTB3_OUTPUT_OSCERCLK
    SIM->SOPT2 |= SIM_SOPT2_CLKOUTSEL(0b110);
    gpio_init_port(GPIO_PIN(PORT_B,  3), PORT_PCR_MUX(4));

    /* enable 32KHz oscillator output on PTB3 */
#elif PTB3_OUTPUT_OSC32K
    SIM->SOPT1 |= SIM_SOPT1_OSC32KOUT_MASK;
#endif
}
