/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level flash page driver implementation
 *
 * The sam0 has its flash memory organized in pages and rows, where each row
 * consists of 4 pages. While pages are writable one at a time, it is only
 * possible to delete a complete row. This implementation abstracts this
 * behavior by only writing complete rows at a time, so the FLASH_PAGE_SIZE we
 * use in RIOT is actually the row size as specified in the datasheet.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/flashpage.h"

#define NVMCTRL_PAC_BIT     (0x00000002)

void flashpage_write(int page, void *data)
{
    assert(page < FLASHPAGE_NUMOF);

    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);

    /* remove peripheral access lock for the NVMCTRL peripheral */
#ifdef CPU_FAM_SAML21
    PAC->WRCTRL.reg = (PAC_WRCTRL_KEY_CLR | ID_NVMCTRL);
#else
    if (PAC1->WPSET.reg & NVMCTRL_PAC_BIT) {
        PAC1->WPCLR.reg = NVMCTRL_PAC_BIT;
    }
#endif

    /* erase given page (the ADDR register uses 16-bit addresses) */
    NVMCTRL->ADDR.reg = (((uint32_t)page_addr) >> 1);
    NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER);
    while (!NVMCTRL->INTFLAG.bit.READY) {}

    /* write data to page */
    if (data != NULL) {
        uint32_t *data_addr = (uint32_t *)data;
        NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC);
        for (unsigned i = 0; i < (FLASHPAGE_SIZE / 4); i++) {
            *page_addr++ = data_addr[i];
        }
        NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP);
    }
}
