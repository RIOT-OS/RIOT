/*
 * SPDX-FileCopyrightText: 2017-2018 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_adxl345
 * @{
 *
 * @file
 * @brief       Device driver implementation for the ADXL345 accelerometer (i2c only)
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "periph/i2c.h"
#include "byteorder.h"
#include "adxl345.h"
#include "adxl345_regs.h"
#include "adxl345_params.h"

#define ENABLE_DEBUG                0
#include "debug.h"

#define ADXL345_BUS                 (dev->params.i2c)
#define ADXL345_ADDR                (dev->params.addr)

#define ADXL345_PARAM_SCALE_FACTOR  (4)

int adxl345_init(adxl345_t *dev, const adxl345_params_t* params)
{
    uint8_t reg;

    assert(dev && params);

    /* get device descriptor */
    dev->params = *params;

    /* get scale_factor from full_res and range parameters */
    dev->scale_factor = (dev->params.full_res ? ADXL345_PARAM_SCALE_FACTOR : (4 << dev->params.range));

    /* Acquire exclusive access */
    i2c_acquire(ADXL345_BUS);

    /* test if the target device responds */
    i2c_read_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_CHIP_ID_REG, &reg, 0);
    if (reg != ADXL345_CHIP_ID) {
        i2c_release(ADXL345_BUS);
        DEBUG("[adxl345] init - error: invalid id value [0x%02x]\n", (int)reg);
        return ADXL345_NODEV;
    }
    /* configure the user offset */
    i2c_write_regs(ADXL345_BUS, ADXL345_ADDR, ADXL345_OFFSET_X, dev->params.offset, 3, 0);
    /* Basic device setup */
    reg = (dev->params.full_res ? ADXL345_FULL_RES : 0);
    reg |= dev->params.range;
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_DATA_FORMAT, reg, 0);
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_BW_RATE, dev->params.rate, 0);
    /* Put device in measure mode */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, ADXL345_MEASURE_BIT, 0);

    /* Release the bus */
    i2c_release(ADXL345_BUS);

    DEBUG("[adxl345] init: successful\n");

    return ADXL345_OK;
}

void adxl345_read(const adxl345_t *dev, adxl345_data_t *data)
{
    int16_t result[3];

    assert(dev && data);

    i2c_acquire(ADXL345_BUS);
    i2c_read_regs(ADXL345_BUS, ADXL345_ADDR, ADXL345_DATA_X0, (void *)result, 6, 0);
    i2c_release(ADXL345_BUS);

    /* ADXL345 returns value in little endian, so swap is needed on big endian
     * platforms. See Analog Devices ADXL345 datasheet page 27. */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    for (int i = 0; i < 3; i++) {
        result[i] = byteorder_swaps((uint16_t)result[i]);
    }
#endif

    data->x = result[0] * dev->scale_factor;
    data->y = result[1] * dev->scale_factor;
    data->z = result[2] * dev->scale_factor;
}

void adxl345_set_interrupt(const adxl345_t *dev)
{
    assert(dev);

    DEBUG("[adxl345] Update interruptions configuration\n");

    i2c_acquire(ADXL345_BUS);
    /* Set threshold */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_THRESH_TAP, dev->interrupt.thres_tap, 0);
    /* Set Map */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_INT_MAP, dev->interrupt.map, 0);
    /* Set Duration */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_TAP_DUR, dev->interrupt.thres_dur, 0);
    /* Enable axes */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_TAP_AXES, dev->interrupt.tap_axes, 0);
    /* Set source */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_INT_SOURCE, dev->interrupt.source, 0);
    /* Set latent threshold */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_TAP_LAT, dev->interrupt.thres_latent, 0);
    /* Set window threshold */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_TAP_WIN, dev->interrupt.thres_window, 0);
    /* Set activity threshold */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_THRESH_ACT, dev->interrupt.thres_act, 0);
    /* Set inactivity threshold */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_THRESH_INACT, dev->interrupt.thres_inact, 0);
    /* Set inactivity time */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_TIME_INACT, dev->interrupt.time_inact, 0);
    /* Set free-fall threshold */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_THRESH_FF, dev->interrupt.thres_ff, 0);
    /* Set free-fall time */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_TIME_FF, dev->interrupt.time_ff, 0);
    /* Set axis control */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_ACT_INACT_CTL, dev->interrupt.act_inact, 0);
    /* Enable interrupt */
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_INT_ENABLE, dev->interrupt.enable, 0);

    /* Release the bus */
    i2c_release(ADXL345_BUS);
}

void adxl345_set_measure(const adxl345_t *dev)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[adxl345] set device to measure mode\n");

    i2c_acquire(ADXL345_BUS);
    i2c_read_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, &reg, 0);
    reg |= ADXL345_MEASURE_BIT;
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, reg, 0);
    i2c_release(ADXL345_BUS);
}

void adxl345_set_standby(const adxl345_t *dev)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[adxl345] set device to standby mode\n");

    i2c_acquire(ADXL345_BUS);
    i2c_read_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, &reg, 0);
    reg &= ~ADXL345_MEASURE_BIT;
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, reg, 0);
    i2c_release(ADXL345_BUS);
}

void adxl345_set_sleep(const adxl345_t *dev)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[adxl345] set device to sleep mode\n");

    i2c_acquire(ADXL345_BUS);
    i2c_read_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, &reg, 0);
    reg |= ADXL345_SLEEP_BIT;
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, reg, 0);
    i2c_release(ADXL345_BUS);
}

void adxl345_set_autosleep(const adxl345_t *dev)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[adxl345] set device to autosleep mode\n");

    i2c_acquire(ADXL345_BUS);
    i2c_read_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, &reg, 0);
    reg |= ADXL345_AUTOSLEEP_BIT;
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_POWER_CTL, reg, 0);
    i2c_release(ADXL345_BUS);
}

void adxl345_set_bandwidth_rate(const adxl345_t *dev, uint8_t bw_rate)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[adxl345] set device rate to %d Hz\n", (int)bw_rate);

    i2c_acquire(ADXL345_BUS);
    i2c_read_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_BW_RATE, &reg, 0);
    reg |= bw_rate;
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_BW_RATE, reg, 0);
    i2c_release(ADXL345_BUS);
}

void adxl345_set_fifo_mode(const adxl345_t *dev, uint8_t mode,
                           uint8_t output, uint8_t value)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[adxl345] set fifo mode to %d, output trigger to %d and trigger "
          "value to :%d\n", (int)mode, (int)output, (int)value);

    i2c_acquire(ADXL345_BUS);
    reg = ((mode << ADXL345_FIFO_MODE_POS) | (output << ADXL345_FIFO_TRIGGER_POS) | value);
    i2c_write_reg(ADXL345_BUS, ADXL345_ADDR, ADXL345_FIFO_CTL, reg, 0);
    i2c_release(ADXL345_BUS);
}
