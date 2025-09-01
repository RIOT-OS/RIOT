/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
    /* The boot pin must be set to input otherwise it may lock the bootloader */
    gpio_init(BOOT_PIN, GPIO_IN);

    gpio_init(RF24_SWITCH_CC2538_PIN, GPIO_OUT);
    gpio_init(RF24_SWITCH_AT86RF215_PIN, GPIO_OUT);

#ifdef MODULE_CC2538_RF
    /* use cc2538 2.4ghz radio*/
    RF24_SWITCH_AT86RF215_OFF;
    RF24_SWITCH_CC2538_ON;
#else
    /* use at86rf215 2.4ghz radio*/
    RF24_SWITCH_CC2538_OFF;
    RF24_SWITCH_AT86RF215_ON;
#endif

}
