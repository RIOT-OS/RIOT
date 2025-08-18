/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
