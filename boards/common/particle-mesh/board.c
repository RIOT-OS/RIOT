/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_particle-mesh
 * @{
 *
 * @file
 * @brief       Common board initialization for the Particle Mesh
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"

void board_nrfantenna_select(enum board_nrfantenna_selection choice)
{
    switch (choice) {
        case BOARD_NRFANTENNA_BUILTIN:
            /* Suppress output to the UFL connector */
            gpio_set(VCTL1_PIN);
#ifdef VCTL2_PIN
            /* Enable output to the built-in antenna */
            gpio_clear(VCTL2_PIN);
#endif
            break;
        case BOARD_NRFANTENNA_EXTERNAL:
            gpio_clear(VCTL1_PIN);
#ifdef VCTL2_PIN
            gpio_set(VCTL2_PIN);
#endif
            break;
    }
}

void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_set(LED1_PIN);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_set(LED2_PIN);

    gpio_init(VCTL1_PIN, GPIO_OUT);
#ifdef VCTL2_PIN
    /* On boards without VCLT2_PIN (Boron), the VCTL2 net is driven by NOT(VCTL1) */
    gpio_init(VCTL2_PIN, GPIO_OUT);
#endif

    board_nrfantenna_select(BOARD_NRFANTENNA_DEFAULT);

    /* initialize the CPU */
    cpu_init();
}
