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

#ifndef NRF_NVMC
#define NRF_NVMC NRF_NVMC_S
#endif

void flashpage_erase(unsigned page)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);

    /* erase given page */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
#ifndef NRF_NVMC_S
    NRF_NVMC->ERASEPAGE = (uint32_t)page_addr;
#else
    /* NRF_NVMC->ERASEPAGE doesn't exist on nRF9160 family, the proper
       way to erase a page is to write 0xFFFFFFFF in the first 32-bit
       word of the page to erase it (see Chap 4.4.2 from nRF9160 datasheet ) */
    *page_addr = UINT32_MAX;
#endif
    while (NRF_NVMC->READY == 0) {}
}

void flashpage_write(void *target_addr, const void *data, size_t len)
{
    /* assert multiples of FLASHPAGE_WRITE_BLOCK_SIZE are written and no less of
       that length. */
    assert(!(len % FLASHPAGE_WRITE_BLOCK_SIZE));

    /* ensure writes are aligned */
    assert(!(((unsigned)target_addr % FLASHPAGE_WRITE_BLOCK_ALIGNMENT) ||
            ((unsigned)data % FLASHPAGE_WRITE_BLOCK_ALIGNMENT)));

    /* ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)) + 1);

    uint32_t *page_addr = target_addr;
    const uint32_t *data_addr = data;

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    for (unsigned i = 0; i < (len / FLASHPAGE_WRITE_BLOCK_SIZE); i++) {
        *page_addr++ = data_addr[i];
    }

    /* finish up */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
}
