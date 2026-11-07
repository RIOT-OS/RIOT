/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_seeedstudio-sensecap-t1000e
 * @{
 *
 * @file
 * @brief       Board initialization for the SeeedStudio SenseCAP T1000-E
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 *
 * @}
 */

#include "board.h"
#include "kernel_defines.h"
#include "periph/gpio.h"

void board_init(void)
{
    if (IS_ACTIVE(CONFIG_T1000E_ENABLE_3V3)) {
        gpio_init(T1000E_3V3_EN_PIN, GPIO_OUT);
        gpio_set(T1000E_3V3_EN_PIN);
    }

    if (IS_ACTIVE(CONFIG_T1000E_ENABLE_SENSOR)) {
        gpio_init(T1000E_SENSOR_EN_PIN, GPIO_OUT);
        gpio_set(T1000E_SENSOR_EN_PIN);
    }

    if (IS_ACTIVE(CONFIG_T1000E_ENABLE_ACC)) {
        gpio_init(T1000E_3V3_ACC_EN_PIN, GPIO_OUT);
        gpio_set(T1000E_3V3_ACC_EN_PIN);
    }
}
