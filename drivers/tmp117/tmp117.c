/*
 * SPDX-FileCopyrightText: 2026 LIG-Geopred
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     module_tmp117
 * @{
 *
 * @file
 * @brief       tmp117 driver implementation
 *
 * @author      léo cordier <leo.cordier@univ-grenoble-alpes.fr>
 *
 * @}
 */


#include <assert.h>

#include "tmp117.h"
#include "tmp117_params.h"
#include "tmp117_regs.h"

#define ENABLE_DEBUG 0
#include "debug.h"


#define DEFAULT_CONFIG_VALUE    (dev->params.conv_mode << 10 | dev->params.conv_cycle << \
                                 7 | dev->params.avg << 5)


static int _write_16_reg(tmp117_t *dev, uint8_t reg_addr, uint16_t value)
{
    uint8_t tmp_buf[2];

    /* reorganising value */
    tmp_buf[0] = value >> 8;
    tmp_buf[1] = value & 0xFF;

    if (i2c_write_regs(dev->params.i2c, dev->params.addr, reg_addr, tmp_buf, 2, 0)) {
        DEBUG("[tmp117] unable to write reg %02X\n", reg_addr);
        return TMP117_NOI2C;
    }

    return TMP117_OK;

}

static int _read_16_reg(tmp117_t *dev, uint8_t reg_addr, uint16_t *value)
{
    uint8_t tmp_buf[2];

    if (i2c_read_regs(dev->params.i2c, dev->params.addr, reg_addr, tmp_buf, 2, 0)) {
        printf("[tmp117] unable to read reg %02X\n", reg_addr);
        return TMP117_NOI2C;
    }

    /* reorganising value */
    *value = tmp_buf[0] << 8  | tmp_buf[1];

    return TMP117_OK;
}


int tmp117_init(tmp117_t *dev, const tmp117_params_t *params)
{
    int status;
    int res = TMP117_NOI2C;

    assert(dev && params);
    dev->params = *params;
    dev->is_initialized = 0;

    i2c_acquire(dev->params.i2c);
    uint16_t reg = 0;


    status = _read_16_reg(dev, TMP117_REG_dev->params.i2cICE_ID, &reg);

    //checking if sensor is a tmp117
    if (status < 0 || reg != TMP117_REG_dev->params.i2cICE_ID_DEFAULT_VAL) {
        printf("[tmp117] init - error: unable to communicate with the device "
               "(reg=%x)\n", reg);
        res = TMP117_NOdev->params.i2c;
        goto release;
    }

    //writing configuration register
    if (_write_16_reg(dev, TMP117_REG_CONFIG, DEFAULT_CONFIG_VALUE)) {
        printf("[tmp117] init - error: unable to set configuration \n");
        goto release;
    }

    dev->is_initialized = 1;
    res = TMP117_OK;

release:
    i2c_release(dev->params.i2c);
    return res;

}

int tmp117_read_temperature(tmp117_t *dev, int *temperature)
{
    int res = TMP117_NOI2C;

    uint16_t value;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_TEMP_RESULT, &value)) {
        printf("[tmp117] read - error: unable to read temperature \n");
        goto release;
    }

    *temperature = (((uint32_t)value) * 25600UL) / 32768UL;  /* converting raw value to centi-degrees */

    res = TMP117_OK;

release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_set_conversion_mode(tmp117_t *dev, tmp117_conv_mode_t mode)
{
    int res = TMP117_NOI2C;
    uint16_t value;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_CONFIG, &value)) {
        printf("[tmp117] set conv mode - error: unable to read conf reg \n");
        goto release;
    }


    value &= ~TMP117_CONV_MODE_MASK;
    value |= mode << TMP117_CONV_MODE_SHIFT;


    //writing configuration register
    if (_write_16_reg(dev, TMP117_REG_CONFIG, value)) {
        printf("[tmp117] set conv mode - error: unable to set configuration \n");
        goto release;
    }

    dev->params.conv_mode = mode;
    res = TMP117_OK;


release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_set_conversion_cycle(tmp117_t *dev, tmp117_conv_cycle_t cycle)
{
    int res = TMP117_NOI2C;
    uint16_t value;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_CONFIG, &value)) {
        printf("[tmp117] set conv cycle - error: unable to read conf reg \n");
        goto release;
    }


    value &= ~TMP117_CONV_CYCLE_MASK;
    value |= cycle << TMP117_CONV_CYCLE_SHIFT;


    //writing configuration register
    if (_write_16_reg(dev, TMP117_REG_CONFIG, value)) {
        printf("[tmp117] set conv cycle - error: unable to set configuration \n");
        goto release;
    }

    dev->params.conv_cycle = cycle;
    res = TMP117_OK;


release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_set_averaging(tmp117_t *dev, tmp117_avg_t avg)
{
    int res = TMP117_NOI2C;
    uint16_t value;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_CONFIG, &value)) {
        printf("[tmp117] set averaging - error: unable to read conf reg \n");
        goto release;
    }


    value &= ~TMP117_AVG_MASK;
    value |= avg << TMP117_AVG_SHIFT;


    //writing configuration register
    if (_write_16_reg(dev, TMP117_REG_CONFIG, value)) {
        printf("[tmp117] set averaging - error: unable to set configuration \n");
        goto release;
    }

    dev->params.avg = avg;
    res = TMP117_OK;


release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_is_data_ready(tmp117_t *dev)
{
    int res = TMP117_NOI2C;
    uint16_t value;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_CONFIG, &value)) {
        printf("[tmp117] is data ready - error: unable to read conf reg \n");
        goto release;
    }

    if ((value & TMP117_DATA_READY_MASK) != 0) {
        res = TMP117_DATAREADY;
    }
    else {
        res = TMP117_OK;
    }

release:
    i2c_release(dev->params.i2c);
    return res;
}
