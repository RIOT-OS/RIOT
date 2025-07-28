/*
 * SPDX-FileCopyrightText: 2017 Thomas Stilwell <stilwellt@openlabs.co>
 * SPDX-License-Identifier: LGPL-2.1-only
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

    /* enable OSCERCLK output on PTB3 */
#if PTB3_OUTPUT_OSCERCLK
    SIM->SOPT2 |= SIM_SOPT2_CLKOUTSEL(0b110);
    gpio_init_port(GPIO_PIN(PORT_B,  3), PORT_PCR_MUX(4));

    /* enable 32KHz oscillator output on PTB3 */
#elif PTB3_OUTPUT_OSC32K
    SIM->SOPT1 |= SIM_SOPT1_OSC32KOUT_MASK;
#endif
}
