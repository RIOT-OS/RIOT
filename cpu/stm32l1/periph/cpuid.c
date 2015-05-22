/*
 * Copyright (C) 2014 FU Berlin
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
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#include <string.h>
#include "cpu-conf.h"

#include "periph/cpuid.h"

#define STM32L1_CPUID_ADDR  (0x1ff800d0)

void cpuid_get(void *id)
{
    memcpy(id, (void *)(STM32L1_CPUID_ADDR), CPUID_ID_LEN);
}

/** @} */
