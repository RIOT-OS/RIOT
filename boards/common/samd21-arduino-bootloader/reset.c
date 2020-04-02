/*
 * Copyright (C)  2019 Inria
 *                2019 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common
 * @{
 * @file
 * @brief       Board common implementations for managing an Arduino bootloader
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Kees Bakker <kees@sodaq.com>
 *
 * @}
 */

#ifdef MODULE_USB_BOARD_RESET

#define USB_H_USER_IS_RIOT_INTERNAL

#include "usb_board_reset.h"


#define SAMD21_DOUBLE_TAP_ADDR              (0x20007FFCUL)
#define SAMD21_DOUBLE_TAP_MAGIC_NUMBER      (0x07738135UL)

void usb_board_reset_in_bootloader(void)
{
    /* The Arduino bootloader checks for a magic number in SRAM to remain in
       bootloader mode.
       See
       https://github.com/arduino/ArduinoCore-samd/blob/master/bootloaders/zero/board_definitions_arduino_mkr1000.h#L38
       and
       https://github.com/arduino/ArduinoCore-samd/blob/master/bootloaders/zero/main.c#L94
       for implementation details. */
    uint32_t *reset_addr = (uint32_t *)SAMD21_DOUBLE_TAP_ADDR;
    *reset_addr = (uint32_t)SAMD21_DOUBLE_TAP_MAGIC_NUMBER;

    usb_board_reset_in_application();
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_USB_BOARD_RESET */
