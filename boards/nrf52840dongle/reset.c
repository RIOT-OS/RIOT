/*
 * Copyright (C)  2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dongle
 * @{
 * @file
 * @brief       Allows reboot into bootloader via hardware reset
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#ifdef MODULE_USB_BOARD_RESET

#include "periph/gpio.h"

/* P0.19 is connected to the RESET line */
#define BSP_SELF_PINRESET_PIN GPIO_PIN(0, 19)

void usb_board_reset_in_bootloader(void)
{
    gpio_init(BSP_SELF_PINRESET_PIN, GPIO_OUT);
    gpio_clear(BSP_SELF_PINRESET_PIN);
}

#endif /* MODULE_USB_BOARD_RESET */
