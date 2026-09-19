/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "periph/flashpage.h"

void flashpage_read(unsigned page, void *data, size_t offset, size_t len)
{
    assert(page < FLASHPAGE_NUMOF);
    assert(offset + len <= flashpage_size(page) && offset + len > offset);

#if defined(CPU_FAM_STM32WB) || (defined(CPU_FAM_STM32WL) && \
                                 !defined(CPU_LINE_STM32WLE5xx))
    assert(page < (FLASH->SFR & FLASH_SFR_SFSA));
#endif

    memcpy(data, (uint8_t *)flashpage_addr(page) + offset, len);
}

int flashpage_verify(unsigned page, const void *data, size_t offset, size_t len)
{
    assert(page < (int)FLASHPAGE_NUMOF);
    assert(offset + len <= flashpage_size(page) && offset + len > offset);

#if defined(CPU_FAM_STM32WB) || (defined(CPU_FAM_STM32WL) && \
                                 !defined(CPU_LINE_STM32WLE5xx))
    assert(page < (int)(FLASH->SFR & FLASH_SFR_SFSA));
#endif

    if (memcmp((uint8_t *)flashpage_addr(page) + offset, data, len) == 0) {
        return FLASHPAGE_OK;
    }
    else {
        return FLASHPAGE_NOMATCH;
    }
}

int flashpage_write_and_verify(unsigned page, const void *data)
{
    flashpage_write_page(page, data);
    return flashpage_verify(page, data, 0, flashpage_size(page));
}

#ifdef FLASHPAGE_SIZE
void flashpage_write_page(unsigned page, const void *data)
{
    assert((unsigned) page < FLASHPAGE_NUMOF);
    assert(data != NULL);

    flashpage_erase(page);

    /* write page */
    flashpage_write(flashpage_addr(page), data, FLASHPAGE_SIZE);
}
#endif

#if defined(FLASHPAGE_RWWEE_NUMOF)
void flashpage_rwwee_read(unsigned page, void *data, size_t offset, size_t len)
{
    assert(page < (int)FLASHPAGE_RWWEE_NUMOF);
    assert(offset + len <= FLASHPAGE_SIZE && offset + len > offset);

    memcpy(data, (uint8_t *)flashpage_rwwee_addr(page) + offset, len);
}

int flashpage_rwwee_verify(unsigned page, const void *data, size_t offset, size_t len)
{
    assert(page < (int)FLASHPAGE_RWWEE_NUMOF);
    assert(offset + len <= FLASHPAGE_SIZE && offset + len > offset);

    if (memcmp((uint8_t *)flashpage_rwwee_addr(page) + offset, data, len) == 0) {
        return FLASHPAGE_OK;
    }
    else {
        return FLASHPAGE_NOMATCH;
    }
}

int flashpage_rwwee_write_and_verify(unsigned page, const void *data)
{
    flashpage_rwwee_write_page(page, data);
    return flashpage_rwwee_verify(page, data, 0, FLASHPAGE_SIZE);
}
#endif /* FLASHPAGE_RWWEE_NUMOF */

#ifdef PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_ADDR
void *flashpage_addr(unsigned page)
{
    uintptr_t addr = CPU_FLASH_BASE;

    while (page) {
        addr += flashpage_size(--page);
    }

    return (void*)addr;
}
#endif /* PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_ADDR */

#ifdef PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_PAGE
unsigned flashpage_page(const void *addr)
{
    unsigned page = 0;

    for (uintptr_t pos = CPU_FLASH_BASE; (uintptr_t)addr >= pos; ++page) {
        pos += flashpage_size(page);
    }

    return page - 1;
}
#endif /* PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_PAGE */
