/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Low-level flash page driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "assert.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#include "periph/flashpage.h"

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
/* Data EEPROM and control register unlock keys */
#define FLASH_KEY1             ((uint32_t)0x89ABCDEF)
#define FLASH_KEY2             ((uint32_t)0x02030405)
/* Program memory unlock keys */
#define FLASH_PRGKEY1          ((uint32_t)0x8C9DAEBF)
#define FLASH_PRGKEY2          ((uint32_t)0x13141516)
#define CNTRL_REG              (FLASH->PECR)
#define CNTRL_REG_LOCK         (FLASH_PECR_PELOCK)
#define KEY_REG                (FLASH->PEKEYR)
#define FLASH_CR_PER           (FLASH_PECR_ERASE | FLASH_PECR_PROG)
#define FLASH_CR_PG            (FLASH_PECR_FPRG | FLASH_PECR_PROG)
#define FLASHPAGE_DIV          (4U) /* write 4 bytes in one go */
#else
#define CNTRL_REG              (FLASH->CR)
#define CNTRL_REG_LOCK         (FLASH_CR_LOCK)
#define KEY_REG                (FLASH->KEYR)
#define FLASHPAGE_DIV          (2U)
#endif

static void _unlock(void)
{
    DEBUG("[flashpage] unlocking the flash module\n");
    if (CNTRL_REG & CNTRL_REG_LOCK) {
        KEY_REG = FLASH_KEY1;
        KEY_REG = FLASH_KEY2;
    }

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    DEBUG("[flashpage] unlocking the flash program memory\n");
    if (!(CNTRL_REG & CNTRL_REG_LOCK)) {
        if (CNTRL_REG & FLASH_PECR_PRGLOCK) {
            DEBUG("[flashpage] setting the program memory unlock keys\n");
            FLASH->PRGKEYR = FLASH_PRGKEY1;
            FLASH->PRGKEYR = FLASH_PRGKEY2;
        }
    }
#endif
}

static void _lock(void)
{
    DEBUG("[flashpage] locking the flash module\n");
    CNTRL_REG |= CNTRL_REG_LOCK;
}

static void _erase_page(void *page_addr)
{
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    uint32_t *dst = page_addr;
#else
    uint16_t *dst = page_addr;

    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);
    /* the internal RC oscillator (HSI) must be enabled */
    stmclk_enable_hsi();
#endif

   /* unlock the flash module */
    _unlock();

    /* make sure no flash operation is ongoing */
    DEBUG("[flashpage] erase: waiting for any operation to finish\n");
    while (FLASH->SR & FLASH_SR_BSY) {}
    /* set page erase bit and program page address */
    DEBUG("[flashpage] erase: setting the erase bit\n");
    CNTRL_REG |= FLASH_CR_PER;
    DEBUG("address to erase: %p\n", page_addr);
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    DEBUG("[flashpage] erase: trigger the page erase\n");
    *dst = (uint32_t)0;
#else
    DEBUG("[flashpage] erase: setting the page address\n");
    FLASH->AR = (uint32_t)dst;
    /* trigger the page erase and wait for it to be finished */
    DEBUG("[flashpage] erase: trigger the page erase\n");
    CNTRL_REG |= FLASH_CR_STRT;
#endif
    DEBUG("[flashpage] erase: wait as long as device is busy\n");
    while (FLASH->SR & FLASH_SR_BSY) {}
    /* reset PER bit */
    DEBUG("[flashpage] erase: resetting the page erase bit\n");
    CNTRL_REG &= ~(FLASH_CR_PER);

    /* lock the flash module again */
    _lock();

#if !(defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1))
    /* restore the HSI state */
    if (!hsi_state) {
        stmclk_disable_hsi();
    }
#endif
}

void flashpage_write_raw(void *target_addr, void *data, size_t len)
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

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    uint32_t *dst = target_addr;
    uint32_t *data_addr = (uint32_t *)data;
#else
    uint16_t *dst = (uint16_t *)target_addr;
    uint16_t *data_addr = (uint16_t *)data;

    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);
    /* the internal RC oscillator (HSI) must be enabled */
    stmclk_enable_hsi();
#endif

    DEBUG("[flashpage_raw] unlocking the flash module\n");
    _unlock();

    DEBUG("[flashpage] write: now writing the data\n");
#if !(defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1))
    /* set PG bit and program page to flash */
    CNTRL_REG |= FLASH_CR_PG;
#endif
    for (size_t i = 0; i < (len / FLASHPAGE_DIV); i++) {
        DEBUG("[flashpage_raw] writing %c to %p\n", (char)data_addr[i], dst);
        *dst++ = data_addr[i];
        while (FLASH->SR & FLASH_SR_BSY) {}
    }

    /* clear program bit again */
    CNTRL_REG &= ~(FLASH_CR_PG);
    DEBUG("[flashpage] write: done writing data\n");

    DEBUG("flashpage_raw] now locking the flash module again\n");
    _lock();

#if !(defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1))
    /* restore the HSI state */
    if (!hsi_state) {
        stmclk_disable_hsi();
    }
#endif
}

void flashpage_write(int page, void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    /* STM32L0/L1 only supports word sizes */
    uint32_t *page_addr = flashpage_addr(page);
#else
    /* Default is to support half-word sizes */
    uint16_t *page_addr = flashpage_addr(page);
#endif

    /* ERASE sequence */
    _erase_page(page_addr);

    /* WRITE sequence */
    if (data != NULL) {
        flashpage_write_raw(page_addr, data, FLASHPAGE_SIZE);
    }
}
