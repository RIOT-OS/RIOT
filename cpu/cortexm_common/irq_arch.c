/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "irq.h"
#include "cpu.h"

/**
 * @brief Disable all maskable interrupts
 */
unsigned int irq_disable(void)
{
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    return mask;
}

/**
 * @brief Enable all maskable interrupts
 */
__attribute__((used)) unsigned int irq_enable(void)
{
    __enable_irq();
    return __get_PRIMASK();
}

/**
 * @brief Restore the state of the IRQ flags
 */
void irq_restore(unsigned int state)
{
    __set_PRIMASK(state);
}

/**
 * @brief See if the current context is inside an ISR
 */
int irq_is_in(void)
{
    return (__get_IPSR() & 0xFF);
}
