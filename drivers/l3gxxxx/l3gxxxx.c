/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_l3gxxxx
 * @brief       Device Driver for ST L3Gxxxx 3-axis gyroscope family
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <string.h>
#include <stdlib.h>

#include "l3gxxxx_regs.h"
#include "l3gxxxx.h"

#include "byteorder.h"
#include "log.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[l3gxxxx] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#define DEBUG_DEV(m, d, ...) \
        DEBUG("[l3gxxxx] %s dev=%" PRIxPTR ": " m "\n", \
              __func__, (unsigned int)d, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond);
#define DEBUG_DEV(m, d, ...)

#endif /* ENABLE_DEBUG */

/* set a single bit defined by mask `m` in 8-bit register value `r` to value `v` */
#define _SET_REG_BIT(r, m, v) r = ((v) & ~m) ? r | m : r & ~m
/* get the value of a single bit defined by mask `m` from 8-bit register value `r` */
#define _GET_REG_BIT(r, m) (r & m) ? 1 : 0

/* set the bits defined by mask `m` in 8-bit register value `r` to value `v` */
#define _SET_REG_VALUE(r, m, v) r = (r & ~m) | (((v) << m ## _S) & m)
/* get the value the bits defined by mask `m` from 8-bit register value `r` */
#define _GET_REG_VALUE(r, m) ((r & ~m) >> m ## _S)

#if IS_USED(MODULE_L3GXXXX_SPI)
#define _SPI_DEV    (dev->params.if_params.spi.dev)
#define _SPI_CS     (dev->params.if_params.spi.cs)
#define _SPI_CLK    (dev->params.if_params.spi.clk)
#define _IS_DEV_SPI (dev->params.if_params.type == L3GXXXX_SPI)
#endif

#if IS_USED(MODULE_L3GXXXX_I2C)
#define _I2C_DEV    (dev->params.if_params.i2c.dev)
#define _I2C_ADDR   (dev->params.if_params.i2c.addr)
#define _IS_DEV_I2C (dev->params.if_params.type == L3GXXXX_I2C)
#endif

#define L3GXXXX_INT_FIFO (L3GXXXX_INT_FIFO_WATERMARK | \
                          L3GXXXX_INT_FIFO_OVERRUN)

int l3gxxxx_reg_update(const l3gxxxx_t *dev,
                       uint8_t reg, uint8_t mask, uint8_t val);

/** Forward declaration of functions for internal use */

static int _is_available(l3gxxxx_t *dev);
static void _acquire(const l3gxxxx_t *dev);
static void _release(const l3gxxxx_t *dev);
static int _read(const l3gxxxx_t *dev, uint8_t reg, uint8_t *data, uint8_t len);
static int _write(const l3gxxxx_t *dev, uint8_t reg, const uint8_t *data, uint8_t len);
static int _update(const l3gxxxx_t *dev, uint8_t reg, uint8_t mask, uint8_t val);

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT)
static int _get_int_event_src(const l3gxxxx_t *dev, l3gxxxx_int_event_src_t *src);
#endif
#if IS_USED(MODULE_L3GXXXX_IRQ_DATA)
static int _get_int_data_src(const l3gxxxx_t *dev, l3gxxxx_int_data_src_t *src);
#endif

#define msb_lsb_to_type(t, b, o) (t)(((t)b[o] << 8) | b[o + 1])

int l3gxxxx_init(l3gxxxx_t *dev, const l3gxxxx_params_t *params)
{
    int res = L3GXXXX_OK;

    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);
    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->params = *params;
    mutex_init(&dev->int_lock);

#if IS_USED(MODULE_L3GXXXX_SPI)
    ASSERT_PARAM(gpio_is_valid(_SPI_CS));
    /* for SPI, we only need to initialize the chip select pin */
    if (spi_init_cs(_SPI_DEV, _SPI_CS) != SPI_OK) {
        return -L3GXXXX_ERROR_SPI;
    }
#endif /* IS_USED(MODULE_L3GXXXX_SPI) */

    _acquire(dev);

    /* check availability of the sensor */
    if ((res = _is_available(dev)) != L3GXXXX_OK) {
        _release(dev);
        return res;
    }

#if IS_USED(MODULE_A3G4250D)
    ASSERT_PARAM((dev->params.scale == L3GXXXX_SCALE_245_DPS) ||
                 (dev->sensor != X3G42XXD));
#endif

    uint8_t reg[8] = { };

    res |= _write(dev, L3GXXXX_REG_CTRL1, reg, 6);
    res |= _write(dev, L3GXXXX_REG_FIFO_CTRL, reg, 1);
    res |= _write(dev, L3GXXXX_REG_IG_CFG, reg, 1);
    res |= _write(dev, L3GXXXX_REG_IG_THS_XH, reg, IS_USED(MODULE_L3GD20H) ? 8 : 7);

    reg[0] = (dev->params.odr << L3GXXXX_ODR_BW_S) | L3GXXXX_POWER_MODE
                                                   | L3GXXXX_XYZ_ENABLED;
    reg[1] = (dev->params.hpf_mode << L3GXXXX_HPF_MODE_S) |
             (dev->params.hpf_cutoff & L3GXXXX_HPF_CUTOFF);
    reg[3] = (dev->params.scale << L3GXXXX_FULL_SCALE_S) | L3GXXXX_BLOCK_DATA_UPDATE;
    reg[4] = (dev->params.filter_sel & L3GXXXX_OUT_SEL);

    /* HPF has to be enabled by a separate bit if filter mode is 1 or 3 */
    reg[4] |= (dev->params.filter_sel & 1) ? L3GXXXX_HP_ENABLED : 0;

    /* initialize sensor completely including setting in power down mode */
    res |= _write(dev, L3GXXXX_REG_CTRL1, reg, 6);

#if IS_USED(MODULE_L3GD20H) && IS_USED(MODULE_L3GXXXX_LOW_ODR)
    /* in case of low ODR, low data rate flag has to be set */
    if (dev->params.odr >= L3GXXXX_ODR_12) {
        ASSERT_PARAM(dev->sensor == L3GD20H);
        res |= _update(dev, L3GXXXX_REG_LOW_ODR, L3GXXXX_LOW_ODR, 1);
    }
#endif

#if IS_USED(MODULE_L3GXXXX_FIFO)
    uint8_t fifo_ctrl = (dev->params.fifo_mode << L3GXXXX_FIFO_MODE_S)
                      | (dev->params.fifo_watermark & L3GXXXX_FIFO_WATERMARK);
    res |= _write(dev, L3GXXXX_REG_FIFO_CTRL, &fifo_ctrl, 1);

    reg[4] |= L3GXXXX_FIFO_EN;
#endif /* IS_USED(MODULE_L3GXXXX_FIFO) */

    res |= _write(dev, L3GXXXX_REG_CTRL1, reg, 5);
    _release(dev);

    if (res != L3GXXXX_OK) {
        return res;
    }

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA)
    /* enable FIFO interrupts if FIFO is used, DRDY interrupt otherwise */
    if ((res = l3gxxxx_enable_int(dev, IS_USED(MODULE_L3GXXXX_FIFO)
                                             ? L3GXXXX_INT_FIFO
                                             : L3GXXXX_INT_DATA_READY, true))) {
        return res;
    }
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_DATA) */

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT)
    if ((res = l3gxxxx_set_int_event_cfg(dev, &dev->params.int1_cfg)) ||
        (res = l3gxxxx_enable_int(dev, L3GXXXX_INT_EVENT, true), res)) {
        return res;
    }
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_EVENT) */

#if IS_USED(MODULE_L3GXXXX_I2C)
    if (_IS_DEV_I2C) {
        DEBUG_DEV("I2C device initialized: bus %d, addr=%02x",
                  dev, _I2C_DEV, _I2C_ADDR);
    }
#endif /* IS_USED(MODULE_L3GXXXX_I2C) */

#if IS_USED(MODULE_L3GXXXX_SPI)
    if (_IS_DEV_SPI) {
        DEBUG_DEV("SPI device initialized: bus %d, cs=%u",
                  dev, _SPI_DEV, (unsigned)_SPI_CS);
    }
#endif /* IS_USED(MODULE_L3GXXXX_SPI) */

    return res;
}

int l3gxxxx_data_ready(const l3gxxxx_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    int res = L3GXXXX_OK;

    uint8_t reg;

#if IS_USED(MODULE_L3GXXXX_FIFO)
    if (dev->params.fifo_mode == L3GXXXX_BYPASS) {
        res = l3gxxxx_reg_read(dev, L3GXXXX_REG_STATUS, &reg, 1);
        return res ? res : _GET_REG_BIT(reg, L3GXXXX_ANY_DATA_READY);
    }
    else {
        res = l3gxxxx_reg_read(dev, L3GXXXX_REG_FIFO_SRC, &reg, 1);
        return res ? res : _GET_REG_VALUE(reg, L3GXXXX_FIFO_FFS);
    }
#else
    res = l3gxxxx_reg_read(dev, L3GXXXX_REG_STATUS, &reg, 1);
    return res ? res : _GET_REG_BIT(reg, L3GXXXX_ANY_DATA_READY);
#endif
}

/*
 * scale factors for conversion of raw sensor data to degree for possible
 * sensitivities according to mechanical characteristics in datasheet
 * multiplied by 4,000
 */
static const uint16_t L3GXXXX_SCALES[3] = { 35, 70, 280 };

int l3gxxxx_read(const l3gxxxx_t *dev, l3gxxxx_data_t *data)

{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("data=%p", dev, data);

    int res = L3GXXXX_OK;

    l3gxxxx_raw_data_t raw;

    if ((res = l3gxxxx_read_raw(dev, &raw)) != L3GXXXX_OK) {
        return res;
    }

    data->x = (raw.x * L3GXXXX_SCALES[dev->params.scale]) >> 2;
    data->y = (raw.y * L3GXXXX_SCALES[dev->params.scale]) >> 2;
    data->z = (raw.z * L3GXXXX_SCALES[dev->params.scale]) >> 2;

    return res;
}

int l3gxxxx_read_raw(const l3gxxxx_t *dev, l3gxxxx_raw_data_t *raw)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL);
    DEBUG_DEV("raw=%p", dev, raw);

    int res = L3GXXXX_OK;

#if IS_USED(MODULE_L3GXXXX_FIFO)
    /* if not in bypass mode we read out the FIFO and return newest data sample */
    if (dev->params.fifo_mode != L3GXXXX_BYPASS) {
        l3gxxxx_raw_data_fifo_t raw_fifo = {};
        int num;

        /* read raw data samples from FIFO */
        if ((num = l3gxxxx_read_raw_fifo(dev, raw_fifo)) < 0) {
            DEBUG_DEV("reading raw data samples from FIFO failed", dev);
            return num;
        }

        if (num == 0) {
            DEBUG_DEV("no raw data in FIFO", dev);
            return -L3GXXXX_ERROR_NO_NEW_DATA;
        }

        *raw = raw_fifo[num - 1];

        return res;
    }
#endif /* IS_USED(MODULE_L3GXXXX_FIFO) */

    uint8_t data[6];

    /* read raw data sample and clear interrupt signal INT2/DRDY if used */
    if (l3gxxxx_reg_read(dev, L3GXXXX_REG_OUT_X_L, data, 6)) {
        DEBUG_DEV("could not get raw data", dev);
        return -L3GXXXX_ERROR_RAW_DATA;
    }

    /* L3GXXXX_REG_CTRL4.BLE = 0, Data LSB @ lower address */
    raw->x = byteorder_lebuftohs(&data[0]);
    raw->y = byteorder_lebuftohs(&data[2]);
    raw->z = byteorder_lebuftohs(&data[4]);

    return res;
}

#if IS_USED(MODULE_L3GXXXX_FIFO)

int l3gxxxx_read_fifo(const l3gxxxx_t *dev, l3gxxxx_data_fifo_t data)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("", dev);

    l3gxxxx_raw_data_fifo_t raw = {};
    int num;

    if ((num = l3gxxxx_read_raw_fifo(dev, raw)) < 0) {
        DEBUG_DEV("reading raw data samples from FIFO failed", dev);
        return num;
    }

    for (int i = 0; i < num; i++) {
        data[i].x = (raw[i].x * L3GXXXX_SCALES[dev->params.scale]) >> 2;
        data[i].y = (raw[i].y * L3GXXXX_SCALES[dev->params.scale]) >> 2;
        data[i].z = (raw[i].z * L3GXXXX_SCALES[dev->params.scale]) >> 2;
    }

    return num;
}

int l3gxxxx_read_raw_fifo(const l3gxxxx_t *dev, l3gxxxx_raw_data_fifo_t raw)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL);
    DEBUG_DEV("", dev);

    int res = L3GXXXX_OK;

    /* in bypass mode, we use l3gxxxx_read_raw to return one sample */
    if (dev->params.fifo_mode == L3GXXXX_BYPASS) {
        res = l3gxxxx_read_raw(dev, raw);
        return (res == L3GXXXX_OK) ? 1 : res;
    }

    uint8_t reg;

    _acquire(dev);

    /* read FIFO state */
    if (_read(dev, L3GXXXX_REG_FIFO_SRC, &reg, 1) != L3GXXXX_OK) {
        _release(dev);
        return res;
    }

    /* if nothing is in the FIFO, just return with 0 */
    if (reg & L3GXXXX_FIFO_EMPTY) {
        _release(dev);
        return 0;
    }

    /* read samples from FIFO */
    int samples = (reg & L3GXXXX_FIFO_FFS) + ((reg & L3GXXXX_FIFO_OVR) ? 1 : 0);

    /* read samples from FIFO */
    for (int i = 0; i < samples; i++) {

        uint8_t data[6];

        /* read raw data sample */
        res |= _read(dev, L3GXXXX_REG_OUT_X_L, data, 6);

        /* L3GXXXX_REG_CTRL4.BLE = 0, Data LSB @ lower address */
        raw[i].x = byteorder_lebuftohs(&data[0]);
        raw[i].y = byteorder_lebuftohs(&data[2]);
        raw[i].z = byteorder_lebuftohs(&data[4]);
    }

    res |= _read(dev, L3GXXXX_REG_FIFO_SRC, &reg, 1);

    if (reg & L3GXXXX_FIFO_FFS) {
        DEBUG_DEV("New samples stored in FIFO while reading out the FIFO, "
                  "output data rate (ODR) is too high", dev);
    }

    if (dev->params.fifo_mode == L3GXXXX_FIFO && samples == 32) {
        /* clean FIFO (see app note) */
        res |= _update(dev, L3GXXXX_REG_FIFO_CTRL, L3GXXXX_FIFO_MODE, L3GXXXX_BYPASS);
        res |= _update(dev, L3GXXXX_REG_FIFO_CTRL, L3GXXXX_FIFO_MODE, L3GXXXX_FIFO);
    }

    _release(dev);

    return (res == L3GXXXX_OK) ? samples : res;
}
#endif /* IS_USED(MODULE_L3GXXXX_FIFO) */

int l3gxxxx_power_down(l3gxxxx_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    return l3gxxxx_reg_update(dev, L3GXXXX_REG_CTRL1, L3GXXXX_POWER_MODE, 0);
}

int l3gxxxx_power_up(l3gxxxx_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    int res = l3gxxxx_reg_update(dev, L3GXXXX_REG_CTRL1, L3GXXXX_POWER_MODE, 1);

    return res;
}

#if IS_USED(MODULE_L3GXXXX_SLEEP)

int l3gxxxx_sleep(l3gxxxx_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    return l3gxxxx_reg_update(dev, L3GXXXX_REG_CTRL1, L3GXXXX_XYZ_ENABLED, 0);
}

int l3gxxxx_wake_up(l3gxxxx_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    int res = l3gxxxx_reg_update(dev, L3GXXXX_REG_CTRL1, L3GXXXX_XYZ_ENABLED,
                                                  L3GXXXX_XYZ_ENABLED);
    return res;
}
#endif /* IS_USED(MODULE_L3GXXXX_SLEEP) */

#if IS_USED(MODULE_L3GXXXX_IRQ)

int l3gxxxx_enable_int(const l3gxxxx_t *dev,
                       l3gxxxx_int_types_t mask, bool enable)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("mask=%02x enable=%d", dev, mask, enable);

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT)
    if (enable && (mask & L3GXXXX_INT_EVENT)) {
        ASSERT_PARAM(gpio_is_valid(dev->params.int1_pin));
    }
#endif

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA)
    if (enable && (mask & L3GXXXX_INT_DATA)) {
        ASSERT_PARAM(gpio_is_valid(dev->params.int2_pin));
    }
#endif

    return l3gxxxx_reg_update(dev, L3GXXXX_REG_CTRL3, mask, enable ? 0xff : 0x00);
}

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT)
static void _cb_int1(void *arg)
{
    l3gxxxx_t *dev = (l3gxxxx_t *)arg;
    dev->int_type |= L3GXXXX_INT_EVENT;
    mutex_unlock(&dev->int_lock);
}
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_EVENT) */

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA)
static void _cb_int2(void *arg)
{
    l3gxxxx_t *dev = (l3gxxxx_t *)arg;
    dev->int_type |= L3GXXXX_INT_DATA;
    mutex_unlock(&dev->int_lock);
}
#endif /* IS_USED(L3GXXXX_IRQ_DRDY) */

l3gxxxx_int_src_t l3gxxxx_wait_int(l3gxxxx_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    dev->int_type = 0;

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT)
    ASSERT_PARAM(gpio_is_valid(dev->params.int1_pin));
    /* init INT1 signal pin and enable the interrupt */
    gpio_init_int(dev->params.int1_pin, GPIO_IN, GPIO_RISING, _cb_int1, dev);
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_EVENT) */

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA)
    ASSERT_PARAM(gpio_is_valid(dev->params.int2_pin));
    /* init INT2/DRDY signal pin and enable the interrupt */
    gpio_init_int(dev->params.int2_pin, GPIO_IN, GPIO_RISING, _cb_int2, dev);
#endif /* IS_USED(L3GXXXX_IRQ_DRDY) */

    /* wait for an interrupt */
    mutex_lock(&dev->int_lock);

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT)
    gpio_irq_disable(dev->params.int1_pin);
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_EVENT) */

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA)
    gpio_irq_disable(dev->params.int2_pin);
#endif /* IS_USED(L3GXXXX_IRQ_DRDY) */

    l3gxxxx_int_src_t int_src = { };

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT)
    if (dev->int_type & L3GXXXX_INT_EVENT) {
        _get_int_event_src(dev, &int_src.event);
    }
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_EVENT) */

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA)
    if (dev->int_type & L3GXXXX_INT_DATA) {
        _get_int_data_src(dev, &int_src.data);
    }
#endif /* IS_USED(L3GXXXX_IRQ_DRDY) */

    return int_src;
}

#endif /* IS_USED(MODULE_L3GXXXX_IRQ) */

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT)

int l3gxxxx_set_int_event_cfg(const l3gxxxx_t *dev,
                              const l3gxxxx_int_event_cfg_t *cfg)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(cfg != NULL);
    DEBUG_DEV("config=%p", dev, cfg);

    int res = L3GXXXX_OK;

    uint8_t ig_cfg = 0;
    uint8_t ig_dur = 0;
    uint8_t ig_ths[6] = { 0 };

    _SET_REG_BIT(ig_cfg, L3GXXXX_INT1_X_LOW, cfg->x_low_enabled);
    _SET_REG_BIT(ig_cfg, L3GXXXX_INT1_X_HIGH, cfg->x_high_enabled);

    _SET_REG_BIT(ig_cfg, L3GXXXX_INT1_Y_LOW, cfg->y_low_enabled);
    _SET_REG_BIT(ig_cfg, L3GXXXX_INT1_Y_HIGH, cfg->y_high_enabled);

    _SET_REG_BIT(ig_cfg, L3GXXXX_INT1_Z_LOW, cfg->z_low_enabled);
    _SET_REG_BIT(ig_cfg, L3GXXXX_INT1_Z_HIGH, cfg->z_high_enabled);

    _SET_REG_BIT(ig_cfg, L3GXXXX_INT1_LATCH, cfg->latch);
    _SET_REG_BIT(ig_cfg, L3GXXXX_INT1_AND_OR, cfg->and_or);

    _SET_REG_BIT(ig_dur, L3GXXXX_INT1_WAIT, cfg->wait);
    _SET_REG_VALUE(ig_dur, L3GXXXX_INT1_DURATION, cfg->duration);

    ig_ths[0] = (cfg->x_threshold >> 8) & 0x7f;
    ig_ths[1] = (cfg->x_threshold & 0xff);
    ig_ths[2] = (cfg->y_threshold >> 8) & 0x7f;
    ig_ths[3] = (cfg->y_threshold & 0xff);
    ig_ths[4] = (cfg->z_threshold >> 8) & 0x7f;
    ig_ths[5] = (cfg->z_threshold & 0xff);

    _acquire(dev);

    /* write the thresholds to registers IG_THS_* */
    res |= _write(dev, L3GXXXX_REG_IG_THS_XH, ig_ths, 6);

    /* write duration configuration to IG_DURATION  */
    res |= _write(dev, L3GXXXX_REG_IG_DURATION, &ig_dur, 1);

    /* write INT1 configuration  to IG_CFG */
    res |= _write(dev, L3GXXXX_REG_IG_CFG, &ig_cfg, 1);

    /* filter selection used for threshold comparison for INT1 generation */
    res |= _update(dev, L3GXXXX_REG_CTRL5, L3GXXXX_IG_SEL, cfg->filter);

    /* try to set HPen in case LPF2 and HPF is used */
    if (cfg->filter == L3GXXXX_HPF_AND_LPF2) {
        res |= _update(dev, L3GXXXX_REG_CTRL5, L3GXXXX_HP_ENABLED, 1);
    }

    _release(dev);

    return res;
}

static int _get_int_event_src(const l3gxxxx_t *dev,
                              l3gxxxx_int_event_src_t *src)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(src != NULL);
    DEBUG_DEV("src=%p", dev, src);

    int res = L3GXXXX_OK;

    uint8_t ig_cfg;
    uint8_t ig_src;

    _acquire(dev);
    res |= _read(dev, L3GXXXX_REG_IG_CFG, (uint8_t *)&ig_cfg, 1);
    res |= _read(dev, L3GXXXX_REG_IG_SRC, (uint8_t *)&ig_src, 1);
    _release(dev);

    src->val = ig_src & ig_cfg & (L3GXXXX_INT1_X_LOW | L3GXXXX_INT1_X_HIGH |
                                  L3GXXXX_INT1_Y_LOW | L3GXXXX_INT1_Y_HIGH |
                                  L3GXXXX_INT1_Z_LOW | L3GXXXX_INT1_Z_HIGH);
    src->active = (ig_src & L3GXXXX_INT1_ACTIVE) ? 1 : 0;

    return res;
}
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_EVENT) */

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA)

static int _get_int_data_src(const l3gxxxx_t *dev,
                             l3gxxxx_int_data_src_t *src)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(src != NULL);
    DEBUG_DEV("src=%p", dev, src);

    int res = L3GXXXX_OK;

    uint8_t fifo_src;
    uint8_t status;

    _acquire(dev);
    res |= _read(dev, L3GXXXX_REG_STATUS, &status, 1);
    res |= _read(dev, L3GXXXX_REG_FIFO_SRC, &fifo_src, 1);
    _release(dev);

    src->data_ready     = _GET_REG_BIT(status, L3GXXXX_ANY_DATA_READY);
    src->fifo_watermark = _GET_REG_BIT(fifo_src, L3GXXXX_FIFO_WTM);
    src->fifo_overrun   = _GET_REG_BIT(fifo_src, L3GXXXX_FIFO_OVR);
    src->fifo_empty     = _GET_REG_BIT(fifo_src, L3GXXXX_FIFO_EMPTY);

    return res;
}
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_DATA) */

#if IS_USED(MODULE_L3GXXXX_CONFIG)

int l3gxxxx_set_mode(l3gxxxx_t *dev, l3gxxxx_odr_t odr,
                     bool x, bool y, bool z)
{
    ASSERT_PARAM(dev != NULL);

    DEBUG_DEV("odr=%02x x=%d y=%d z=%d", dev, odr, x, y, z);

    int res = L3GXXXX_OK;

    dev->params.odr = odr;

    uint8_t ctrl1;

    ctrl1  = (odr << L3GXXXX_ODR_BW_S) & L3GXXXX_ODR_BW;
    ctrl1 |= L3GXXXX_POWER_MODE;
    ctrl1 |= x ? L3GXXXX_X_ENABLED : 0;
    ctrl1 |= y ? L3GXXXX_Y_ENABLED : 0;
    ctrl1 |= z ? L3GXXXX_Z_ENABLED : 0;

    _acquire(dev);
    res |= _write(dev, L3GXXXX_REG_CTRL1, &ctrl1, 1);

#if IS_USED(MODULE_L3GD20H) && IS_USED(MODULE_L3GXXXX_LOW_ODR)
    /* in case of low ODR, low data rate flag has to be set */
    if (dev->params.odr >= L3GXXXX_ODR_12) {
        ASSERT_PARAM(dev->sensor == L3GD20H);
        res |= _update(dev, L3GXXXX_REG_LOW_ODR, L3GXXXX_LOW_ODR, 1);
    }
#endif
    _release(dev);

    return res;
}

int l3gxxxx_set_scale(l3gxxxx_t *dev, l3gxxxx_scale_t scale)
{
    ASSERT_PARAM(dev != NULL);
#if IS_USED(MODULE_A3G4250)
    ASSERT_PARAM((scale == L3GXXXX_SCALE_245_DPS) || (dev->sensor != X3G42XXD));
#endif
    DEBUG_DEV("scale=%02x", dev, scale);

    dev->params.scale = scale;

    /* read CTRL4 register and write scale */
    return l3gxxxx_reg_update(dev, L3GXXXX_REG_CTRL4, L3GXXXX_FULL_SCALE, scale);
}

int l3gxxxx_select_output_filter(l3gxxxx_t *dev,
                                 l3gxxxx_filter_sel_t filter)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("filter=%02x", dev, filter);

    dev->params.filter_sel = filter;

    int res = L3GXXXX_OK;

    _acquire(dev);

    /* set the register OUT_SEL in any case */
    res |= _update(dev, L3GXXXX_REG_CTRL5, L3GXXXX_OUT_SEL, filter);
    /* HPF has to be enabled by a separate bit if filter mode is 1 or 3 */
    res |= _update(dev, L3GXXXX_REG_CTRL5, L3GXXXX_HP_ENABLED, filter & 1);

    _release(dev);

    return res;
}

int l3gxxxx_config_hpf(const l3gxxxx_t *dev,
                       l3gxxxx_hpf_mode_t mode, uint8_t cutoff)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(cutoff <= 9);
    DEBUG_DEV("mode=%02x cutoff=%d", dev, mode, cutoff);

    int res = L3GXXXX_OK;

    _acquire(dev);
    res |= _update(dev, L3GXXXX_REG_CTRL2, L3GXXXX_HPF_MODE, mode);
    res |= _update(dev, L3GXXXX_REG_CTRL2, L3GXXXX_HPF_CUTOFF, cutoff);
    _release(dev);

    return res;
}

int l3gxxxx_set_hpf_ref(const l3gxxxx_t *dev, int8_t ref)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("ref=%d", dev, ref);

    return l3gxxxx_reg_write(dev, L3GXXXX_REG_REFERENCE, (uint8_t *)&ref, 1);
}

int l3gxxxx_get_hpf_ref(const l3gxxxx_t *dev, int8_t *ref)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(ref != NULL);
    DEBUG_DEV("ref=%p", dev, ref);

    return l3gxxxx_reg_read(dev, L3GXXXX_REG_REFERENCE, (uint8_t *)&ref, 1);
}

#if IS_USED(MODULE_L3GXXXX_FIFO)

int l3gxxxx_set_fifo_mode(l3gxxxx_t *dev, l3gxxxx_fifo_mode_t mode,
                          uint8_t watermark)
{
    ASSERT_PARAM(dev != NULL);
#if !IS_USED(L3GD20H) && !IS_USED(L3GD20)
    ASSERT_PARAM(mode <= L3GXXXX_STREAM);
#endif
#if !IS_USED(L3GD20H)
    ASSERT_PARAM(mode <= L3GXXXX_DYNAMIC_STREAM);
#endif
    DEBUG_DEV("mode=%d watermark=%d", dev, mode, watermark);

    int res = L3GXXXX_OK;

    dev->params.fifo_mode = mode;

    _acquire(dev);

    /* read CTRL5 register and write FIFO_EN flag */
    res |= _update(dev, L3GXXXX_REG_CTRL5, L3GXXXX_FIFO_EN, (mode != L3GXXXX_BYPASS));

    /* read FIFO_CTRL register and write FIFO mode */
    res |= _update(dev, L3GXXXX_REG_FIFO_CTRL, L3GXXXX_FIFO_WATERMARK, watermark);
    res |= _update(dev, L3GXXXX_REG_FIFO_CTRL, L3GXXXX_FIFO_MODE, mode);

    _release(dev);

    return res;
}
#endif /* IS_USED(MODULE_L3GXXXX_FIFO) */

int l3gxxxx_get_int_event_cfg(const l3gxxxx_t *dev,
                                 l3gxxxx_int_event_cfg_t *cfg)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(cfg != NULL);
    DEBUG_DEV("config=%p", dev, cfg);

    int res = L3GXXXX_OK;

    uint8_t ig_cfg;
    uint8_t ig_dur;
    uint8_t ig_ths[6];
    uint8_t ctrl3;
    uint8_t ctrl5;

    _acquire(dev);
    res |= _read(dev, L3GXXXX_REG_IG_THS_XH, ig_ths, 6);
    res |= _read(dev, L3GXXXX_REG_IG_CFG, &ig_cfg, 1);
    res |= _read(dev, L3GXXXX_REG_IG_DURATION, &ig_dur, 1);
    res |= _read(dev, L3GXXXX_REG_CTRL3, &ctrl3, 1);
    res |= _read(dev, L3GXXXX_REG_CTRL5, &ctrl5, 1);
    _release(dev);

    cfg->x_low_enabled   = _GET_REG_BIT(ig_cfg, L3GXXXX_INT1_X_LOW);
    cfg->x_high_enabled  = _GET_REG_BIT(ig_cfg, L3GXXXX_INT1_X_HIGH);

    cfg->y_low_enabled   = _GET_REG_BIT(ig_cfg, L3GXXXX_INT1_Y_LOW);
    cfg->y_high_enabled  = _GET_REG_BIT(ig_cfg, L3GXXXX_INT1_Y_HIGH);

    cfg->z_low_enabled   = _GET_REG_BIT(ig_cfg, L3GXXXX_INT1_Z_LOW);
    cfg->z_high_enabled  = _GET_REG_BIT(ig_cfg, L3GXXXX_INT1_Z_HIGH);

    cfg->x_threshold     = msb_lsb_to_type(uint16_t, ig_ths, 0);
    cfg->y_threshold     = msb_lsb_to_type(uint16_t, ig_ths, 2);
    cfg->z_threshold     = msb_lsb_to_type(uint16_t, ig_ths, 4);

    cfg->filter          = _GET_REG_VALUE(ctrl5, L3GXXXX_IG_SEL);

    cfg->and_or          = _GET_REG_BIT(ig_cfg, L3GXXXX_INT1_AND_OR);
    cfg->latch           = _GET_REG_BIT(ig_cfg, L3GXXXX_INT1_LATCH);

    cfg->wait            = _GET_REG_BIT(ig_dur, L3GXXXX_INT1_WAIT);
    cfg->duration        = _GET_REG_VALUE(ig_dur, L3GXXXX_INT1_DURATION);

    cfg->counter_mode    = 0;

    return res;
}

#endif /* IS_USED(MODULE_L3GXXXX_CONFIG) */

int l3gxxxx_reg_read(const l3gxxxx_t *dev,
                     uint8_t reg, uint8_t *data, uint8_t len)
{
    int res;

    _acquire(dev);
    res = _read(dev, reg, data, len);
    _release(dev);

    return res;
}

int l3gxxxx_reg_write(const l3gxxxx_t *dev,
                      uint8_t reg, const uint8_t *data, uint8_t len)
{
   int res;

    _acquire(dev);
    res = _write(dev, reg, data, len);
    _release(dev);

    return res;
}

int l3gxxxx_reg_update(const l3gxxxx_t *dev,
                       uint8_t reg, uint8_t mask, uint8_t val)
{
   int res;

    _acquire(dev);
    res = _update(dev, reg, mask, val);
    _release(dev);

    return res;
}

/** Functions for internal use only */

#define L3GXXXX_I2C_L3GD20X_ADDR_1  (0x6a)      /* L3GD20x I2C address 1 */
#define L3GXXXX_I2C_L3GD20X_ADDR_2  (0x6b)      /* L3GD20x I2C address 2 */
#define L3GXXXX_I2C_X3G42XXD_ADDR_1 (0x68)      /* x3G42xxD I2C address 1 */
#define L3GXXXX_I2C_X3G42XXD_ADDR_2 (0x69)      /* x3G42xxD I2C address 2 */

/**
 * @brief   Check the chip ID to test whether sensor is available
 * @note    Communication bus has to be acquired when enter.
 */
static int _is_available(l3gxxxx_t *dev)
{
    DEBUG_DEV("", dev);

    int res;

    uint8_t chip_id, tries = 16;

    /* try to read the chip id from L3GXXXX_REG_WHO_AM_I */
    do {
        res = _read(dev, L3GXXXX_REG_WHO_AM_I, &chip_id, 1);
    } while ((chip_id == 0xFF || chip_id == 0x0) && --tries);

#if IS_USED(MODULE_L3GXXXX_I2C)
    if ((dev->params.if_params.type == L3GXXXX_I2C) && (res != L3GXXXX_OK)) {
        /* if the interface type is I2C but the chip ID couldn't be read
           from configured address, we probe possible I2C addresses */

#if IS_USED(MODULE_L3GD20H) || IS_USED(MODULE_L3GD20)
        if (res != L3GXXXX_OK) {
            dev->params.if_params.i2c.addr = L3GXXXX_I2C_L3GD20X_ADDR_1;
            res = _read(dev, L3GXXXX_REG_WHO_AM_I, &chip_id, 1);
        }
        if (res != L3GXXXX_OK) {
            dev->params.if_params.i2c.addr = L3GXXXX_I2C_L3GD20X_ADDR_2;
            res = _read(dev, L3GXXXX_REG_WHO_AM_I, &chip_id, 1);
        }
#endif /* IS_USED(MODULE_L3GD20H) || IS_USED(MODULE_L3GD20) */

#if IS_USED(MODULE_L3G4200D_NG) || IS_USED(MODULE_I3G4250D) || IS_USED(MODULE_A3G4250D)
        if (res != L3GXXXX_OK) {
            dev->params.if_params.i2c.addr = L3GXXXX_I2C_X3G42XXD_ADDR_1;
            res = _read(dev, L3GXXXX_REG_WHO_AM_I, &chip_id, 1);
        }
        if (res != L3GXXXX_OK) {
            dev->params.if_params.i2c.addr = L3GXXXX_I2C_X3G42XXD_ADDR_2;
            res = _read(dev, L3GXXXX_REG_WHO_AM_I, &chip_id, 1);
        }
#endif /* IS_USED(MODULE_L3G4200D_NG) || IS_USED(MODULE_I3G4250D) || IS_USED(MODULE_A3G4250D) */

    }
#endif /* #if IS_USED(MODULE_L3GXXXX_I2C) */

    if (res != L3GXXXX_OK) {
        return res;
    }

    /* determine the sensor type */
    switch (chip_id) {
#if IS_USED(MODULE_L3GD20H)
        case L3GXXXX_CHIP_ID_L3GD20H:  dev->sensor = L3GD20H;
                                       break;
#endif
#if IS_USED(MODULE_L3GD20)
        case L3GXXXX_CHIP_ID_L3GD20:   dev->sensor = L3GD20;
                                       break;
#endif
#if IS_USED(MODULE_L3G4200D_NG) || IS_USED(MODULE_I3G4250D) || IS_USED(MODULE_A3G4250D)
        case L3GXXXX_CHIP_ID_X3G42XXD: dev->sensor = X3G42XXD;
                                       break;
#endif
        default: DEBUG_DEV("sensor is not available, wrong chip id %02x",
                           dev, chip_id);
                 return -L3GXXXX_ERROR_WRONG_CHIP_ID;
    }

    return res;
}

static int _update(const l3gxxxx_t *dev,
                   uint8_t reg, uint8_t mask, uint8_t val)
{
    DEBUG_DEV("reg=%02x mask=%02x val=%02x", dev, reg, mask, val);

    int res = L3GXXXX_OK;

    uint8_t reg_val;
    uint8_t shift = 0;

    while (!((mask >> shift) & 0x01)) {
        shift++;
    }

    /* read current register value */
    res |= _read(dev, reg, &reg_val, 1);

    /* set masked bits to the given value  */
    reg_val = (reg_val & ~mask) | ((val << shift) & mask);

    /* write back new register value */
    res |= _write(dev, reg, &reg_val, 1);

    return res;
}

#define L3GXXXX_SPI_READ_FLAG      0x80
#define L3GXXXX_SPI_WRITE_FLAG     0x00
#define L3GXXXX_SPI_AUTO_INC_FLAG  0x40

#define L3GXXXX_I2C_AUTO_INC_FLAG  0x80

static void _acquire(const l3gxxxx_t *dev)
{
    ASSERT_PARAM(dev != NULL);
#if IS_USED(MODULE_L3GXXXX_SPI)
    if (dev->params.if_params.type == L3GXXXX_SPI) {
        spi_acquire(_SPI_DEV, _SPI_CS, SPI_MODE_3, _SPI_CLK);
        return;
    }
#endif
#if IS_USED(MODULE_L3GXXXX_I2C)
    if (dev->params.if_params.type == L3GXXXX_I2C) {
        i2c_acquire(_I2C_DEV);
        return;
    }
#endif
    return;
}

static void _release(const l3gxxxx_t *dev)
{
    ASSERT_PARAM(dev != NULL);
#if IS_USED(MODULE_L3GXXXX_SPI)
    if (dev->params.if_params.type == L3GXXXX_SPI) {
        spi_release(_SPI_DEV);
        return;
    }
#endif
#if IS_USED(MODULE_L3GXXXX_I2C)
    if (dev->params.if_params.type == L3GXXXX_I2C) {
        i2c_release(_I2C_DEV);
        return;
    }
#endif
    return;
}

static int _read(const l3gxxxx_t *dev, uint8_t reg, uint8_t *data, uint8_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);

    DEBUG_DEV("read %d byte from sensor registers starting at addr 0x%02x",
              dev, len, reg);

#if IS_USED(MODULE_L3GXXXX_SPI)
    if (dev->params.if_params.type == L3GXXXX_SPI) {
        reg &= 0x3f;
        reg |= L3GXXXX_SPI_READ_FLAG;
        reg |= L3GXXXX_SPI_AUTO_INC_FLAG;

        /* the first byte sent is the register address */
        spi_transfer_byte(_SPI_DEV, _SPI_CS, true, reg);
        spi_transfer_bytes(_SPI_DEV, _SPI_CS, false, 0, data, len);

        if (ENABLE_DEBUG) {
            printf("[l3gxxxx] %s dev=%" PRIxPTR ": read following bytes: ",
                   __func__,  (unsigned int)dev);
            for (uint8_t i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        return L3GXXXX_OK;
    }
#endif /* IS_USED(MODULE_L3GXXXX_SPI) */

#if IS_USED(MODULE_L3GXXXX_I2C)
    if (dev->params.if_params.type == L3GXXXX_I2C) {
        if (len > 1) {
            reg |= L3GXXXX_I2C_AUTO_INC_FLAG;
        }

        int res = i2c_read_regs(_I2C_DEV, _I2C_ADDR, reg, data, len, 0);

        if (res != 0) {
            DEBUG_DEV("I2C addr=%02x could not read %d bytes from sensor "
                      "registers starting at addr %02x, reason %d (%s)",
                      dev, _I2C_ADDR, len, reg, res, strerror(res * -1));
            return -L3GXXXX_ERROR_I2C;
        }

        if (ENABLE_DEBUG) {
            printf("[l3gxxxx] %s dev=%" PRIxPTR ": read following bytes: ",
                   __func__,  (unsigned int)dev);
            for (uint8_t i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
        return L3GXXXX_OK;
    }
#endif /* IS_USED(MODULE_L3GXXXX_I2C) */

    return -L3GXXXX_ERROR_INV_DEV;
}

static int _write(const l3gxxxx_t *dev,
                  uint8_t reg, const uint8_t *data, uint8_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);

    DEBUG_DEV("write %d bytes to sensor registers starting at addr 0x%02x",
              dev, len, reg);

#if IS_USED(MODULE_L3GXXXX_SPI)
    if (dev->params.if_params.type == L3GXXXX_SPI) {
        reg &= 0x3f;
        reg |= L3GXXXX_SPI_WRITE_FLAG;
        reg |= L3GXXXX_SPI_AUTO_INC_FLAG;

        if (ENABLE_DEBUG) {
            printf("[l3gxxxx] %s dev=%" PRIxPTR ": write following bytes: ",
                   __func__,  (unsigned int)dev);
            for (uint8_t i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }

        spi_transfer_byte(_SPI_DEV, _SPI_CS, true, reg);
        spi_transfer_bytes(_SPI_DEV, _SPI_CS, false, data, 0, len);

        return L3GXXXX_OK;
    }
#endif /* IS_USED(MODULE_L3GXXXX_SPI) */

#if IS_USED(MODULE_L3GXXXX_I2C)
    if (dev->params.if_params.type == L3GXXXX_I2C) {
        if (len > 1) {
            reg |= L3GXXXX_I2C_AUTO_INC_FLAG;
        }

        if (ENABLE_DEBUG) {
            printf("[l3gxxxx] %s dev=%" PRIxPTR ": write following bytes: ",
                   __func__,  (unsigned int)dev);
            for (uint8_t i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }

        int res;

        if (!data || !len) {
            res = i2c_write_byte(_I2C_DEV, _I2C_ADDR, reg, 0);
        }
        else {
            res = i2c_write_regs(_I2C_DEV, _I2C_ADDR, reg, data, len, 0);
        }

        if (res != L3GXXXX_OK) {
            DEBUG_DEV("I2C addr=%02x could not write %d bytes to sensor "
                      "registers starting at addr 0x%02x, reason %d (%s)",
                      dev, _I2C_ADDR, len, reg, res, strerror(res * -1));
            return -L3GXXXX_ERROR_I2C;
        }

        return res;
    }
#endif /* IS_USED(MODULE_L3GXXXX_I2C) */

    return -L3GXXXX_ERROR_INV_DEV;
}
