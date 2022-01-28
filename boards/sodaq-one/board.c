/*
 * Copyright (C) 2017 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-one
 * @{
 *
 * @file
 * @brief       Board common implementations for the SODAQ ONE board
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{

    /* reset RN2483 (LoRa) */
    LORA_RESET_OFF;
    gpio_init(LORA_RESET_PIN, GPIO_OUT);

    GPS_ENABLE_OFF;
    gpio_init(GPS_ENABLE_PIN, GPIO_OUT);

}
