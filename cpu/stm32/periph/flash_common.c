/*
 * Copyright (C) 2018 Inria
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
 *
 * @}
 */

#include "cpu.h"

#define ENABLE_DEBUG           0
#include "debug.h"

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
/* Data EEPROM and control register unlock keys */
#define FLASH_KEY1             ((uint32_t)0x89ABCDEF)
#define FLASH_KEY2             ((uint32_t)0x02030405)
#define CNTRL_REG              (FLASH->PECR)
#define CNTRL_REG_LOCK         (FLASH_PECR_PELOCK)
#define KEY_REG                (FLASH->PEKEYR)
#elif defined(CPU_FAM_STM32L5)
#define FLASH_KEY1             ((uint32_t)0x45670123)
#define FLASH_KEY2             ((uint32_t)0xCDEF89AB)
#define CNTRL_REG              (FLASH->NSCR)
#define CNTRL_REG_LOCK         (FLASH_NSCR_NSLOCK)
#define KEY_REG                (FLASH->NSKEYR)
#define FLASH_SR_EOP           (FLASH_NSSR_NSEOP)
#else
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7)
#define FLASH_KEY1             ((uint32_t)0x45670123)
#define FLASH_KEY2             ((uint32_t)0xCDEF89AB)
#endif
#define CNTRL_REG              (FLASH->CR)
#define CNTRL_REG_LOCK         (FLASH_CR_LOCK)
#define KEY_REG                (FLASH->KEYR)
#endif

#if defined(CPU_FAM_STM32G0)
#define FLASH_SR_BSY           (FLASH_SR_BSY1)
#endif

#if defined(CPU_FAM_STM32L5)
#define FLASH_SR_BSY            (FLASH_NSSR_NSBSY)
#define FLASH_SR_REG            (FLASH->NSSR)
#else
#define FLASH_SR_REG            (FLASH->SR)
#endif

void _unlock(void)
{
    if (CNTRL_REG & CNTRL_REG_LOCK) {
        DEBUG("[flash-common] unlocking the flash module\n");
        KEY_REG = FLASH_KEY1;
        KEY_REG = FLASH_KEY2;
    }
}

void _lock(void)
{
    if (!(CNTRL_REG & CNTRL_REG_LOCK)) {
        DEBUG("[flash-common] locking the flash module\n");
        CNTRL_REG |= CNTRL_REG_LOCK;
    }
}

void _wait_for_pending_operations(void)
{
    if (FLASH_SR_REG & FLASH_SR_BSY) {
        DEBUG("[flash-common] waiting for any pending operation to finish\n");
        while (FLASH_SR_REG & FLASH_SR_BSY) {}
    }

    /* Clear 'end of operation' bit in status register, for other STM32 boards
       this bit is set only if EOPIE is set, which is currently not done */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1)
    FLASH_SR_REG |= FLASH_SR_EOP;
#endif
}
