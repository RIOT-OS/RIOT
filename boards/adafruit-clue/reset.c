/*
 * Copyright (C)  2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-clue
 * @{
 * @file
 * @brief       Implementation for managing the nrfutil bootloader
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifdef MODULE_USB_BOARD_RESET

#define USB_H_USER_IS_RIOT_INTERNAL

#include "cpu.h"
#include "usb_board_reset.h"

/* Set the value used by the bootloader to select between boot in
   application and boot in bootloader mode. */
#define NRF52_DOUBLE_TAP_MAGIC_NUMBER       (0x4e)

void usb_board_reset_in_bootloader(void)
{
    NRF_POWER->GPREGRET = NRF52_DOUBLE_TAP_MAGIC_NUMBER;

    usb_board_reset_in_application();
}

#endif /* MODULE_USB_BOARD_RESET */
