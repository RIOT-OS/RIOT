/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Low-level flash page driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/flashpage.h"

void flashpage_write_raw(void *target_addr, const void *data, size_t len)
{
    /* assert multiples of FLASHPAGE_RAW_BLOCKSIZE are written and no less of
       that length. */
    assert(!(len % FLASHPAGE_RAW_BLOCKSIZE));

    /* ensure writes are aligned */
    assert(!(((unsigned)target_addr % FLASHPAGE_RAW_ALIGNMENT) ||
            ((unsigned)data % FLASHPAGE_RAW_ALIGNMENT)));

    /* ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)) + 1);

    uint32_t *page_addr = target_addr;
    const uint32_t *data_addr = data;

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    for (unsigned i = 0; i < (len / FLASHPAGE_RAW_BLOCKSIZE); i++) {
        *page_addr++ = data_addr[i];
    }

    /* finish up */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
}

void flashpage_write(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);

    /* erase given page */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
    NRF_NVMC->ERASEPAGE = (uint32_t)page_addr;
    while (NRF_NVMC->READY == 0) {}

    /* write data to page */
    if (data != NULL) {
        flashpage_write_raw(page_addr, data, FLASHPAGE_SIZE);
    }
}
