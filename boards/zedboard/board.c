/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_zedboard
 * @{
 *
 * @file
 * @brief       Board specific implementations for the zedboard
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();
}
