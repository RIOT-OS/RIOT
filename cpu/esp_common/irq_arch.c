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

#include "esp/common_macros.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Set on entry into and reset on exit from an ISR
 */
volatile uint32_t irq_interrupt_nesting = 0;

/**
 * @brief See if the current context is inside an ISR
 */
bool IRAM irq_is_in(void)
{
    DEBUG("irq_interrupt_nesting = %" PRIu32 "\n", irq_interrupt_nesting);
    return irq_interrupt_nesting;
}
