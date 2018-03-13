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

void flashpage_write(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);
    const uint32_t *data_addr = data;

    MSC_Init();
    MSC_ErasePage(page_addr);

    if (data != NULL) {
        MSC_WriteWord(page_addr, data_addr, FLASHPAGE_SIZE);
    }

    MSC_Deinit();
}
