/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
