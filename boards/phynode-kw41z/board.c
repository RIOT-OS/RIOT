/*
 * Copyright (C) 2018 Eistec AB
 * Copyright (C) HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_phynode-kw41z
 * @{
 *
 * @file
 * @brief       Board specific initialization for the PhyNODE KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      José Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU core */
    cpu_init();

    /* initialize and turn off LEDs */
#ifdef LED0_PIN
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
#endif
#ifdef LED1_PIN
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_set(LED1_PIN);
#endif
#ifdef LED2_PIN
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_set(LED2_PIN);
#endif
#ifdef LED3_PIN
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_set(LED3_PIN);
#endif
}
