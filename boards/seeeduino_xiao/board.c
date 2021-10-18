/*
 * Copyright (C)    2021 Franz Freitag, Justus Krebs, Nick Weiler
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeeduino_xiao
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Seeeduino XIAO board
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "timex.h"

void board_init(void)
{
    /* initialize the on-board LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    LED0_OFF;
    LED1_OFF;
    LED2_OFF;
}
