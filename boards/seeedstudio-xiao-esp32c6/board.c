/*
 * SPDX-FileCopyrightText: 2026 Stepan Konoplev
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* enable the RF switch (active low) */
    gpio_init(RF_SWITCH_EN_GPIO, GPIO_OUT);
    gpio_clear(RF_SWITCH_EN_GPIO);

    /* select antenna */
    gpio_init(RF_SWITCH_ANT_GPIO, GPIO_OUT);
    if (IS_ACTIVE(CONFIG_XIAO_ESP32C6_EXT_ANTENNA)) {
        gpio_set(RF_SWITCH_ANT_GPIO);
    }
    else {
        gpio_clear(RF_SWITCH_ANT_GPIO);
    }
}
