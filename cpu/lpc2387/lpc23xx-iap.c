/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup lpc2387
 * @{
 */

/**
 * @file
 * @brief       lpc2387 in-application programming driver (for flashrom)
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#include <stdint.h>
#include "flashrom.h"
#include "iap.h"

uint8_t iap_get_sector(uint32_t addr)
{
    if (addr <= 0x00000FFF) {
        return 0;
    }

    if ((addr >= 0x00001000) && (addr <= 0x00001FFF)) {
        return 1;
    }

    if ((addr >= 0x00002000) && (addr <= 0x00002FFF)) {
        return 2;
    }

    if ((addr >= 0x00003000) && (addr <= 0x00003FFF)) {
        return 3;
    }

    if ((addr >= 0x00004000) && (addr <= 0x00004FFF)) {
        return 4;
    }

    if ((addr >= 0x00005000) && (addr <= 0x00005FFF)) {
        return 5;
    }

    if ((addr >= 0x00006000) && (addr <= 0x00006FFF)) {
        return 6;
    }

    if ((addr >= 0x00007000) && (addr <= 0x00007FFF)) {
        return 7;
    }

    if ((addr >= 0x00008000) && (addr <= 0x0000FFFF)) {
        return 8;
    }

    if ((addr >= 0x00010000) && (addr <= 0x00017FFF)) {
        return 9;
    }

    if ((addr >= 0x00018000) && (addr <= 0x0001FFFF)) {
        return 10;
    }

    if ((addr >= 0x00020000) && (addr <= 0x00027FFF)) {
        return 11;
    }

    if ((addr >= 0x00028000) && (addr <= 0x0002FFFF)) {
        return 12;
    }

    if ((addr >= 0x00030000) && (addr <= 0x00037FFF)) {
        return 13;
    }

    if ((addr >= 0x00038000) && (addr <= 0x0003FFFF)) {
        return 14;
    }

    if ((addr >= 0x00040000) && (addr <= 0x00047FFF)) {
        return 15;
    }

    if ((addr >= 0x00048000) && (addr <= 0x0004FFFF)) {
        return 16;
    }

    if ((addr >= 0x00050000) && (addr <= 0x00057FFF)) {
        return 17;
    }

    if ((addr >= 0x00058000) && (addr <= 0x0005FFFF)) {
        return 18;
    }

    if ((addr >= 0x00060000) && (addr <= 0x00067FFF)) {
        return 19;
    }

    if ((addr >= 0x00068000) && (addr <= 0x0006FFFF)) {
        return 20;
    }

    if ((addr >= 0x00070000) && (addr <= 0x00077FFF)) {
        return 21;
    }

    if ((addr >= 0x00078000) && (addr <= 0x00078FFF)) {
        return 22;
    }

    if ((addr >= 0x00079000) && (addr <= 0x00079FFF)) {
        return 23;
    }

    if ((addr >= 0x0007A000) && (addr <= 0x0007AFFF)) {
        return 24;
    }

    if ((addr >= 0x0007B000) && (addr <= 0x0007BFFF)) {
        return 25;
    }

    if ((addr >= 0x0007C000) && (addr <= 0x0007CFFF)) {
        return 26;
    }

    if ((addr >= 0x0007D000) && (addr <= 0x0007DFFF)) {
        return 27;
    }

    /* no valid address within flash */
    return INVALID_ADDRESS;
}
