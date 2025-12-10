/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "irq_arch.h"

#include "esp_attr.h"
#include "esp_cpu.h"
#include "soc/periph_defs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define RVHAL_EXCM_LEVEL    4

/**
 * @brief Disable all maskable interrupts
 */
unsigned int IRAM_ATTR irq_disable(void)
{
    uint32_t mstatus;
    /* clear MIE bit in register mstatus */
    __asm__ volatile ("csrrc %0, mstatus, %1" : "=r"(mstatus) : "rK"(MSTATUS_MIE) : "memory");

    /* save interrupt priority level threshold */
    uint32_t state = *((volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_THRESH_REG);
    /* set interrupt priority level threshold to exception level */
    *((volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_THRESH_REG) = RVHAL_EXCM_LEVEL;

    /* set MIE bit in register mstatus */
    __asm__ volatile ("csrrs %0, mstatus, %1" : "=r"(mstatus) : "rK"(MSTATUS_MIE) : "memory");

    DEBUG("%s %02x(%02x)\n", __func__, RVHAL_EXCM_LEVEL, (unsigned)state);
    return state;
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int IRAM_ATTR irq_enable(void)
{
    uint32_t state = *((volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_THRESH_REG);

    /* set interrupt priority level threshold to 0 */
    *((volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_THRESH_REG) = 0;

    /* small delay needed here */
    __asm__ volatile ( "nop" );
    __asm__ volatile ( "nop" );
    __asm__ volatile ( "nop" );

    DEBUG("%s %02x(%02x)\n", __func__, 0, (unsigned)state);
    return state;
}

/**
 * @brief Restore the state of the IRQ flags
 */
void IRAM_ATTR irq_restore(unsigned int state)
{
    uint32_t old = *((volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_THRESH_REG);

    /* set interrupt priority level threshold to old level */
    *((volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_THRESH_REG) = state;

    /* small delay needed here */
    __asm__ volatile ( "nop" );
    __asm__ volatile ( "nop" );
    __asm__ volatile ( "nop" );

    DEBUG("%s %02x(%02x)\n", __func__, (unsigned)state, (unsigned)old);
}

/**
 * @brief Test if IRQs are currently enabled
 */
bool IRAM_ATTR irq_is_enabled(void)
{
    return *((volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_THRESH_REG) == 0;
}
