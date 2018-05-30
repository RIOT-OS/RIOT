/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_sens0
 * @{
 *
 * @file
 * @brief       Sens0 I2C sensor driver implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#ifdef MODULE_SENS0_I2C
#include <stdint.h>
#include <errno.h>

#include "assert.h"
#include "xtimer.h"
#include "periph/i2c.h"
#include "log.h"
#include "sens0.h"
#include "sens0/i2c.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/* Internal use default parsing function defined in sens0.c */
int sens0_parse_raw(const sens0_output_t *out_spec, phydat_t *data, uint8_t *raw);

static int sens0_i2c_write_seq(i2c_t i2c, uint8_t addr, uint8_t nelem, const sens0_reg_t *regs)
{
    DEBUG("sens0: write seq, nelem %u\n", (unsigned)nelem);
    for (unsigned k = 0; k < nelem; ++k) {
        int res = i2c_write_reg(i2c, addr, regs[k].reg, regs[k].value, 0);
        DEBUG("sens0: write i2c %u, addr 0x%02x, reg 0x%02x, value 0x%02x\n",
            (unsigned)i2c, (unsigned)addr, (unsigned)regs[k].reg, (unsigned)regs[k].value);
        if (res != I2C_ACK) {
            DEBUG("sens0: error writing i2c %u, addr 0x%02x, reg 0x%02x: %d\n",
                (unsigned)i2c, (unsigned)addr, (unsigned)regs[k].reg, res);
            return -EIO;
        }
    }
    return 0;
}

int sens0_i2c_init(const sens0_i2c_t *dev)
{
    assert(dev);
    const sens0_i2c_params_t *params = dev->params;
    assert(params);
    const sens0_specs_t *specs = dev->specs;
    assert(specs);

    int ret = 0;
    /* prepare the I2C bus */
    int res = i2c_acquire(params->i2c);
    if (res < 0) {
        LOG_ERROR("sens0: i2c_acquire error: %d\n", res);
        return -EINVAL;
    }
    do {
        uint8_t reg;
        if (specs->id.mask) {
            /* Check hardware identification */
            res = i2c_read_reg(params->i2c, params->addr, specs->id.reg, &reg, 0);
            if (res != I2C_ACK) {
                LOG_ERROR("sens0: error reading hardware ID: %d\n", res);
                ret = -EIO;
                break;
            }
            if ((reg & specs->id.mask) != specs->id.value) {
                LOG_ERROR("sens0: wrong hardware ID, expected 0x%02x, got 0x%02x\n",
                       (unsigned)specs->id.value, (unsigned)reg);
                ret = -ENODEV;
                break;
            }
        }
        /* perform hardware reset sequence */
        res = sens0_i2c_write_seq(params->i2c, params->addr, specs->reset_regs_nelem, specs->reset_regs);
        if (res < 0) {
            /* Some devices do not ACK the reset command, but instead resets immediately */
            DEBUG("sens0: error writing reset sequence (ignored)\n");
        }
        if (specs->reset_delay > 0) {
            xtimer_usleep(specs->reset_delay);
        }
        /* perform initialization sequence */
        res = sens0_i2c_write_seq(params->i2c, params->addr, specs->init_regs_nelem, specs->init_regs);
        if (res < 0) {
            LOG_ERROR("sens0: error writing init sequence\n");
            ret = -EIO;
            break;
        }
        DEBUG("sens0: init complete\n");
    } while(0);
    i2c_release(params->i2c);
    return ret;
}

static void sens0_i2c_set_power(const sens0_i2c_t *dev, uint8_t value)
{
    const sens0_i2c_params_t *params = dev->params;
    const sens0_specs_t *specs = dev->specs;
    if (specs->power_mask == 0) {
        DEBUG("sens0: set_power ignored\n");
        return;
    }
    int res = i2c_acquire(params->i2c);
    if (res < 0) {
        LOG_ERROR("sens0: i2c_acquire error: %d\n", res);
        return;
    }
    do {
        uint8_t tmp = 0;
        res = i2c_read_reg(params->i2c, params->addr, specs->power_reg, &tmp, 0);
        if (res != I2C_ACK) {
            LOG_ERROR("sens0: error reading power reg: %d\n", res);
            break;
        }
        tmp = (tmp & ~(specs->power_mask)) | value;
        res = i2c_write_reg(params->i2c, params->addr, specs->power_reg, tmp, 0);
        if (res != I2C_ACK) {
            LOG_ERROR("sens0: error writing power reg: %d\n", res);
            break;
        }
    } while(0);
    i2c_release(params->i2c);
}

void sens0_i2c_set_active(const sens0_i2c_t *dev)
{
    assert(dev);
    DEBUG("sens0: active mode\n");
    sens0_i2c_set_power(dev, dev->specs->power_value_active);
}

void sens0_i2c_set_standby(const sens0_i2c_t *dev)
{
    assert(dev);
    DEBUG("sens0: standby mode\n");
    sens0_i2c_set_power(dev, dev->specs->power_value_standby);
}

int sens0_i2c_read(const sens0_i2c_t *dev, phydat_t *data, uint8_t iout)
{
    assert(dev);
    const sens0_i2c_params_t *params = dev->params;
    const sens0_specs_t *specs = dev->specs;
    assert(iout < specs->outputs_nelem);
    const sens0_output_t *out_spec = &specs->outputs[iout];
    const uint8_t raw_size = out_spec->nelem * out_spec->elem_size;
    uint8_t buf[raw_size];

    int res = i2c_acquire(params->i2c);
    if (res < 0) {
        LOG_ERROR("sens0: i2c_acquire error: %d\n", res);
        return -EINVAL;
    }
    int ret = 0;
    do {
        if (specs->measurement_req.mask == 0) {
            /* Skip measurement request if mask is zero */
            break;
        }
        uint8_t tmp = 0;
        if (specs->measurement_req.mask < 0xffu) {
            /* Read modify write */
            res = i2c_read_reg(params->i2c, params->addr, specs->measurement_req.reg, &tmp, 0);
            if (res != I2C_ACK) {
                LOG_ERROR("sens0: error reading measurement request reg: %d\n", res);
                ret = -EIO;
                break;
            }
            tmp &= ~(specs->measurement_req.mask);
        }
        tmp |= specs->measurement_req.value;
        res = i2c_write_reg(params->i2c, params->addr, specs->measurement_req.reg, tmp, 0);
        if (res != I2C_ACK) {
            LOG_ERROR("sens0: error writing measurement request: %d\n", res);
            ret = -EIO;
            break;
        }
    } while(0);
    i2c_release(params->i2c);
    if (ret != 0) {
        return ret;
    }
    if (specs->measurement_delay > 0) {
        xtimer_usleep(specs->measurement_delay);
    }
    res = i2c_acquire(params->i2c);
    if (res < 0) {
        LOG_ERROR("sens0: i2c_acquire error: %d\n", res);
        return -EINVAL;
    }
    res = i2c_read_regs(params->i2c, params->addr, out_spec->reg, buf, raw_size, 0);
    i2c_release(params->i2c);
    if (res != I2C_ACK) {
        LOG_ERROR("sens0: error reading values (i2c=%u, addr=0x%02x, reg=0x%02x, size=%u): %d\n",
            params->i2c, params->addr, out_spec->reg, raw_size, res);
        return -EIO;
    }

    return sens0_parse_raw(out_spec, data, buf);
}

#else /* MODULE_SENS0_I2C */
typedef int dont_be_pedantic;
#endif
