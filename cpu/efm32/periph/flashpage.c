/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Low-level flash page driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"

#include "periph/flashpage.h"

#include "em_msc.h"

void flashpage_erase(unsigned page)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);

    /* erase given page */
    MSC_Init();
    MSC_ErasePage(page_addr);
    MSC_Deinit();
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

    MSC_Init();
    MSC_WriteWord(page_addr, data_addr, len);
    MSC_Deinit();
}
