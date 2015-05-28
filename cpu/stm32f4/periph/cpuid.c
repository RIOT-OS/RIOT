/*
 * Copyright (C) 2015 James Hollister
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      James Hollister <jhollisterjr@gmail.com>
 */

#include <string.h>

#include "periph/cpuid.h"

#define STM32F4_CPUID_ADDR  (0x1fff7a10)

void cpuid_get(void *id)
{
    memcpy(id, (void *)(STM32F4_CPUID_ADDR), CPUID_ID_LEN);
}

/** @} */
