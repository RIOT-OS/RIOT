/*
 * Copyright (C)  2020 Bruno Chianca
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-itsybitsy-nrf52
 * @{
 * @file
 * @brief       Allows reboot into bootloader via prompt
 *
 * @author      Bruno Chianca
 *
 * @}
 */

#ifdef MODULE_USB_BOARD_RESET

#include "periph/gpio.h"
#include "usb_board_reset.h"

#define NRF52_DOUBLE_TAP_MAGIC_NUMBER       (0x4e)

void usb_board_reset_in_bootloader(void)
{
    NRF_POWER->GPREGRET = NRF52_DOUBLE_TAP_MAGIC_NUMBER;

    usb_board_reset_in_application();
}

#endif /* MODULE_USB_BOARD_RESET */
