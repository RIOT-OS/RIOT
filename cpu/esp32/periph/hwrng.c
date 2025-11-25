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
#include "soc/wdev_reg.h"

#define RNG_DATA_REG (*(volatile uint32_t *)RNG_DATA_REG_ADDR)

void hwrng_init(void)
{
    if (!IS_USED(MODULE_WIFI_ANY)) {
        /*
         * The hardware RNG generates random numbers uses the noise in the
         * RF system of the WiFi or the BT interface as entropy source.
         * If both are disabled, the random number generator just returns
         * pseudo-random numbers.
         * However, the bootloader use an internal non-RF entropy source,
         * the internal reference voltage noise. This can be re-enabled
         * after startup as entropy source for applications that don't
         * use the WiFi or the BT interface.
         */
        bootloader_random_enable();
    }
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
    esp_fill_random(buf, num);
}

uint32_t hwrand(void)
{
    return esp_random();
}
