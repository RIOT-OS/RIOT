/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_common_kw41z
 * @{
 *
 * @file
 * @brief       Board specific initialization for the FRDM-KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

static void led_init(void)
{
    /* initialize and turn off LEDs */
#ifdef LED0_PIN
    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;
#endif
#ifdef LED1_PIN
    gpio_init(LED1_PIN, GPIO_OUT);
    LED1_OFF;
#endif
#ifdef LED2_PIN
    gpio_init(LED2_PIN, GPIO_OUT);
    LED2_OFF;
#endif
#ifdef LED3_PIN
    gpio_init(LED3_PIN, GPIO_OUT);
    LED3_OFF;
#endif
}

void board_init(void)
{
    /* initialize the CPU core */
    cpu_init();

    /* initialize LEDs, skip for riotboot */
    if (!IS_ACTIVE(RIOTBOOT)) {
        led_init();
    }
}
