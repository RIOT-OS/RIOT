/*
 * Copyright (C) 2025 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_gp8xxx
 * @{
 *
 * @file
 * @brief       Device driver implementation for the gp8xxx
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "gp8xxx.h"
#include "gp8xxx_constants.h"
#include "gp8xxx_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define GP8XXX_I2C      (dev->params.i2c_dev)
#define GP8XXX_ADDR     (dev->params.address)

#if GP8XXX_HAS_VDAC
#define GP8XXX_RANGE    (dev->params.range)
#endif

int gp8xxx_init(gp8xxx_t *dev, const gp8xxx_params_t *params)
{
    /* initialize the device descriptor */
    dev->params = *params;

    /* setup the i2c bus */
    i2c_acquire(GP8XXX_I2C);

    /* initialize the peripheral */
    if (i2c_write_byte(GP8XXX_I2C, GP8XXX_ADDR, 0, 0) != 0) {
        DEBUG("[gp8xxx] gp8xxx_init: init failed\n");
        i2c_release(GP8XXX_I2C);
        return GP8XXX_ERR_NODEV;
    }

    i2c_release(GP8XXX_I2C);

    /* configure a peripheral to a known state */
#if GP8XXX_HAS_VDAC
    if (dev->params.info->type == GP8XXX_INFO_TYPE_VDAC) {
        gp8xxx_set_voltage_range(dev, GP8XXX_RANGE);
        gp8xxx_set_voltage(dev, GP8XXX_CHANNEL_ALL, 0);
    }
#endif
#if GP8XXX_HAS_IDAC
    if (dev->params.info->type == GP8XXX_INFO_TYPE_IDAC) {
        dev->range = GP8XXX_OUTPUT_RANGE_25_MA;
        gp8xxx_set_current(dev, GP8XXX_CHANNEL_ALL, 0);
    }
#endif

    return GP8XXX_OK;
}

int gp8xxx_set_dac(gp8xxx_t *dev, gp8xxx_channel_t channel, uint16_t value)
{
    assert(channel < dev->params.info->channels || channel == GP8XXX_CHANNEL_ALL);
    assert(value <= dev->params.info->resolution);

    DEBUG("[gp8xxx] gp8xxx_set_dac: channel=%d value=%d\n", (int)channel, value);

    /* value must be sent MSB */
    if (dev->params.info->resolution == GP8XXX_INFO_RESOLUTION_12_BIT) {
        value = value << 4;
    }
    else if (dev->params.info->resolution == GP8XXX_INFO_RESOLUTION_15_BIT) {
        value = value << 1;
    }

    /* when both channels are selected, write 4 bytes instead of 2 bytes */
    uint8_t out[] = {
        GP8XXX_REG_CHANNEL << (int)(channel == GP8XXX_CHANNEL_ALL ? 0 : channel),
            value & 0xff,
            value >> 8,
            value & 0xff,
            value >> 8
    };

    size_t size = (channel == GP8XXX_CHANNEL_ALL ? 5 : 3);

    i2c_acquire(GP8XXX_I2C);

    if (i2c_write_bytes(GP8XXX_I2C, GP8XXX_ADDR, &out, size, 0) != 0) {
        DEBUG("[gp8xxx] gp8xxx_set_dac: set raw value failed\n");
        i2c_release(GP8XXX_I2C);
        return GP8XXX_ERR_I2C;
    }

    i2c_release(GP8XXX_I2C);

    return GP8XXX_OK;
}

#if GP8XXX_HAS_VDAC
int gp8xxx_set_voltage_range(gp8xxx_t *dev, gp8xxx_output_range_t range)
{
    assert(dev->params.info->type == GP8XXX_INFO_TYPE_VDAC);

    /* only the 5 V/10 V models have a customizable range */
    if (dev->params.info->range == GP8XXX_INFO_RANGE_5V_10V) {
        uint8_t value = 0x00;

        switch (range) {
        case GP8XXX_OUTPUT_RANGE_5V:
            value = 0x00;
            break;
        case GP8XXX_OUTPUT_RANGE_10V:
            value = 0x11;
            break;
        default:
            return GP8XXX_ERR_RANGE;
        }

        i2c_acquire(GP8XXX_I2C);

        if (i2c_write_reg(GP8XXX_I2C, GP8XXX_ADDR, GP8XXX_REG_OUTPUT, value, 0) != 0) {
            DEBUG("[ERROR] Cannot set voltage range.\n");
            i2c_release(GP8XXX_I2C);
            return GP8XXX_ERR_I2C;
        }

        i2c_release(GP8XXX_I2C);
    }

    /* store range for calculation purposes */
    dev->range = range;

    return GP8XXX_OK;
}

int gp8xxx_set_voltage(gp8xxx_t *dev, gp8xxx_channel_t channel, uint16_t voltage)
{
    assert(dev->params.info->type == GP8XXX_INFO_TYPE_VDAC);

    if (voltage > dev->range) {
        DEBUG("[gp8xxx] gp8xxx_set_voltage: voltage out of range\n");
        return GP8XXX_ERR_RANGE;
    }

    uint32_t value = ((uint32_t)voltage * (dev->params.info->resolution - 1) + (dev->range / 2)) /
                     dev->range;

    return gp8xxx_set_dac(dev, channel, (uint16_t)value);
}
#endif /* GP8XXX_HAS_VDAC */

#if GP8XXX_HAS_IDAC
void gp8xxx_set_current_calibration(gp8xxx_t *dev, uint16_t calibration_4ma,
                                    uint16_t calibration_20ma)
{
    assert(dev->params.info->type == GP8XXX_INFO_TYPE_IDAC);
    assert((calibration_4ma == 0 && calibration_20ma == 0) || calibration_4ma < calibration_20ma);

    dev->calibration_4ma = calibration_4ma;
    dev->calibration_20ma = calibration_20ma;
}

int gp8xxx_set_current(gp8xxx_t *dev, gp8xxx_channel_t channel, uint16_t current)
{
    assert(dev->params.info->type == GP8XXX_INFO_TYPE_IDAC);

    /* ensure current is within configured range */
    if (current > dev->range) {
        DEBUG("[gp8xxx] gp8xxx_set_current: current out of range\n");
        return GP8XXX_ERR_RANGE;
    }

    /* compute ADC value based on either calibration data or configured range */
    uint32_t value;

    if (dev->calibration_4ma > 0 && dev->calibration_20ma > 0 && current >= 4000 &&
        current <= 20000) {
        value = dev->calibration_4ma +
                ((uint32_t)(current - 4000) * (dev->calibration_20ma - dev->calibration_4ma)) /
                (20000 - 4000);
    }
    else {
        value = ((uint32_t)current * (dev->params.info->resolution - 1) + (dev->range / 2)) /
                (dev->range);
    }

    return gp8xxx_set_dac(dev, channel, (uint16_t)value);
}
#endif /* GP8XXX_HAS_IDAC */
