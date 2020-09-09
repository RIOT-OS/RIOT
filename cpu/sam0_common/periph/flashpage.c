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
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph/flashpage.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/**
 * @brief   NVMCTRL selection macros
 */
#ifdef CPU_FAM_SAML11
#define _NVMCTRL NVMCTRL_SEC
#else
#define _NVMCTRL NVMCTRL
#endif

static inline void wait_nvm_is_ready(void)
{
#ifdef NVMCTRL_STATUS_READY
    while (!_NVMCTRL->STATUS.bit.READY) {}
#else
    while (!_NVMCTRL->INTFLAG.bit.READY) {}
#endif
}

static void _unlock(void)
{
    /* remove peripheral access lock for the NVMCTRL peripheral */
#ifdef REG_PAC_WRCTRL
    PAC->WRCTRL.reg = (PAC_WRCTRL_KEY_CLR | ID_NVMCTRL);
#else
    PAC1->WPCLR.reg = PAC1_WPROT_DEFAULT_VAL;
#endif
}

static void _lock(void)
{
    wait_nvm_is_ready();

    /* put peripheral access lock for the NVMCTRL peripheral */
#ifdef REG_PAC_WRCTRL
    PAC->WRCTRL.reg = (PAC_WRCTRL_KEY_SET | ID_NVMCTRL);
#else
    PAC1->WPSET.reg = PAC1_WPROT_DEFAULT_VAL;
#endif
}

static void _cmd_clear_page_buffer(void)
{
    wait_nvm_is_ready();

#ifdef NVMCTRL_CTRLB_CMDEX_KEY
    _NVMCTRL->CTRLB.reg = (NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_PBC);
#else
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC);
#endif
}

static void _cmd_erase_row(void)
{
    wait_nvm_is_ready();

    /* send Row/Block erase command */
#ifdef NVMCTRL_CTRLB_CMDEX_KEY
    _NVMCTRL->CTRLB.reg = (NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_EB);
#else
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER);
#endif
}

static void _cmd_write_page(void)
{
    wait_nvm_is_ready();

    /* write page */
#ifdef NVMCTRL_CTRLB_CMDEX_KEY
    _NVMCTRL->CTRLB.reg = (NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_WP);
#else
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP);
#endif
}

static void _write_page(void* dst, const void *data, size_t len, void (*cmd_write)(void))
{
    uint32_t *dst32 = dst;

    _unlock();
    _cmd_clear_page_buffer();

    /* copy whole words */
    const uint32_t *data32 = data;
    while (len) {
        *dst32++ = *data32++;
        len -= sizeof(uint32_t);
    }

    cmd_write();
    _lock();
}

static void _erase_page(void* page, void (*cmd_erase)(void))
{
    uintptr_t page_addr = (uintptr_t)page;

    /* erase given page (the ADDR register uses 16-bit addresses) */
    _unlock();

    /* ADDR drives the hardware (16-bit) address to the NVM when a command is executed using CMDEX.
     * 8-bit addresses must be shifted one bit to the right before writing to this register.
     */
#if defined(CPU_COMMON_SAMD21) || defined(CPU_COMMON_SAML21)
    page_addr >>= 1;
#endif

    /* set Row/Block start address */
    _NVMCTRL->ADDR.reg = page_addr;

    cmd_erase();
    _lock();
}

/* dst must be row-aligned */
static void _write_row(uint8_t *dst, const void *_data, size_t len, size_t chunk_size,
                       void (*cmd_write)(void))
{
    const uint8_t *data = _data;

    /* One RIOT page is FLASHPAGE_PAGES_PER_ROW SAM0 flash pages (a row) as
     * defined in the file cpu/sam0_common/include/cpu_conf.h, therefore we
     * have to split the write into FLASHPAGE_PAGES_PER_ROW raw calls
     * underneath, each writing a physical page in chunks of 4 bytes (see
     * flashpage_write_raw)
     * The erasing is done once as a full row is always erased.
     */
    while (len) {
        size_t chunk = MIN(len, chunk_size);
        _write_page(dst, data, chunk, cmd_write);
        data += chunk;
        dst  += chunk;
        len  -= chunk;
    }
}

void flashpage_write(int page, const void *data)
{
    assert((unsigned)page < FLASHPAGE_NUMOF);

    _erase_page(flashpage_addr(page), _cmd_erase_row);

    if (data == NULL) {
        return;
    }

    _write_row(flashpage_addr(page), data, FLASHPAGE_SIZE, NVMCTRL_PAGE_SIZE,
               _cmd_write_page);
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
    assert(((unsigned)target_addr + len) <=
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)));

    _write_page(target_addr, data, len, _cmd_write_page);

}

#ifdef FLASHPAGE_RWWEE_NUMOF

static void _cmd_erase_row_rwwee(void)
{
    wait_nvm_is_ready();

    /* send erase row command */
#ifdef NVMCTRL_CTRLA_CMD_RWWEEER
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_RWWEEER);
#else
    /* SAML1X use the same Erase command for both flash memories */
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER);
#endif
}

static void _cmd_write_page_rwwee(void)
{
    wait_nvm_is_ready();

    /* write page */
#ifdef NVMCTRL_CTRLA_CMD_RWWEEWP
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_RWWEEWP);
#else
    /* SAML1X use the same Write Page command for both flash memories */
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP);
#endif
}

void flashpage_rwwee_write_raw(void *target_addr, const void *data, size_t len)
{
    /* The actual minimal block size for writing is 16B, thus we
     * assert we write on multiples and no less of that length.
     */
    assert(!(len % FLASHPAGE_RAW_BLOCKSIZE));

    /* ensure 4 byte aligned writes */
    assert(!(((unsigned)target_addr % FLASHPAGE_RAW_ALIGNMENT) ||
            ((unsigned)data % FLASHPAGE_RAW_ALIGNMENT)));

    assert(((unsigned)target_addr + len) <=
           (CPU_FLASH_RWWEE_BASE + (FLASHPAGE_SIZE * FLASHPAGE_RWWEE_NUMOF)));

    _write_page(target_addr, data, len, _cmd_write_page_rwwee);
}

void flashpage_rwwee_write(int page, const void *data)
{
    assert((unsigned)page < FLASHPAGE_RWWEE_NUMOF);

    _erase_page(flashpage_rwwee_addr(page), _cmd_erase_row_rwwee);

    if (data == NULL) {
        return;
    }

    _write_row(flashpage_rwwee_addr(page), data, FLASHPAGE_SIZE, NVMCTRL_PAGE_SIZE,
               _cmd_write_page_rwwee);
}
#endif /* FLASHPAGE_RWWEE_NUMOF */
