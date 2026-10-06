/*
 * SPDX-FileCopyrightText: 2026 Mathis LECRIVAIN <lecrivain.mathis@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_max1161x
 * @{
 *
 * @file
 * @brief       MAX1161X ADC device driver
 *
 * @author      Mathis Lécrivain <lecrivain.mathis@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "max1161x.h"
#include "max1161x_params.h"
#include "max1161x_regs.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV             (dev->params.i2c)
#define ADDR            (dev->params.addr)

/* Power-up values of the setup and configuration bytes */
#define SETUP_POWER_UP  (MAX1161X_SETUP_REG | MAX1161X_SETUP_NO_RESET)
#define CONFIG_POWER_UP (MAX1161X_CONFIG_REG | MAX1161X_CONFIG_SINGLE_ENDED)

static int _write_byte(const max1161x_t *dev, uint8_t byte)
{
    return i2c_write_bytes(DEV, ADDR, &byte, sizeof(byte), 0);
}

int max1161x_init(max1161x_t *dev, const max1161x_params_t *params)
{
    int status;

    assert(dev && params);

    dev->params = *params;

    /* Prepare setup byte. */
    dev->setup = MAX1161X_SETUP_REG | MAX1161X_SETUP_NO_RESET |
                 (params->ref << MAX1161X_SETUP_SEL_POS);

    if (params->clock == MAX1161X_CLOCK_EXT) {
        dev->setup |= MAX1161X_SETUP_CLK_EXTERNAL;
    }

    if (params->unibipolar == MAX1161X_BIPOLAR) {
        dev->setup |= MAX1161X_SETUP_BIPOLAR;
    }

    /* Prepare configuration byte. */
    dev->config = MAX1161X_CONFIG_REG |
                  (params->scan << MAX1161X_CONFIG_SCAN_POS) |
                  (params->channel << MAX1161X_CONFIG_CS_POS);

    if (params->sgldiff == MAX1161X_SINGLE_ENDED) {
        dev->config |= MAX1161X_CONFIG_SINGLE_ENDED;
    }

    i2c_acquire(DEV);

    status = _write_byte(dev, dev->setup);
    if (status < 0) {
        i2c_release(DEV);
        DEBUG("[max1161x] init error: unable to write setup byte (err=%d)\n", status);
        return -ENODEV;
    }

    status = _write_byte(dev, dev->config);
    if (status < 0) {
        i2c_release(DEV);
        DEBUG("[max1161x] init error: unable to write config byte (err=%d)\n", status);
        return status;
    }

    i2c_release(DEV);

    return 0;
}

int max1161x_reset(max1161x_t *dev)
{
    int status;

    assert(dev);

    /* Clearing the reset bit also resets the configuration byte to its default. */
    i2c_acquire(DEV);
    status = _write_byte(dev, MAX1161X_SETUP_REG);
    i2c_release(DEV);
    if (status < 0) {
        DEBUG("[max1161x] reset error: unable to write setup byte (err=%d)\n", status);
        return status;
    }

    dev->setup = SETUP_POWER_UP;
    dev->config = CONFIG_POWER_UP;

    return 0;
}

int max1161x_read_raw(const max1161x_t *dev, int16_t *raw)
{
    uint8_t buf[2];
    int status;

    assert(dev && raw);

    i2c_acquire(DEV);
    status = i2c_read_bytes(DEV, ADDR, buf, sizeof(buf), 0);
    i2c_release(DEV);
    if (status < 0) {
        DEBUG("[max1161x] read error: unable to read conversion result (err=%d)\n", status);
        return status;
    }

    int16_t value = (((buf[0] << 8) | buf[1]) & MAX1161X_DATA_MASK);

    /* Bipolar conversion results are in two's complement format. */
    if ((dev->setup & MAX1161X_SETUP_BIPOLAR) && (value & MAX1161X_DATA_SIGN_BIT)) {
        value -= (MAX1161X_DATA_MASK + 1);
    }

    *raw = value;

    return 0;
}

int max1161x_read_channel_raw(max1161x_t *dev, max1161x_channel_t chan, int16_t *raw)
{
    int status;

    assert(dev && raw);
    assert((int)chan < MAX1161X_NUM_CHANNELS);

    /* Select the channel and request a single conversion. */
    dev->config &= ~(MAX1161X_CONFIG_SCAN_MASK | MAX1161X_CONFIG_CS_MASK);
    dev->config |= (MAX1161X_SCAN_NONE << MAX1161X_CONFIG_SCAN_POS) |
                   (chan << MAX1161X_CONFIG_CS_POS);

    i2c_acquire(DEV);
    status = _write_byte(dev, dev->config);
    i2c_release(DEV);
    if (status < 0) {
        DEBUG("[max1161x] read error: unable to write config byte (err=%d)\n", status);
        return status;
    }

    return max1161x_read_raw(dev, raw);
}
