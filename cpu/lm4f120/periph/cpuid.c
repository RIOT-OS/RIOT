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
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#include <string.h>
#include "cpu-conf.h"

#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    memcpy(id, (void *)(CPUID_ADDR), CPUID_ID_LEN);
}

/** @} */
