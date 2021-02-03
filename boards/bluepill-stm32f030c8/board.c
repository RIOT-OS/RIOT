/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_bluepill-stm32f030c8
 * @{
 *
 * @file
 * @brief       Board initialization code for the bluepill-stm32f030c8 board.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void board_init(void)
{
    cpu_init();
    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;
}
