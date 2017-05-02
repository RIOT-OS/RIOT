/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @brief       Low-level flash sector driver implementation
 *
 * @author      Jannik Beyerstedt <jannik.beyerstedt@haw-hamburg.de>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#if defined(FLASHSECTOR_SMALL_SECTOR) && defined(FLASHSECTOR_BANK_SIZE)
#include "periph/flashsector.h"

void flashsector_write(int sector, void *data, int size)
{
    assert(sector < FLASHSECTOR_NUMOF);

    uint16_t *sector_addr = flashsector_addr(sector);
    uint16_t *data_addr = (uint16_t *)data;
    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);

    /* the internal RC oscillator (HSI) must be enabled */
    RCC->CR |= (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {
    }

    /* unlock the flash module */
    DEBUG("[flashsector] unlocking the flash module\n");
    if ((FLASH->CR & FLASH_CR_LOCK)) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }

    /* ERASE sequence */
    /* make sure no flash operation is ongoing */
    DEBUG("[flashsector] erase: waiting for any operation to finish\n");
    while (FLASH->SR & FLASH_SR_BSY) {
    }
    /* set sector erase bit and program sector address */
    DEBUG("[flashsector] erase: setting the erase bit programming parallelism\n");
    FLASH->CR &= ~FLASH_CR_PSIZE;       /* clear PSIZE first */
    FLASH->CR |= FLASH_PSIZE_HALF_WORD; /* 16 bit programming */
    FLASH->CR &= ~FLASH_CR_SNB;         /* clear FLASH_CR_SNB first */
    FLASH->CR |= FLASH_CR_SER | (sector << POSITION_VAL(FLASH_CR_SNB));
    DEBUG("address to erase: %p\n", flashsector_addr(sector));
    /* trigger the sector erase and wait for it to be finished */
    DEBUG("[flashsector] erase: trigger the sector erase\n");
    FLASH->CR |= FLASH_CR_STRT;
    DEBUG("[flashsector] erase: wait as long as device is busy\n");
    while (FLASH->SR & FLASH_SR_BSY) {
    }
    /* clear SER bit */
    FLASH->CR &= ~FLASH_CR_SER;

    /* WRITE sequence */
    if (data != NULL) {
        DEBUG("[flashsector] write: now writing the data\n");
        /* set the PG bit and programming word size, then program data */
        FLASH->CR &= ~FLASH_CR_PSIZE;       /* clear PSIZE first */
        FLASH->CR |= FLASH_PSIZE_HALF_WORD; /* 16 bit programming */
        FLASH->CR |= FLASH_CR_PG;
        for (unsigned i = 0; i < (size / 2); i++) {
            *(__IO uint16_t*)sector_addr++ = data_addr[i];
            while (FLASH->SR & FLASH_SR_BSY) {
            }
        }
        /* clear program bit again */
        FLASH->CR &= (~FLASH_CR_PG);
        DEBUG("[flashsector] write: done writing data\n");
    }

    /* finally, lock the flash module again */
    DEBUG("flashsector] now locking the flash module again\n");
    FLASH->CR |= FLASH_CR_LOCK;

    /* restore the HSI state */
    if (!hsi_state) {
        RCC->CR &= ~(RCC_CR_HSION);
        while (RCC->CR & RCC_CR_HSIRDY) {
        }
    }
}

void flashsector_write_only(void *target, void *data, int size) {
    __IO uint16_t *target_addr = (uint16_t *)target;
    __IO uint16_t *data_addr = (uint16_t *)data;
    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);

    /* the internal RC oscillator (HSI) must be enabled */
    RCC->CR |= (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {
    }

    /* unlock the flash module */
    DEBUG("[flashsector] unlocking the flash module\n");
    if ((FLASH->CR & FLASH_CR_LOCK)) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }

    /* WRITE sequence */
    if (data != NULL) {
        DEBUG("[flashsector] write: now writing the data\n");
        /* set the PG bit and programming word size, then program data */
        FLASH->CR &= ~FLASH_CR_PSIZE;       /* clear PSIZE first */
        FLASH->CR |= FLASH_PSIZE_HALF_WORD; /* 16 bit programming */
        FLASH->CR |= FLASH_CR_PG;
        for (unsigned i = 0; i < (size / 2); i++) {
            *(__IO uint16_t*)target_addr++ = data_addr[i];
            while (FLASH->SR & FLASH_SR_BSY) {
            }
        }
        /* clear program bit again */
        FLASH->CR &= (~FLASH_CR_PG);
        DEBUG("[flashsector] write: done writing data\n");
    }

    /* finally, lock the flash module again */
    DEBUG("flashsector] now locking the flash module again\n");
    FLASH->CR |= FLASH_CR_LOCK;

    /* restore the HSI state */
    if (!hsi_state) {
        RCC->CR &= ~(RCC_CR_HSION);
        while (RCC->CR & RCC_CR_HSIRDY) {
        }
    }
}

/* remember, that the first sector is number 0 */
void *flashsector_addr(int sector)
{
    uint32_t offset = 0;

    if (sector < 5) {
        offset = FLASHSECTOR_SMALL_SECTOR * 1024U * sector;
    }
    else if (5 == sector) {
        offset = FLASHSECTOR_SMALL_SECTOR * 1024U * 8;
    }
    else {
        offset = FLASHSECTOR_SMALL_SECTOR * 1024U * 8 + (FLASHSECTOR_SMALL_SECTOR * 1024U * 8) * (sector - 5);
    }

    return (void *)(FLASH_BASE + offset);
}

/* remember, that the first sector is number 0 */
int flashsector_sector(void *addr)
{
    uint8_t pseudo_sector = ((uint32_t)addr - FLASH_BASE) / (uint32_t)(FLASHSECTOR_SMALL_SECTOR * 1024U);

    if (pseudo_sector < 4) {
        return pseudo_sector;
    }
    else if (4 <= pseudo_sector && pseudo_sector < 8) {
        return 4;
    }
    else {
        return ((pseudo_sector / 8) + 4);
    }

}

#endif /* defined(FLASHSECTOR_SMALL_SECTOR) && defined(FLASHSECTOR_BANK_SIZE) */
