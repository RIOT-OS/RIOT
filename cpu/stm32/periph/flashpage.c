/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
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

#include "periph/flashpage.h"

#define ENABLE_DEBUG           0
#include "debug.h"

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
/* Program memory unlock keys */
#define FLASH_PRGKEY1          ((uint32_t)0x8C9DAEBF)
#define FLASH_PRGKEY2          ((uint32_t)0x13141516)
#define CNTRL_REG              (FLASH->PECR)
#define CNTRL_REG_LOCK         (FLASH_PECR_PELOCK)
#define FLASH_CR_PER           (FLASH_PECR_ERASE | FLASH_PECR_PROG)
#elif defined(CPU_FAM_STM32L5)
#define CNTRL_REG              (FLASH->NSCR)
#define CNTRL_REG_LOCK         (FLASH_NSCR_NSLOCK)
#define FLASH_CR_PNB           (FLASH_NSCR_NSPNB)
#define FLASH_CR_PNB_Pos       (FLASH_NSCR_NSPNB_Pos)
#define FLASH_CR_STRT          (FLASH_NSCR_NSSTRT)
#define FLASH_CR_PER           (FLASH_NSCR_NSPER)
#define FLASH_CR_BKER          (FLASH_NSCR_NSBKER)
#define FLASH_CR_PG            (FLASH_NSCR_NSPG)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7)
#define FLASHPAGE_DIV          (4U)
#define FLASH_CR_PER           (FLASH_CR_SER)
#define FLASH_CR_PNB           (FLASH_CR_SNB)
#define FLASH_CR_PNB_Pos       (FLASH_CR_SNB_Pos)
#define CNTRL_REG              (FLASH->CR)
#else
#define CNTRL_REG              (FLASH->CR)
#define CNTRL_REG_LOCK         (FLASH_CR_LOCK)
#endif

extern void _lock(void);
extern void _unlock(void);
extern void _wait_for_pending_operations(void);

#if defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32L5)
#define MAX_PAGES_PER_BANK      (128)
#else /* CPU_FAM_STM32L4 */
#define MAX_PAGES_PER_BANK      (256)
#endif

static void _unlock_flash(void)
{
    _unlock();

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

static void _erase_page(void *page_addr)
{
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);
    /* the internal RC oscillator (HSI) must be enabled */
    stmclk_enable_hsi();
#endif

    /* unlock the flash module */
    _unlock_flash();

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    /* set page erase bit and program page address */
    DEBUG("[flashpage] erase: setting the erase bit\n");
    CNTRL_REG |= FLASH_CR_PER;
    DEBUG("address to erase: %p\n", page_addr);
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    DEBUG("[flashpage] erase: trigger the page erase\n");
    *(uint32_t *)page_addr = 0;
#elif defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
      defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
      defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32F2) || \
      defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7)
    DEBUG("[flashpage] erase: setting the page address\n");
    uint8_t pn;
#if (FLASHPAGE_NUMOF <= MAX_PAGES_PER_BANK) || defined(CPU_FAM_STM32WB)
    pn = (uint8_t)flashpage_page(page_addr);
#else
    uint16_t page = flashpage_page(page_addr);
    if (page > MAX_PAGES_PER_BANK - 1) {
        CNTRL_REG |= FLASH_CR_BKER;
    }
    else {
        CNTRL_REG &= ~FLASH_CR_BKER;
    }
    pn = (uint8_t)page;
#endif
    CNTRL_REG &= ~FLASH_CR_PNB;
#if FLASHPAGE_DUAL_BANK
    if (pn > (FLASHPAGE_NUMOF / 2 - 1)) {
        pn = pn - (FLASHPAGE_NUMOF / 2);
        CNTRL_REG |= FLASH_CR_SNB_4 | (uint32_t)(pn << FLASH_CR_PNB_Pos);
    }
    else {
        CNTRL_REG |= (uint32_t)(pn << FLASH_CR_PNB_Pos);
    }
#else
    CNTRL_REG |= (uint32_t)(pn << FLASH_CR_PNB_Pos);
#endif
    CNTRL_REG |= FLASH_CR_STRT;
    DEBUG("[flashpage] erase: the page address is set and started\n");
#else /* CPU_FAM_STM32F0 || CPU_FAM_STM32F1 || CPU_FAM_STM32F3 */
    DEBUG("[flashpage] erase: setting the page address\n");
    FLASH->AR = (uint32_t)page_addr;
    /* trigger the page erase and wait for it to be finished */
    DEBUG("[flashpage] erase: trigger the page erase\n");
    CNTRL_REG |= FLASH_CR_STRT;
#endif
    /* wait as long as device is busy */
    _wait_for_pending_operations();

#ifdef FLASH_ACR_DCEN /* Flush the data cache after page erase */
    if (FLASH->ACR & FLASH_ACR_DCEN) {
        FLASH->ACR &= ~FLASH_ACR_DCEN;
        FLASH->ACR |= FLASH_ACR_DCRST;
        FLASH->ACR |= FLASH_ACR_DCEN;
    }
#endif
#ifdef FLASH_ACR_ICEN /* Flush the instruction cache after page erase */
    if (FLASH->ACR & FLASH_ACR_ICEN) {
        FLASH->ACR &= ~FLASH_ACR_ICEN;
        FLASH->ACR |= FLASH_ACR_ICRST;
        FLASH->ACR |= FLASH_ACR_ICEN;
    }
#endif

#ifdef FLASH_CR_PNB
    /* reset PER bit (if the register settings exist) */
    DEBUG("[flashpage] erase: resetting the page erase bit\n");
    CNTRL_REG &= ~(FLASH_CR_PER | FLASH_CR_PNB);
#endif

    /* lock the flash module again */
    _lock();

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
    /* restore the HSI state */
    if (!hsi_state) {
        stmclk_disable_hsi();
    }
#endif
}

void flashpage_erase(unsigned page)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    /* ensure there is no attempt to write to CPU2 protected area */
#if defined(CPU_FAM_STM32WB)
    assert(page < (int)(FLASH->SFR & FLASH_SFR_SFSA));
#endif

    void *page_addr = flashpage_addr(page);

    /* ERASE sequence */
    _erase_page(page_addr);
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
    assert(((uintptr_t)(target_addr) + len) <
            (uintptr_t)flashpage_addr(FLASHPAGE_NUMOF + 1));

    stm32_flashpage_block_t *dst = target_addr;
    const stm32_flashpage_block_t *data_addr = data;

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);
    /* the internal RC oscillator (HSI) must be enabled */
    stmclk_enable_hsi();
#endif

#ifdef FLASH_ACR_DCEN
    /* Disable the data cache during page writes */
    bool data_cache = FLASH->ACR & FLASH_ACR_DCEN;
    if (data_cache) {
        FLASH->ACR &= ~FLASH_ACR_DCEN;
    }
#endif
#ifdef FLASH_ACR_ICEN
    /* Disable the instruction cache during page writes */
    bool instruction_cache = FLASH->ACR & FLASH_ACR_ICEN;
    if (instruction_cache) {
        FLASH->ACR &= ~FLASH_ACR_ICEN;
    }
#endif

    /* unlock the flash module */
    _unlock_flash();

#ifdef FLASH_CR_PSIZE_1
    CNTRL_REG |= FLASH_CR_PSIZE_1; /* Word size parallelism */
#endif

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    DEBUG("[flashpage_raw] write: now writing the data\n");
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7)
    /* set PG bit and program page to flash */
    CNTRL_REG |= FLASH_CR_PG;
#endif
    for (size_t i = 0; i < (len / sizeof(stm32_flashpage_block_t)); i++) {
        DEBUG("[flashpage_raw] writing %c to %p\n", (char)data_addr[i], dst);
        *dst++ = data_addr[i];
#if defined(CPU_FAM_STM32F7)
        __DMB();
#endif
        /* wait as long as device is busy */
        _wait_for_pending_operations();
    }

    /* clear program bit again */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7)
    CNTRL_REG &= ~(FLASH_CR_PG);
#endif
    DEBUG("[flashpage_raw] write: done writing data\n");

    /* lock the flash module again */
    _lock();

#ifdef FLASH_ACR_DCEN
    /* Enable the data cache if it was enabled before. Always reset it */
    FLASH->ACR |= FLASH_ACR_DCRST;
    if (data_cache) {
        FLASH->ACR |= FLASH_ACR_DCEN;
    }
#endif
#ifdef FLASH_ACR_ICEN
    /* Enable the instruction cache if it was enabled before. Always reset it */
    FLASH->ACR |= FLASH_ACR_ICRST;
    if (instruction_cache) {
        FLASH->ACR |= FLASH_ACR_ICEN;
    }
#endif

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
    /* restore the HSI state */
    if (!hsi_state) {
        stmclk_disable_hsi();
    }
#endif
}

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7)
size_t flashpage_size(unsigned page)
{
    if (page < 4) {
        return FLASHPAGE_MIN_SECTOR_SIZE;
    }
    else if (page == 4) {
        return 4 * FLASHPAGE_MIN_SECTOR_SIZE;
    }
    else {
        return 8 * FLASHPAGE_MIN_SECTOR_SIZE;
    }
}

void *flashpage_addr(unsigned page)
{
    uintptr_t addr = CPU_FLASH_BASE;
    while (page) {
        addr += flashpage_size(--page);
    }

    return (void*)addr;
}

unsigned flashpage_page(void *addr)
{
    /* Calculates the flashpage number based on the address for the
     * non-homogeneous flashpage stm32 series.
     * These all follow the same pattern of 4 sectors of base size, 1 sector of
     * 4 times the base size and the rest of the pages are 8 times the base
     * size. Here we calculate the page number as if all pages are of base size
     * and then compensate for the larger sectors */
    unsigned page = (((intptr_t)addr - CPU_FLASH_BASE) /
                        FLASHPAGE_MIN_SECTOR_SIZE);

    /* check if beyond the 4 base sectors + the 4 * base size sector */
    if (page > 7) {
        /* Divide by 8 and compensate for the initial 5 sectors */
        page = (page / 8) + 4;
    }
    /* If the page number is between 4 and 7 (inclusive), the address is in the
     * single 4 * base size sector */
    else if (page > 3) {
        page = 4;
    }

    return page;
}
#endif
