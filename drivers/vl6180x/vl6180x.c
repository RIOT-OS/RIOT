/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_vl6180x
 * @brief       Device driver for the ST VL6180X Ranging and Ambient Light Sensing (ALS) module
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <string.h>
#include <stdlib.h>

#include "vl6180x_regs.h"
#include "vl6180x.h"

#include "irq.h"
#include "log.h"
#include "ztimer.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    do { \
        if (!(cond)) { \
            DEBUG("[vl6180x] %s: %s\n", \
                  __func__, "parameter condition (" # cond ") not fulfilled"); \
            assert(cond); \
        } \
    } while (0)

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[vl6180x] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.i2c_dev, d->params.i2c_addr, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond)
#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[vl6180x] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.i2c_dev, d->params.i2c_addr, ## __VA_ARGS__)

#define _SET_REG_VALUE(r, m, v) r = (r & ~m) | (((v) << m ## _S) & m)
#define _GET_REG_VALUE(r, m) ((r & ~m) >> m ## _S)

/** Forward declaration of functions for internal use */
static int _is_available(const vl6180x_t *dev);
static int _reset(const vl6180x_t *dev);
static int _init(vl6180x_t *dev);

inline static void _acquire(const vl6180x_t *dev);
inline static void _release(const vl6180x_t *dev);

static int _read(const vl6180x_t *dev,
                 uint16_t reg, uint8_t *pdata, uint8_t len);
static int _write(const vl6180x_t *dev,
                  uint16_t reg, const uint8_t *pdata, uint8_t len);

inline static int  _read_word(const vl6180x_t *dev, uint16_t reg, uint16_t *word);
inline static int  _write_byte(const vl6180x_t *dev, uint16_t reg, uint8_t byte);
inline static int  _write_word(const vl6180x_t *dev, uint16_t reg, uint16_t word);

inline static int  _read_word_x(const vl6180x_t *dev, uint16_t reg, uint16_t *word);
inline static int  _write_byte_x(const vl6180x_t *dev, uint16_t reg, uint8_t byte);
inline static int  _write_word_x(const vl6180x_t *dev, uint16_t reg, uint16_t word);

int vl6180x_init(vl6180x_t *dev, const vl6180x_params_t *params)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);

    DEBUG_DEV("params=%p", dev, params);

#if IS_USED(MODULE_VL6180X_RNG)
    ASSERT_PARAM(params->rng_max_time > 0 && params->rng_max_time < 64);
#endif
#if IS_USED(MODULE_VL6180X_ALS)
    ASSERT_PARAM(params->als_int_time > 0 && params->als_int_time < 512);
#endif

    /* init sensor data structure */
    dev->params = *params;

#if IS_USED(MODULE_VL6180X_SHUTDOWN)
    /* if shutdown is used, the pin nust not be undefined */
    ASSERT_PARAM(gpio_is_valid(params->shutdown_pin));

    /* shutdown pin is initialized and set to HIGH */
    gpio_init(params->shutdown_pin, GPIO_OUT);
    gpio_write(params->shutdown_pin, 1);
#endif /* IS_USED(MODULE_VL6180X_SHUTDOWN) */

    /* init the sensor and start measurement if periodic measurements used */
    return _init(dev);
}

int vl6180x_start_cont(vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(dev->params.period != 0); /* only possible of period != 0 */
    ASSERT_PARAM(!dev->cont_meas);         /* continuous mode is not yet started */

    /* cppcheck-suppress unusedVariable
     * (reason: only unused if neither vl6180x_rng nor vl6180x_als is used) */
    uint8_t status;
    int res = VL6180X_OK;

    _acquire(dev);

#if IS_USED(MODULE_VL6180X_ALS)
    res |= _read(dev, VL6180X_REG_ALS_STATUS, &status, 1);
    if ((status & VL6180X_ALS_DEVICE_RDY) == 0) {
        res |= _write_byte(dev, VL6180X_REG_ALS_START, VL6180X_ALS_START_STOP);
    }
    while ((status & VL6180X_ALS_DEVICE_RDY) == 0) {
        /* start measurement only when device is ready to start*/
        res |= _read(dev, VL6180X_REG_ALS_STATUS, &status, 1);
    }
    /*
     * Start either only ALS measurements or both ALS and range
     * measurements in continuous mode. According to the data sheet,
     * the interleaved mode should be used in latter case. This means
     * that the INTERLEAVED_MODE__ENABLE register must be set to 0x01
     * and the ALS measurement must be started as configured. The same
     * period is then used for both measurements.
     */
    res |= _write_byte(dev, VL6180X_REG_INTERLEAVED_MODE,
                            IS_USED(MODULE_VL6180X_RNG));
    res |= _write_byte(dev, VL6180X_REG_ALS_START,
                            VL6180X_ALS_START_STOP | VL6180X_ALS_MODE_CONT);
#elif IS_USED(MODULE_VL6180X_RNG)
    res |= _read(dev, VL6180X_REG_RNG_STATUS, &status, 1);
    if ((status & VL6180X_RNG_DEVICE_RDY) == 0) {
        res |= _write_byte(dev, VL6180X_REG_RNG_START, VL6180X_RNG_START_STOP);
    }
    while ((status & VL6180X_RNG_DEVICE_RDY) == 0) {
        /* start measurement only when device is ready to start*/
        res |= _read(dev, VL6180X_REG_RNG_STATUS, &status, 1);
    }
    if ((status & VL6180X_RNG_DEVICE_RDY) == 0) {
        /* start measurement only when device is ready to start*/
        return -VL6180X_ERROR_NOT_READY;
    }

    /* start only range measurements in continuous mode */
    res |= _write_byte(dev, VL6180X_REG_INTERLEAVED_MODE, 0);
    res |= _write_byte(dev, VL6180X_REG_RNG_START,
                            VL6180X_RNG_START_STOP | VL6180X_RNG_MODE_CONT);
#endif /* IS_USED(MODULE_VL6180X_ALS) */

    _release(dev);

    dev->cont_meas = (res == VL6180X_OK) ? true : dev->cont_meas;

    return res;
}

int vl6180x_stop_cont(vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(dev->params.period != 0); /* operation is only valid if period != 0 */
    ASSERT_PARAM(dev->cont_meas);   /* continuous mode is started */

    /* cppcheck-suppress unusedVariable
     * (reason: only unused if neither vl6180x_rng nor vl6180x_als is used) */
    uint8_t status;
    int res = VL6180X_OK;

    _acquire(dev);

#if IS_USED(MODULE_VL6180X_ALS)
    res |= _read(dev, VL6180X_REG_ALS_STATUS, &status, 1);
    if ((status & VL6180X_ALS_DEVICE_RDY) == 0) {
        /* stop only when device is not ready, otherwise it is already stopped */
        res |= _write_byte(dev, VL6180X_REG_ALS_START, VL6180X_ALS_START_STOP);
    }
    /* wait that the device becomes ready */
    do {
        res |= _read(dev, VL6180X_REG_ALS_STATUS, &status, 1);
    } while ((res != VL6180X_OK) || ((status & VL6180X_ALS_DEVICE_RDY) == 0));

#elif IS_USED(MODULE_VL6180X_RNG)
    res |= _read(dev, VL6180X_REG_RNG_STATUS, &status, 1);
    if ((status & VL6180X_RNG_DEVICE_RDY) == 0) {
        /* stop only when device is not ready, otherwise it is already stopped */
        res |= _write_byte(dev, VL6180X_REG_RNG_START, VL6180X_RNG_START_STOP);
    }
    /* wait that the device becomes ready */
    do {
        res |= _read(dev, VL6180X_REG_RNG_STATUS, &status, 1);
    } while ((res != VL6180X_OK) || ((status & VL6180X_RNG_DEVICE_RDY) == 0));
#endif

    res |= _write_byte(dev, VL6180X_REG_INTERLEAVED_MODE, 0);
    _release(dev);

    dev->cont_meas = (res == VL6180X_OK) ? false : dev->cont_meas;

    return res;
}

#if IS_USED(MODULE_VL6180X_RNG)

int vl6180x_rng_data_ready (const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t byte;

    int res = vl6180x_reg_read(dev, VL6180X_REG_INT_STATUS, &byte, 1);
    if (res != VL6180X_OK) {
        return res;
    }

    return ((byte & VL6180X_INT_RNG_DRDY) != 0) ? VL6180X_OK
                                                : -VL6180X_ERROR_NO_DATA;
}

int vl6180x_rng_read(vl6180x_t *dev, uint8_t *mm)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(mm != NULL);
    DEBUG_DEV("mm=%p", dev, mm);

    _acquire(dev);

    /* read range results */
    int res;
    res = _read(dev, VL6180X_REG_RNG_VALUE, mm, 1);

    /* read range measurement status */
    uint8_t status;
    res |= _read(dev, VL6180X_REG_RNG_STATUS, &status, 1);

    _release(dev);

    dev->rng_status = (status & VL6180X_ALS_ERR_CODE) >> VL6180X_RNG_ERR_CODE_S;

    if (res != VL6180X_OK) {
        return res;
    }
    else {
        /* return with error code if status isn't zero */
        return (dev->rng_status) ? -VL6180X_ERROR_RNG : VL6180X_OK;
    }
}

vl6180x_rng_status_t vl6180x_rng_status(const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);
    return dev->rng_status;
}

int vl6180x_rng_start_single(const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(!dev->cont_meas);  /* continuous mode is not started */

    uint8_t status;
    int res;

    _acquire(dev);
    res = _read(dev, VL6180X_REG_RNG_STATUS, &status, 1);
    if ((res == VL6180X_OK) && ((status & VL6180X_RNG_DEVICE_RDY) == 1)) {
        /* start measurement only when device is ready to start*/
        res = _write_byte_x(dev, VL6180X_REG_RNG_START, VL6180X_RNG_START_STOP);
    }
    _release(dev);

    return res;
}

#if IS_USED(MODULE_VL6180X_CONFIG)

int vl6180x_rng_config(vl6180x_t *dev, uint8_t period, uint8_t max_time)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(max_time < 64);

    DEBUG_DEV("period=%u max_time=%u", dev, period, max_time);

    int res;

    /* write new ranging configuration */
    dev->params.period = period;
    dev->params.rng_max_time = max_time;
    period -= 1; /* register value is decremented by 1 */

    _acquire(dev);
    res  = _write(dev, VL6180X_REG_RNG_PERIOD, &period, 1);
    res |= _write(dev, VL6180X_REG_RNG_MAX_TIME, &max_time, 1);
    _release(dev);

    /* if running in continuous mode, stop and restart measurements */
    if ((res == VL6180X_OK) && dev->cont_meas) {
        res = vl6180x_stop_cont(dev);
        if ((res == VL6180X_OK) && period) {
            res = vl6180x_start_cont(dev);
        }
    }

    return res;
}

#endif /* IS_USED(MODULE_VL6180X_CONFIG) */
#endif /* IS_USED(MODULE_VL6180X_RNG) */

#if IS_USED(MODULE_VL6180X_ALS)

int vl6180x_als_data_ready(const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t byte;

    int res = vl6180x_reg_read(dev, VL6180X_REG_INT_STATUS, &byte, 1);
    if (res != VL6180X_OK) {
        return res;
    }

    return ((byte & VL6180X_INT_ALS_DRDY) != 0) ? VL6180X_OK
                                                : -VL6180X_ERROR_NO_DATA;
}

static const uint16_t _als_gains[] = {
/*  20     10.32  5.21  2.56  1.72  1.28  1.01  40     actual analogue gain */
    20000, 10320, 5210, 2560, 1720, 1280, 1010, 40000
};

int vl6180x_als_read(vl6180x_t *dev, uint16_t *raw, uint16_t *lux)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL || lux != NULL);
    DEBUG_DEV("raw=%p lux=%p", dev, raw, lux);

    int res;

    _acquire(dev);

    /* read raw ALS results */
    uint16_t cnt = 0;
    res = _read_word(dev, VL6180X_REG_ALS_VALUE, &cnt);

    /* read range measurement status */
    uint8_t status;
    res |= _read(dev, VL6180X_REG_ALS_STATUS, &status, 1);

    _release(dev);

    if (res) {
        return res;
    }

    dev->als_status = (status & VL6180X_ALS_ERR_CODE_S) >> VL6180X_ALS_ERR_CODE_S;

    if (dev->als_status == VL6180X_ALS_OK) {
        if (raw) {
            *raw = cnt;
        }
        if (lux) {
            /* lux = lux_res * (cnt / als_gain) * (100 / als_int_time) */
            uint32_t lux_tmp;
            lux_tmp  = dev->params.als_lux_res * cnt * 100;
            lux_tmp /= _als_gains[dev->params.als_gain];
            lux_tmp /= dev->params.als_int_time;
            *lux = lux_tmp;
        }

    }

    /* return with error code if status isn't zero */
    return (dev->als_status) ? -VL6180X_ERROR_ALS : VL6180X_OK;
}

vl6180x_als_status_t vl6180x_als_status(const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);
    return dev->als_status;
}

int vl6180x_als_start_single(const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(!dev->cont_meas);  /* continuous mode is not started */

    uint8_t status;
    int res;

    _acquire(dev);

    res = _read(dev, VL6180X_REG_ALS_STATUS, &status, 1);
    if ((res == VL6180X_OK) && ((status & VL6180X_ALS_DEVICE_RDY) == 1)) {
        /* start measurement only when device is ready to start*/
        res = _write_byte(dev, VL6180X_REG_ALS_START, VL6180X_ALS_START_STOP);
    }

    _release(dev);

    return res;
}

#if IS_USED(MODULE_VL6180X_CONFIG)

int vl6180x_als_config(vl6180x_t *dev, uint8_t period, uint8_t int_time,
                       vl6180x_als_gain_t gain)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);

    DEBUG_DEV("period=%u int_time=%u gain=%u", dev, period, int_time, gain);

    /* write new ambient light sensing (ALS) configuration */
    dev->params.period = period;
    dev->params.als_int_time = int_time;
    dev->params.als_gain = gain;

    int res;

    _acquire(dev);
    res  = _write_byte(dev, VL6180X_REG_ALS_PERIOD, period - 1);
    res |= _write_word(dev, VL6180X_REG_ALS_INT_TIME, int_time - 1);
    res |= _write_byte(dev, VL6180X_REG_ALS_GAIN, 0x40 + gain);
    _release(dev);

    /* if running in continuous mode, stop and restart measurements */
    if ((res == VL6180X_OK) && dev->cont_meas) {
         res = vl6180x_stop_cont(dev);
        if ((res == VL6180X_OK) && period) {
            res = vl6180x_start_cont(dev);
        }
    }

    return res;
}
#endif /* IS_USED(MODULE_VL6180X_CONFIG) */
#endif /* IS_USED(MODULE_VL6180X_ALS) */

#if IS_USED(MODULE_VL6180X_SHUTDOWN)
int vl6180x_power_down(const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    /* assert shutdown pin */
    gpio_clear(dev->params.shutdown_pin);
    return VL6180X_OK;
}

int vl6180x_power_up(vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    /* deactivate shutdown pin */
    gpio_set(dev->params.shutdown_pin);

    /* init the sensor and start measurement */
    _init(dev);

    return VL6180X_OK;
}
#endif /* IS_USED(MODULE_VL6180X_SHUTDOWN) */

#if IS_USED(MODULE_VL6180X_IRQ)

static void _isr(void *lock)
{
    mutex_unlock(lock);
}

int vl6180x_int_wait(const vl6180x_t *dev, vl6180x_int_config_t *src)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(src != NULL);
    ASSERT_PARAM(gpio_is_valid(dev->params.int_pin));

    mutex_t lock = MUTEX_INIT_LOCKED;

    /* enable interrupt pin */
    gpio_init_int(dev->params.int_pin, GPIO_IN_PU, GPIO_FALLING, _isr, &lock);

    /* wait for interrupt */
    mutex_lock(&lock);

    /* disable interrupt pin */
    gpio_irq_disable(dev->params.int_pin);

    /* read interrupt sources */
    uint8_t byte;
    int res;

    _acquire(dev);
    res = _read(dev, VL6180X_REG_INT_STATUS, &byte, 1);
#if IS_USED(MODULE_VL6180X_RNG)
    src->rng_int = (byte & VL6180X_INT_RNG) >> VL6180X_INT_RNG_S;
#endif
#if IS_USED(MODULE_VL6180X_ALS)
    src->als_int = (byte & VL6180X_INT_ALS) >> VL6180X_INT_ALS_S;
#endif

    /* clear all interrupt flags */
    res |= _write_byte(dev, VL6180X_REG_INT_CLR, VL6180X_CLR_ALL_INT);
    _release(dev);

    return res;
}

#if IS_USED(MODULE_VL6180X_CONFIG)
int vl6180x_int_enable(vl6180x_t *dev, vl6180x_int_config_t mode)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    dev->params.int_cfg = mode;

    uint8_t byte = 0;
#if IS_USED(MODULE_VL6180X_RNG)
    byte |= (mode.rng_int << VL6180X_INT_RNG_S) & VL6180X_INT_RNG;
#endif
#if IS_USED(MODULE_VL6180X_ALS)
    byte |= (mode.als_int << VL6180X_INT_ALS_S) & VL6180X_INT_ALS;
#endif

    int res;
    _acquire(dev);
    res  = _write_byte(dev, VL6180X_REG_INT_CONFIG, byte);
    res |= _write_byte(dev, VL6180X_REG_GPIO1_MODE,
                            VL6180X_GPIO1_POL_LOW | VL6180X_GPIO1_FUNC_ON);
    res |= _write_byte(dev, VL6180X_REG_INT_CLR, VL6180X_CLR_ALL_INT);
    _release(dev);

    return res;
}

int vl6180x_int_config(vl6180x_t *dev, vl6180x_int_thresh_t thresh)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    dev->params.int_thresh = thresh;

    int res = VL6180X_OK;

    _acquire(dev);
#if IS_USED(MODULE_VL6180X_RNG)
    res |= _write_byte(dev, VL6180X_REG_RNG_THRESH_HI, thresh.rng_high);
    res |= _write_byte(dev, VL6180X_REG_RNG_THRESH_LO, thresh.rng_low);
#endif

#if IS_USED(MODULE_VL6180X_ALS)
    res |= _write_word(dev, VL6180X_REG_ALS_THRESH_HI, thresh.als_high);
    res |= _write_word(dev, VL6180X_REG_ALS_THRESH_LO, thresh.als_low);
#endif
    _release(dev);

    return res;
}
#endif /* IS_USED(MODULE_VL6180X_CONFIG) */

#endif /* IS_USED(MODULE_VL6180X_IRQ) */

int vl6180x_reg_read(const vl6180x_t *dev,
                     uint16_t reg, uint8_t *pdata, uint8_t len)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("reg=%04x pdata=%p len=%u", dev, reg, pdata, len);

    int res;

    _acquire(dev);
    res = _read(dev, reg, pdata, len);
    _release(dev);

    return res;
}

int vl6180x_reg_write(const vl6180x_t *dev,
                      uint16_t reg, const uint8_t *pdata, uint8_t len)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("reg=%04x pdata=%p len=%u", dev, reg, pdata, len);

    int res;

    _acquire(dev);
    res = _write(dev, reg, pdata, len);
    _release(dev);

    return res;
}

/** Functions for internal use only */

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(const vl6180x_t *dev)
{
    DEBUG_DEV("", dev);

    uint8_t data[5];

    /* read the chip id from VL6180X_REG_ID_X */
    if (_read(dev, VL6180X_REG_MODEL_ID, data, 5) != VL6180X_OK) {
        return VL6180X_ERROR_I2C;
    }

    if (data[0] != VL6180X_MODEL_ID) {
        DEBUG_DEV("sensor is not available, wrong device id %02x, "
                  "should be %02x", dev, data[0], VL6180X_MODEL_ID);
        return -VL6180X_ERROR_WRONG_ID;
    }

    DEBUG_DEV("rev=%u.%u, module rev=%d.%d",
              dev, data[1], data[2], data[3], data[4]);

    return VL6180X_OK;
}

static int _init(vl6180x_t *dev)
{
#if IS_USED(MODULE_VL6180X_RNG)
    dev->rng_status = VL6180X_RNG_OK;
#endif /* IS_USED(MODULE_VL6180X_RNG) */
#if IS_USED(MODULE_VL6180X_ALS)
    dev->als_status = VL6180X_ALS_OK;
#endif /* IS_USED(MODULE_VL6180X_ALS) */

    dev->cont_meas = false;

    /* we have to wait at least 400 us after power on, with msec timer 1 ms */
    ztimer_sleep(ZTIMER_MSEC, 1);

    int res;

    _acquire(dev);

    /* check availability of the sensor */
    res = _is_available(dev);
    if (res != VL6180X_OK) {
        _release(dev);
        return res;
    }

    /* reset the device to recommended configuration */
    res = _reset(dev);

    /* clear all interrupt flags */
    res |= _write_byte(dev, VL6180X_REG_INT_CLR, VL6180X_CLR_ALL_INT);

    /* set range measurement configuration */
    uint8_t rng_int = 0;

#if IS_USED(MODULE_VL6180X_RNG)
#if IS_USED(MODULE_VL6180X_IRQ)
    res |= _write_byte(dev, VL6180X_REG_RNG_THRESH_HI, dev->params.int_thresh.rng_high);
    res |= _write_byte(dev, VL6180X_REG_RNG_THRESH_LO, dev->params.int_thresh.rng_low);
    rng_int = (dev->params.int_cfg.rng_int << VL6180X_INT_RNG_S) & VL6180X_INT_RNG;
#else
    rng_int = VL6180X_INT_RNG_DRDY;
#endif /* IS_USED(MODULE_VL6180X_IRQ) */
    res |= _write_byte(dev, VL6180X_REG_RNG_PERIOD, dev->params.period - 1);
    res |= _write_byte(dev, VL6180X_REG_RNG_MAX_TIME, dev->params.rng_max_time);
#endif /* IS_USED(MODULE_VL6180X_RNG) */

    /* set ALS measurement configuration */
    uint8_t als_int = 0;

#if IS_USED(MODULE_VL6180X_ALS)
#if IS_USED(MODULE_VL6180X_IRQ)
    res |= _write_byte(dev, VL6180X_REG_ALS_THRESH_HI, dev->params.int_thresh.als_high);
    res |= _write_byte(dev, VL6180X_REG_ALS_THRESH_LO, dev->params.int_thresh.als_low);
    als_int = (dev->params.int_cfg.als_int << VL6180X_INT_ALS_S) & VL6180X_INT_ALS;
#else  /* IS_USED(MODULE_VL6180X_IRQ) */
    als_int = VL6180X_INT_ALS_DRDY;
#endif /* IS_USED(MODULE_VL6180X_IRQ) */
    res |= _write_byte(dev, VL6180X_REG_ALS_PERIOD, dev->params.period - 1);
    res |= _write_byte(dev, VL6180X_REG_ALS_INT_TIME, dev->params.als_int_time - 1);
    res |= _write_byte(dev, VL6180X_REG_ALS_GAIN, 0x40 + dev->params.als_gain);
#endif /* IS_USED(MODULE_VL6180X_ALS) */

#if IS_USED(MODULE_VL6180X_IRQ)
    res |= _write_byte(dev, VL6180X_REG_GPIO1_MODE,
                            VL6180X_GPIO1_POL_LOW | VL6180X_GPIO1_FUNC_ON);
#endif
    /* cppcheck-suppress knownConditionTrueFalse
     * (reason: it is not a condition but a bitwise OR) */
    res |= _write_byte(dev, VL6180X_REG_INT_CONFIG, rng_int | als_int);
    _release(dev);

    if (res != VL6180X_OK) {
        return res;
    }

    /* if period is defined, continuous mode measurements are started */
    if (dev->params.period) {
        return vl6180x_start_cont(dev);
    }

    return VL6180X_OK;
}

typedef struct {
    uint16_t reg;
    uint8_t value;
} _vl6180x_reg_value_t;

/*
 * Private registers and standard register initialization values that have to
 * be set after power on reset, see [Application Note AN4545, section 9]
 * (https://www.st.com/resource/en/application_note/dm00122600.pdf)
 */
static const _vl6180x_reg_value_t _init_values [] = {
    /* Mandatory : private registers */
    { .reg = 0x0207, .value = 0x01 },
    { .reg = 0x0208, .value = 0x01 },
    { .reg = 0x0096, .value = 0x00 },
    { .reg = 0x0097, .value = 0xfd },
    { .reg = 0x00e3, .value = 0x01 },
    { .reg = 0x00e4, .value = 0x03 },
    { .reg = 0x00e5, .value = 0x02 },
    { .reg = 0x00e6, .value = 0x01 },
    { .reg = 0x00e7, .value = 0x03 },
    { .reg = 0x00f5, .value = 0x02 },
    { .reg = 0x00d9, .value = 0x05 },
    { .reg = 0x00db, .value = 0xce },
    { .reg = 0x00dc, .value = 0x03 },
    { .reg = 0x00dd, .value = 0xf8 },
    { .reg = 0x009f, .value = 0x00 },
    { .reg = 0x00a3, .value = 0x3c },
    { .reg = 0x00b7, .value = 0x00 },
    { .reg = 0x00bb, .value = 0x3c },
    { .reg = 0x00b2, .value = 0x09 },
    { .reg = 0x00ca, .value = 0x09 },
    { .reg = 0x0198, .value = 0x01 },
    { .reg = 0x01b0, .value = 0x17 },
    { .reg = 0x01ad, .value = 0x00 },
    { .reg = 0x00ff, .value = 0x05 },
    { .reg = 0x0100, .value = 0x05 },
    { .reg = 0x0199, .value = 0x05 },
    { .reg = 0x01a6, .value = 0x1b },
    { .reg = 0x01ac, .value = 0x3e },
    { .reg = 0x01a7, .value = 0x1f },
    { .reg = 0x0030, .value = 0x00 },

    /* Recommended : Public registers - See data sheet for more detail */
    { .reg = 0x0011, .value = 0x10 }, /* GPIO1 is HIGH active and OFF (Hi-Z), polling enabled */
    { .reg = 0x010a, .value = 0x30 }, /* Range averaging period is 4.3 ms*/
    { .reg = 0x003f, .value = 0x46 }, /* ALS analogue gain is 1.01, dark gain upper nibble */
    { .reg = 0x0031, .value = 0xff }, /* Calibration every 255 measurements */
    { .reg = 0x0041, .value = 0x63 }, /* ALS integration time is 100 ms */
    { .reg = 0x002e, .value = 0x01 }, /* Perform temperature calibration */

    /* Optional: Public registers - See data sheet for more detail */
    { .reg = 0x001b, .value = 0x13 }, /* Range measurement period is 200 ms */
    { .reg = 0x003e, .value = 0x13 }, /* ALS measurement period is 200 ms */
    { .reg = 0x0014, .value = 0x00 }, /* Interrupts disabled */
    { .reg = 0x02a3, .value = 0x00 }, /* Interleave mode disabled */

    /* change fresh out of status to 0 */
    { .reg = 0x0016, .value = 0x00 },
};

static int _reset(const vl6180x_t *dev)
{
    uint8_t byte;
    int res = VL6180X_OK;

    /* read the fresh out from reset status */
    res |= _read(dev, VL6180X_REG_FRESH_RST, &byte, 1);

    /* if not fresh out from reset, stop measurements if necessary */
    if ((byte & 0x01) == 0) {
        uint8_t status;
        res |= _read(dev, VL6180X_REG_RNG_STATUS, &status, 1);
        if ((status & VL6180X_RNG_DEVICE_RDY) == 0) {
            /* stop measurement when device is not ready */
            res |= _write_byte(dev, VL6180X_REG_RNG_START, VL6180X_RNG_START_STOP);
        }
    }
    /* load initial register value set */
    for (unsigned i = 0; i < ARRAY_SIZE(_init_values); i++) {
        res |= _write_byte(dev, _init_values[i].reg, _init_values[i].value);
    }

    return res;
}

inline static void _acquire(const vl6180x_t *dev)
{
    i2c_acquire(dev->params.i2c_dev);
}

inline static void _release(const vl6180x_t *dev)
{
    i2c_release(dev->params.i2c_dev);
}

#define VL6180X_BUFSIZ   32
static uint8_t _buffer[VL6180X_BUFSIZ] = {};

static int _write(const vl6180x_t *dev,
                  uint16_t reg, const uint8_t *pdata, uint8_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(pdata != NULL);
    ASSERT_PARAM(len <= (VL6180X_BUFSIZ - 2));
    DEBUG_DEV("reg=%04x pdata=%p len=%u", dev, reg, pdata, len);

#if ENABLE_DEBUG
    printf("[vl6180x] %s i2c dev=%d addr=%02x: ",
           __func__, dev->params.i2c_dev, dev->params.i2c_addr);
    for (uint8_t i = 0; i < len; i++) {
        printf("%02x ", pdata[i]);
    }
    printf("\n");
#endif /* ENABLE_DEBUG */

    /* fill the first 2 bytes of the buffer with the reg */
    _buffer[0] = (reg >> 8) & 0xff;
    _buffer[1] = reg & 0xff;

    /* fill the buffer with data bytes */
    memcpy(_buffer + 2, pdata, len);

    int res = i2c_write_bytes(dev->params.i2c_dev,
                              dev->params.i2c_addr, _buffer, len + 2, 0);
    if (res) {
        DEBUG_DEV("I2C communication error: %d", dev, res);
        return -VL6180X_ERROR_I2C;
    }

    return VL6180X_OK;
}

inline static int _write_byte(const vl6180x_t *dev, uint16_t reg, uint8_t byte)
{
    return _write(dev, reg, &byte, 1);
}

__attribute__((unused))
inline static int _write_word(const vl6180x_t *dev, uint16_t reg, uint16_t data)
{
    uint8_t bytes[2] = { (data >> 8) & 0xff, data & 0xff };
    return _write(dev, reg, (uint8_t*)&bytes, 2);
}

static int _read(const vl6180x_t *dev,
                 uint16_t reg, uint8_t *pdata, uint8_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(pdata != NULL);
    ASSERT_PARAM(len <= VL6180X_BUFSIZ);
    DEBUG_DEV("reg=%04x pdata=%p len=%u", dev, reg, pdata, len);

    uint8_t bytes[2] = { (reg >> 8) & 0xff, reg & 0xff };

    /* write the register reg and if successful, read the data */
    if (i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, bytes, 2, 0) ||
        i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr, pdata, len, 0)) {
        DEBUG_DEV("I2C communication error", dev);
        return -VL6180X_ERROR_I2C;
    }

#if ENABLE_DEBUG
    printf("[vl6180x] %s i2c dev=%d addr=%02x: ",
           __func__, dev->params.i2c_dev, dev->params.i2c_addr);
    for (uint8_t i = 0; i < len; i++) {
        printf("%02x ", pdata[i]);
    }
    printf("\n");
#endif /* ENABLE_DEBUG */

    return VL6180X_OK;
}

__attribute__((unused))
inline static int _read_word(const vl6180x_t *dev, uint16_t reg, uint16_t *word)
{
    uint8_t bytes[2];
    int res = _read(dev, reg, bytes, 2);
    *word = (bytes[0] << 8) + (bytes[1]);
    return res;
}

__attribute__((unused))
inline static int _read_word_x(const vl6180x_t *dev, uint16_t reg, uint16_t *word)
{
    uint8_t bytes[2];
    int res = vl6180x_reg_read(dev, reg, bytes, 2);
    *word = (bytes[0] << 8) + (bytes[1]);
    return res;
}

inline static int _write_byte_x(const vl6180x_t *dev, uint16_t reg, uint8_t byte)
{
    return vl6180x_reg_write(dev, reg, &byte, 1);
}

__attribute__((unused))
inline static int _write_word_x(const vl6180x_t *dev, uint16_t reg, uint16_t data)
{
    uint8_t bytes[2] = { (data >> 8) & 0xff, data & 0xff };
    return vl6180x_reg_write(dev, reg, (uint8_t*)&bytes, 2);
}
