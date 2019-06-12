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
/* Program memory unlock keys */
#define FLASH_PRGKEY1          ((uint32_t)0x8C9DAEBF)
#define FLASH_PRGKEY2          ((uint32_t)0x13141516)
#define CNTRL_REG              (FLASH->PECR)
#define CNTRL_REG_LOCK         (FLASH_PECR_PELOCK)
#define FLASH_CR_PER           (FLASH_PECR_ERASE | FLASH_PECR_PROG)
#define FLASHPAGE_DIV          (4U) /* write 4 bytes in one go */
#else
#if defined(CPU_FAM_STM32L4)
#define FLASHPAGE_DIV          (8U)
#elif defined(CPU_FAM_STM32F4)
#define FLASHSECTORS_BANK      (12)
#define FLASHPAGE_DIV          (4U)
#else
#define FLASHPAGE_DIV          (2U)
#endif
#define CNTRL_REG              (FLASH->CR)
#define CNTRL_REG_LOCK         (FLASH_CR_LOCK)
#endif

extern void _lock(void);
extern void _unlock(void);
extern void _wait_for_pending_operations(void);

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

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
static inline int flashbank_sector(void *addr) {
    uint8_t sn = (uint8_t)(((uint32_t)addr - CPU_FLASH_BASE) / FLASHSECTOR_SIZE_MIN);
    if(sn > 3 && sn < 8) {
        sn = 4;
    }
    else if(sn > 8){
        sn = (sn / 8) + 4;
    }
    return sn;
}

static inline int flashsector_sector(void *addr) {
#if(FLASH_DUAL_BANK == 1)
    if((uint32_t) addr >= (STM32_FLASHSIZE / 2) + CPU_FLASH_BASE) {
        DEBUG("[flashsector]: dual bank sector \n");
        addr = (void *)((uint32_t) addr - (STM32_FLASHSIZE / 2));
        return FLASHSECTORS_BANK + flashbank_sector(addr);
    }
    else {
        DEBUG("[flashsector]: single bank sector \n");
        return flashbank_sector(addr);
    }
#else
    return flashbank_sector(addr);
#endif
}

static void _erase_sector(uint8_t sn)
{
    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    /* unlock the flash module */
    _unlock_flash();

    /* set parallelism to 32bits */
    CNTRL_REG &= FLASH_CR_PSIZE_Msk;
    CNTRL_REG |= (0x2 << FLASH_CR_PSIZE_Pos);

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    DEBUG("[flashsector] erase: setting the sector erase code\n");
    CNTRL_REG |= ((sn % FLASHSECTORS_BANK) << FLASH_CR_SNB_Pos);
#if( FLASH_DUAL_BANK == 1)
    CNTRL_REG |= (sn / FLASHSECTORS_BANK) * FLASH_CR_SNB_4;
#endif
    DEBUG("[flashsector] erase: setting the erase bit\n");
    CNTRL_REG |= FLASH_CR_SER;

    DEBUG("[flashsector] erase: trigger the page erase\n");
    CNTRL_REG |= FLASH_CR_STRT;

    /* wait as long as device is busy */
    _wait_for_pending_operations();

    /* reset PER bit */
    DEBUG("[flashsector] erase: resetting the sector erase bit\n");
    CNTRL_REG &= ~FLASH_CR_SER;

    /* lock the flash module again */
    _lock();
}

static void _erase_sector_page(void *page_addr)
{
    DEBUG("[flashsector] erase: address to erase: %p\n", page_addr);
    /* avoid erasing whole sector if "page" is blank*/
    bool blank = true;
    for (unsigned i = 0; i < FLASHPAGE_SIZE; i += sizeof(uint32_t)) {
        if (*(uint32_t *) (page_addr + i) != 0xffffffff) {
            blank = false;
            break;
        }
    }
    /* erase the sector if it failed the blank check */
    if (!blank) {
        uint8_t sn = flashsector_sector(page_addr);
        DEBUG("[flashsector] erase: erasing sector: %d\n", sn);
        _erase_sector(sn);
    }
}
#endif

#if !(defined(CPU_FAM_STM32F4))
static void _erase_page(void *page_addr)
{
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || \
    defined(CPU_FAM_STM32L4)
    uint32_t *dst = page_addr;
#else
    uint16_t *dst = page_addr;
#endif
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
    *dst = (uint32_t)0;
#elif defined(CPU_FAM_STM32L4)
    DEBUG("[flashpage] erase: setting the page address\n");
    uint8_t pn;
#if FLASHPAGE_NUMOF <= 256
    pn = (uint8_t)flashpage_page(dst);
#else
    uint16_t page = flashpage_page(dst);
    if (page > 255) {
        CNTRL_REG |= FLASH_CR_BKER;
    }
    else {
        CNTRL_REG &= ~FLASH_CR_BKER;
    }
    pn = (uint8_t)page;
#endif
    CNTRL_REG &= ~FLASH_CR_PNB;
    CNTRL_REG |= (uint32_t)(pn << FLASH_CR_PNB_Pos);
    CNTRL_REG |= FLASH_CR_STRT;
#else /* CPU_FAM_STM32F0 || CPU_FAM_STM32F1 || CPU_FAM_STM32F3 */
    DEBUG("[flashpage] erase: setting the page address\n");
    FLASH->AR = (uint32_t)dst;
    /* trigger the page erase and wait for it to be finished */
    DEBUG("[flashpage] erase: trigger the page erase\n");
    CNTRL_REG |= FLASH_CR_STRT;
#endif
    /* wait as long as device is busy */
    _wait_for_pending_operations();

    /* reset PER bit */
    DEBUG("[flashpage] erase: resetting the page erase bit\n");
    CNTRL_REG &= ~(FLASH_CR_PER);

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
#endif

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

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || \
    defined(CPU_FAM_STM32F4)
    uint32_t *dst = target_addr;
    const uint32_t *data_addr = data;
#elif defined(CPU_FAM_STM32L4)
    uint64_t *dst = target_addr;
    const uint64_t *data_addr = data;
#else
    uint16_t *dst = (uint16_t *)target_addr;
    const uint16_t *data_addr = data;
#endif

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

#if defined(CPU_FAM_STM32F4)
    /* set parallelism to 32bits */
    CNTRL_REG &= FLASH_CR_PSIZE_Msk;
    CNTRL_REG |= (0x2 << FLASH_CR_PSIZE_Pos);
#endif

    DEBUG("[flashpage_raw] write: now writing the data\n");
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4)
    /* set PG bit and program page to flash */
    CNTRL_REG |= FLASH_CR_PG;
#endif
    for (size_t i = 0; i < (len / FLASHPAGE_DIV); i++) {
        DEBUG("[flashpage_raw] writing %c to %p\n", (char)data_addr[i], dst);
        *dst++ = data_addr[i];
        /* wait as long as device is busy */
        _wait_for_pending_operations();
    }
    /* clear program bit again */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4)
    CNTRL_REG &= ~(FLASH_CR_PG);
#endif
    DEBUG("[flashpage_raw] write: done writing data\n");


    /* lock the flash module again */
    _lock();

#if !(defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || \
      defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F4))
    /* restore the HSI state */
    if (!hsi_state) {
        stmclk_disable_hsi();
    }
#endif
}

void flashpage_write(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    /* STM32L0/L1 only supports word sizes */
    uint32_t *page_addr = flashpage_addr(page);
#elif defined(CPU_FAM_STM32L4)
    uint64_t *page_addr = flashpage_addr(page);
#else
    /* Default is to support half-word sizes */
    uint16_t *page_addr = flashpage_addr(page);
#endif

#if !(defined(CPU_FAM_STM32F4))
    /* ERASE sequence */
    _erase_page(page_addr);
#else
    _erase_sector_page(page_addr);
#endif
    /* WRITE sequence */
    if (data != NULL) {
        flashpage_write_raw(page_addr, data, FLASHPAGE_SIZE);
    }
}
