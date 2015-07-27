/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <string.h>

#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    uint32_t tmp;
    uint8_t *res = (uint8_t *)id;

    tmp = DEVINFO->UNIQUEL;
    memcpy((res + 4), &tmp, sizeof(uint32_t));
    tmp = DEVINFO->UNIQUEH;
    memcpy(res, &tmp, sizeof(uint32_t));
}
