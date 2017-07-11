/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_onboard_led
 * @{
 *
 * @file
 * @brief       On-board LED abstraction implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "onboard_led.h"

void onboard_led_init(void)
{
    for (unsigned i = 0; i < LED_NUMOF; i++) {
        gpio_init(led_config[i], GPIO_OUT);
        onboard_led_off(i);
    }
}
