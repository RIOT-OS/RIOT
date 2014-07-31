/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
    memset(id, 0xff, CPUID_ID_LEN);   /* Just in case _native_id is shorter
                                         than CPUID_ID_LEN. */
    memcpy(id, &(_native_id), sizeof(_native_id));
}

/**
 * @}
 */
