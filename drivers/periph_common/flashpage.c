/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Common flash page functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include "cpu.h"
#include "assert.h"

/* guard this file, must be done before including periph/flashpage.h
 * TODO: remove as soon as periph drivers can be build selectively */
#if defined(FLASHPAGE_NUMOF) && defined(FLASHPAGE_SIZE)

#include "periph/flashpage.h"

void flashpage_read(int page, void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    memcpy(data, flashpage_addr(page), FLASHPAGE_SIZE);
}

int flashpage_verify(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    if (memcmp(flashpage_addr(page), data, FLASHPAGE_SIZE) == 0) {
        return FLASHPAGE_OK;
    }
    else {
        return FLASHPAGE_NOMATCH;
    }
}

int flashpage_write_and_verify(int page, const void *data)
{
    flashpage_write(page, data);
    return flashpage_verify(page, data);
}

#endif
