/*
 * Copyright (C) 2019 Kees Bakker
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-sara-sff
 * @{
 *
 * @file
 * @brief       Board common implementations for the SODAQ SARA SFF board
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* Disable power to the SARA/UBlox by default */
    SARA_ENABLE_OFF;
    gpio_init(SARA_ENABLE_PIN, GPIO_OUT);

    /* Disable level shifters SARA/UBlox by default */
    SARA_TX_ENABLE_OFF;
    gpio_init(SARA_TX_ENABLE_PIN, GPIO_OUT);

    /* The R4XX module has a PWR_ON pin. Make it low to
     * switch on the module.
     * Here it is made an input so that it will float HIGH.
     */
    SARA_R4XX_PWR_ON_ON;
    gpio_init(SARA_R4XX_PWR_ON_PIN, GPIO_IN);

    /* Disable GPS by default */
    GPS_ENABLE_OFF;
    gpio_init(GPS_ENABLE_PIN, GPIO_OUT);

    /* Initialize the output ports for the LEDs */
    LED_RED_OFF;
    gpio_init(LED_RED_PIN, GPIO_OUT);

    LED_BLUE_OFF;
    gpio_init(LED_BLUE_PIN, GPIO_OUT);

    LED_GREEN_OFF;
    gpio_init(LED_GREEN_PIN, GPIO_OUT);

    /* initialize the CPU */
    cpu_init();
}
