/*
 * SPDX-FileCopyrightText: 2023 Bennet Blischke <bennet.blischke@haw-hamburg.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Test application for the pmp_noexec_ram pseudo-module
 *
 * @author Sören Tempel <tempel@uni-bremen.de>
 * @author Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "cpu.h"
#include "pmp.h"

#define JMPBUF_SIZE 3

int main(void)
{
    uint32_t buf[JMPBUF_SIZE];

    /* Fill the buffer with invalid instructions */
    for (unsigned i = 0; i < JMPBUF_SIZE; i++) {
        buf[i] = UINT32_MAX;
    }

    puts("Attempting to jump to stack buffer ...\n");
    __asm__ volatile ("jr %0" :: "r" ((uint8_t*)&buf));

    return 0;
}
