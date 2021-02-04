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

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* guard this file, must be done before including periph/flashpage.h
 * TODO: remove as soon as periph drivers can be build selectively */
#if defined(FLASHPAGE_NUMOF) && defined(FLASHPAGE_SIZE)

#include "periph/flashpage.h"

#ifdef MODULE_PERIPH_FLASHPAGE_PAGEWISE
void flashpage_read(unsigned page, void *data)
{
    assert(page < FLASHPAGE_NUMOF);

#if defined(CPU_FAM_STM32WB)
    assert(page < (FLASH->SFR & FLASH_SFR_SFSA));
#endif

    memcpy(data, flashpage_addr(page), flashpage_size(page));
}

int flashpage_verify(unsigned page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

#if defined(CPU_FAM_STM32WB)
    assert(page < (int)(FLASH->SFR & FLASH_SFR_SFSA));
#endif

    if (memcmp(flashpage_addr(page), data, flashpage_size(page)) == 0) {
        return FLASHPAGE_OK;
    }
    else {
        return FLASHPAGE_NOMATCH;
    }
}

int flashpage_write_and_verify(unsigned page, const void *data)
{
    flashpage_write_page(page, data);
    return flashpage_verify(page, data);
}

void flashpage_write_page(unsigned page, const void *data)
{
    assert((unsigned) page < FLASHPAGE_NUMOF);

    flashpage_erase(page);

    /* write page */
    if (data != NULL) {
        flashpage_write(flashpage_addr(page), data, FLASHPAGE_SIZE);
    }
}
#endif /* MODULE_PERIPH_FLASHPAGE_PAGEWISE */

#if defined(FLASHPAGE_RWWEE_NUMOF)

void flashpage_rwwee_read(unsigned page, void *data)
{
    assert(page < (int)FLASHPAGE_RWWEE_NUMOF);

    memcpy(data, flashpage_rwwee_addr(page), FLASHPAGE_SIZE);
}

int flashpage_rwwee_verify(unsigned page, const void *data)
{
    assert(page < (int)FLASHPAGE_RWWEE_NUMOF);

    if (memcmp(flashpage_rwwee_addr(page), data, FLASHPAGE_SIZE) == 0) {
        return FLASHPAGE_OK;
    }
    else {
        return FLASHPAGE_NOMATCH;
    }
}

int flashpage_rwwee_write_and_verify(unsigned page, const void *data)
{
    flashpage_rwwee_write_page(page, data);
    return flashpage_rwwee_verify(page, data);
}

#endif

/* We have to write whole words, but writing 0xFF is basically a no-op
 * so fill the unaligned bytes with 0xFF to get a whole extra word.
 * To support writes of data with less than 4 bytes, an offset needs
 * to be supplied.
 */
static void unaligned_pad_start(const void *dst, uint8_t *buffer,
                                const void *data, uint8_t len, uint8_t offset)
{
    if (IS_ACTIVE(FLASHPAGE_NO_BLIND_WRITES)) {
        memcpy(buffer, dst, FLASHPAGE_WRITE_BLOCK_SIZE);
    } else {
        memset(buffer, FLASHPAGE_ERASE_STATE, FLASHPAGE_WRITE_BLOCK_SIZE);
    }
    memcpy(buffer + offset, data, len);
}

/* We have to write whole words, but writing 0xFF is basically a no-op
 * so fill the unaligned bytes with 0xFF to get a whole extra word.
 */
static void unaligned_pad_end(const void *dst, uint8_t *buffer,
                              const void *data, uint8_t len)
{
    if (IS_ACTIVE(FLASHPAGE_NO_BLIND_WRITES)) {
        memcpy(buffer, dst, FLASHPAGE_WRITE_BLOCK_SIZE);
    } else {
        memset(buffer, FLASHPAGE_ERASE_STATE, FLASHPAGE_WRITE_BLOCK_SIZE);
    }
    memcpy(buffer, data, len);
}

void flashpage_write_unaligned(void *_dst, const void *_data, size_t len)
{
    uint8_t *dst = _dst;
    const uint8_t *data = _data;

    const uint8_t mask = FLASHPAGE_WRITE_BLOCK_ALIGNMENT - 1;
    uint8_t padding[FLASHPAGE_WRITE_BLOCK_SIZE];

    /* set bytes in the first, unaligned word */
    uint8_t offset_unaligned_start = (uintptr_t)dst & mask;

    /* word align destination address */
    dst = (void*)((uintptr_t)dst & ~mask);

    if (offset_unaligned_start) {
        /* use MIN to support short data sizes below the block size */
        uint8_t len_unaligned_start = (FLASHPAGE_WRITE_BLOCK_SIZE - offset_unaligned_start)
                                    & (FLASHPAGE_WRITE_BLOCK_SIZE - 1);
        len_unaligned_start = MIN(len_unaligned_start, len);
        len -= len_unaligned_start;

        /* write the first, unaligned bytes */
        unaligned_pad_start(dst, padding, data, len_unaligned_start, offset_unaligned_start);
        flashpage_write(dst, padding, sizeof(padding));

        data += len_unaligned_start;
        dst += FLASHPAGE_WRITE_BLOCK_SIZE;
    }

    /* set bytes in the last, unaligned word */
    uint8_t len_unaligned_end = len & mask;
    len -= len_unaligned_end;

    /* write aligned chunks */
    if (len) {
        flashpage_write(dst, data, len);
    }

    /* write the last, unaligned bytes */
    if (len_unaligned_end) {
        unaligned_pad_end(dst + len, padding, data + len, len_unaligned_end);
        flashpage_write(dst + len, padding, sizeof(padding));
    }
}

#endif
