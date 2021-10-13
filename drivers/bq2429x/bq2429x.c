/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bq2429x
 * @{
 *
 * @file
 * @brief       Device driver implementation for BQ2429x power ICs
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include "bq2429x.h"
#include "bq2429x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[bq2429x] %s i2c dev=%d : " f "\n", \
              __func__, d->params.i2c, ## __VA_ARGS__);

#define EXEC_RET(func) \
    do { \
        int _ret; \
        if ((_ret = (func)) != BQ2429X_OK) { \
            return _ret; \
        } \
    } while (0)

static inline int _read_reg(const bq2429x_t *dev, uint8_t reg, uint8_t *value)
{
    int ret;
    i2c_acquire(dev->params.i2c);
    if ((ret = i2c_read_reg(dev->params.i2c, BQ2429X_I2C_ADDR, reg, value,
                            0)) < 0) {
        DEBUG_DEV("couldn't read reg=%02x ret=%d", dev, (unsigned)reg, ret);
        i2c_release(dev->params.i2c);
        return BQ2429X_ERR_I2C;
    }
    i2c_release(dev->params.i2c);

    return BQ2429X_OK;
}

static inline int _write_reg(const bq2429x_t *dev, uint8_t reg, uint8_t value)
{
    int ret;
    i2c_acquire(dev->params.i2c);
    if ((ret = i2c_write_reg(dev->params.i2c, BQ2429X_I2C_ADDR, reg, value,
                             0)) < 0) {
        DEBUG_DEV("couldn't write reg=%02x: ret=%d", dev, (unsigned)reg, ret);
        i2c_release(dev->params.i2c);
        return BQ2429X_ERR_I2C;
    }
    i2c_release(dev->params.i2c);

    return BQ2429X_OK;
}

static int _update_bits(const bq2429x_t *dev, uint8_t reg, uint8_t mask,
                        uint8_t value)
{
    uint8_t tmp;

    EXEC_RET(_read_reg(dev, reg, &tmp));

    tmp &= ~mask;
    tmp |= value & mask;

    return _write_reg(dev, reg, tmp);
}

static int _update_charge_params(const bq2429x_t *dev)
{
    assert(dev);

    DEBUG_DEV("", dev);
    EXEC_RET(_update_bits(dev, BQ2429X_REG00, BQ2429X_REG00_VINDPM_m,
                          dev->params.vlim << BQ2429X_REG00_VINDPM_s));
    EXEC_RET(_update_bits(dev, BQ2429X_REG00, BQ2429X_REG00_IINLIM_m,
                          dev->params.ilim << BQ2429X_REG00_IINLIM_s));
    EXEC_RET(_update_bits(dev, BQ2429X_REG02, BQ2429X_REG02_ICHG_m,
                          dev->params.ichg << BQ2429X_REG02_ICHG_s));
    EXEC_RET(_update_bits(dev, BQ2429X_REG04, BQ2429X_REG04_VREG_m,
                          dev->params.vreg << BQ2429X_REG04_VREG_s));
    return BQ2429X_OK;
}

int bq2429x_init(bq2429x_t *dev, const bq2429x_params_t *params)
{
    assert(dev && params);

    DEBUG_DEV("", dev);
    dev->params = *params;

    if (!gpio_is_equal(dev->params.ce_pin, GPIO_UNDEF)) {
        if (gpio_init(dev->params.ce_pin, GPIO_OUT) < 0) {
            DEBUG_DEV("couldn't initialize CE pin", dev);
            return BQ2429X_ERR_GPIO;
        }
        /* enable charge by default */
        gpio_clear(dev->params.ce_pin);
    }
    if (!gpio_is_equal(dev->params.otg_pin, GPIO_UNDEF)) {
        if (gpio_init(dev->params.otg_pin, GPIO_OUT) < 0) {
            DEBUG_DEV("couldn't initialize OTG pin", dev);
            return BQ2429X_ERR_GPIO;
        }
        /* disable OTG by default */
        gpio_clear(dev->params.otg_pin);
    }

    /* update parameters on the device */
    EXEC_RET(_update_charge_params(dev));

    return BQ2429X_OK;
}

#if IS_USED(MODULE_BQ2429X_INT)
int bq2429x_init_int(bq2429x_t *dev, bq2429x_int_cb_t cb, void *arg)
{
    assert(dev && !gpio_is_equal(dev->params.int_pin, GPIO_UNDEF) && cb);
    DEBUG_DEV("cb=%p arg=%p", dev, cb, arg);

    /* the device always emits an active-low 256-us pulse */
    if (gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING, cb,
                      arg) < 0) {
        return BQ2429X_ERR_GPIO;
    }

    return BQ2429X_OK;
}
#endif

int bq2429x_get_status(const bq2429x_t *dev, bq2429x_status_t *status)
{
    assert(dev && status);

    DEBUG_DEV("", dev);

    uint8_t val;
    EXEC_RET(_read_reg(dev, BQ2429X_REG08, &val));

    status->vbus  = (val & BQ2429X_REG08_VBUS_STATUS_m)
                 >> BQ2429X_REG08_VBUS_STATUS_s;
    status->chrg  = (val & BQ2429X_REG08_CHG_STAT_m)
                 >> BQ2429X_REG08_CHG_STAT_s;
    status->dpm   = (val & BQ2429X_REG08_DPM_STAT_m)
                 >> BQ2429X_REG08_DPM_STAT_s;
    status->pg    = (val & BQ2429X_REG08_PG_STAT_m)
                 >> BQ2429X_REG08_PG_STAT_s;
    status->therm = (val & BQ2429X_REG08_THERM_STAT_m)
                 >> BQ2429X_REG08_THERM_STAT_s;
    status->vsys  = (val & BQ2429X_REG08_VSYS_STAT_m)
                 >> BQ2429X_REG08_VSYS_STAT_s;

    return BQ2429X_OK;
}

int bq2429x_get_fault(const bq2429x_t *dev, bq2429x_fault_t *fault)
{
    assert(dev && fault);

    DEBUG_DEV("", dev);

    uint8_t val;
    EXEC_RET(_read_reg(dev, BQ2429X_REG09, &val));

    fault->watchdog    = (val & BQ2429X_REG09_WD_FAULT_m)
                      >> BQ2429X_REG09_WD_FAULT_s;
    fault->otg         = (val & BQ2429X_REG09_OTG_FAULT_m)
                      >> BQ2429X_REG09_OTG_FAULT_s;
    fault->chrg        = (val & BQ2429X_REG09_CHRG_FAULT_m)
                      >> BQ2429X_REG09_CHRG_FAULT_s;
    fault->bat         = (val & BQ2429X_REG09_BAT_FAULT_m)
                      >> BQ2429X_REG09_BAT_FAULT_s;
    fault->ntc_fault_1 = (val & BQ2429X_REG09_NTC_FAULT_1_m)
                      >> BQ2429X_REG09_NTC_FAULT_1_s;
    fault->ntc_fault_0 = (val & BQ2429X_REG09_NTC_FAULT_0_m)
                      >> BQ2429X_REG09_NTC_FAULT_0_s;

    return BQ2429X_OK;
}

int bq2429x_enable_otg(const bq2429x_t *dev)
{
    assert(dev);

    DEBUG_DEV("", dev);
    if (!gpio_is_equal(dev->params.otg_pin, GPIO_UNDEF)) {
        gpio_set(dev->params.otg_pin);
    }
    uint8_t val = BQ2429X_REG01_OTG_ENABLE << BQ2429X_REG01_OTG_s;
    return _update_bits(dev, BQ2429X_REG01, BQ2429X_REG01_OTG_m, val);
}

int bq2429x_disable_otg(const bq2429x_t *dev)
{
    assert(dev);

    DEBUG_DEV("", dev);
    if (!gpio_is_equal(dev->params.otg_pin, GPIO_UNDEF)) {
        gpio_clear(dev->params.otg_pin);
    }
    uint8_t val = BQ2429X_REG01_OTG_DISABLE << BQ2429X_REG01_OTG_s;
    return _update_bits(dev, BQ2429X_REG01, BQ2429X_REG01_OTG_m, val);
}

int bq2429x_enable_charge(const bq2429x_t *dev)
{
    assert(dev);

    DEBUG_DEV("", dev);
    if (!gpio_is_equal(dev->params.ce_pin, GPIO_UNDEF)) {
        gpio_clear(dev->params.ce_pin);
    }
    uint8_t val = BQ2429X_REG01_CHG_ENABLE << BQ2429X_REG01_CHG_s;
    return _update_bits(dev, BQ2429X_REG01, BQ2429X_REG01_CHG_m, val);
}

int bq2429x_disable_charge(const bq2429x_t *dev)
{
    assert(dev);

    DEBUG_DEV("", dev);
    if (!gpio_is_equal(dev->params.ce_pin, GPIO_UNDEF)) {
        /* set CE pin to high to disable charge */
        gpio_set(dev->params.ce_pin);
    }
    uint8_t val = BQ2429X_REG01_CHG_DISABLE << BQ2429X_REG01_CHG_s;
    return _update_bits(dev, BQ2429X_REG01, BQ2429X_REG01_CHG_m, val);
}

int bq2429x_set_vlim(bq2429x_t *dev, bq2429x_input_voltage_limit_t vlim)
{
    assert(dev);

    DEBUG_DEV("vlim=%u", dev, (unsigned)vlim);
    EXEC_RET(_update_bits(dev, BQ2429X_REG00, BQ2429X_REG00_VINDPM_m,
                          vlim << BQ2429X_REG00_VINDPM_s));
    dev->params.vlim = vlim;

    return BQ2429X_OK;
}

int bq2429x_get_vlim(const bq2429x_t *dev, bq2429x_input_voltage_limit_t *vlim)
{
    assert(dev && vlim);

    DEBUG_DEV("", dev);

    uint8_t val;
    EXEC_RET(_read_reg(dev, BQ2429X_REG00, &val));
    *vlim = (val & BQ2429X_REG00_VINDPM_m) >> BQ2429X_REG00_VINDPM_s;

    return BQ2429X_OK;
}

int bq2429x_set_ilim(bq2429x_t *dev, bq2429x_input_current_limit_t ilim)
{
    assert(dev);

    DEBUG_DEV("ilim=%u", dev, (unsigned)ilim);
    EXEC_RET(_update_bits(dev, BQ2429X_REG00, BQ2429X_REG00_IINLIM_m,
                          ilim << BQ2429X_REG00_IINLIM_s));
    dev->params.ilim = ilim;
    return BQ2429X_OK;
}

int bq2429x_get_ilim(const bq2429x_t *dev, bq2429x_input_current_limit_t *ilim)
{
    assert(dev && ilim);

    DEBUG_DEV("", dev);

    uint8_t val;
    EXEC_RET(_read_reg(dev, BQ2429X_REG00, &val));
    *ilim = (val & BQ2429X_REG00_IINLIM_m) >> BQ2429X_REG00_IINLIM_s;

    return BQ2429X_OK;
}

int bq2429x_set_ichg(bq2429x_t *dev, bq2429x_charge_current_t ichg)
{
    assert(dev);

    DEBUG_DEV("ichg=%u", dev, (unsigned)ichg);
    EXEC_RET(_update_bits(dev, BQ2429X_REG02, BQ2429X_REG02_ICHG_m,
                          ichg << BQ2429X_REG02_ICHG_s));
    dev->params.ichg = ichg;
    return BQ2429X_OK;
}

int bq2429x_get_ichg(const bq2429x_t *dev, bq2429x_charge_current_t *ichg)
{
    assert(dev && ichg);

    DEBUG_DEV("", dev);

    uint8_t val;
    EXEC_RET(_read_reg(dev, BQ2429X_REG02, &val));
    *ichg = (val & BQ2429X_REG02_ICHG_m) >> BQ2429X_REG02_ICHG_s;

    return BQ2429X_OK;
}

int bq2429x_set_vreg(bq2429x_t *dev, bq2429x_charge_voltage_limit_t vreg)
{
    assert(dev);

    DEBUG_DEV("vreg=%u", dev, (unsigned)vreg);
    EXEC_RET(_update_bits(dev, BQ2429X_REG04, BQ2429X_REG04_VREG_m,
                          vreg << BQ2429X_REG04_VREG_s));
    dev->params.vreg = vreg;
    return BQ2429X_OK;
}

int bq2429x_get_vreg(const bq2429x_t *dev, bq2429x_charge_voltage_limit_t *vreg)
{
    assert(dev && vreg);

    DEBUG_DEV("", dev);

    uint8_t val;
    EXEC_RET(_read_reg(dev, BQ2429X_REG04, &val));
    *vreg = (val & BQ2429X_REG04_VREG_m) >> BQ2429X_REG04_VREG_s;

    return BQ2429X_OK;
}
