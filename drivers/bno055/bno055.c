/*
 * Copyright (C) 2019 Wouter Horlings
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bno055
 * @{
 *
 * @file
 * @brief       Device driver interface for the BNO055 9-axis sensor
 *
 * @author      Wouter Horlings <wouter@horlin.gs>
 * @}
 */


#include "bno055.h"
#include "bno055_internal.h"
#include "bno055_params.h"
#include "periph/i2c.h"
#define ENABLE_DEBUG        (0)
#include "debug.h"

static int read_u8_regs(const bno055_t *dev, uint8_t reg, void *data,
                        size_t len);
static int write_u8_reg(const bno055_t *dev, uint8_t reg, uint8_t data);

int bno055_init(bno055_t *dev, const bno055_params_t *params)
{
    uint8_t id[4];

    dev->p = params;
    /* set the i2c register page to 0 */
    bno055_set_page(dev, 0x0);

    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    /* Check if the device has the correct chips on boad */
    int ret = read_u8_regs(dev, BNO055_REG_CHIPID, &id, 4);
    if (ret) {
        i2c_release(dev->p->i2c);
        return BNO055_NORW;
    }
    if (id[0] != BNO055_VAL_MCU_WHO_AM_I ||
        id[1] != BNO055_VAL_ACC_WHO_AM_I ||
        id[2] != BNO055_VAL_MAG_WHO_AM_I ||
        id[3] != BNO055_VAL_GYR_WHO_AM_I) {
        DEBUG("[bno055] error: no connection to sensor\n");
        return BNO055_NODEV;
    }

    /* set sensor operation mode */
    ret = write_u8_reg(dev, BNO055_REG_OPR_MOD, dev->p->opr);
    if (ret) {
        i2c_release(dev->p->i2c);
        return BNO055_NORW;
    }
    ret = write_u8_reg(dev, BNO055_REG_PWR_MOD, dev->p->pwr);
    if (ret) {
        i2c_release(dev->p->i2c);
        return BNO055_NORW;
    }
    ret = write_u8_reg(dev, BNO055_REG_UNIT_SEL, dev->p->unit_sel);
    if (ret) {
        i2c_release(dev->p->i2c);
        return BNO055_NORW;
    }
    i2c_release(dev->p->i2c);
    return BNO055_OK;

}

int bno055_set_page(const bno055_t *dev, uint8_t page)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = write_u8_reg(dev, BNO055_REG_PAGEID, page);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_set_mode(const bno055_t *dev, bno055_opr_mode_t mode)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = write_u8_reg(dev, BNO055_REG_OPR_MOD, mode);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_quat_read(const bno055_t *dev, bno055_4bit16_t *data)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = read_u8_regs(dev, BNO055_REG_QUA_DAT, data->u8bit, 8);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_eul_read(const bno055_t *dev, bno055_3bit16_t *data)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = read_u8_regs(dev, BNO055_REG_EUL_DAT, data, 6);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_lia_read(const bno055_t *dev, bno055_3bit16_t *data)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = read_u8_regs(dev, BNO055_REG_LIA_DAT, data, 6);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_grv_read(const bno055_t *dev, bno055_3bit16_t *data)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = read_u8_regs(dev, BNO055_REG_GRV_DAT, data, 6);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_acc_read(const bno055_t *dev, bno055_3bit16_t *data)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = read_u8_regs(dev, BNO055_REG_ACC_DAT, data, 6);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_mag_read(const bno055_t *dev, bno055_3bit16_t *data)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = read_u8_regs(dev, BNO055_REG_MAG_DAT, data, 6);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_gyr_read(const bno055_t *dev, bno055_3bit16_t *data)
{
    if (i2c_acquire(dev->p->i2c)) {
        return BNO055_NOBUS;
    }
    int ret = read_u8_regs(dev, BNO055_REG_GYR_DAT, data, 6);
    i2c_release(dev->p->i2c);
    if (ret) {
        return BNO055_NORW;
    }
    return BNO055_OK;
}

int bno055_quat_float(const bno055_t *dev, quat_t *data)
{
    bno055_4bit16_t rawdata;
    int ret = bno055_quat_read(dev, &rawdata);

    if (ret) {
        return ret;
    }
    data->w = rawdata.i16_bit[0] / BNO055_DIV_SCALE_QUAT_UN;
    data->x = rawdata.i16_bit[1] / BNO055_DIV_SCALE_QUAT_UN;
    data->y = rawdata.i16_bit[2] / BNO055_DIV_SCALE_QUAT_UN;
    data->z = rawdata.i16_bit[3] / BNO055_DIV_SCALE_QUAT_UN;
    return BNO055_OK;
}

int bno055_eul_float(const bno055_t *dev, eul_t *data)
{
    bno055_3bit16_t rawdata;
    int ret = bno055_eul_read(dev, &rawdata);

    if (ret) {
        return ret;
    }
    data->heading = rawdata.i16_bit[0] / BNO055_DIV_SCALE_EUL_RAD;
    data->roll = rawdata.i16_bit[1] / BNO055_DIV_SCALE_EUL_RAD;
    data->pitch = rawdata.i16_bit[2] / BNO055_DIV_SCALE_EUL_RAD;
    return BNO055_OK;
}

int bno055_grv_float(const bno055_t *dev, euclid_t *data)
{
    bno055_3bit16_t rawdata;
    int ret = bno055_grv_read(dev, &rawdata);

    if (ret) {
        return ret;
    }
    data->x = rawdata.i16_bit[0] / BNO055_DIV_SCALE_ACC_MSS;
    data->y = rawdata.i16_bit[1] / BNO055_DIV_SCALE_ACC_MSS;
    data->z = rawdata.i16_bit[2] / BNO055_DIV_SCALE_ACC_MSS;
    return BNO055_OK;
}

int bno055_acc_float(const bno055_t *dev, euclid_t *data)
{
    bno055_3bit16_t rawdata;
    int ret = bno055_acc_read(dev, &rawdata);

    if (ret) {
        return ret;
    }
    data->x = rawdata.i16_bit[0] / BNO055_DIV_SCALE_ACC_MSS;
    data->y = rawdata.i16_bit[1] / BNO055_DIV_SCALE_ACC_MSS;
    data->z = rawdata.i16_bit[2] / BNO055_DIV_SCALE_ACC_MSS;
    return BNO055_OK;
}

int bno055_mag_float(const bno055_t *dev, euclid_t *data)
{
    bno055_3bit16_t rawdata;
    int ret = bno055_mag_read(dev, &rawdata);

    if (ret) {
        return ret;
    }
    data->x = rawdata.i16_bit[0] / BNO055_DIV_SCALE_MAG;
    data->y = rawdata.i16_bit[1] / BNO055_DIV_SCALE_MAG;
    data->z = rawdata.i16_bit[2] / BNO055_DIV_SCALE_MAG;
    return BNO055_OK;
}

int bno055_gyr_float(const bno055_t *dev, euclid_t *data)
{
    bno055_3bit16_t rawdata;
    int ret = bno055_gyr_read(dev, &rawdata);

    if (ret) {
        return ret;
    }
    data->x = rawdata.i16_bit[0] / BNO055_DIV_SCALE_GYR_RPS;
    data->y = rawdata.i16_bit[1] / BNO055_DIV_SCALE_GYR_RPS;
    data->z = rawdata.i16_bit[2] / BNO055_DIV_SCALE_GYR_RPS;
    return BNO055_OK;
}

int bno055_lia_float(const bno055_t *dev, euclid_t *data)
{
    bno055_3bit16_t rawdata;
    int ret = bno055_lia_read(dev, &rawdata);

    if (ret) {
        return ret;
    }
    data->x = rawdata.i16_bit[0] / BNO055_DIV_SCALE_ACC_MSS;
    data->y = rawdata.i16_bit[1] / BNO055_DIV_SCALE_ACC_MSS;
    data->z = rawdata.i16_bit[2] / BNO055_DIV_SCALE_ACC_MSS;
    return BNO055_OK;
}

static int read_u8_regs(const bno055_t *dev, uint8_t reg, void *data,
                        size_t len)
{
    return i2c_read_regs(dev->p->i2c, dev->p->addr, reg, data, len, 0);
}

static int write_u8_reg(const bno055_t *dev, uint8_t reg, uint8_t data)
{
    return i2c_write_reg(dev->p->i2c, dev->p->addr, reg, data, 0);
}
