/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vl6180x
 * @brief       Device driver for the ST VL6180X Ranging and Ambient Light Sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "vl6180x_regs.h"
#include "vl6180x.h"

#include "irq.h"
#include "log.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    do { \
        if (!(cond)) { \
            DEBUG("[vl6180x] %s: %s\n", \
                  __func__, "parameter condition (" # cond ") not fulfilled"); \
            assert(cond); \
        } \
    } while(0)

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[vl6180x] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.i2c_dev, dev->params.i2c_addr, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond)
#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[vl6180x] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.i2c_dev, dev->params.i2c_addr, ## __VA_ARGS__)

#define EXEC_RET(f) \
    do { \
        int _r; \
        if ((_r = f) != VL6180X_OK) { \
            DEBUG("[vl6180x] %s: error code %d\n", __func__, _r); \
            return _r; \
        } \
    } while(0)

#define EXEC_RET_CODE(f, c) \
    do { \
        int _r; \
        if ((_r = f) != VL6180X_OK) { \
            DEBUG("[vl6180x] %s: error code %d\n", __func__, _r); \
            return c; \
        } \
    } while(0)

#define EXEC(f) \
    do { \
        int _r; \
        if ((_r = f) != VL6180X_OK) { \
            DEBUG("[vl6180x] %s: error code %d\n", __func__, _r); \
            return; \
        } \
    } while(0)

/** Forward declaration of functions for internal use */
static int _is_available(const vl6180x_t *dev);
static int _reset(const vl6180x_t *dev);
static int _init(vl6180x_t *dev);

static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit);
#ifndef MODULE_VL6180X_BASIC
static uint8_t _get_reg_bit(uint8_t byte, uint8_t mask);
#endif

static int _read(const vl6180x_t *dev, uint16_t index, uint8_t *data, uint32_t len);
static int _write(const vl6180x_t *dev, uint16_t index, const uint8_t *data, uint32_t len);
static int _update(const vl6180x_t *dev, uint16_t index, uint8_t mask, uint8_t data);

inline static int _read_word(const vl6180x_t *dev, uint16_t index, uint16_t *word);
inline static int _write_byte(const vl6180x_t *dev, uint16_t index, uint8_t byte);
inline static int _write_word(const vl6180x_t *dev, uint16_t index, uint16_t word);

int vl6180x_init(vl6180x_t *dev, const vl6180x_params_t *params)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);

    DEBUG_DEV("params=%p", dev, params);

    ASSERT_PARAM(params->rng_max_time > 0 && params->rng_max_time < 64);
    ASSERT_PARAM(params->als_int_time > 0 && params->als_int_time < 512);

    /* init sensor data structure */
    dev->params = *params;

#ifndef MODULE_VL6180X_BASIC
    dev->int_init = false;

    /* if shutdown pin is defined, it is initialized first and set */
    if (params->pin_shutdown != GPIO_UNDEF) {
        gpio_init(params->pin_shutdown, GPIO_OUT);
        gpio_write(params->pin_shutdown, 1);
    }

    dev->rng_status = VL6180X_RNG_OK;
    dev->als_status = VL6180X_ALS_OK;
#endif /* !MODULE_VL6180X_BASIC */

    /* init the sensor and start measurement */
    return _init(dev);
}

int vl6180x_rng_data_ready (const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t byte;
    EXEC_RET(_read(dev, VL6180X_REG_INT_STATUS, &byte, 1));

    return ((byte & VL6180X_RNG_INT) != 0) ? VL6180X_OK
                                           : -VL6180X_ERROR_NO_DATA;
}

int vl6180x_rng_read (vl6180x_t *dev, uint8_t *mm)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(mm != NULL);
    DEBUG_DEV("mm=%p", dev, mm);

    /* test for new data */
    EXEC_RET(vl6180x_rng_data_ready(dev));

    /* read range results */
    EXEC_RET(_read(dev, VL6180X_REG_RNG_VALUE, mm, 1));

    /* read range measurement status */
    uint8_t status;
    EXEC_RET(_read(dev, VL6180X_REG_RNG_STATUS, &status, 1));

    /* clear range interrupt flag */
    EXEC_RET(_write_byte(dev, VL6180X_REG_INT_CLR, VL6180X_CLR_RNG_INT));

#ifndef MODULE_VL6180X_BASIC
    dev->rng_status = _get_reg_bit(status, VL6180X_RNG_ERR_CODE);
#endif

    return (status & VL6180X_RNG_ERR_CODE) ? VL6180X_ERROR_RNG : VL6180X_OK;
}

#ifndef MODULE_VL6180X_BASIC
vl6180x_rng_status_t vl6180x_rng_status(vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);
    return dev->rng_status;
}

int vl6180x_rng_start (vl6180x_t *dev)
{
    uint8_t status;

    EXEC_RET(_read(dev, VL6180X_REG_RNG_STATUS, &status, 1));
    if ((status & VL6180X_RNG_DEVICE_RDY) == 1) {
        /* start measurement only when device is ready to start*/
        EXEC_RET(_update(dev, VL6180X_REG_RNG_START, VL6180X_RNG_START_STOP, 1));
    }
    return VL6180X_OK;
}

int vl6180x_rng_stop (vl6180x_t *dev)
{
    uint8_t status;

    EXEC_RET(_read(dev, VL6180X_REG_RNG_STATUS, &status, 1));
    if ((status & VL6180X_RNG_DEVICE_RDY) == 0) {
        /* stop only when device is not ready, otherwise it is already stopped */
        EXEC_RET(_update(dev, VL6180X_REG_RNG_START, VL6180X_RNG_START_STOP, 1));
    }
    /* wait that the device becomes ready */
    do {
        EXEC_RET(_read(dev, VL6180X_REG_RNG_STATUS, &status, 1));
    } while ((status & VL6180X_RNG_DEVICE_RDY) == 0);

    return VL6180X_OK;
}

int vl6180x_rng_config(vl6180x_t *dev, uint8_t period, uint8_t max_time)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(max_time < 64);

    DEBUG_DEV("period=%u max_time=%u", dev, period, max_time);

    /* stop the device if started */
    EXEC_RET(vl6180x_rng_stop(dev));

    /* write new ranging configuration */
    dev->params.rng_period = period;
    dev->params.rng_max_time = max_time;
    period -= 1; /* register value is decremented by 1 */
    EXEC_RET(_write(dev, VL6180X_REG_RNG_PERIOD, &period, 1));
    EXEC_RET(_write(dev, VL6180X_REG_RNG_MAX_TIME, &max_time, 1));

    /* restart range measurements if continuous mode is configured */
    if (dev->params.rng_period != 0) {
        EXEC_RET(vl6180x_rng_start(dev));
    }
    return VL6180X_OK;
}

#endif

int vl6180x_als_data_ready (const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t byte;
    EXEC_RET(_read(dev, VL6180X_REG_INT_STATUS, &byte, 1));

    return ((byte & VL6180X_ALS_INT) != 0) ? VL6180X_OK
                                           : -VL6180X_ERROR_NO_DATA;
}

static const double _als_gains[] = {
    20.0, 100.0, 5.0, 2.5, 1.67, 1.25, 1.0, 40.0
};

int vl6180x_als_read (vl6180x_t *dev, uint16_t *raw, uint16_t *lux)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL || lux != NULL);
    DEBUG_DEV("raw=%p lux=%p", dev, raw, lux);

    /* test for ALS new data */
    EXEC_RET(vl6180x_als_data_ready(dev));

    /* read raw ALS results */
    uint16_t cnt;
    EXEC_RET(_read_word(dev, VL6180X_REG_ALS_VALUE, &cnt));

    /* read range measurement status */
    uint8_t status;
    EXEC_RET(_read(dev, VL6180X_REG_ALS_STATUS, &status, 1));

#ifndef MODULE_VL6180X_BASIC
    dev->als_status = _get_reg_bit(status, VL6180X_ALS_ERR_CODE);
#endif

    if ((status & VL6180X_ALS_ERR_CODE) == 0) {
        if (raw) {
            *raw = cnt;
        }
        if (lux) {
            /* calculation from datasheet with lux resolution is 0.32/count */
            float luxf = 0.32 * (float)cnt / _als_gains[dev->params.als_gain]
                              * 100.0 / dev->params.als_int_time;
            *lux = round(luxf);
        }
    }

    /* clear ALS interrupt flag */
    EXEC_RET(_write_byte(dev, VL6180X_REG_INT_CLR, VL6180X_CLR_ALS_INT));

    return (status & VL6180X_ALS_ERR_CODE) ? VL6180X_ERROR_ALS : VL6180X_OK;
}

#ifndef MODULE_VL6180X_BASIC
vl6180x_als_status_t vl6180x_als_status(vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);
    return dev->als_status;
}

int vl6180x_als_start (vl6180x_t *dev)
{
    uint8_t status;

    EXEC_RET(_read(dev, VL6180X_REG_ALS_STATUS, &status, 1));
    if ((status & VL6180X_ALS_DEVICE_RDY) == 1) {
        /* start measurement only when device is ready to start*/
        EXEC_RET(_update(dev, VL6180X_REG_ALS_START, VL6180X_ALS_START_STOP, 1));
    }
    return VL6180X_OK;
}

int vl6180x_als_stop (vl6180x_t *dev)
{
    uint8_t status;

    EXEC_RET(_read(dev, VL6180X_REG_ALS_STATUS, &status, 1));
    if ((status & VL6180X_ALS_DEVICE_RDY) == 0) {
        /* stop only when device is not ready, otherwise it is already stopped */
        EXEC_RET(_update(dev, VL6180X_REG_ALS_START, VL6180X_ALS_START_STOP, 1));
    }
    /* wait that the device becomes ready */
    do {
        EXEC_RET(_read(dev, VL6180X_REG_ALS_STATUS, &status, 1));
    } while ((status & VL6180X_ALS_DEVICE_RDY) == 0);

    return VL6180X_OK;
}

int vl6180x_als_config(vl6180x_t *dev, uint8_t period, uint8_t int_time,
                                       vl6180x_als_gain_t gain)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);

    DEBUG_DEV("period=%u int_time=%u gain=%u", dev, period, int_time, gain);

    /* stop the device if started */
    EXEC_RET(vl6180x_als_stop(dev));

    /* write new ambient light sensing (ALS) configuration */
    dev->params.als_period = period;
    dev->params.als_int_time = int_time;
    dev->params.als_gain = gain;
    period -= 1; /* register value is decremented by 1 */
    int_time -= 1; /* register value is decremented by 1 */
    EXEC_RET(_write(dev, VL6180X_REG_ALS_PERIOD, &period, 1));
    EXEC_RET(_write_word(dev, VL6180X_REG_ALS_INT_TIME, int_time));
    EXEC_RET(_update(dev, VL6180X_REG_ALS_GAIN, VL6180X_ALS_GAIN_LIGHT, gain));

    /* restart ambient light sensing (ALS) if continuous mode is configured */
    if (dev->params.als_period != 0) {
        EXEC_RET(vl6180x_als_start(dev));
    }
    return VL6180X_OK;
}
#endif /* !MODULE_VL6180X_BASIC */

#ifndef MODULE_VL6180X_BASIC
int vl6180x_power_down(const vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->params.pin_shutdown == GPIO_UNDEF) {
        DEBUG_DEV("Pin connected to sensor's GPIO0/CE pin not defined", dev);
        return -VL6180X_ERROR_NO_PIN;
    }

    /* assert shutdown pin */
    gpio_clear(dev->params.pin_shutdown);
    return VL6180X_OK;
}

int vl6180x_power_up(vl6180x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->params.pin_shutdown == GPIO_UNDEF) {
        DEBUG_DEV("Pin connected to sensor's GPIO0/CE pin not defined", dev);
        return -VL6180X_ERROR_NO_PIN;
    }

    /* deassert shutdown pin and wait 400 us */
    gpio_set(dev->params.pin_shutdown);
    xtimer_usleep(400);

    /* init the sensor and start measurement */
    _init(dev);

    return VL6180X_OK;
}

int vl6180x_int_config(vl6180x_t *dev, vl6180x_int_config_t *cfg,
                                       void (*isr)(void *),
                                       void *isr_arg)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(dev->params.pin_int != GPIO_UNDEF);

    DEBUG_DEV("cfg=%p isr=%p isr_arg=%p", dev, cfg, isr, isr_arg);

    if (!dev->int_init) {
        ASSERT_PARAM(isr != NULL);
        dev->int_init = true;
        gpio_init_int(dev->params.pin_int, GPIO_IN_PU, GPIO_FALLING,
                      isr, isr_arg);
    }

    uint8_t byte = 0;
    if (cfg) {
        _set_reg_bit(&byte, VL6180X_ALS_INT_MODE, cfg->als_int_mode);
        _set_reg_bit(&byte, VL6180X_RNG_INT_MODE, cfg->rng_int_mode);
        EXEC_RET(_write(dev, VL6180X_REG_INT_CONFIG, &byte, 1));
        EXEC_RET(_write(dev, VL6180X_REG_RNG_THRESH_HI, &cfg->rng_thresh_high, 1));
        EXEC_RET(_write(dev, VL6180X_REG_RNG_THRESH_LO, &cfg->rng_thresh_low, 1));
        EXEC_RET(_write_word(dev, VL6180X_REG_ALS_THRESH_HI, cfg->als_thresh_high));
        EXEC_RET(_write_word(dev, VL6180X_REG_ALS_THRESH_LO, cfg->als_thresh_low));
    }
    else {
        _set_reg_bit(&byte, VL6180X_ALS_INT_MODE, VL6180X_DATA_RDY);
        _set_reg_bit(&byte, VL6180X_RNG_INT_MODE, VL6180X_DATA_RDY);
        EXEC_RET(_write(dev, VL6180X_REG_INT_CONFIG, &byte, 1));
    }

    if (cfg == NULL || cfg->rng_int_mode != VL6180X_INT_DISABLED
                    || cfg->als_int_mode != VL6180X_INT_DISABLED) {
        /* enable low active interrupt pin */
        EXEC_RET(_write_byte(dev, VL6180X_REG_GPIO1_MODE,
                                  VL6180X_GPIO1_POL_LOW | VL6180X_GPIO1_FUNC_INT));
    }
    else {
        /* disable low active interrupt pin */
        EXEC_RET(_write_byte(dev, VL6180X_REG_GPIO1_MODE,
                                  VL6180X_GPIO1_POL_LOW | VL6180X_GPIO1_FUNC_OFF));
    }

    return VL6180X_OK;
}

int vl6180x_int_source(vl6180x_t *dev, vl6180x_int_source_t *src)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(src != NULL);

    DEBUG_DEV("src=%p", dev, src);

    uint8_t byte;

    EXEC_RET(_read(dev, VL6180X_REG_INT_STATUS, &byte, 1));
    src->als_int = _get_reg_bit(byte, VL6180X_ALS_INT);
    src->rng_int = _get_reg_bit(byte, VL6180X_RNG_INT);

    return VL6180X_OK;
}

#endif /* !MODULE_VL6180X_BASIC */

/** Functions for internal use only */

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(const vl6180x_t *dev)
{
    DEBUG_DEV("", dev);

    uint8_t data[5];

    /* read the chip id from VL6180X_REG_ID_X */
    EXEC_RET(_read(dev, VL6180X_REG_MODEL_ID, data, 5));

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
    /* wait for 1 ms after power on */
    xtimer_usleep(US_PER_MS);

    /* check availability of the sensor */
    EXEC_RET(_is_available(dev));

    /* reset the device to a basic recommended configuration */
    EXEC_RET(_reset(dev));

#ifndef MODULE_VL6180X_BASIC
    if (dev->params.pin_shutdown != GPIO_UNDEF) {
        EXEC_RET(_write_byte(dev, VL6180X_REG_GPIO0_MODE,
                                  VL6180X_GPIO0_POL_HIGH | VL6180X_GPIO0_SHUT_ON));
    }
#endif

    /* clear all interrupt flag */
    EXEC_RET(_write_byte(dev, VL6180X_REG_INT_CLR, VL6180X_CLR_ALL_INT));

    /* set ranging measurement mode and inter-measurement period */
    uint8_t byte = 0;
    _set_reg_bit(&byte, VL6180X_RNG_START_STOP, 0);
    _set_reg_bit(&byte, VL6180X_RNG_MODE, dev->params.rng_period != 0);
    EXEC_RET(_write(dev, VL6180X_REG_RNG_START, &byte, 1));
    EXEC_RET(_write_byte(dev, VL6180X_REG_RNG_PERIOD, dev->params.rng_period-1));
    EXEC_RET(_write(dev, VL6180X_REG_RNG_MAX_TIME, &dev->params.rng_max_time, 1));
    EXEC_RET(_update(dev, VL6180X_REG_INT_CONFIG,
                          VL6180X_RNG_INT_MODE, VL6180X_DATA_RDY));

    /* set ALS measurement mode and inter-measurement period */
    byte = 0;
    _set_reg_bit(&byte, VL6180X_ALS_START_STOP, 0);
    _set_reg_bit(&byte, VL6180X_ALS_MODE, dev->params.als_period != 0);
    EXEC_RET(_write(dev, VL6180X_REG_ALS_START, &byte, 1));
    EXEC_RET(_write_byte(dev, VL6180X_REG_ALS_PERIOD, dev->params.als_period-1));
    EXEC_RET(_write_word(dev, VL6180X_REG_ALS_INT_TIME, dev->params.als_int_time-1));
    EXEC_RET(_update(dev, VL6180X_REG_ALS_GAIN,
                          VL6180X_ALS_GAIN_LIGHT, dev->params.als_gain));
    EXEC_RET(_update(dev, VL6180X_REG_INT_CONFIG,
                          VL6180X_ALS_INT_MODE, VL6180X_DATA_RDY));

    /* start range measurements if continuous mode is configured */
    if (dev->params.rng_period != 0) {
        EXEC_RET(_update(dev, VL6180X_REG_RNG_START, VL6180X_RNG_START_STOP, 1));
    }

    /* start als measurements if continuous mode is configured */
    if (dev->params.als_period != 0) {
        EXEC_RET(_update(dev, VL6180X_REG_ALS_START, VL6180X_ALS_START_STOP, 1));
    }

    return VL6180X_OK;
}

typedef struct {
    uint16_t index;
    uint8_t value;
} _vl6180x_index_value_t;

/*
 * Private registers and standard register initialization values that have to
 * be set after power on reset, see [Application Note AN4545, section 9]
 * (https://www.st.com/resource/en/application_note/dm00122600.pdf)
 */
static const _vl6180x_index_value_t _init_values [] = {
    /* Mandatory : private registers */
    { .index = 0x0207, .value = 0x01 },
    { .index = 0x0207, .value = 0x01 },
    { .index = 0x0096, .value = 0x00 },
    { .index = 0x0097, .value = 0xfd },
    { .index = 0x00e3, .value = 0x01 },
    { .index = 0x00e4, .value = 0x03 },
    { .index = 0x00e5, .value = 0x02 },
    { .index = 0x00e6, .value = 0x01 },
    { .index = 0x00e7, .value = 0x03 },
    { .index = 0x00f5, .value = 0x02 },
    { .index = 0x00d9, .value = 0x05 },
    { .index = 0x00db, .value = 0xce },
    { .index = 0x00dc, .value = 0x03 },
    { .index = 0x00dd, .value = 0xf8 },
    { .index = 0x009f, .value = 0x00 },
    { .index = 0x00a3, .value = 0x3c },
    { .index = 0x00b7, .value = 0x00 },
    { .index = 0x00bb, .value = 0x3c },
    { .index = 0x00b2, .value = 0x09 },
    { .index = 0x00ca, .value = 0x09 },
    { .index = 0x0198, .value = 0x01 },
    { .index = 0x01b0, .value = 0x17 },
    { .index = 0x01ad, .value = 0x00 },
    { .index = 0x00ff, .value = 0x05 },
    { .index = 0x0100, .value = 0x05 },
    { .index = 0x0199, .value = 0x05 },
    { .index = 0x01a6, .value = 0x1b },
    { .index = 0x01ac, .value = 0x3e },
    { .index = 0x01a7, .value = 0x1f },
    { .index = 0x0030, .value = 0x00 },

    /* Recommended : Public registers - See data sheet for more detail */
    { .index = 0x0011, .value = 0x00 }, /* GPIO1 is low active, OFF (Hi-Z) */
    { .index = 0x0010, .value = 0x20 }, /* GPIO0 is high active, OFF (Hi-Z) */
    { .index = 0x010a, .value = 0x30 }, /* Range averaging period is 4.3 ms*/
    { .index = 0x003f, .value = 0x46 }, /* Light gain is 1.0, dark gain ? */
    { .index = 0x0031, .value = 0xff }, /* Calibration every 255 measurements */
    { .index = 0x0041, .value = 0x63 }, /* ALS integration time is 100 ms */
    { .index = 0x002e, .value = 0x01 }, /* Perform temperature calibration */

    /* Optional: Public registers - See data sheet for more detail */
    { .index = 0x001b, .value = 0x09 }, /* Range measurement period is 100 ms */
    { .index = 0x003e, .value = 0x31 }, /* ALS measurement period is 500 ms */
    { .index = 0x0014, .value = 0x00 }, /* No threshold interrupts */

    /* change fresh out of status to 0 */
    { .index = 0x016, .value = 0x00 },
};

static int _reset(const vl6180x_t *dev)
{
    uint8_t byte;

    /* read the fresh out from reset status */
    EXEC_RET(_read(dev, VL6180X_REG_FRESH_RST, &byte, 1));

    /* if not fresh out from reset, stop measurements if necessary */
    if ((byte & 0x01) == 0) {
        uint8_t status;

        EXEC_RET(_read(dev, VL6180X_REG_RNG_STATUS, &status, 1));
        if ((status & VL6180X_RNG_DEVICE_RDY) == 0) {
            /* stop measurement when device is not ready */
            EXEC_RET(_write_byte(dev, VL6180X_REG_RNG_START,
                                      VL6180X_RNG_START_STOP));
        }
        EXEC_RET(_read(dev, VL6180X_REG_ALS_STATUS, &status, 1));
        if ((status & VL6180X_ALS_DEVICE_RDY) == 0) {
            /* stop measurement when device is not ready */
            EXEC_RET(_write_byte(dev, VL6180X_REG_ALS_START,
                                      VL6180X_ALS_START_STOP));
        }
    }

    /* load inital register values */
    for (unsigned i = 0; i < sizeof(_init_values) / sizeof(_init_values[0]); i++) {
        EXEC_RET(_write(dev, _init_values[i].index,
                             &_init_values[i].value, 1));
    }

    return VL6180X_OK;
}

static int _read(const vl6180x_t *dev, uint16_t index, uint8_t *pdata, uint32_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(pdata != NULL);
    ASSERT_PARAM(len <= 32);
    DEBUG_DEV("index=%04x pdata=%p len=%u", dev, index, pdata, len);

    /* acquire the I2C device */
    if (i2c_acquire(dev->params.i2c_dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -VL6180X_ERROR_I2C;
    }

    uint8_t bytes[2] = { (index >> 8) & 0xff, index & 0xff };

    /* write the register index and if successful, read the data */
    if (i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, bytes, 2, 0) ||
        i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr, pdata, len, 0)) {
        i2c_release(dev->params.i2c_dev);
        return -VL6180X_ERROR_I2C;
    }

    /* release the I2C device */
    i2c_release(dev->params.i2c_dev);

    return VL6180X_OK;
}

inline static int _read_word(const vl6180x_t *dev, uint16_t index, uint16_t *word)
{
    uint8_t bytes[2];
    EXEC_RET(_read (dev, index, bytes, 2));
    *word = (bytes[0] << 8) + (bytes[1]);

    return VL6180X_OK;
}

inline static int _write_byte(const vl6180x_t *dev, uint16_t index, uint8_t byte)
{
    return _write (dev, index, &byte, 1);
}

#define VL6180X_BUFSIZ   32
static uint8_t _buffer[VL6180X_BUFSIZ] = {};

static int _write(const vl6180x_t *dev, uint16_t index, const uint8_t *pdata, uint32_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(pdata != NULL);
    ASSERT_PARAM(len <= 30);
    DEBUG_DEV("index=%04x pdata=%p len=%u", dev, index, pdata, len);

    if (i2c_acquire(dev->params.i2c_dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -VL6180X_ERROR_I2C;
    }

    /* fill the first 2 bytes of the buffer with the index */
    _buffer[0] = (index >> 8) & 0xff;
    _buffer[1] = index & 0xff;

    /* fill the buffer with data bytes */
    memcpy(_buffer + 2, pdata, len);

    if (i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, _buffer, len + 2, 0)) {
        i2c_release(dev->params.i2c_dev);
        return -VL6180X_ERROR_I2C;
    }

    /* release the I2C device */
    i2c_release(dev->params.i2c_dev);

    return VL6180X_OK;
}

inline static int _write_word(const vl6180x_t *dev, uint16_t index, uint16_t data)
{
    uint8_t bytes[2] = { (data >> 8) & 0xff, data & 0xff };
    return _write (dev, index, (uint8_t*)&bytes, 2);
}

static int _update(const vl6180x_t *dev, uint16_t index, uint8_t mask,
                                                         uint8_t data)
{
    uint8_t byte;
    uint8_t shift = 0;

    while (!((mask >> shift) & 0x01)) {
        shift++;
    }

    /* read current register value */
    EXEC_RET(_read(dev, index, &byte, 1));

    /* set masked bits to the given value  */
    byte = (byte & ~mask) | ((data << shift) & mask);

    /* write back new register value */
    EXEC_RET(_write(dev, index, &byte, 1));

    return VL6180X_OK;
}

static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit)
{
    ASSERT_PARAM(byte != NULL);

    uint8_t shift = 0;
    while (!((mask >> shift) & 0x01)) {
        shift++;
    }
    *byte = ((*byte & ~mask) | ((bit << shift) & mask));
}

#ifndef MODULE_VL6180X_BASIC
static uint8_t _get_reg_bit(uint8_t byte, uint8_t mask)
{
    uint8_t shift = 0;
    while (!((mask >> shift) & 0x01)) {
        shift++;
    }
    return (byte & mask) >> shift;
}
#endif /* MODULE_VL6180X_BASIC */
