/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/hwrng.h"

#if !defined(MCU_ESP8266)
#include "bootloader_random.h"
#include "esp_random.h"
#include "soc/wdev_reg.h"

#define ESP32_USE_ESP_IDF_RANDOM    1

#endif

#define RNG_DATA_REG (*(volatile uint32_t *)RNG_DATA_REG_ADDR)

void hwrng_init(void)
{
#if defined(MCU_ESP8266)
    /* no need for initialization */
#else
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
#endif
}

#if defined(MCU_ESP8266) || !defined(ESP32_USE_ESP_IDF_RANDOM)

void hwrng_read(void *buf, unsigned int num)
{
    unsigned int count = 0;
    uint8_t *b = (uint8_t *)buf;

    while (count < num) {
        /* read next 4 bytes of random data */
        uint32_t tmp = RNG_DATA_REG;

        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            b[count++] = (uint8_t)tmp;
            tmp = tmp >> 8;
        }
    }
}

uint32_t hwrand(void)
{
    uint32_t _tmp;
    hwrng_read(&_tmp, sizeof(uint32_t));
    return _tmp;
}

#else

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

#endif
