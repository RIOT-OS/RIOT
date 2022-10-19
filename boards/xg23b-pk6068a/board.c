/*
 * Copyright (C) 2022 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_xg23b-pk6068a
 * @{
 *
 * @file
 * @brief       Board specific implementations for the xG23B-PK6068A board
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* Enable VCOM iface */
    gpio_init(VCOM_EN_PIN, GPIO_OUT);
    gpio_set(VCOM_EN_PIN);
}
