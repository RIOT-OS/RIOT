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
#include <string.h>

#include "cpu.h"
#include "macros/utils.h"
#include "periph/flashpage.h"
#include "unaligned.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Write Quad Word is the only allowed operation on AUX pages */
#if defined(NVMCTRL_CTRLB_CMD_WQW)
#define AUX_CHUNK_SIZE  (4 * sizeof(uint32_t))
#elif defined(AUX_PAGE_SIZE)
#define AUX_CHUNK_SIZE AUX_PAGE_SIZE
#else
#define AUX_CHUNK_SIZE FLASH_USER_PAGE_SIZE
#endif

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
    while (!(_NVMCTRL->STATUS.reg & NVMCTRL_STATUS_READY)) {}
#else
    while (!(_NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY)) {}
#endif
}

static unsigned _unlock(void)
{
    /* remove peripheral access lock for the NVMCTRL peripheral */
#ifdef REG_PAC_WRCTRL
    PAC->WRCTRL.reg = (PAC_WRCTRL_KEY_CLR | ID_NVMCTRL);
#else
    PAC1->WPCLR.reg = PAC1_WPROT_DEFAULT_VAL;
#endif

    return irq_disable();
}

static void _lock(unsigned state)
{
    wait_nvm_is_ready();

    /* put peripheral access lock for the NVMCTRL peripheral */
#ifdef REG_PAC_WRCTRL
    PAC->WRCTRL.reg = (PAC_WRCTRL_KEY_SET | ID_NVMCTRL);
#else
    PAC1->WPSET.reg = PAC1_WPROT_DEFAULT_VAL;
#endif

    /* cached flash contents may have changed - invalidate cache */
#ifdef CMCC
    CMCC->MAINT0.reg |= CMCC_MAINT0_INVALL;
#endif

    irq_restore(state);
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

static void _cmd_erase_aux(void)
{
    wait_nvm_is_ready();

    /* send Erase Page/Auxiliary Row command */
#if defined(NVMCTRL_CTRLB_CMD_EP)
    _NVMCTRL->CTRLB.reg = (NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_EP);
#elif defined(NVMCTRL_CTRLA_CMD_EAR)
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_EAR);
#else
    /* SAML1x uses same command for all areas */
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER);
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

static void _cmd_write_aux(void)
{
    wait_nvm_is_ready();

    /* write auxiliary page */
#if defined(NVMCTRL_CTRLA_CMD_WAP)
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WAP);
#elif defined(NVMCTRL_CTRLB_CMD_WQW)
    _NVMCTRL->CTRLB.reg = (NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_WQW);
#else
    /* SAML1x uses same command for all areas */
    _NVMCTRL->CTRLA.reg = (NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP);
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

/* We have to write whole words, but writing 0xFF is basically a no-op
 * so fill the unaligned bytes with 0xFF to get a whole extra word.
 * To support writes of data with less than 4 bytes, an offset needs
 * to be supplied.
 */
static uint32_t unaligned_pad_start(const void *_data, uint8_t len, uint8_t offset)
{
    assert((4 - offset) >= len);

    const uint8_t *data = _data;
    union {
        uint32_t u32;
        uint8_t  u8[4];
    } buffer = {.u32 = ~0};

    switch (len) {
        case 3:
            buffer.u8[offset + len - 3] = *data++;
            /* fall-through */
        case 2:
            buffer.u8[offset + len - 2] = *data++;
            /* fall-through */
        case 1:
            buffer.u8[offset + len - 1] = *data++;
    }

    return buffer.u32;
}

/* We have to write whole words, but writing 0xFF is basically a no-op
 * so fill the unaligned bytes with 0xFF to get a whole extra word.
 */
static uint32_t unaligned_pad_end(const void *_data, uint8_t len)
{
    const uint8_t *data = _data;
    union {
        uint32_t u32;
        uint8_t  u8[4];
    } buffer = {.u32 = ~0};

    switch (len) {
        case 3:
            buffer.u8[2] = data[2];
            /* fall-through */
        case 2:
            buffer.u8[1] = data[1];
            /* fall-through */
        case 1:
            buffer.u8[0] = data[0];
    }

    return buffer.u32;
}

static void _write_page(void* dst, const void *data, size_t len, void (*cmd_write)(void))
{
    /* set bytes in the first, unaligned word */
    uint8_t offset_unaligned_start = (uintptr_t)dst & 0x3;
    /* use MIN to support short data sizes below 3 bytes */
    uint8_t len_unaligned_start = MIN((4 - offset_unaligned_start) & 0x3, len);
    len -= len_unaligned_start;

    /* set bytes in the last, unaligned word */
    uint8_t len_unaligned_end = len & 0x3;
    len -= len_unaligned_end;

    /* word align destination address */
    uint32_t *dst32 = (void*)((uintptr_t)dst & ~0x3);

    unsigned state = _unlock();
    _cmd_clear_page_buffer();

    /* write the first, unaligned bytes */
    if (len_unaligned_start) {
        *dst32++ = unaligned_pad_start(data, len_unaligned_start,
                offset_unaligned_start);
        data = (void *)((uintptr_t)data + len_unaligned_start);
    }

    /* copy whole words */
    while (len) {
        /* due to unknown input data alignment and the conditional
         * shift applied above, data might not be aligned to a 4 byte
         * boundary at this point
         */
        *dst32++ = unaligned_get_u32(data);
        data = (void *)((uintptr_t)data + sizeof(uint32_t));
        len -= sizeof(uint32_t);
    }

    /* write the last, unaligned bytes */
    if (len_unaligned_end) {
        *dst32 = unaligned_pad_end(data, len_unaligned_end);
    }

    cmd_write();
    _lock(state);
}

static void _erase_page(void* page, void (*cmd_erase)(void))
{
    uintptr_t page_addr = (uintptr_t)page;

    /* erase given page (the ADDR register uses 16-bit addresses) */
    unsigned state = _unlock();

    /* ADDR drives the hardware (16-bit) address to the NVM when a command is executed using CMDEX.
     * 8-bit addresses must be shifted one bit to the right before writing to this register.
     */
#if defined(CPU_COMMON_SAMD21) || defined(CPU_COMMON_SAML21)
    page_addr >>= 1;
#endif

    /* set Row/Block start address */
    _NVMCTRL->ADDR.reg = page_addr;

    cmd_erase();
    _lock(state);
}

static void _write_row(uint8_t *dst, const void *_data, size_t len, size_t chunk_size,
                       void (*cmd_write)(void))
{
    const uint8_t *data = _data;

    size_t next_chunk = chunk_size - ((uintptr_t)dst & (chunk_size - 1));
    next_chunk = next_chunk ? next_chunk : chunk_size;

    while (len) {
        size_t chunk = MIN(len, next_chunk);
        next_chunk = chunk_size;

        _write_page(dst, data, chunk, cmd_write);
        data += chunk;
        dst  += chunk;
        len  -= chunk;
    }
}

void flashpage_erase(unsigned page)
{
    assert((unsigned)page < FLASHPAGE_NUMOF);
    _erase_page(flashpage_addr(page), _cmd_erase_row);
}

void flashpage_write(void *target_addr, const void *data, size_t len)
{
    /* ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <=
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)));

    _write_row(target_addr, data, len, NVMCTRL_PAGE_SIZE, _cmd_write_page);
}

void sam0_flashpage_aux_write(uint32_t offset, const void *data, size_t len)
{
    uintptr_t dst = NVMCTRL_USER + sizeof(nvm_user_page_t) + offset;

#ifdef FLASH_USER_PAGE_SIZE
    assert(dst + len <= NVMCTRL_USER + FLASH_USER_PAGE_SIZE);
#else
    assert(dst + len <= NVMCTRL_USER + AUX_PAGE_SIZE * AUX_NB_OF_PAGES);
#endif

    _write_row((void*)dst, data, len, AUX_CHUNK_SIZE, _cmd_write_aux);
}

void sam0_flashpage_aux_reset(const nvm_user_page_t *cfg)
{
    nvm_user_page_t old_cfg;

    if (cfg == NULL) {
        cfg = &old_cfg;
        memcpy(&old_cfg, (void*)NVMCTRL_USER, sizeof(*cfg));
    }

    _erase_page((void*)NVMCTRL_USER, _cmd_erase_aux);
    _write_row((void*)NVMCTRL_USER, cfg, sizeof(*cfg), AUX_CHUNK_SIZE, _cmd_write_aux);
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

void flashpage_rwwee_write(void *target_addr, const void *data, size_t len)
{
    assert(((unsigned)target_addr + len) <=
           (CPU_FLASH_RWWEE_BASE + (FLASHPAGE_SIZE * FLASHPAGE_RWWEE_NUMOF)));

    _write_row(target_addr, data, len, NVMCTRL_PAGE_SIZE, _cmd_write_page_rwwee);
}

void flashpage_rwwee_write_page(unsigned page, const void *data)
{
    assert((unsigned)page < FLASHPAGE_RWWEE_NUMOF);

    _erase_page(flashpage_rwwee_addr(page), _cmd_erase_row_rwwee);

    if (data == NULL) {
        return;
    }

    /* One RIOT page is FLASHPAGE_PAGES_PER_ROW SAM0 flash pages (a row) as
     * defined in the file cpu/sam0_common/include/cpu_conf.h, therefore we
     * have to split the write into FLASHPAGE_PAGES_PER_ROW raw calls
     * underneath, each writing a physical page in chunks of 4 bytes (see
     * flashpage_write_raw)
     * The erasing is done once as a full row is always erased.
     */
    _write_row(flashpage_rwwee_addr(page), data, FLASHPAGE_SIZE, NVMCTRL_PAGE_SIZE,
               _cmd_write_page_rwwee);
}
#endif /* FLASHPAGE_RWWEE_NUMOF */
