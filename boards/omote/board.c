/*
 * SPDX-FileCopyrightText: 2020 Oppila Microsystems - http://www.oppila.in
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_omote
 * @{
 *
 * @file
 * @brief       Board specific implementations for the omote board
 * @}
 */

#include "board.h"
#include "cpu.h"

void board_init(void)
{
    /* The boot pin must be set to input otherwise it may lock the bootloader */
    gpio_init(BOOT_PIN, GPIO_IN);
}
