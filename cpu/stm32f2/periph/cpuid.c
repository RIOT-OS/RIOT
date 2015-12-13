/*
 * Copyright (C) 2015 Engineering-Spirit
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
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#include <string.h>

#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    memcpy(id, (void *)(0x1fff7a10), CPUID_ID_LEN);
}

/** @} */
