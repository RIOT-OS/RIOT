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

#include "periph/cpuid.h"

extern volatile uint32_t _cpuid_address[3];

void cpuid_get(void *id)
{
    memcpy(id, (void *)(&_cpuid_address), CPUID_ID_LEN);
}

/** @} */
