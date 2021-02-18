/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */

#include <string.h>

#include "xtimer.h"
#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "fido2/ctap/ctap_mem.h"

static void flash_write_raw(int page, int offset,
                            const void *data, size_t len);
static int flash_verify(int page, int offset, const void *data, size_t len);
static void flash_write(int page, const void *data, size_t len);
static bool is_erased(int page, int offset, size_t len);

void fido2_ctap_mem_read(int page, void *buf)
{
    return flashpage_read(page, buf);
}

/**
 * write_raw should fail once for every page => erase page
 * afterwards we should be able to selectively write to page with write_raw,
 * which is faster than erasing and writing whole pages.
 */
int fido2_ctap_mem_write_and_verify(int page, int offset,
                                    const void *data, size_t len)
{
    assert(!(len % FLASHPAGE_WRITE_BLOCK_SIZE));

    if (is_erased(page, offset, len)) {
        flash_write_raw(page, offset, data, len);
    }
    else {
        flash_write(page, data, len);
    }

    return flash_verify(page, offset, data, len);
}

static bool is_erased(int page, int offset, size_t len)
{
    uint32_t *addr = (uint32_t *)((uint8_t *)flashpage_addr(page) + offset);

    for (size_t i = 0; i < len / 4; i++) {
        if (addr[i] != 0xffffffff) {
            return false;
        }
    }

    return true;
}

static void flash_write_raw(int page, int offset,
                            const void *data, size_t len)
{
    uint32_t *addr = (uint32_t *)((uint8_t *)flashpage_addr(page) + offset);

    flashpage_write(addr, data, len);
}

static int flash_verify(int page, int offset, const void *data, size_t len)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    uint8_t *addr = (uint8_t *)flashpage_addr(page) + offset;

    if (memcmp(addr, data, len) == 0) {
        return FLASHPAGE_OK;
    }
    else {
        return FLASHPAGE_NOMATCH;
    }
}

static void flash_write(int page, const void *data, size_t len)
{
    assert(page < (int)FLASHPAGE_NUMOF);
    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);

    /* erase given page */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
    NRF_NVMC->ERASEPAGE = (uint32_t)page_addr;
    while (NRF_NVMC->READY == 0) {}

    /* write data to page */
    if (data != NULL) {
        flashpage_write(page_addr, data, len);
    }
}
