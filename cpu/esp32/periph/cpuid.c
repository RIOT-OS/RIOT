/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * ESP32x SoCs don't have a real chip id. The factory-programmed MAC address
 * from eFuse is used instead. It always has 6 bytes, also on SoCs with
 * IEEE 802.15.4 radio where esp_efuse_mac_get_default would return the
 * 8 byte EUI-64 address.
 */

void cpuid_get(void *id)
{
    _Static_assert(CPUID_LEN == 6, "CPUID_LEN has to be 6");
    esp_read_mac(id, ESP_MAC_EFUSE_FACTORY);
}
