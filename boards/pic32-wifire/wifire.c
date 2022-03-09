/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include <stdint.h>
#include "periph/gpio.h"
#include "periph/hwrng.h"
#include "bitarithm.h"
#include "board.h"
#include "cpu.h"

extern void dummy(void);

void board_init(void)
{
    /* Stop the linker from throwing away the PIC32 config register settings */
    dummy();
}
