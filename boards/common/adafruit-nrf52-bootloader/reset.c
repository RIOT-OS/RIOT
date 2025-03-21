/*
 * Copyright (C)  2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_adafruit-nrf52-bootloader
 * @{
 * @file
 * @brief       Reset Logic for the Adafruit nRF52 Bootloader
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
#define NRF52_DOUBLE_TAP_MAGIC_NUMBER       (0x57)

void usb_board_reset_in_bootloader(void)
{
    NRF_POWER->GPREGRET = NRF52_DOUBLE_TAP_MAGIC_NUMBER;

    /* Going with a hard reset rather than a pm_off, as that might be altered
     * to do *anything* -- which is not safe any more now that we've forsaken
     * the RAM content */
    NVIC_SystemReset();
}

#endif /* MODULE_USB_BOARD_RESET */
