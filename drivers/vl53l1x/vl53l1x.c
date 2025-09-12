/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_vl53l1x
 * @brief       Device driver for the ST VL53L1X Time-of-Flight (ToF) ranging sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "vl53l1x_regs.h"
#include "vl53l1x.h"

#include "VL53L1X_api.h"

#include "irq.h"
#include "log.h"
#include "time_units.h"
#include "ztimer.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#if IS_ACTIVE(ENABLE_DEBUG)

#  define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[vl53l1x] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#  define DEBUG_DEV(f, d, ...) \
        DEBUG("[vl53l1x] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params->i2c_dev, d->params->i2c_addr, ## __VA_ARGS__);

#else /* IS_ACTIVE(ENABLE_DEBUG) */

#  define ASSERT_PARAM(cond) assert(cond);
#  define DEBUG_DEV(f, d, ...)

#endif /* IS_ACTIVE(ENABLE_DEBUG) */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[vl53l1x] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params->i2c_dev, d->params->i2c_addr, ## __VA_ARGS__);

#define EXEC_RET(f) { \
    int _r; \
    if ((_r = f) != 0) { \
        DEBUG("[vl53l1x] %s: error code %d\n", __func__, _r); \
        return _r; \
    } \
}

#define EXEC_RET_CODE(f, c) { \
    int _r; \
    if ((_r = f) != 0) { \
        DEBUG("[vl53l1x] %s: error code %d\n", __func__, _r); \
        return c; \
    } \
}

#define EXEC(f) { \
    int _r; \
    if ((_r = f) != 0) { \
        DEBUG("[vl53l1x] %s: error code %d\n", __func__, _r); \
        return; \
    } \
}

#define VL53L1X_BOOT_TIMEOUT_MS   10      /* 10 ms */

/** Forward declaration of functions for internal use */
static int _is_available(const vl53l1x_t *dev);
static int _reset(vl53l1x_t *dev);
static int _init(vl53l1x_t *dev);
//static int _update(const vl53l1x_t *dev, uint16_t index, uint8_t mask, uint8_t data);

#define _read_byte(d, i, b)   VL53L1_RdByte(d->addr, i, b)
#define _read_word(d, i, w)   VL53L1_RdWord(d->addr, i, w)
#define _read(d, i, p, l)     VL53L1_ReadMulti(d->addr, i, p, l)

#define _write_byte(d, i, b)  VL53L1_WrByte(d->addr, i, b)
#define _write_word(d, i, w)  VL53L1_WrWord(d->addr, i, w)
#define _write(d, i, p, l)    VL53L1_WriteMulti(d->addr, i, p, l)

#define _start_meas(d)        VL53L1X_StartRanging(d->addr)

int vl53l1x_init(vl53l1x_t *dev, const vl53l1x_params_t *params)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);

#if !IS_USED(MODULE_VL53L1X_BASIC)
    ASSERT_PARAM(params->budget <= 500);
    ASSERT_PARAM(params->period >= (uint32_t)params->budget + 4);

    dev->budget = params->budget;
    dev->period = params->period;
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->params = params;
    dev->int_init = false;

    dev->addr = dev->params->i2c_dev << 8 | dev->params->i2c_addr;

#if !IS_USED(MODULE_VL53L1X_BASIC)
    /* if shutdown pin is defined, it is initialized first and set */
    if (params->pin_shutdown != GPIO_UNDEF) {
        gpio_init(params->pin_shutdown, GPIO_OUT);
        gpio_write(params->pin_shutdown, 1);
    }
#endif

    /* init the sensor and start measurement */
    EXEC_RET(_init(dev));

    return 0;
}

int vl53l1x_data_ready(vl53l1x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t ready;
    EXEC_RET(VL53L1X_CheckForDataReady(dev->addr, &ready));
    return (ready) ? 0 : -EAGAIN;
}

#if !IS_USED(MODULE_VL53L1X_BASIC)
int vl53l1x_read_data(vl53l1x_t *dev, vl53l1x_data_t *data)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("data=%p", dev, data);

    EXEC_RET(VL53L1X_GetRangeStatus(dev->addr, &data->status));
    EXEC_RET(VL53L1X_GetDistance(dev->addr, &data->distance));
    EXEC_RET(VL53L1X_GetSignalRate(dev->addr, &data->signal_rate));
    EXEC_RET(VL53L1X_GetAmbientRate(dev->addr, &data->ambient_rate));
    EXEC_RET(VL53L1X_ClearInterrupt(dev->addr));

    return 0;
}
#endif

int vl53l1x_read_mm(vl53l1x_t *dev, uint16_t *mm)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(mm != NULL);
    DEBUG_DEV("mm=%p", dev, mm);

    uint8_t status;
    EXEC_RET(VL53L1X_GetRangeStatus(dev->addr, &status));
    EXEC_RET(VL53L1X_GetDistance(dev->addr, mm));
    EXEC_RET(VL53L1X_ClearInterrupt(dev->addr));

    return 0;
}

int vl53l1x_int_config(vl53l1x_t *dev, vl53l1x_int_mode_t mode,
                                       vl53l1x_thresh_config_t* cfg,
                                       void (*isr)(void *),
                                       void *isr_arg)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(dev->params->pin_int != GPIO_UNDEF);

    DEBUG_DEV("cfg=%p isr=%p isr_arg=%p", dev, cfg, isr, isr_arg);

    if (!dev->int_init) {
        /* set polarity to LOW active */
        EXEC_RET(VL53L1X_SetInterruptPolarity(dev->addr, 0));

        dev->int_init = true;
        gpio_init_int(dev->params->pin_int, GPIO_IN_PU, GPIO_FALLING,
                      isr, isr_arg);
    }

    if (IS_USED(MODULE_VL53L1X_BASIC) || (mode == VL53L1X_INT_DATA_READY)) {
        /* User manual UM2510, section: write 0x20 to the 0x46 register (8 bits) */
        EXEC_RET(_write_byte(dev, SYSTEM__INTERRUPT_CONFIG_GPIO, 0x20));
    }
    else {
        ASSERT_PARAM(cfg);
        EXEC_RET(VL53L1X_StopRanging(dev->addr));
        EXEC_RET(VL53L1X_SetDistanceThreshold(dev->addr,
                                              cfg->low, cfg->high,
                                              cfg->mode, 0));
        EXEC_RET(VL53L1X_StartRanging(dev->addr));
    }

    EXEC_RET(VL53L1X_ClearInterrupt(dev->addr));

    return 0;
}

#if !IS_USED(MODULE_VL53L1X_BASIC)

int vl53l1x_power_down(const vl53l1x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->params->pin_shutdown == GPIO_UNDEF) {
        DEBUG_DEV("Pin connected to sensor's XSHUT pin not defined", dev);
        return -ENODEV;
    }

    gpio_clear(dev->params->pin_shutdown);
    return 0;
}

int vl53l1x_power_up(vl53l1x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->params->pin_shutdown == GPIO_UNDEF) {
        DEBUG_DEV("Pin connected to sensor's XSHUT pin not defined", dev);
        return -ENODEV;
    }

    gpio_set(dev->params->pin_shutdown);

    /* init the sensor and start measurement */
    EXEC_RET(_init(dev));

    return 0;
}

int vl53l1x_set_timing_budget(vl53l1x_t *dev, uint16_t budget_ms)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("budget_ms=%" PRIu16, dev, budget_ms);

    /* VL53L1X_SetTimingBudgetInMs sets a hard error code of 1 */
    int8_t ret = VL53L1X_SetTimingBudgetInMs(dev->addr, budget_ms);
    if (ret == 1) {
        /* current distance mode or the given parameter budget_ms is invalid */
        return -EINVAL;
    }
    else if (ret) {
        return ret;
    }

    /* save the timing budget */
    dev->budget = budget_ms;

    return 0;
}

int vl53l1x_set_measurement_period(vl53l1x_t *dev, uint32_t period_ms)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM((period_ms + 4) > dev->budget);
    DEBUG_DEV("period_ms=%" PRIu32, dev, period_ms);

    EXEC_RET(VL53L1X_SetInterMeasurementInMs(dev->addr, period_ms));

    /* save the measurement period */
    dev->period = period_ms;

    return 0;
}

int vl53l1x_set_distance_mode(vl53l1x_t *dev, vl53l1x_dist_mode_t mode)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("mode=%u", dev, mode);

    /* VL53L1X_SetDistanceMode sets a hard error code of 1 */
    int8_t ret = VL53L1X_SetDistanceMode(dev->addr, mode);
    if (ret == 1) {
        /* the given mode parameter or the current timing budget is invalid */
        return -EINVAL;
    }
    else if (ret) {
        return ret;
    }

    /* save the distance mode */
    dev->mode = mode;

    return 0;
}

int vl53l1x_set_roi(vl53l1x_t *dev, vl53l1x_roi_t *roi)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(roi != NULL);

    ASSERT_PARAM(roi->x_size >= 4);
    ASSERT_PARAM(roi->x_size <= 15);
    ASSERT_PARAM(roi->y_size >= 4);
    ASSERT_PARAM(roi->y_size <= 15);

    DEBUG_DEV("roi=%p", dev, roi);

    EXEC_RET(VL53L1X_SetROI(dev->addr, roi->x_size, roi->y_size));

    return 0;
}

int vl53l1x_get_roi(vl53l1x_t *dev, vl53l1x_roi_t *roi)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(roi != NULL);

    DEBUG_DEV("roi=%p", dev, roi);

    EXEC_RET(VL53L1X_GetROI_XY(dev->addr, &roi->x_size, &roi->y_size));

    return 0;
}

#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

/** Functions for internal use only */

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(const vl53l1x_t *dev)
{
    DEBUG_DEV("", dev);

    uint16_t id;
    /* read the chip id */
    EXEC_RET(VL53L1X_GetSensorId(dev->addr, &id));

    if (id != VL53L1X_DEVICE_ID) {
        DEBUG_DEV("sensor is not available, wrong device id %02x, "
                  "should be %02x", dev, id, VL53L1X_DEVICE_ID);
        return -ENODEV;
    }

#if IS_ACTIVE(ENABLE_DEBUG)
    uint8_t rev;
    uint8_t mod_type;
    uint16_t mod_id = 0;

    EXEC_RET(_read_byte(dev, VL53L1X_IDENTIFICATION__REVISION_ID, &rev));
    EXEC_RET(_read_byte(dev, VL53L1X_IDENTIFICATION__MODULE_TYPE, &mod_type));
    EXEC_RET(_read_word(dev, VL53L1X_IDENTIFICATION__MODULE_ID, &mod_id));

    DEBUG_DEV("rev=%02x, module type=%02x id=%04x", dev, rev, mod_type, mod_id);
#endif

    return 0;
}

static int _init(vl53l1x_t *dev)
{
    /* wait for 6 ms after power on reset */
    ztimer_sleep(ZTIMER_MSEC, 6);

    /* check availability of the sensor */
    EXEC_RET(_is_available(dev));

    /* reset, wait for 4 ms, and wait that devices has been booted */
    EXEC_RET(_reset(dev));

    EXEC_RET(VL53L1X_SensorInit(dev->addr));

#if !IS_USED(MODULE_VL53L1X_BASIC)
    /* set measurement parameters */
    EXEC_RET(vl53l1x_set_distance_mode(dev, dev->params->mode));
    EXEC_RET(vl53l1x_set_timing_budget(dev, dev->params->budget));
    EXEC_RET(vl53l1x_set_measurement_period(dev, dev->params->period));
#endif /* IS_USED(MODULE_VL53L1X_BASIC) */

    /* start measurement */
    EXEC_RET(_start_meas(dev));

    return 0;
}

static int _reset(vl53l1x_t *dev)
{
    /* write reset impuls of at least 100 us, we use 1 ms */
    _write_byte(dev, VL53L1X_SOFT_RESET, 0);
    ztimer_sleep(ZTIMER_MSEC, 1);
    _write_byte(dev, VL53L1X_SOFT_RESET, 1);

    /* wait until the sensor has been booted in 1 ms steps*/
    unsigned timeout = VL53L1X_BOOT_TIMEOUT_MS;
    uint8_t state;
    while (timeout) {
        timeout--;
        EXEC_RET(VL53L1X_BootState(dev->addr, &state));
        if (state) {
            break;
        }
        ztimer_sleep(ZTIMER_MSEC, 1);
    }
    if (timeout == 0) {
        DEBUG_DEV("could not boot the sensor", dev);
        return EBUSY;
    }
#if 0
    /* set VDDIO */
    EXEC_RET(_update(dev, VL53L1X_PAD_I2C_HV__EXTSUP_CONFIG,
                          0xfe, dev->params->vddio_2v8 ? 1 : 0));
#endif

    return 0;
}
#if 0
static int _update(const vl53l1x_t *dev, uint16_t index, uint8_t mask,
                                                         uint8_t data)
{
    uint8_t byte;
    EXEC_RET(_read_byte(dev, index, &byte));
    byte &= mask;
    byte |= data;
    EXEC_RET(_write_byte(dev, index, byte));

    return 0;
}
#endif
