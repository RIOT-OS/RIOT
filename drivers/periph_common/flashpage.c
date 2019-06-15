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

#if defined(MODULE_PERIPH_FLASHPAGE_RAW)
int flashpage_verify_raw(void *target_addr, const void *data, size_t len)
{
    if (memcmp(target_addr, data, len) == 0) {
        return FLASHPAGE_OK;
    }
    else {
        return FLASHPAGE_NOMATCH;
    }
}

int flashpage_write_and_verify_raw(void *target_addr, const void *data, size_t len)
{
    flashpage_write_raw(target_addr, data, len);
    return flashpage_verify_raw(target_addr, data, len);
}

#endif

#if defined(FLASHPAGE_RWWEE_NUMOF)

void flashpage_rwwee_read(int page, void *data)
{
    assert(page < (int)FLASHPAGE_RWWEE_NUMOF);

    memcpy(data, flashpage_rwwee_addr(page), FLASHPAGE_SIZE);
}

int flashpage_rwwee_verify(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_RWWEE_NUMOF);

    if (memcmp(flashpage_rwwee_addr(page), data, FLASHPAGE_SIZE) == 0) {
        return FLASHPAGE_OK;
    }
    else {
        return FLASHPAGE_NOMATCH;
    }
}

int flashpage_rwwee_write_and_verify(int page, const void *data)
{
    flashpage_rwwee_write(page, data);
    return flashpage_rwwee_verify(page, data);
}

#endif

#endif
