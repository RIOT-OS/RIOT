/*
 * Copyright (C) 2014 INRIA
 *
 * The source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
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
 *
 */

#include <stddef.h>
#include <stdint.h>
#include "cpu.h"
#include "irq.h"

static uint8_t prepare(void);
static void finish(uint8_t istate);
static inline void busy_wait(void);

/*---------------------------------------------------------------------------*/
uint8_t flashrom_erase(uint8_t *addr)
{
    // TODO implement this function
    return 0;
}
/*---------------------------------------------------------------------------*/
void flashrom_write(uint8_t *dst, uint8_t *src, size_t size)
{
    // TODO implement this function
}

/*---------------------------------------------------------------------------*/
static uint8_t prepare(void)
{
    // TODO implement this function

    return 0;
}
/*---------------------------------------------------------------------------*/
void finish(uint8_t istate)
{
    // TODO implement this function
}
/*---------------------------------------------------------------------------*/
static inline void busy_wait(void)
{
    /* Wait for BUSY = 0, not needed unless run from RAM */
    while (FCTL3 & 0x0001) {
        nop();
    }
}
