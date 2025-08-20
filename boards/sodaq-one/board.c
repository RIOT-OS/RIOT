/*
 * SPDX-FileCopyrightText: 2017 Kees Bakker, SODAQ
 * SPDX-License-Identifier: LGPL-2.1-only
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
