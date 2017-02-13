/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Low-level flash page driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#if defined(FLASHPAGE_SIZE) && defined(FLASHPAGE_NUMOF)
#include "periph/flashpage.h"

void flashpage_write(int page, void *data)
{
    assert(page < FLASHPAGE_NUMOF);

    uint16_t *page_addr = flashpage_addr(page);
    uint16_t *data_addr = (uint16_t *)data;
    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);

    /* the internal RC oscillator (HSI) must be enabled */
    RCC->CR |= (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}

    /* unlock the flash module */
    DEBUG("[flashpage] unlocking the flash module\n");
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }

    /* ERASE sequence */
    /* make sure no flash operation is ongoing */
    DEBUG("[flashpage] erase: waiting for any operation to finish\n");
    while (FLASH->SR & FLASH_SR_BSY) {}
    /* set page erase bit and program page address */
    DEBUG("[flashpage] erase: setting the erase bit and page address\n");
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = (uint32_t)flashpage_addr(page);
    DEBUG("address to erase: %p\n", flashpage_addr(page));
    /* trigger the page erase and wait for it to be finished */
    DEBUG("[flashpage] erase: trigger the page erase\n");
    FLASH->CR |= FLASH_CR_STRT;
    DEBUG("[flashpage] erase: wait as long as device is busy\n");
    while (FLASH->SR & FLASH_SR_BSY) {}
    /* reset PER bit */
    DEBUG("[flashpage] erase: resetting the page erase bit\n");
    FLASH->CR &= ~(FLASH_CR_PER);

    /* WRITE sequence */
    if (data != NULL) {
        DEBUG("[flashpage] write: now writing the data\n");
        /* set PG bit and program page to flash */
        FLASH->CR |= FLASH_CR_PG;
        for (unsigned i = 0; i < (FLASHPAGE_SIZE / 2); i++) {
            *page_addr++ = data_addr[i];
            while (FLASH->SR & FLASH_SR_BSY) {}
        }
        /* clear program bit again */
        FLASH->CR &= ~(FLASH_CR_PG);
        DEBUG("[flashpage] write: done writing data\n");
    }

    /* finally, lock the flash module again */
    DEBUG("flashpage] now locking the flash module again\n");
    FLASH->CR |= FLASH_CR_LOCK;

    /* restore the HSI state */
    if (!hsi_state) {
        RCC->CR &= ~(RCC_CR_HSION);
        while (RCC->CR & RCC_CR_HSIRDY) {}
    }
}

#endif /* defined(FLASHPAGE_SIZE) && defined(FLASHPAGE_NUMOF) */
