/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "irq.h"
#include "cpu.h"

#include "esp_common.h"
#include "esp/common_macros.h"

#include "esp/xtensa_ops.h"
#include "xtensa/config/core-isa.h"
#include "xtensa/xtensa_context.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Disable all maskable interrupts
 */
unsigned int IRAM irq_disable(void)
{
    uint32_t state;

    /* read and set interrupt level (RSIL) */
    __asm__ volatile ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) : "=a" (state) :: "memory");
    /* mask out everything else of the PS register that do not belong to
       interrupt level (bits 3..0) */
    state &= 0xf;

    DEBUG("%s %02x(%02x)\n", __func__, XCHAL_EXCM_LEVEL, (unsigned)state);
    return state;
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int IRAM irq_enable(void)
{
    uint32_t state;

    /* read and set interrupt level (RSIL) */
    __asm__ volatile ("rsil %0, 0" : "=a" (state) :: "memory");
    /* mask out everything else of the PS register that do not belong to
       interrupt level (bits 3..0) */
    state &= 0xf;

    DEBUG("%s %02x(%02x)\n", __func__, 0, (unsigned)state);
    return state;
}

/**
 * @brief Restore the state of the IRQ flags
 */
void IRAM irq_restore(unsigned int state)
{
    uint32_t old = 0;

    /* write interrupt level and sync */
    __asm__ volatile ("extui  %1, %1, 0, 4  \n" /* mask intlevel bits in param */
                      "rsr.ps %0            \n" /* read current PS value */
                      "movi.n a4, -16       \n"
                      "and    a4, a4, %0    \n" /* mask out intlevel bits in PS */
                      "or     a4, a4, %1    \n" /* or intlevel with PS */
                      "wsr.ps a4            \n" /* write back PS */
                      "rsync                \n"
                     : "+a" (old) : "a" (state) : "memory");

    DEBUG("%s %02x(%02x)\n", __func__, (unsigned)state, (unsigned)old & 0xf);
}

/**
 * @brief Test if IRQs are currently enabled
 */
bool IRAM irq_is_enabled(void)
{
    uint32_t reg;

    RSR(reg, 230);
    return (reg & 0xf) == 0;
}
