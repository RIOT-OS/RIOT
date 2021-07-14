/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         boards_rpi_pico
 * @{
 *
 * @file
 * @brief           Board specific implementations for the Raspberry Pi Pico
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    gpio_init(LED0_PIN, GPIO_OUT);
}
