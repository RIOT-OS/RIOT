/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_qma6100p
 * @{
 *
 * @file
 * @brief       Driver for the 3-axis QMA6100P accelerometer
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 *
 * @}
 */

#include <assert.h>

#include "qma6100p_params.h"
#include "qma6100p_regs.h"

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "ztimer.h"

#define ENABLE_DEBUG        1
#include "debug.h"

#define BUS                 (dev->params.i2c)
#define ADDR                (dev->params.addr)

/** Print out a message that function is not yet implemented */
#define NOT_YET_IMPLEMENTED()     DEBUG("%s not yet implemented\n", __func__)

static inline int _read_reg(i2c_t i2c, uint8_t addr, uint8_t reg_addr,
                            uint8_t *reg_value)
{
    int res = i2c_read_reg(i2c, addr, reg_addr, reg_value, 0);
    if (res == -ENXIO) {
        return QMA6100P_NODEV;
    }
    if (res == -EINVAL) {
        return QMA6100P_INVALID_ARG;
    }
    if (res < 0) {
        return QMA6100P_NOI2C;
    }
    return QMA6100P_OK;
}

static inline int _write_reg(i2c_t i2c, uint8_t addr, uint8_t reg_addr,
                             uint8_t reg_value)
{
    int res = i2c_write_reg(i2c, addr, reg_addr, reg_value, 0);
    if (res == -ENXIO) {
        return QMA6100P_NODEV;
    }
    if (res == -EINVAL) {
        return QMA6100P_INVALID_ARG;
    }
    if (res < 0) {
        return QMA6100P_NOI2C;
    }
    return QMA6100P_OK;
}

/**
 * @brief Test the QMA6100P I2C communication
 *
 * @param[in] i2c   I2C device
 * @param[in] addr  I2C address
 *
 * @return zero on success, non zero on error
 */
static int _qma6100p_init_test(i2c_t i2c, uint8_t addr)
{
    uint8_t reg;

    i2c_acquire(i2c);

    int res = _read_reg(i2c, addr, QMA6100P_REG_CHIP_ID, &reg);
    if (res < 0) {
        DEBUG("[qma6100p] init - error: read chip id test failed (reg=%02x)\n", reg);
        goto out;
    }
out:
    i2c_release(i2c);
    return res;
}

int qma6100p_init(qma6100p_t *dev, const qma6100p_params_t *params)
{
    assert(dev && params);
    assert(params->addr != QMA6100P_PARAM_I2C_ADDR_UNDEF);

    DEBUG("[qma6100p] init - i2c=%d, addr=0x%02x\n", params->i2c, params->addr);

    int res = _qma6100p_init_test(params->i2c, params->addr);
    if (res < 0) {
        return res;
    }

    dev->params = *params;

    res = qma6100p_set_mode(dev, params->mode);
    if (res < 0) {
        return res;
    }

    DEBUG("[qma6100p] init: successful\n");
    return QMA6100P_OK;
}

/* TODO: implement qma6100p_disable_set_interrupt (during interrupt feature support) */
static int qma6100p_disable_set_interrupt(const qma6100p_t *dev)
{
    (void) dev;
    NOT_YET_IMPLEMENTED();
    return 0;
}

/* TODO: test and add named macros for the ULPS register sequence (0x46, 0x4A) */
static int qma6100p_enter_ulps_mode(const qma6100p_t *dev)
{
    int res;

    res = _write_reg(BUS, ADDR, QMA6100P_REG_PM, 0x87);
    if (res < 0) {
        return res;
    }

    res = _write_reg(BUS, ADDR, 0x46, 0x0F);
    if (res < 0) {
        return res;
    }

    res = _write_reg(BUS, ADDR, 0x4A, 0x00);
    if (res < 0) {
        return res;
    }

    res = qma6100p_disable_set_interrupt(dev);
    if (res < 0) {
        DEBUG("[qma6100p] enter_ulps_mode - error failed to disable set interrupt\n");
        return res;
    }

    return res;
}

int qma6100p_set_mode(qma6100p_t *dev, qma6100p_mode_t mode)
{
    int res;
    uint8_t pm;

    assert(dev);

    i2c_acquire(BUS);

    res = _read_reg(BUS, ADDR, QMA6100P_REG_PM, &pm);
    if (res < 0) {
        DEBUG("[qma6100p] set_mode - error: failed to read PM register\n");
        goto out;
    }

    pm &= ~QMA6100P_PM_MODE_MASK;
    if (mode == QMA6100P_MODE_ULPS) {
        res = qma6100p_enter_ulps_mode(dev);
        if (res < 0) {
            DEBUG("[qma6100p] set_mode - error: failed to enter ulps mode\n");
            goto out;
        }
    }

    pm |= mode;
    res = _write_reg(BUS, ADDR, QMA6100P_REG_PM, pm);
    if (res < 0) {
        DEBUG("[qma6100p] set_mode - error: failed to write PM register\n");
        goto out;
    }

    dev->params.mode = mode;

out:
    i2c_release(BUS);
    return res;
}
