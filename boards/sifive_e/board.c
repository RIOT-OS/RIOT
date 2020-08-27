/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_sifive_e
 * @{
 *
 * @file
 * @brief       Support for the Qemu sifive_e RISC-V board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "cpu.h"

void board_init(void)
{
    cpu_init();
}
