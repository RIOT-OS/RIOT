/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation for ESP32x SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/hwrng.h"

#include "bootloader_random.h"
#include "esp_random.h"

void hwrng_init(void)
{
    /* If the ADC SAR is used, the Bootloader RNG must not be enabled before
     * the random numbers are actually required. The reason is that the
     * Bootloader RNG uses the noise of the ADC SAR reference voltage as
     * a non-RF entropy source. The calibration of the ADC SAR does not
     * work correctly in this case. Therefore, the Bootloader RNG is only
     * enabled if random numbers are really required. */
}

/**
 * The RNG implementation of the ESP-IDF also uses the hardware RNG register
 * RNG_DATA_REG (WDEV_RND_REG) for random number generation, which adds 2 bits
 * of entropy at each APB clock cycle when WiFi or BT are enabled. However,
 * it ensures that the random numbers are not read faster than generated
 * in hardware by including some busy waiting. We therefore use this
 * implementation here.
 */
void hwrng_read(void *buf, unsigned int num)
{
    if (!IS_USED(MODULE_ESP_WIFI_ANY) && !IS_USED(MODULE_ESP_BLE)) {
        /* enable the Bootloader RNG if WiFi and BT are not used */
        bootloader_random_enable();
    }

    esp_fill_random(buf, num);

    if (!IS_USED(MODULE_ESP_WIFI_ANY) && !IS_USED(MODULE_ESP_BLE)) {
        /* disable the Bootloader RNG to ensure that ADC SAR calibration works */
        bootloader_random_disable();
    }
}

uint32_t hwrand(void)
{
    uint32_t rand;
    hwrng_read(&rand, 4);
    return rand;
}
