/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Low-level flash lock/unlock implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Oleg Artamonov <oleg@unwds.com>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "periph/flashpage.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define FLASHPAGE_DIV          (2U)
#define FLASH_KEY1             ((uint32_t)0x45670123)
#define FLASH_KEY2             ((uint32_t)0xCDEF89AB)

static void _unlock_flash(void)
{
    if (FMC->CTL0 & FMC_CTL0_LK_Msk) {
        DEBUG("[flash-common] unlocking the flash module\n");
        FMC->KEY0 = FLASH_KEY1;
        FMC->KEY0 = FLASH_KEY2;
    }
}

static void _lock_flash(void)
{
    if (!(FMC->CTL0 & FMC_CTL0_LK_Msk)) {
        DEBUG("[flash-common] locking the flash module\n");
        FMC->CTL0 |= FMC_CTL0_LK_Msk;
    }
}

static void _wait_for_pending_operations(void)
{
    if (FMC->STAT0 & FMC_STAT0_BUSY_Msk) {
        DEBUG("[flash-common] waiting for any pending operation to finish\n");
        while (FMC->STAT0 & FMC_STAT0_BUSY_Msk) {}
    }

    /* Clear 'end of operation' bit in status register */
    FMC->STAT0 |= FMC_STAT0_ENDF_Msk;
}

static void _erase_page(void *page_addr)
{
    uint16_t *dst = page_addr;
    uint32_t irc8_state = (RCU->CTL & RCU_CTL_IRC8MEN_Msk);

    /* the internal RC oscillator (HSI) must be enabled */
    gd32v_enable_irc8();

    /* unlock the flash module */
    _unlock_flash();

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    /* set page erase bit and program page address */
    DEBUG("[flashpage] erase: setting the erase bit\n");
    FMC->CTL0 |= FMC_CTL0_PER_Msk;
    DEBUG("address to erase: %p\n", page_addr);
    DEBUG("[flashpage] erase: setting the page address\n");
    FMC->ADDR0 = (uint32_t)dst;
    /* trigger the page erase and wait for it to be finished */
    DEBUG("[flashpage] erase: trigger the page erase\n");
    FMC->CTL0 |= FMC_CTL0_START_Msk;
    /* wait as long as device is busy */
    _wait_for_pending_operations();

    /* reset PER bit */
    DEBUG("[flashpage] erase: resetting the page erase bit\n");
    FMC->CTL0 &= ~(FMC_CTL0_PER_Msk);

    /* lock the flash module again */
    _lock_flash();

    /* restore the HSI state */
    if (!irc8_state) {
        gd32v_disable_irc8();
    }
}

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

    uint16_t *dst = (uint16_t *)target_addr;
    const uint16_t *data_addr = data;

    uint32_t irc8_state = (RCU->CTL & RCU_CTL_IRC8MEN_Msk);

    /* the internal RC oscillator (HSI) must be enabled */
    gd32v_enable_irc8();

    /* unlock the flash module */
    _unlock_flash();

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    /* set PG bit and program page to flash */
    FMC->CTL0 |= FMC_CTL0_PG_Msk;
    for (size_t i = 0; i < (len / FLASHPAGE_DIV); i++) {
        DEBUG("[flashpage_raw] writing %c to %p\n", (char)data_addr[i], dst);
        *dst++ = data_addr[i];
        /* wait as long as device is busy */
        _wait_for_pending_operations();
    }

    FMC->CTL0 &= ~(FMC_CTL0_PG_Msk);
    DEBUG("[flashpage_raw] write: done writing data\n");

    /* lock the flash module again */
    _lock_flash();

    /* restore the HSI state */
    if (!irc8_state) {
        gd32v_disable_irc8();
    }
}

void flashpage_write(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    /* Default is to support half-word sizes */
    uint16_t *page_addr = flashpage_addr(page);

    /* ERASE sequence */
    _erase_page(page_addr);

    /* WRITE sequence */
    if (data != NULL) {
        flashpage_write_raw(page_addr, data, FLASHPAGE_SIZE);
    }
}
