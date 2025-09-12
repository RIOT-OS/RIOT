/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_cpuid
 * @{
 *
 * @file
 * @brief       Implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <string.h>
#include <stdint.h>

#include "periph/cpuid.h"
#include "sdk/sdk.h"

void cpuid_get(void *id)
{
    uint32_t chip_id = system_get_chip_id();
    memcpy(id, &(chip_id), CPUID_LEN);
}
