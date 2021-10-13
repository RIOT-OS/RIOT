/*
 * Copyright (C)  2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840-mdk-dongle
 * @{
 * @file
 * @brief       Implementation for managing the UF2 bootloader
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#ifdef MODULE_USB_BOARD_RESET

#define USB_H_USER_IS_RIOT_INTERNAL

#include "cpu.h"
#include "usb_board_reset.h"

/* from uf2-bootloader src/main.c */
#define DFU_MAGIC_UF2_RESET 0x57

void usb_board_reset_in_bootloader(void)
{
    NRF_POWER->GPREGRET = DFU_MAGIC_UF2_RESET;

    usb_board_reset_in_application();
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_USB_BOARD_RESET */
