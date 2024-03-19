/*
 * Copyright (C) 2020 Oppila Microsystems - http://www.oppila.in
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
