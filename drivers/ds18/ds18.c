/*
 * Copyright (C) 2017 Frits Kuipers
 *               2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ds18
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Maxim Integrated DS1822 and
 *              DS18B20 temperature sensors.
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "ds18.h"
#include "ds18_internal.h"

#include "log.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void ds18_low(const ds18_t *dev)
{
    /* Set gpio as output and clear pin */
    gpio_init(dev->params.pin, GPIO_OUT);
    gpio_clear(dev->params.pin);
}

static void ds18_release(const ds18_t *dev)
{
    /* Init pin as input */
    gpio_init(dev->params.pin, dev->params.in_mode);
}

static void ds18_write_bit(const ds18_t *dev, uint8_t bit)
{
    /* Initiate write slot */
    ds18_low(dev);

    /* Release pin when bit==1 */
    if (bit) {
        ds18_release(dev);
    }

    /* Wait for slot to end */
    xtimer_usleep(DS18_DELAY_SLOT);
    ds18_release(dev);
    xtimer_usleep(1);
}

static int ds18_read_bit(const ds18_t *dev, uint8_t *bit)
{
    /* Initiate read slot */
    ds18_low(dev);
    ds18_release(dev);

#if defined(MODULE_DS18_OPTIMIZED)
    xtimer_usleep(DS18_SAMPLE_TIME);
    *bit = gpio_read(dev->params.pin);
    xtimer_usleep(DS18_DELAY_R_RECOVER);
    return DS18_OK;
#else
    uint32_t start, measurement = 0;

    /* Measure time low of device pin, timeout after slot time*/
    start = xtimer_now_usec();
    while (!gpio_read(dev->params.pin) && measurement < DS18_DELAY_SLOT) {
        measurement = xtimer_now_usec() - start;
    }

    /* If there was a timeout return error */
    if (measurement >= DS18_DELAY_SLOT) {
        return DS18_ERROR;
    }

    /* When gpio was low for less than the sample time, bit is high*/
    *bit = measurement < DS18_SAMPLE_TIME;

    /* Wait for slot to end */
    xtimer_usleep(DS18_DELAY_SLOT - measurement);

    return DS18_OK;
#endif
}

static int ds18_read_byte(const ds18_t *dev, uint8_t *byte)
{
    uint8_t bit = 0;
    *byte = 0;

    for (int i = 0; i < 8; i++) {
        if (ds18_read_bit(dev, &bit) == DS18_OK) {
            *byte |= (bit << i);
        }
        else {
            return DS18_ERROR;
        }
    }

    return DS18_OK;
}

static void ds18_write_byte(const ds18_t *dev, uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        ds18_write_bit(dev, byte & (0x01 << i));
    }
}

static int ds18_reset(const ds18_t *dev)
{
    int res;

    /* Line low and sleep the reset delay */
    ds18_low(dev);
    xtimer_usleep(DS18_DELAY_RESET);

    /* Release and wait for the presence response */
    ds18_release(dev);
    xtimer_usleep(DS18_DELAY_PRESENCE);

    /* Check device presence */
    res = gpio_read(dev->params.pin);

    /* Sleep for reset delay */
    xtimer_usleep(DS18_DELAY_RESET);

    return res;
}

int ds18_trigger(const ds18_t *dev)
{
    int res;

    res = ds18_reset(dev);
    if (res) {
        return DS18_ERROR;
    }

    /* Please note that this command triggers a conversion on all devices
     * connected to the bus. */
    ds18_write_byte(dev, DS18_CMD_SKIPROM);
    ds18_write_byte(dev, DS18_CMD_CONVERT);

    return DS18_OK;
}

int ds18_read(const ds18_t *dev, int16_t *temperature)
{
    int res;
    uint8_t b1 = 0, b2 = 0;

    DEBUG("[DS18] Reset and read scratchpad\n");
    res = ds18_reset(dev);
    if (res) {
        return DS18_ERROR;
    }

    ds18_write_byte(dev, DS18_CMD_SKIPROM);
    ds18_write_byte(dev, DS18_CMD_RSCRATCHPAD);

    if (ds18_read_byte(dev, &b1) != DS18_OK) {
        DEBUG("[DS18] Error reading temperature byte 1\n");
        return DS18_ERROR;
    }

    DEBUG("[DS18] Received byte: 0x%02x\n", b1);

    if (ds18_read_byte(dev, &b2) != DS18_OK) {
        DEBUG("[DS18] Error reading temperature byte 2\n");
        return DS18_ERROR;
    }

    DEBUG("[DS18] Received byte: 0x%02x\n", b2);

    int32_t measurement = (int16_t)(b2 << 8 | b1);
    *temperature = (int16_t)((100 * measurement) >> 4);

    return DS18_OK;
}

int ds18_get_temperature(const ds18_t *dev, int16_t *temperature)
{

    DEBUG("[DS18] Convert T\n");
    if (ds18_trigger(dev)) {
        return DS18_ERROR;
    }

    DEBUG("[DS18] Wait for convert T\n");
    xtimer_usleep(DS18_DELAY_CONVERT);

    return ds18_read(dev, temperature);
}

int ds18_init(ds18_t *dev, const ds18_params_t *params)
{
    int res;

    dev->params = *params;

    /* Deduct the input mode from the output mode. If pull-up resistors are
     * used for output then will be used for input as well. */
    dev->params.in_mode = (dev->params.out_mode == GPIO_OD_PU) ? GPIO_IN_PU : GPIO_IN;

    /* Initialize the device and the pin */
    res = gpio_init(dev->params.pin, dev->params.in_mode) == 0 ? DS18_OK : DS18_ERROR;

    return res;
}
