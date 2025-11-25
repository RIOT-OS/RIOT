/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_b-l072z-lrwan1
 * @{
 *
 * @file
 * @brief       Board specific implementations for the ST B-L072Z-LRWAN1 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
#if defined(MODULE_SX1276)
    /* Enable TCXO */
    gpio_init(RADIO_TCXO_VCC_PIN, GPIO_OUT);
    gpio_set(RADIO_TCXO_VCC_PIN);
#endif
}
