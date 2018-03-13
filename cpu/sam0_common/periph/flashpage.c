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
 * behavior by only writing complete rows at a time, so the FLASHPAGE_SIZE we
 * use in RIOT is actually the row size as specified in the datasheet.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph/flashpage.h"

#define NVMCTRL_PAC_BIT     (0x00000002)

static void _unlock(void)
{
    /* remove peripheral access lock for the NVMCTRL peripheral */
#ifdef CPU_FAM_SAML21
    PAC->WRCTRL.reg = (PAC_WRCTRL_KEY_CLR | ID_NVMCTRL);
#else
    if (PAC1->WPSET.reg & NVMCTRL_PAC_BIT) {
        PAC1->WPCLR.reg = NVMCTRL_PAC_BIT;
    }
#endif
}

static void _lock(void)
{
    /* put peripheral access lock for the NVMCTRL peripheral */
#ifdef CPU_FAM_SAML21
    PAC->WRCTRL.reg = (PAC_WRCTRL_KEY_SET | ID_NVMCTRL);
#else
    if (PAC1->WPCLR.reg & NVMCTRL_PAC_BIT) {
        PAC1->WPSET.reg = NVMCTRL_PAC_BIT;
    }
#endif
}

void flashpage_write_raw(void *target_addr, const void *data, size_t len)
{
    /* The actual minimal block size for writing is 16B, thus we
     * assert we write on multiples and no less of that length.
     */
    assert(!(len % FLASHPAGE_RAW_BLOCKSIZE));

    /* ensure 4 byte aligned writes */
    assert(!(((unsigned)target_addr % FLASHPAGE_RAW_ALIGNMENT) ||
            ((unsigned)data % FLASHPAGE_RAW_ALIGNMENT)));

    /* ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)));

    uint32_t *dst = (uint32_t *)target_addr;
    const uint32_t *data_addr = data;

    /* write 4 bytes in one go */
    len /= 4;

    _unlock();

    NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC);
    for (unsigned i = 0; i < len; i++) {
        *dst++ = *data_addr++;
    }
    NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP);

    _lock();
}

void flashpage_write(int page, const void *data)
{
    assert(page < FLASHPAGE_NUMOF);

    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);

    /* erase given page (the ADDR register uses 16-bit addresses) */
    _unlock();
    NVMCTRL->ADDR.reg = (((uint32_t)page_addr) >> 1);
    NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER);
    while (!NVMCTRL->INTFLAG.bit.READY) {}
    _lock();

    /* write data to page */
    if (data != NULL) {
        flashpage_write_raw(page_addr, data, FLASHPAGE_SIZE);
    }

}
