/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
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

void flashpage_write(int page, void *data)
{
    assert(page < FLASHPAGE_NUMOF);

    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);
    uint32_t *data_addr = (uint32_t *)data;

    /* erase given page */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
    NRF_NVMC->ERASEPAGE = (uint32_t)page_addr;
    while (NRF_NVMC->READY == 0) {}

    /* write data to page */
    if (data != NULL) {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        for (unsigned i = 0; i < (FLASHPAGE_SIZE / 4); i++) {
            *page_addr++ = data_addr[i];
        }
    }

    /* finish up */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
}
