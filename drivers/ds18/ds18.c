/*
 * Copyright (C) 2017 Frits Kuipers
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
 * @brief       Device driver implementation for the Maxim Integrated DS1822 and DS18B20 temperature sensors.
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 *
 * @}
 */

#include "ds18.h"
#include "ds18_internal.h"

#include "log.h"
#include "periph/gpio.h"
#include "timex.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void ds18_low(ds18_t *dev)
{
    /* Set gpio as output and clear pin */
    gpio_init(dev->pin, GPIO_OUT);
    gpio_clear(dev->pin);
}

static void ds18_release(ds18_t *dev)
{
    /* Init pin as input and enable pull-up */
    gpio_init(dev->pin, dev->in_mode);
}

static void ds18_write_bit(ds18_t *dev, uint8_t bit)
{
    /* Initiate write slot */
    ds18_low(dev);
    xtimer_usleep(1);

    /* Release pin when bit==1 */
    if (bit) {
        ds18_release(dev);
    }

    /* Wait for slot to end */
    xtimer_usleep(DS18_DELAY_SLOT);
    ds18_release(dev);
    xtimer_usleep(1);
}

static uint8_t ds18_read_bit(ds18_t *dev)
{
    uint8_t bit;
    uint32_t start, end, measurement;

    /* Initiate read slot */
    ds18_low(dev);
    xtimer_usleep(1);
    ds18_release(dev);

    /* Measure time low off device pin*/
    start = xtimer_now_usec();
    while (!gpio_read(dev->pin)) ;
    end = xtimer_now_usec();

    /* When gpio was low for less than the sample time, bit is high*/
    measurement = (end - start);
    bit = measurement < DS18_SAMPLE_TIME;

    /* Wait for slot to end */
    xtimer_usleep(DS18_DELAY_SLOT - measurement);

    return bit;
}

static uint8_t ds18_read_byte(ds18_t *dev)
{
    uint8_t byte = 0;

    for (int i = 0; i < 8; i++) {
        byte |= (ds18_read_bit(dev) << i);
    }

    return byte;
}

static void ds18_write_byte(ds18_t *dev, uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        ds18_write_bit(dev, byte & (0x01 << i));
    }
}

static int ds18_reset(ds18_t *dev)
{
    int res;

    /* Line low and sleep the reset delay */
    ds18_low(dev);
    xtimer_usleep(DS18_DELAY_RESET);

    /* Release and wait for the presence response */
    ds18_release(dev);
    xtimer_usleep(DS18_DELAY_PRESENCE);

    /* Check device presence */
    res = gpio_read(dev->pin);;

    /* Sleep for reset delay */
    xtimer_usleep(DS18_DELAY_RESET);

    return res;
}

int ds18_get_temperature(ds18_t *dev, int16_t *temperature)
{
    int res;

    res = ds18_reset(dev);
    if (res) {
        return DS18_ERROR;
    }

    DEBUG("[DS18] Convert T\n");
    ds18_write_byte(dev, DS18_CMD_SKIPROM);
    ds18_write_byte(dev, DS18_CMD_CONVERT);

    DEBUG("[DS18] Wait for convert T\n");
    xtimer_usleep(DS18_DELAY_CONVERT);

    DEBUG("[DS18] Reset and read scratchpad\n");
    res = ds18_reset(dev);
    if (res) {
        return DS18_ERROR;
    }

    ds18_write_byte(dev, DS18_CMD_SKIPROM);
    ds18_write_byte(dev, DS18_CMD_RSCRATCHPAD);

    uint8_t b1 = ds18_read_byte(dev);
    uint8_t b2 = ds18_read_byte(dev);

    DEBUG("[DS18] Received byte: 0x%02x\n", b1);
    DEBUG("[DS18] Received byte: 0x%02x\n", b2);

    /* Fixed point value to uint16_t */
    uint16_t degrees  = (b2 << 4) + (b1 >> 4);
    uint16_t mdegrees = (b1 & 0xF) * 6.25;

    /* Temperature in centi-degrees */
    *temperature = degrees * 100 + mdegrees;

    return DS18_OK;
}

int ds18_init(ds18_t *dev, ds18_params_t *params)
{
    /* Initialize the device and the pin */
    dev->pin = params->pin;
    dev->in_mode = params->in_mode;
    gpio_init(dev->pin, dev->in_mode);
    return DS18_OK;
}
