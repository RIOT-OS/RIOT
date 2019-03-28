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

#define FLASH_MAIN          0
#define FLASH_RWWEE         1

/**
 * @brief   NVMCTRL selection macros
 */
#ifdef CPU_FAM_SAML11
#define _NVMCTRL NVMCTRL_SEC
#else
#define _NVMCTRL NVMCTRL
#endif

static inline void wait_nvm_is_ready(void) __attribute__((always_inline));
static inline void wait_nvm_is_ready(void)
{
#ifdef CPU_SAML1X
    while (!_NVMCTRL->STATUS.bit.READY) {}
#else
    while (!_NVMCTRL->INTFLAG.bit.READY) {}
#endif
}

static void _unlock(void)
{
    /* remove peripheral access lock for the NVMCTRL peripheral */
#if defined(CPU_FAM_SAML21) || defined(CPU_SAML1X)
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
#if defined(CPU_FAM_SAML21) || defined(CPU_SAML1X)
    PAC->WRCTRL.reg = (PAC_WRCTRL_KEY_SET | ID_NVMCTRL);
#else
    if (PAC1->WPCLR.reg & NVMCTRL_PAC_BIT) {
        PAC1->WPSET.reg = NVMCTRL_PAC_BIT;
    }
#endif
}

#ifdef FLASHPAGE_RWWEE_NUMOF
void flashpage_write_raw_internal(void *target_addr, const void *data, size_t len, int flash_type)
#else
void flashpage_write_raw(void *target_addr, const void *data, size_t len)
#endif
{
    /* The actual minimal block size for writing is 16B, thus we
     * assert we write on multiples and no less of that length.
     */
    assert(!(len % FLASHPAGE_RAW_BLOCKSIZE));

    /* ensure 4 byte aligned writes */
    assert(!(((unsigned)target_addr % FLASHPAGE_RAW_ALIGNMENT) ||
            ((unsigned)data % FLASHPAGE_RAW_ALIGNMENT)));

    /* ensure the length doesn't exceed the actual flash size */
#ifdef FLASHPAGE_RWWEE_NUMOF
    if (flash_type == FLASH_RWWEE) {
        assert(((unsigned)target_addr + len) <=
               (CPU_FLASH_RWWEE_BASE + (FLASHPAGE_SIZE * FLASHPAGE_RWWEE_NUMOF)));
    } else {
#endif
        assert(((unsigned)target_addr + len) <=
               (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)));
#ifdef FLASHPAGE_RWWEE_NUMOF
    }
#endif

    uint32_t *dst = (uint32_t *)target_addr;
    const uint32_t *data_addr = data;

    /* write 4 bytes in one go */
    len /= 4;

    _unlock();
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC);
    wait_nvm_is_ready();
    for (unsigned i = 0; i < len; i++) {
        *dst++ = *data_addr++;
    }
#ifdef FLASHPAGE_RWWEE_NUMOF
    if (flash_type == FLASH_RWWEE) {
#ifdef CPU_SAML1X
         /* SAML1X use the same Write Page command for both flash memories */
        _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP);
#else
        _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_RWWEEWP);
#endif
    } else {
#endif
        _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP);
#ifdef FLASHPAGE_RWWEE_NUMOF
    }
#endif
    wait_nvm_is_ready();
    _lock();
}

#ifdef FLASHPAGE_RWWEE_NUMOF
void flashpage_write_internal(int page, const void *data, int flash_type)
#else
void flashpage_write(int page, const void *data)
#endif
{
    uint32_t *page_addr;

#ifdef FLASHPAGE_RWWEE_NUMOF
    if (flash_type == FLASH_RWWEE) {
        page_addr = (uint32_t *)flashpage_rwwee_addr(page);
    } else {
#endif
        page_addr = (uint32_t *)flashpage_addr(page);
#ifdef FLASHPAGE_RWWEE_NUMOF
    }
#endif

    /* erase given page (the ADDR register uses 16-bit addresses) */
    _unlock();
#ifdef CPU_SAML1X
    /* Ensure address alignment */
    _NVMCTRL->ADDR.reg = (((uint32_t)page_addr) & 0xfffffffe);
#else
    _NVMCTRL->ADDR.reg = (((uint32_t)page_addr) >> 1);
#endif
#ifdef FLASHPAGE_RWWEE_NUMOF
    if (flash_type == FLASH_RWWEE) {
#ifdef CPU_SAML1X
         /* SAML1X use the same Erase command for both flash memories */
        _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER);
#else
        _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_RWWEEER);
#endif
    } else {
#endif
        _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER);
#ifdef FLASHPAGE_RWWEE_NUMOF
    }
#endif
    wait_nvm_is_ready();
    _lock();

    /* write data to page */
    if (data != NULL) {
        /* One RIOT page is FLASHPAGE_PAGES_PER_ROW SAM0 flash pages (a row) as
         * defined in the file cpu/sam0_common/include/cpu_conf.h, therefore we
         * have to split the write into FLASHPAGE_PAGES_PER_ROW raw calls
         * underneath, each writing a physical page in chunks of 4 bytes (see
         * flashpage_write_raw)
         * The erasing is done once as a full row is always reased.
         */
        for (unsigned curpage = 0; curpage < FLASHPAGE_PAGES_PER_ROW; curpage++) {
#ifdef FLASHPAGE_RWWEE_NUMOF
            flashpage_write_raw_internal(page_addr + (curpage * NVMCTRL_PAGE_SIZE / 4),
                                        (void *) ((uint32_t *) data + (curpage * NVMCTRL_PAGE_SIZE / 4)),
                                        NVMCTRL_PAGE_SIZE, flash_type);
#else
            flashpage_write_raw(page_addr + (curpage * NVMCTRL_PAGE_SIZE / 4),
                               (void *) ((uint32_t *) data + (curpage * NVMCTRL_PAGE_SIZE / 4)),
                               NVMCTRL_PAGE_SIZE);
#endif
        }
    }
}


#ifdef FLASHPAGE_RWWEE_NUMOF
/*
 * If RWWEE flash is present then we create an additional layer for the write functions
 * so we can specify the type (either MAIN or RWWEE) we want to access, keeping the
 * standard API unchanged and code for systems without RWWEE at a minimum at the cost
 * of some more #defines in the code
 */
void flashpage_write_raw(void *target_addr, const void *data, size_t len)
{
    flashpage_write_raw_internal(target_addr, data, len, FLASH_MAIN);
}

void flashpage_write(int page, const void *data)
{
    assert((uint32_t)page < FLASHPAGE_NUMOF);

    flashpage_write_internal(page, data, FLASH_MAIN);
}

void flashpage_rwwee_write_raw(void *target_addr, const void *data, size_t len)
{
    flashpage_write_raw_internal(target_addr, data, len, FLASH_RWWEE);
}

void flashpage_rwwee_write(int page, const void *data)
{
    assert((uint32_t)page < FLASHPAGE_RWWEE_NUMOF);

    flashpage_write_internal(page, data, FLASH_RWWEE);
}
#endif
