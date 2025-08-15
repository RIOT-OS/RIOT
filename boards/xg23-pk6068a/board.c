/*
 * SPDX-FileCopyrightText: 2022 SSV Software Systems GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_xg23-pk6068a
 * @{
 *
 * @file
 * @brief       Board specific implementations for the xG23-PK6068A board
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

#ifndef RIOTBOOT
#ifdef MODULE_SI7021
    /* Enable Si7021 sensor */
    gpio_init(SI7021_EN_PIN, GPIO_OUT);
    gpio_set(SI7021_EN_PIN);
#endif
#endif
}
