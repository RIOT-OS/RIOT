/*
 * Copyright (C) 2018 Tristan Bruns
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_kw41z-mini
 * @{
 *
 * @file
 * @brief       Board specific initialization for the kw41z-mini
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Maximilian Luenert <malu@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
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
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
}
