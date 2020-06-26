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

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include <stdint.h>
#include <stdio.h>

#include "irq.h"
#include "cpu.h"

#include "esp_common.h"
#include "esp/common_macros.h"
#include "esp/xtensa_ops.h"
#include "xtensa/xtensa_context.h"

/**
 * @brief Set on entry into and reset on exit from an ISR
 */
volatile uint32_t irq_interrupt_nesting = 0;

