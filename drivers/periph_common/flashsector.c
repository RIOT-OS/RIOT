/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       Common flash sector functions
 *
 * @author      Jannik Beyerstedt <jannik.beyerstedt@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include "cpu.h"
#include "assert.h"

/* guard this file, must be done before including periph/flashsector.h
 * TODO: remove as soon as periph drivers can be build selectively */
#if defined(FLASHSECTOR_SMALL_SECTOR) && defined(FLASHSECTOR_BANK_SIZE)

#include "periph/flashsector.h"

void flashsector_read(int sector, void *data, int size)
{
    assert(sector < FLASHSECTOR_NUMOF);

    memcpy(data, flashsector_addr(sector), size);
}

int flashsector_verify(int sector, void *data, int size)
{
    assert(sector < FLASHSECTOR_NUMOF);

    if (memcmp(flashsector_addr(sector), data, size) == 0) {
        return FLASHSECTOR_OK;
    }
    else {
        return FLASHSECTOR_NOMATCH;
    }
}

int flashsector_write_and_verify(int sector, void *data, int size)
{
    flashsector_write(sector, data, size);
    return flashsector_verify(sector, data, size);
}

#endif /* defined(FLASHSECTOR_SMALL_SECTOR) && defined(FLASHSECTOR_BANK_SIZE) */
