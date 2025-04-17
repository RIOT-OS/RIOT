/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup cpu_native
 * @ingroup drivers_periph_flashpage
 * @brief   Low-level flashpage driver emulation
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <assert.h>
#include <stdalign.h>
#include <string.h>

#include "cpu.h"
#include "periph/flashpage.h"

#define ENABLE_DEBUG 0
#include "debug.h"

alignas(FLASHPAGE_SIZE * FLASHPAGE_NUMOF)
char _native_flash[FLASHPAGE_SIZE * FLASHPAGE_NUMOF];

void flashpage_erase(unsigned page)
{
    assert(page < FLASHPAGE_NUMOF);

    DEBUG("%p: erase %u bytes\n", flashpage_addr(page), FLASHPAGE_SIZE);

    memset(flashpage_addr(page), FLASHPAGE_ERASE_STATE, FLASHPAGE_SIZE);
}

static void _flash_write(uint8_t *dst, const char *src, size_t len)
{
    while (len--) {
#if FLASHPAGE_ERASE_STATE == 0x0
        *dst++ |= *src++;
#else
        *dst++ &= *src++;
#endif
    }
}

void flashpage_write(void *target_addr, const void *data, size_t len)
{
    assert((uintptr_t)target_addr >= (uintptr_t)_native_flash);
    assert((uintptr_t)target_addr + len <= (uintptr_t)_native_flash + sizeof(_native_flash));
    assert(!(len % FLASHPAGE_WRITE_BLOCK_SIZE));
    assert(!((uintptr_t)target_addr % FLASHPAGE_WRITE_BLOCK_ALIGNMENT));

    DEBUG("%p: write %zu bytes\n", target_addr, len);

    _flash_write(target_addr, data, len);
}
