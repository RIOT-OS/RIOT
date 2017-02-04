/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */



#include <stdio.h>

#include "periph/uart.h"

#define MIPS_MALTA_ADDR (0xbf000500)
#define MIPS_MALTA_VAL_RST (0x42)

static void malta_reset(void)
{
    *(volatile long *)MIPS_MALTA_ADDR = MIPS_MALTA_VAL_RST;
    asm volatile ("wait");
}

void board_init(void)
{
    /* Board initialisation is done by the bootloader (u-boot) on Malta */
}

void reboot(void)
{
    malta_reset();
}
