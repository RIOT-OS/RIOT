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

void led_init(void);

void board_init(void)
{
    /* initialize the CPU core */
    cpu_init();

    /* initialize LEDs */
    led_init();
}
