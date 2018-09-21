/*
 * Copyright (C)  2018 Nalys bvba
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_olimex-stm32-p405
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Olimex STM32-P405
 *
 * @author      Toon Stegen <tstegen@nalys-group.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the boards LED */
    gpio_init(LED0_PIN, GPIO_OUT);

    /* initialize the CPU */
    cpu_init();
}
