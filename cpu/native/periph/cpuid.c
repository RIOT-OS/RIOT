/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file        cpuid.c
 * @brief       Implementation
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <string.h>
#include <stdint.h>

#include "cpu-conf.h"
#include "native_internal.h"

#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    /* Just in case _native_id is shorter than CPUID_ID_LEN: */
    size_t len = CPUID_ID_LEN;
    if (sizeof(_native_id) < CPUID_ID_LEN) {
        memset(((char*)id) + sizeof(_native_id), 0xff,
                CPUID_ID_LEN - sizeof(_native_id));
        len = sizeof(_native_id);
    }
    memcpy(id, &(_native_id), len);
}

/**
 * @}
 */
