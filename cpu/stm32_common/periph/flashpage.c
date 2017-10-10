/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#include "periph/flashpage.h"

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
void flashpage_write(int page, void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

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
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
/* Flash program erase key1 */
#define PEKEY1                    (0x89ABCDEF)

/* Flash program erase key: used with FLASH_PEKEY2 to unlock the write
 * access to the FLASH_PECR register and data EEPROM
 */
#define PEKEY2                    (0x02030405)

/* Flash program memory key1 */
#define PRGKEY1                   (0x8C9DAEBF)

/* Flash program memory key2: used with FLASH_PRGKEY2 to unlock
 * the program memory
 */
#define PRGKEY2                   (0x13141516)

void flashpage_write(int page, void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    /* Using 32bit addresses since we are attempting fast-word erasing/writing*/
    uint32_t *page_addr = flashpage_addr(page);
    uint32_t *data_addr = (uint32_t*)data;

    DEBUG("[flashpage] unlocking the flash module\n");
    /* Unlocking the Data memory and FLASH_PECR register access*/
    if(FLASH->PECR & FLASH_PECR_PRGLOCK)
    {
        /* Unlock for erase */
        FLASH->PEKEYR = PEKEY1;
        FLASH->PEKEYR = PEKEY2;

        /* Unlock flash registers */
        FLASH->PRGKEYR = PRGKEY1;
        FLASH->PRGKEYR = PRGKEY2;
    }

    /* ERASE sequence */
    /* make sure no flash operation is ongoing */
    DEBUG("[flashpage] erase: waiting for any operation to finish\n");
    while (FLASH->SR & FLASH_SR_BSY) {}

    DEBUG("[flashpage] erase: setting the erase and program bits\n");
    /* Set the ERASE bit */
    FLASH->PECR |= FLASH_PECR_ERASE;
    /* Set PROG bit */
    FLASH->PECR |= FLASH_PECR_PROG;

    DEBUG("address to erase: %p\n", flashpage_addr(page));
    DEBUG("[flashpage] erase: trigger the page erase\n");
    /* Write 00000000h to the first word of the program page to erase */
    *page_addr = 0x00000000;
    /* Wait for it to be finished */
    DEBUG("[flashpage] erase: wait as long as device is busy\n");
    while (FLASH->SR & FLASH_SR_BSY) {}

    /* If the erase operation is completed, disable the ERASE and PROG bits */
    DEBUG("[flashpage] erase: resetting the page erase and prog bit\n");
    FLASH->PECR &= (uint32_t)(~FLASH_PECR_PROG);
    FLASH->PECR &= (uint32_t)(~FLASH_PECR_ERASE);

    if (data != NULL) {
        DEBUG("[flashpage] write: now writing the data\n");
        for (unsigned i = 0; i < (FLASHPAGE_SIZE / 4); i++) {
            DEBUG("[flashpage] writing %c to %p\n", (char)data_addr[i], page_addr);
            *page_addr++ = data_addr[i];
            while (FLASH->SR & FLASH_SR_BSY) {}
        }
        DEBUG("[flashpage] write: done writing data\n");
    }

    DEBUG("flashpage] now locking the flash module again\n");
    /* Set the PRGLOCK and PELOCK Bit to lock the program memory access */
    FLASH->PECR |= FLASH_PECR_PRGLOCK;
    FLASH->PECR |= FLASH_PECR_PELOCK;
}

#endif /* defined(FLASHPAGE_SIZE) && defined(FLASHPAGE_NUMOF) */
