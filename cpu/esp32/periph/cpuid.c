/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
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
#include "soc/efuse_reg.h"

void cpuid_get(void *id)
{
    /* since ESP32 has two cores, the default MAC address is used as CPU id */
    uint32_t rdata1 = REG_READ(EFUSE_BLK0_RDATA1_REG);
    uint32_t rdata2 = REG_READ(EFUSE_BLK0_RDATA2_REG);

    uint8_t *tmp = id;

    tmp[0] = rdata2 >> 16;
    tmp[1] = rdata2 >> 8;
    tmp[2] = rdata2;
    tmp[3] = rdata1 >> 24;
    tmp[4] = rdata1 >> 16;
    tmp[5] = rdata1 >> 8;
    tmp[6] = rdata1;
}
