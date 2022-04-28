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

#include <errno.h>

#include "onewire.h"
#include "onewire_internal.h"

#include "log.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void onewire_low(const onewire_t *dev)
{
    /* Set gpio as output and clear pin */
    gpio_init(dev->params.pin, GPIO_OUT);
    gpio_clear(dev->params.pin);
}

static void onewire_release(const onewire_t *dev)
{
    /* Init pin as input */
    gpio_init(dev->params.pin, dev->params.in_mode);
}

static void onewire_write_bit(const onewire_t *dev, uint8_t bit)
{
    /* Initiate write slot */
    onewire_low(dev);

    /* Release pin when bit==1 */
    if (bit) {
        onewire_release(dev);
    }

    /* Wait for slot to end */
    xtimer_usleep(DS18_DELAY_SLOT);
    onewire_release(dev);
    xtimer_usleep(1);
}

#if 1
static int onewire_read_bit(const onewire_t *dev, uint8_t *bit)
{
    /* Initiate read slot */
    onewire_low(dev);
    onewire_release(dev);

#if defined(MODULE_DS18_OPTIMIZED)
    xtimer_usleep(DS18_SAMPLE_TIME);
    *bit = gpio_read(dev->params.pin);
    xtimer_usleep(DS18_DELAY_R_RECOVER);
    return 0;
#else
    uint32_t start, measurement = 0;

    /* Measure time low of device pin, timeout after slot time*/
    start = xtimer_now_usec();
    while (!gpio_read(dev->params.pin) && measurement < DS18_DELAY_SLOT) {
        measurement = xtimer_now_usec() - start;
    }

    /* If there was a timeout return error */
    if (measurement >= DS18_DELAY_SLOT) {
        return -EIO;
    }

    /* When gpio was low for less than the sample time, bit is high*/
    *bit = measurement < DS18_SAMPLE_TIME;

    /* Wait for slot to end */
    xtimer_usleep(DS18_DELAY_SLOT - measurement);

    return 0;
#endif
}
#endif

#if 1
int onewire_read_byte(const onewire_t *dev, uint8_t *byte)
{
    uint8_t bit = 0;
    *byte = 0;

    for (int i = 0; i < 8; i++) {
        if (onewire_read_bit(dev, &bit) == 0) {
            *byte |= (bit << i);
        }
        else {
            return -EIO;
        }
    }

    return 0;
}
#endif

void onewire_write_byte(const onewire_t *dev, uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        onewire_write_bit(dev, byte & (0x01 << i));
    }
}

#if 0
static int onewire_reset(const onewire_t *dev)
{
    int res;

    /* Line low and sleep the reset delay */
    onewire_low(dev);
    xtimer_usleep(DS18_DELAY_RESET);

    /* Release and wait for the presence response */
    onewire_release(dev);
    xtimer_usleep(DS18_DELAY_PRESENCE);

    /* Check device presence */
    res = gpio_read(dev->params.pin);

    /* Sleep for reset delay */
    xtimer_usleep(DS18_DELAY_RESET);

    return res;
}
#endif

#if 0
int onewire_trigger(const onewire_t *dev)
{
    int res;

    res = onewire_reset(dev);
    if (res) {
        return -EIO;
    }

    /* Please note that this command triggers a conversion on all devices
     * connected to the bus. */
    onewire_write_byte(dev, DS18_CMD_SKIPROM);
    onewire_write_byte(dev, DS18_CMD_CONVERT);

    return 0;
}
#endif

#if 0
int onewire_read(const onewire_t *dev, int16_t *temperature)
{
    int res;
    uint8_t b1 = 0, b2 = 0;

    DEBUG("[DS18] Reset and read scratchpad\n");
    res = ds18_reset(dev);
    if (res) {
        return -EIO;
    }

    ds18_write_byte(dev, DS18_CMD_SKIPROM);
    ds18_write_byte(dev, DS18_CMD_RSCRATCHPAD);

    if (ds18_read_byte(dev, &b1) != 0) {
        DEBUG("[DS18] Error reading temperature byte 1\n");
        return DS18_ERROR;
    }

    DEBUG("[DS18] Received byte: 0x%02x\n", b1);

    if (ds18_read_byte(dev, &b2) != 0) {
        DEBUG("[DS18] Error reading temperature byte 2\n");
        return DS18_ERROR;
    }

    DEBUG("[DS18] Received byte: 0x%02x\n", b2);

    int32_t measurement = (int16_t)(b2 << 8 | b1);
    *temperature = (int16_t)((100 * measurement) >> 4);

    return 0;
}
#endif

int onewire_init(onewire_t *dev, const onewire_params_t *params)
{
    int res;

    dev->params = *params;

    /* Deduct the input mode from the output mode. If pull-up resistors are
     * used for output then will be used for input as well. */
    dev->params.in_mode = (dev->params.out_mode == GPIO_OD_PU) ? GPIO_IN_PU : GPIO_IN;

    /* Initialize the device and the pin */
    res = gpio_init(dev->params.pin, dev->params.in_mode) == 0 ? 0 : -EIO;

    return res;
}
