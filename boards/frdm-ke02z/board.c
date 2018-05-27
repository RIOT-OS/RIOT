/*
 * Copyright (C) 2018 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-ke02z
 * @{
 *
 * @file
 * @brief       Board specific implementations for the FRDM-KE02Z
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* Initialize the Cortex-M0+ core */
    cpu_init();

    /* System clock initialization */

    /* Set BUS_CLOCK = CORE_CLOCK  */
    bit_clear32(&(SIM->BUSDIV), SIM_BUSDIV_BUSDIV_SHIFT);

    /* Trim internal reference clock (FLL input) */
    ICS->C3 = ICS_C3_SCTRIM(ICS_TRIM_VALUE);

    /* Wait for the FLL to lock on target frequency */
    while(!(ICS->S & ICS_S_LOCK_MASK)) {}

    /* Configure the RGB LED pins to be output */
    gpio_init(LED_RGB_RED_PIN, GPIO_OUT);
    gpio_init(LED_RGB_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_RGB_BLUE_PIN, GPIO_OUT);

    /* Turn off the RGB LED */
    gpio_set(LED_RGB_RED_PIN);
    gpio_set(LED_RGB_GREEN_PIN);
    gpio_set(LED_RGB_BLUE_PIN);
}
