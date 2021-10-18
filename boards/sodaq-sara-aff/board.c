/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-sara-aff
 * @{
 *
 * @file
 * @brief       Board common implementations for the SODAQ SARA AFF boards
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    #ifdef AUTO_INIT_LED0
    /* The LED pin is also used for SPI, so we enable it
       only if explicitly wanted by the user */
    LED0_OFF;
    gpio_init(LED0_PIN, GPIO_OUT);
    #endif

    LED_RED_OFF;
    gpio_init(LED_RED_PIN, GPIO_OUT);
    LED_GREEN_OFF;
    gpio_init(LED_GREEN_PIN, GPIO_OUT);
    LED_BLUE_OFF;
    gpio_init(LED_BLUE_PIN, GPIO_OUT);

    /* set NB-IoT device off by default */
    NB_IOT_DISABLE;
    gpio_init(NB_IOT_ENABLE_PIN, GPIO_OUT);
    NB_IOT_TX_EN_OFF;
    gpio_init(NB_IOT_TX_EN_PIN, GPIO_OUT);
    NB_IOT_TOGGLE_ON;
    gpio_init(NB_IOT_TOGGLE_PIN, GPIO_OUT);

    /* set GPS off by default */
    GPS_ENABLE_OFF;
    gpio_init(GPS_ENABLE_PIN, GPIO_OUT);

}
