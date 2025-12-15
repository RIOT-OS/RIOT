/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-FileCopyrightText: 2019 Kees Bakker, SODAQ
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "board.h"
#include "cpu.h"
#include "usb_board_reset.h"

#ifdef HMCRAMC0_ADDR
#  define DBL_TAP_PTR ((volatile uint32_t *)(HMCRAMC0_ADDR + HMCRAMC0_SIZE - 4))
#else
#  define DBL_TAP_PTR ((volatile uint32_t *)(HSRAM_ADDR + HSRAM_SIZE - 4))
#endif

#if BOOTLOADER_UF2
#  define SAMD21_DOUBLE_TAP_ADDR            DBL_TAP_PTR
#  define SAMD21_DOUBLE_TAP_MAGIC_NUMBER    (0xF01669EFUL)
#else
#  define SAMD21_DOUBLE_TAP_ADDR            (0x20007FFCUL)
#  define SAMD21_DOUBLE_TAP_MAGIC_NUMBER    (0x07738135UL)
#endif

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
