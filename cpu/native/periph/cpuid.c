/*
 * SPDX-FileCopyrightText: 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup cpu_native
 * @ingroup drivers_periph_cpuid
 * @brief   CPUID implementation
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <string.h>
#include <stdint.h>

#include "cpu_conf.h"
#include "native_internal.h"

#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    /* Just in case _native_id is shorter than CPUID_LEN: */
    size_t len = CPUID_LEN;
    if (sizeof(_native_id) < CPUID_LEN) {
        memset(((char*)id) + sizeof(_native_id), 0xff,
                CPUID_LEN - sizeof(_native_id));
        len = sizeof(_native_id);
    }
    memcpy(id, &(_native_id), len);
}
