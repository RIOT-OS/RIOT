/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_seeedstudio-gd32
 * @{
 *
 * @file
 * @brief       Support for the SeeedStudio GD32 RISC-V board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* Initialize CPU and clocks */
    cpu_init();
}
