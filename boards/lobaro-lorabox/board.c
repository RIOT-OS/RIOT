/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_lobaro_lorabox
 * @{
 *
 * @file
 * @brief       Board initialization code for Lobaro LoraBox
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    gpio_init(EN3V3_PIN, GPIO_OUT);
}
