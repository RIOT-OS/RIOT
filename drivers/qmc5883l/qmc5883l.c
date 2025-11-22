/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_qmc5883l
 * @{
 *
 * @file
 * @brief       Implementation of the QMC5883L device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <limits.h>

#include "assert.h"
#include "periph/gpio.h"
#include "qmc5883l.h"
#include "qmc5883l_internal.h"

/* shortcut to the i2c device address */
#define ADDR        QMC5883L_ADDR

static int _reg_read(const qmc5883l_t *dev, uint8_t reg,
                     uint8_t *val, int acquire, int release)
{
    if (acquire) {
        i2c_acquire(dev->i2c);
    }
    int res = i2c_read_reg(dev->i2c, ADDR, reg, val, 0);
    if ((release) || (res != 0)) {
        i2c_release(dev->i2c);
    }
    return (res == 0) ? QMC5883L_OK : QMC5883L_BUSERR;
}

static int _reg_write(const qmc5883l_t *dev, uint8_t reg,
                      uint8_t val, int acquire, int release)
{
    if (acquire) {
        i2c_acquire(dev->i2c);
    }
    int res = i2c_write_reg(dev->i2c, ADDR, reg, val, 0);
    if ((release) || (res != 0)) {
        i2c_release(dev->i2c);
    }
    return (res == 0) ? QMC5883L_OK : QMC5883L_BUSERR;
}

int qmc5883l_init(qmc5883l_t *dev, const qmc5883l_params_t *params)
{
    assert(dev);
    assert(params);
    int res;

    dev->i2c = params->i2c;
    dev->pin_drdy = params->pin_drdy;
    dev->cfg = (params->odr | params->rng | params->osr | QMC5883L_CONT);

    /* lets start with a soft reset */
    res = _reg_write(dev, QMC5883L_CTRL2, QMC5883L_SOFT_RST, 1, 0);
    if (res != QMC5883L_OK) {
        return res;
    }

    /* verify the reset by reading the status register, should be all zeros */
    uint8_t tmp;
    if (i2c_read_reg(dev->i2c, ADDR, QMC5883L_STATUS, &tmp, 0) != 0) {
        i2c_release(dev->i2c);
        return QMC5883L_BUSERR;
    }
    if (tmp != 0) {
        i2c_release(dev->i2c);
        return QMC5883L_NOCFG;
    }

    /* write the actual device configuration */
    res = _reg_write(dev, QMC5883L_SETRESET, 0x01, 0, 0);
    if (res != QMC5883L_OK) {
        return res;
    }
    res = _reg_write(dev, QMC5883L_CTRL2, QMC5883L_INT_ENB, 0, 0);
    if (res != QMC5883L_OK) {
        return res;
    }
    return _reg_write(dev, QMC5883L_CTRL1, (dev->cfg | QMC5883L_CONT), 0, 1);
}

int qmc5883l_data_ready(const qmc5883l_t *dev)
{
    assert(dev);
    uint8_t status;

    int res = _reg_read(dev, QMC5883L_STATUS, &status, 1, 1);
    if (res != QMC5883L_OK) {
        return res;
    }
    return (status & QMC5883L_DRDY) ? QMC5883L_OK : QMC5883L_NODATA;
}

int qmc5883l_read(const qmc5883l_t *dev, int16_t *data_out)
{
    assert(data_out);
    int16_t tmp[3];

    int res = qmc5883l_read_raw(dev, tmp);
    if ((res == QMC5883L_OK) || (res == QMC5883L_OVERFLOW)) {
        uint16_t scale = (dev->cfg & QMC5883L_RNG_8G) ? 3 : 12;
        for (unsigned i = 0; i < 3; i++) {
            data_out[i] = tmp[i] / scale;
        }
    }

    return res;
}

int qmc5883l_read_raw(const qmc5883l_t *dev, int16_t *data_out)
{
    assert(dev);
    assert(data_out);

    int res;
    uint8_t status;
    uint8_t tmp[6];

    res = _reg_read(dev, QMC5883L_STATUS, &status, 1, 0);
    if (res != QMC5883L_OK) {
        return res;
    }
    if (!(status & QMC5883L_DRDY)) {
        res = QMC5883L_NODATA;
        goto done;
    }
    if (status & QMC5883L_OVL) {
        res = QMC5883L_OVERFLOW;
    }
    if (i2c_read_regs(dev->i2c, ADDR, QMC5883L_DOXL, tmp, 6, 0) != 0) {
        res = QMC5883L_BUSERR;
        goto done;
    }

    /* convert data to host byte order */
    data_out[0] = (int16_t)tmp[1] << 8 | tmp[0];
    data_out[1] = (int16_t)tmp[3] << 8 | tmp[2];
    data_out[2] = (int16_t)tmp[5] << 8 | tmp[4];

done:
    i2c_release(dev->i2c);
    return res;
}

int qmc5883l_poweron(const qmc5883l_t *dev)
{
    assert(dev);
    return _reg_write(dev, QMC5883L_CTRL1, dev->cfg, 1, 1);
}

int qmc5883l_poweroff(const qmc5883l_t *dev)
{
    assert(dev);
    return _reg_write(dev, QMC5883L_CTRL1, 0, 1, 1);
}

#ifdef MODULE_QMC5883L_INT
int qmc5883l_init_int(const qmc5883l_t *dev, gpio_cb_t cb, void *arg)
{
    assert(dev);
    assert(cb);

    if (!gpio_is_valid(dev->pin_drdy)) {
        return QMC5883L_NOCFG;
    }
    if (gpio_init_int(dev->pin_drdy, GPIO_IN, GPIO_RISING, cb, arg) != 0) {
        return QMC5883L_NOCFG;
    }
    return _reg_write(dev, QMC5883L_CTRL2, 0, 1, 1);
}

int qmc5883l_irq_enable(const qmc5883l_t *dev)
{
    assert(dev);
    gpio_irq_enable(dev->pin_drdy);
    return _reg_write(dev, QMC5883L_CTRL2, 0, 1, 1);
}

int qmc5883l_irq_disable(const qmc5883l_t *dev)
{
    assert(dev);
    gpio_irq_disable(dev->pin_drdy);
    return _reg_write(dev, QMC5883L_CTRL2, QMC5883L_INT_ENB, 1, 1);
}
#endif /* MODULE_QMC5883L_INT */
