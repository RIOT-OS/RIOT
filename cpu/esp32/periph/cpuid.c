/*
 * Copyright (C) 2022 Gunar Schorcht
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

#include "periph_cpu.h"
#include "esp_mac.h"

/*
 * ESP32x SoCs don't have a real chip id. The factory-programmed default MAC
 * address from EFUSE is used instead.
 */

void cpuid_get(void *id)
{
#if defined(CPU_FAM_ESP32H2) && defined(CONFIG_IEEE802154_ENABLED)
    /* ESP32H2 has IEEE802.15.4 radio which has an EUI64 address. Function
     * esp_efuse_mac_get_default will return this 8 byte address if
     * CONFIG_IEEE802154_ENABLED */
    _Static_assert(CPUID_LEN == 8,
                  "CPUID_LEN hast to be 8 if IEEE 802.15.4 interface enabled");
#else
    _Static_assert(CPUID_LEN == 6, "CPU_ID_LEN hast to be 6");
#endif

    esp_efuse_mac_get_default(id);
}
