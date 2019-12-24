/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_openmote-b
 * @{
 *
 * @file
 * @brief       Board specific implementations for the OpenMote-B board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
    /* The boot pin must be set to input otherwise it may lock the bootloader */
    gpio_init(BOOT_PIN, GPIO_IN);

    gpio_init(BTN0_PIN, BTN0_MODE);
    gpio_init(RF24_SWITCH_CC2538_PIN, GPIO_OUT);
    gpio_init(RF24_SWITCH_AT86RF215_PIN, GPIO_OUT);

    /* start with cc2538 2.4ghz radio*/
    RF24_SWITCH_CC2538_ON;
    RF24_SWITCH_AT86RF215_OFF;

    /* initialize the CPU */
    cpu_init();
}
