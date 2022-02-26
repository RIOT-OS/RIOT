/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_init
 * @{
 *
 * @file
 * @brief       Init on board Buttons as input
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "kernel_defines.h"

__attribute__ ((weak))
void button_init(void)
{
    if (!IS_USED(MODULE_PERIPH_GPIO)) {
        return;
    }

#ifdef BTN0_PIN
    gpio_init(BTN0_PIN, BTN0_MODE);
#endif
#ifdef BTN1_PIN
    gpio_init(BTN1_PIN, BTN1_MODE);
#endif
#ifdef BTN2_PIN
    gpio_init(BTN2_PIN, BTN2_MODE);
#endif
#ifdef BTN3_PIN
    gpio_init(BTN3_PIN, BTN3_MODE);
#endif
#ifdef BTN4_PIN
    gpio_init(BTN4_PIN, BTN4_MODE);
#endif
#ifdef BTN5_PIN
    gpio_init(BTN5_PIN, BTN5_MODE);
#endif
#ifdef BTN6_PIN
    gpio_init(BTN6_PIN, BTN6_MODE);
#endif
#ifdef BTN7_PIN
    gpio_init(BTN7_PIN, BTN7_MODE);
#endif
}
