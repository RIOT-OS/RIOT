/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "board.h"

void board_init(void) {
    /* Re-enable the LED0 pin
     * Otherwise the LED will not work after a reset
     * This is needed, esp. when the LED is used via
     * the define macros */
    gpio_init(LED0_PIN_ID, GPIO_OUT);

    gpio_init(OSC_DEBUG_PIN_ID, GPIO_OUT);
}
