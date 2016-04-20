/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cc430
 * @{
 */

/**
 * @file
 * @brief       cc430 flashrom driver
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#include <stddef.h>
#include <stdint.h>
#include "cpu.h"
#include "irq.h"

static inline uint8_t prepare(void);
static inline void finish(uint8_t istate);
static inline void busy_wait(void);

/**
 * @TODO implement this function
 */
uint8_t flashrom_erase(uint8_t *addr)
{
    (void) addr;

    return 0;
}

/**
 * @TODO implement this function
 */
uint8_t flashrom_write(uint8_t *dst, const uint8_t *src, size_t size)
{
    (void) dst;
    (void) src;
    (void) size;

    return 0;
}

/**
 * @TODO implement this function
 */
static inline uint8_t prepare(void)
{
    return 0;
}

/**
 * @TODO implement this function
 */
static inline void finish(uint8_t istate)
{
    (void) istate;
}

static inline void busy_wait(void)
{
    /* Wait for BUSY = 0, not needed unless run from RAM */
    while (FCTL3 & 0x0001) {
        nop();
    }
}
