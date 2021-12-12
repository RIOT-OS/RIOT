/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @note: Parts of the code are based on
 * [Pololu VL53L1X library for Arduino](https://github.com/pololu/vl53l1x-arduino)
 * These parts are marked accordingly and are subject to the copyright below
 * [https://raw.githubusercontent.com/pololu/vl53l1x-arduino/master/LICENSE.txt]
 * (https://raw.githubusercontent.com/pololu/vl53l1x-arduino/master/LICENSE.txt).
 *
 * **************************************************************************
 *
 * Most of the functionality of this library is based on the VL53L1X API
 * provided by ST (STSW-IMG007), and some of the explanatory comments are
 * quoted paraphrased from the API source code, API user manual (UM2356), and
 * VL53L1X datasheet. Therefore, the license terms for the API source code
 * (BSD 3-clause "New" or "Revised" License) also apply to this derivative
 * work, as specified below.
 *
 * For more information, see
 *
 * https://www.pololu.com/
 * https://forum.pololu.com/
 *
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) 2017, STMicroelectronics
 * Copyright (c) 2018, Pololu Corporation
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @ingroup     drivers_vl53l1x
 * @brief       Device driver for the ST VL53L1X Time-of-Flight (ToF) ranging sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <string.h>
#include <stdlib.h>

#include "vl53l1x_regs.h"
#include "vl53l1x.h"

#include "irq.h"
#include "log.h"
#include "ztimer.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#if IS_ACTIVE(ENABLE_DEBUG)

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[vl53l1x] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[vl53l1x] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.i2c_dev, VL53L1X_I2C_ADDRESS, ## __VA_ARGS__);

#else /* IS_ACTIVE(ENABLE_DEBUG) */

#define ASSERT_PARAM(cond) assert(cond);
#define DEBUG_DEV(f, d, ...)

#endif /* IS_ACTIVE(ENABLE_DEBUG) */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[vl53l1x] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.i2c_dev, VL53L1X_I2C_ADDRESS, ## __VA_ARGS__);

#define EXEC_RET(f) { \
    int _r; \
    if ((_r = f) != VL53L1X_OK) { \
        DEBUG("[vl53l1x] %s: error code %d\n", __func__, _r); \
        return _r; \
    } \
}

#define EXEC_RET_CODE(f, c) { \
    int _r; \
    if ((_r = f) != VL53L1X_OK) { \
        DEBUG("[vl53l1x] %s: error code %d\n", __func__, _r); \
        return c; \
    } \
}

#define EXEC(f) { \
    int _r; \
    if ((_r = f) != VL53L1X_OK) { \
        DEBUG("[vl53l1x] %s: error code %d\n", __func__, _r); \
        return; \
    } \
}

/** Forward declaration of functions for internal use */
static int _is_available(const vl53l1x_t *dev);
static int _reset(vl53l1x_t *dev);
static int _init(vl53l1x_t *dev);

#if IS_USED(MODULE_VL53L1X_ST_API)

/* for ST VL53L1X API basic read and write function are already implemented */
#define _read_byte(d, i, b)     VL53L1_RdByte((VL53L1_Dev_t*)d, i, b)
#define _read_word(d, i, w)     VL53L1_RdWord((VL53L1_Dev_t*)d, i, w)
#define _read(d, i, p, l)       VL53L1_ReadMulti((VL53L1_Dev_t*)d, i, p, l)

#define _write_byte(d, i, b)    VL53L1_WrByte((VL53L1_Dev_t*)d, i, b)
#define _write_word(d, i, w)    VL53L1_WrWord((VL53L1_Dev_t*)d, i, w)
#define _write_dword(d, i, w)   VL53L1_WrDWord((VL53L1_Dev_t*)d, i, w)
#define _write(d, i, p, l)      VL53L1_WriteMulti((VL53L1_Dev_t*)d, i, p, l)

#define _start(d)   VL53L1_StartMeasurement((VL53L1_Dev_t*)d)

extern VL53L1_Error VL53L1_UpdateByte(VL53L1_DEV Dev, uint16_t index,
                                                      uint8_t AndData,
                                                      uint8_t OrData);
#else

typedef struct {
    uint8_t range_status        : 5;
    uint8_t max_threshold_hit   : 1;
    uint8_t min_threshold_hit   : 1;
    uint8_t gph_id_range_status : 1;
    uint8_t report_status       : 4;
    uint8_t reserved1           : 4;
    uint8_t stream_count        : 8;
    uint8_t dss_actual_effective_spads_sd0_hi  : 8;   /* fixed point 8.8 */
    uint8_t dss_actual_effective_spads_sd0_lo  : 8;   /* fixed point 8.8 */
    uint8_t peak_signal_count_rate_mcps_sd0_hi : 8;   /* fixed point 9.7 */
    uint8_t peak_signal_count_rate_mcps_sd0_lo : 8;   /* fixed point 9.7 */
    uint8_t ambient_count_rate_mcps_sd0_hi     : 8;   /* fixed point 9.7 */
    uint8_t ambient_count_rate_mcps_sd0_lo     : 8;   /* fixed point 9.7 */
    uint8_t sigma_sd0_hi : 8;                         /* fixed point 14.2 */
    uint8_t sigma_sd0_lo : 8;                         /* fixed point 14.2 */
    uint8_t phase_sd0_hi : 8;                         /* fixed point 5.11 */
    uint8_t phase_sd0_lo : 8;                         /* fixed point 5.11 */
    uint8_t final_crosstalk_corrected_range_mm_sd0_hi : 8;
    uint8_t final_crosstalk_corrected_range_mm_sd0_lo : 8;
    uint8_t peak_signal_count_rate_crosstalk_corrected_mcps_sd0_hi : 8; /* fixed point 9.7 */
    uint8_t peak_signal_count_rate_crosstalk_corrected_mcps_sd0_lo : 8; /* fixed point 9.7 */
} _vl53l1x_raw_data_t;

static int _update(const vl53l1x_t *dev, uint16_t index, uint8_t mask, uint8_t data);
static int _read(const vl53l1x_t *dev, uint16_t index, uint8_t *data, uint32_t len);
static int _read_byte(const vl53l1x_t *dev, uint16_t index, uint8_t *data);
static int _read_word(const vl53l1x_t *dev, uint16_t index, uint16_t *data);
static int _write(const vl53l1x_t *dev, uint16_t index, const uint8_t *data, uint32_t len);
static int _write_byte(const vl53l1x_t *dev, uint16_t index, uint8_t data);
static int _write_word(const vl53l1x_t *dev, uint16_t index, uint16_t data);
#if !IS_USED(MODULE_VL53L1X_BASIC)
static int _write_dword(const vl53l1x_t *dev, uint16_t index, uint32_t data);
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

static int _start(const vl53l1x_t *dev);
static int _update_dss(const vl53l1x_t *dev, _vl53l1x_raw_data_t* raw_data);

#if !IS_USED(MODULE_VL53L1X_BASIC)
static int _setup_manual_calibration(const vl53l1x_t *dev);
static uint16_t _enc_timeout(uint32_t clks);
static uint32_t _dec_timeout(uint16_t val);
static uint32_t _timeout_clk_to_us(uint32_t clks, uint32_t period_us);
static uint32_t _timeout_us_to_clk(uint32_t us, uint32_t period_us);
static uint32_t _calc_macro_period(const vl53l1x_t *dev, uint8_t vcsel_period);

#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

/* Copyrighted POLOLU / ST API code begin */
#if !IS_USED(MODULE_VL53L1X_BASIC)
static const uint32_t _timing_guard = 4528;  /* used in timing budget calculations */
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */
static const uint16_t _target_rate = 0x0A00; /* used in DSS calculations */
/* Copyrighted POLOLU / ST API code end */

#endif /* IS_USED(MODULE_VL53L1X_ST_API) */

int vl53l1x_init(vl53l1x_t *dev, const vl53l1x_params_t *params)
{
    /* some parameter sanity checks */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);
#if !IS_USED(MODULE_VL53L1X_BASIC)
    ASSERT_PARAM(params->budget <= 1000000);
    ASSERT_PARAM(params->period >= params->budget/1000 + 4);
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */
    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->params = *params;
    dev->int_init = false;

#if IS_USED(MODULE_VL53L1X_ST_API)
    dev->dev.I2cDevAddr = VL53L1X_I2C_ADDRESS;
#elif !IS_USED(MODULE_VL53L1X_BASIC)
    dev->calibrated = false;
#endif

#if !IS_USED(MODULE_VL53L1X_BASIC)
    /* if shutdown pin is defined, it is initialized first and set */
    if (params->pin_shutdown != GPIO_UNDEF) {
        gpio_init(params->pin_shutdown, GPIO_OUT);
        gpio_write(params->pin_shutdown, 1);
    }
#endif

    /* init the sensor and start measurement */
    _init(dev);

    return VL53L1X_OK;
}

int vl53l1x_data_ready(vl53l1x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

#if IS_USED(MODULE_VL53L1X_ST_API)
    uint8_t ready;
    EXEC_RET(VL53L1_GetMeasurementDataReady(&dev->dev, &ready));
    return (ready) ? VL53L1X_OK : -VL53L1X_ERROR_NO_DATA;
#else
    uint8_t byte;
    EXEC_RET(_read_byte(dev, VL53L1X_GPIO__TIO_HV_STATUS, &byte));
    return ((byte & 0x01) == 0) ? VL53L1X_OK : -VL53L1X_ERROR_NO_DATA;
#endif
}

#if IS_USED(MODULE_VL53L1X_ST_API)
int vl53l1x_read_details(vl53l1x_t *dev,
                         VL53L1_RangingMeasurementData_t *details)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(details != NULL);
    DEBUG_DEV("details=%p", dev, details);

    EXEC_RET(vl53l1x_data_ready(dev));
    EXEC_RET(VL53L1_GetRangingMeasurementData(&dev->dev, details));
    EXEC_RET(VL53L1_ClearInterruptAndStartMeasurement(&dev->dev));

    return VL53L1X_OK;
}
#endif /* IS_USED(MODULE_VL53L1X_ST_API) */

int vl53l1x_read_data(vl53l1x_t *dev, vl53l1x_data_t *data)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("data=%p", dev, data);

#if IS_USED(MODULE_VL53L1X_ST_API)

    VL53L1_RangingMeasurementData_t details;
    EXEC_RET(vl53l1x_read_details(dev, &details));

    data->status       = details.RangeStatus;
    data->distance     = details.RangeMilliMeter;
    data->signal_rate  = details.SignalRateRtnMegaCps;
    data->ambient_rate = details.AmbientRateRtnMegaCps;

#else /* IS_USED(MODULE_VL53L1X_ST_API) */

    /* read the results into data structure of type _vl53l1x_raw_data_t */
    _vl53l1x_raw_data_t raw;
    EXEC_RET(_read(dev, VL53L1X_RESULT__RANGE_STATUS,
                        (uint8_t*)&raw, sizeof(_vl53l1x_raw_data_t)));

    /* Copyrighted POLOLU / ST API code begin */

#if !IS_USED(MODULE_VL53L1X_BASIC)
    /* set status */
    switch (raw.range_status) {
    case 1:
    case 2:
    case 3:
        data->status = VL53L1X_RANGESTATUS_HARDWARE_FAIL;
        break;
    case 4:
        data->status = VL53L1X_RANGESTATUS_SIGNAL_FAIL;
        break;
    case 5:
        data->status = VL53L1X_RANGESTATUS_OUTOFBOUNDS_FAIL;
        break;
    case 6:
        data->status = VL53L1X_RANGESTATUS_SIGMA_FAIL;
         break;
    case 7:
        data->status = VL53L1X_RANGESTATUS_WRAP_TARGET_FAIL;
        break;
    case 8:
        data->status = VL53L1X_RANGESTATUS_RANGE_VALID_MIN_RANGE_CLIPPED;
        break;
    case 9:
        data->status = (raw.stream_count == 0) ?
                        VL53L1X_RANGESTATUS_RANGE_VALID_NO_WRAP_CHECK_FAIL :
                        VL53L1X_RANGESTATUS_RANGE_VALID;
        break;
    case 12:
        data->status = VL53L1X_RANGESTATUS_XTALK_SIGNAL_FAIL;
        break;
    case 13:
        data->status = VL53L1X_RANGESTATUS_MIN_RANGE_FAIL;
        break;
    case 17:
        data->status = VL53L1X_RANGESTATUS_HARDWARE_FAIL;
        break;
    case 18:
        data->status = VL53L1X_RANGESTATUS_SYNCRONISATION_INT;
        break;
    case 19:
        data->status = VL53L1X_RANGESTATUS_RANGE_VALID_NO_WRAP_CHECK_FAIL;
        break;
    default:
        data->status = VL53L1X_RANGESTATUS_NONE;
    }

    /* calibrate if necessary */
    if (!dev->calibrated) {
        _setup_manual_calibration(dev);
        dev->calibrated = true;
    }
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

    EXEC_RET(_update_dss(dev, &raw));

    /* calculate range */
    uint16_t range;
    range  = (uint16_t)raw.final_crosstalk_corrected_range_mm_sd0_hi << 8;
    range |= raw.final_crosstalk_corrected_range_mm_sd0_lo;

    /*
     * apply correction gain
     * gain factor of 2011 is tuning param default
     * (VL53L1_TUNINGPARM_LITE_RANGING_GAIN_FACTOR_DEFAULT)
     * Basically, this appears to scale the result by 2011/2048, or about 98%
     * (with the 1024 added for proper rounding).
     */
    data->distance = ((uint32_t)range * 2011 + 0x0400) / 0x0800;
    /* Copyrighted POLOLU / ST API code end */

#if !IS_USED(MODULE_VL53L1X_BASIC)
    /* peak signal rate in 9.7 fixed point format */
    data->signal_rate  = (uint32_t)raw.peak_signal_count_rate_mcps_sd0_hi << 8;
    data->signal_rate |= raw.peak_signal_count_rate_mcps_sd0_lo;

    /* ambient rate in in 9.7 fixed point format */
    data->ambient_rate  = (uint32_t)raw.ambient_count_rate_mcps_sd0_hi << 8;
    data->ambient_rate |= raw.ambient_count_rate_mcps_sd0_lo;

    /* convert values from 9.7 to 16.16 */
    data->signal_rate  <<= 9;
    data->ambient_rate <<= 9;

#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

    /* restart measurement */
    EXEC_RET(_start(dev));

#endif /* IS_USED(MODULE_VL53L1X_ST_API) */

    return VL53L1X_OK;
}

int vl53l1x_read_mm(vl53l1x_t *dev, int16_t *mm)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(mm != NULL);
    DEBUG_DEV("mm=%p", dev, mm);

    vl53l1x_data_t data;
    EXEC_RET(vl53l1x_read_data(dev, &data));
    *mm = data.distance;

    return VL53L1X_OK;
}

int vl53l1x_int_config(vl53l1x_t *dev, vl53l1x_int_config_t* cfg,
                                       void (*isr)(void *),
                                       void *isr_arg)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(dev->params.pin_int != GPIO_UNDEF);

    DEBUG_DEV("cfg=%p isr=%p isr_arg=%p", dev, cfg, isr, isr_arg);

    if (!dev->int_init) {
        dev->int_init = true;
        gpio_init_int(dev->params.pin_int, GPIO_IN_PU, GPIO_FALLING,
                      isr, isr_arg);
    }

#if IS_USED(MODULE_VL53L1X_ST_API)
    ASSERT_PARAM(cfg != NULL);

    EXEC_RET(VL53L1_StopMeasurement(&dev->dev));

    VL53L1_DetectionConfig_t dcfg;
    dcfg.DetectionMode = cfg->mode;
    dcfg.Distance.CrossMode = cfg->dist_mode;
    dcfg.Distance.High = cfg->dist_high;
    dcfg.Distance.Low = cfg->dist_low;
    dcfg.Rate.CrossMode = cfg->rate_mode;
    dcfg.Rate.High = cfg->rate_high;
    dcfg.Rate.Low = cfg->rate_low;
    dcfg.IntrNoTarget = 0;

    EXEC_RET(VL53L1_SetThresholdConfig(&dev->dev, &dcfg));
    EXEC_RET(VL53L1_StartMeasurement(&dev->dev));
#else
    (void)cfg;
    DEBUG_DEV("configuration is not used", dev);
#endif

    return VL53L1X_OK;
}

#if !IS_USED(MODULE_VL53L1X_BASIC)

int vl53l1x_power_down(const vl53l1x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->params.pin_shutdown == GPIO_UNDEF) {
        DEBUG_DEV("Pin connected to sensor's XSHUT pin not defined", dev);
        return VL53L1X_ERROR_NO_PIN;
    }

    gpio_clear(dev->params.pin_shutdown);
    return VL53L1X_OK;
}

int vl53l1x_power_up(vl53l1x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->params.pin_shutdown == GPIO_UNDEF) {
        DEBUG_DEV("Pin connected to sensor's XSHUT pin not defined", dev);
        return VL53L1X_ERROR_NO_PIN;
    }

    gpio_set(dev->params.pin_shutdown);

    /* init the sensor and start measurement */
    _init(dev);

    return VL53L1X_OK;
}

int vl53l1x_get_timing_budget(vl53l1x_t *dev, uint32_t *budget)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(budget != NULL);
    DEBUG_DEV("budget=%p", dev, budget);

#if IS_USED(MODULE_VL53L1X_ST_API)
    return VL53L1_GetMeasurementTimingBudgetMicroSeconds(&dev->dev,
                                                         budget);
#else
    /* Copyrighted POLOLU / ST API code begin */

    /* Update Macro Period for Range A VCSEL Period */
    uint8_t  byte;
    uint32_t period_us;

    EXEC_RET(_read_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_A, &byte));
    period_us = _calc_macro_period(dev, byte);

    /* Get Range Timing A timeout */
    uint16_t word;
    uint32_t timeout_us;

    EXEC_RET(_read_word(dev, VL53L1X_RANGE_CONFIG__TIMEOUT_MACROP_A, &word));
    timeout_us = _timeout_clk_to_us(_dec_timeout(word), period_us);

    *budget = 2 * timeout_us + _timing_guard;
    /* Copyrighted POLOLU / ST API code end */

    return VL53L1X_OK;

#endif
}

int vl53l1x_set_timing_budget(vl53l1x_t *dev, uint32_t budget_us)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("budget_us=%u", dev, budget_us);

#if IS_USED(MODULE_VL53L1X_ST_API)

    EXEC_RET(VL53L1_SetMeasurementTimingBudgetMicroSeconds(&dev->dev,
                                                           budget_us));
#else
    /* Copyrighted POLOLU / ST API code begin */
    /* assumes PresetMode is LOWPOWER_AUTONOMOUS */

    if (budget_us <= _timing_guard) {
        return VL53L1X_ERROR_INV_PARAM;
    }

    uint32_t timeout_us = budget_us -= _timing_guard;
    if (timeout_us > 1100000) {
        return VL53L1X_ERROR_INV_PARAM;
    }
    timeout_us /= 2;

    /* Update Macro Period for Range A VCSEL Period */
    uint8_t  byte;
    uint32_t period_us;

    EXEC_RET(_read_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_A, &byte));
    period_us = _calc_macro_period(dev, byte);

    /*
     * Update Phase timeout - uses Timing A
     * Timeout of 1000 is tuning param default
     * (TIMED_PHASECAL_CONFIG_TIMEOUT_US_DEFAULT)
     * via VL53L1_get_preset_mode_timing_cfg().
     */
    uint32_t clks = _timeout_us_to_clk(1000, period_us);
    clks = (clks > 0xff) ? 0xff : clks;
    EXEC_RET(_write_byte(dev, VL53L1X_PHASECAL_CONFIG__TIMEOUT_MACROP, clks));

    /*
     * Update MM Timing A timeout.
     * Timeout of 1 is tuning param default
     * (LOWPOWERAUTO_MM_CONFIG_TIMEOUT_US_DEFAULT) via
     * VL53L1_get_preset_mode_timing_cfg().
     * With the API, the register actually ends up with a slightly different
     * value because it gets assigned, retrieved, recalculated with a different
     * macro period, and reassigned, but it probably doesn't matter because
     * it seems like the MM ("mode mitigation"?) sequence steps are disabled
     * in low power auto mode anyway.
     */
    clks = _enc_timeout(_timeout_us_to_clk(1, period_us));
    EXEC_RET(_write_word(dev, VL53L1X_MM_CONFIG__TIMEOUT_MACROP_A, clks));

    /* Update Range Timing A timeout */
    clks = _enc_timeout(_timeout_us_to_clk(timeout_us, period_us));
    EXEC_RET(_write_word(dev, VL53L1X_RANGE_CONFIG__TIMEOUT_MACROP_A, clks));

    /* Update Macro Period for Range B VCSEL Period */
    EXEC_RET(_read_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_B, &byte));
    period_us = _calc_macro_period(dev, byte);

    /* Update MM Timing B timeout, see comment about MM Timing A timeout. */
    clks = _enc_timeout(_timeout_us_to_clk(1, period_us));
    EXEC_RET(_write_word(dev, VL53L1X_MM_CONFIG__TIMEOUT_MACROP_B, clks));

    /* Update Range Timing B timeout */
    clks = _enc_timeout(_timeout_us_to_clk(timeout_us, period_us));
    EXEC_RET(_write_word(dev, VL53L1X_RANGE_CONFIG__TIMEOUT_MACROP_B, clks));

    /* Copyrighted POLOLU / ST API code end */
#endif

    /* save the timing budget */
    dev->params.budget = budget_us;

    return VL53L1X_OK;
}

int vl53l1x_set_measurement_period(vl53l1x_t *dev, uint32_t period_ms)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM((period_ms + 4) * 1000 > dev->params.budget);
    DEBUG_DEV("period_ms=%u", dev, period_ms);

#if IS_USED(MODULE_VL53L1X_ST_API)

    EXEC_RET(VL53L1_SetInterMeasurementPeriodMilliSeconds(&dev->dev,
                                                          period_ms));
#else
    /* Copyrighted POLOLU / ST API code begin */

    /* from VL53L1_set_inter_measurement_period_ms() */
    uint32_t period = period_ms * dev->osc_calibrate_val;
    EXEC_RET(_write_dword(dev, VL53L1X_SYSTEM__INTERMEASUREMENT_PERIOD, period));

    /* Copyrighted POLOLU / ST API code end */
#endif

    /* save the measurement period */
    dev->params.period = period_ms;

    return VL53L1X_OK;
}

int vl53l1x_set_distance_mode(vl53l1x_t *dev, vl53l1x_dist_mode_t mode)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("mode=%u", dev, mode);

#if IS_USED(MODULE_VL53L1X_ST_API)

    EXEC_RET(VL53L1_SetDistanceMode(&dev->dev, mode));

#else
    /* Copyrighted POLOLU / ST API code begin */

    /* save existing timing budget */
    uint32_t budget_us;
    EXEC_RET(vl53l1x_get_timing_budget(dev, &budget_us));

    switch (mode) {
    case VL53L1X_DIST_SHORT:
        /* from VL53L1_preset_mode_standard_ranging_short_range() */
        /* timing config */
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_A, 0x07));
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_B, 0x05));
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VALID_PHASE_HIGH, 0x38));

        /* dynamic config */
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__WOI_SD0, 0x07));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__WOI_SD1, 0x05));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__INITIAL_PHASE_SD0, 6));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__INITIAL_PHASE_SD1, 6));
        break;

    case VL53L1X_DIST_MEDIUM:
        /* from VL53L1_preset_mode_standard_ranging() */
        /* timing config */
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_A, 0x0b));
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_B, 0x09));
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VALID_PHASE_HIGH, 0x78));

        /* dynamic config */
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__WOI_SD0, 0x0b));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__WOI_SD1, 0x09));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__INITIAL_PHASE_SD0, 10));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__INITIAL_PHASE_SD1, 10));
        break;

    case VL53L1X_DIST_LONG:
        /* from VL53L1_preset_mode_standard_ranging_long_range() */
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_A, 0x0f));
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VCSEL_PERIOD_B, 0x0d));
        EXEC_RET(_write_byte(dev, VL53L1X_RANGE_CONFIG__VALID_PHASE_HIGH, 0xb8));

        /* dynamic config */
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__WOI_SD0, 0x0f));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__WOI_SD1, 0x0d));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__INITIAL_PHASE_SD0, 14));
        EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__INITIAL_PHASE_SD1, 14));
        break;

    default:
        /* unrecognized mode - do nothing */
        return VL53L1X_ERROR_INV_ARG;
    }

    /* reapply timing budget */
    EXEC_RET(vl53l1x_set_timing_budget(dev, budget_us));

    /* Copyrighted POLOLU / ST API code end */
#endif

    /* save the distance mode */
    dev->params.mode = mode;

    return VL53L1X_OK;
}

int vl53l1x_set_roi(vl53l1x_t *dev, vl53l1x_roi_t *roi)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(roi != NULL);

    ASSERT_PARAM(roi->x_tl <= 15 && roi->y_tl <= 15);
    ASSERT_PARAM(roi->x_br <= 15 && roi->y_br <= 15);
    ASSERT_PARAM(roi->x_tl < roi->x_br);
    ASSERT_PARAM(roi->y_tl > roi->y_br);
    ASSERT_PARAM(roi->x_br - roi->x_tl >= 3);
    ASSERT_PARAM(roi->y_tl - roi->y_br >= 3);

    DEBUG_DEV("roi=%p", dev, roi);

#if IS_USED(MODULE_VL53L1X_ST_API)
    return VL53L1_SetUserROI(&dev->dev, (VL53L1_UserRoi_t*)roi);
#else /* IS_USED(MODULE_VL53L1X_ST_API) */

    /* Copyrighted ST API code begin */

    /* center calculation from VL53L1_encode_row_col() */
    uint8_t row = (roi->x_br + roi->x_tl  + 1) / 2;
    uint8_t col = (roi->y_br + roi->y_tl  + 1) / 2;
    uint8_t spad = (row > 7) ? 128 + (col << 3) + (15 - row)
                             : ((15 - col) << 3) + row;
    /* dimension calculation from VL53L1_encode_zone_size() */
    uint8_t w   = (roi->x_br - roi->x_tl);
    uint8_t h   = (roi->y_tl - roi->y_br);
    uint8_t dim = (h << 4) + w;

    /* Copyrighted ST API code end */

    EXEC_RET(_write_byte(dev, VL53L1X_ROI_CONFIG__USER_ROI_CENTRE_SPAD, spad));
    EXEC_RET(_write_byte(dev, VL53L1X_ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE, dim));

#endif /* IS_USED(MODULE_VL53L1X_ST_API) */

    return VL53L1X_OK;
}

int vl53l1x_get_roi(vl53l1x_t *dev, vl53l1x_roi_t *roi)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(roi != NULL);

    DEBUG_DEV("roi=%p", dev, roi);

#if IS_USED(MODULE_VL53L1X_ST_API)
    return VL53L1_GetUserROI(&dev->dev, (VL53L1_UserRoi_t*)roi);
#else /* IS_USED(MODULE_VL53L1X_ST_API) */

    uint8_t spad;
    EXEC_RET(_read_byte(dev, VL53L1X_ROI_CONFIG__USER_ROI_CENTRE_SPAD, &spad));

    /* Copyrighted ST API code begin */

    /* from VL53L1_decode_row_col() */
    uint8_t row;
    uint8_t col;

    if (spad > 127) {
        row = 8 + ((255 - spad) & 0x07);
        col = (spad - 128) >> 3;
    }
    else {
        row = spad & 0x07;
        col = (127 - spad) >> 3;
    }

     uint8_t dim;
    EXEC_RET(_read_byte(dev, VL53L1X_ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE, &dim));

    /* from VL53L1_decode_zone_size() */
    uint8_t h = dim >> 4;
    uint8_t w = dim & 0x0f;

    /* from VL53L1_GetUserROI() */
    roi->x_tl = (2 * col - w) >> 1;
    roi->y_tl = (2 * row - h) >> 1;
    roi->x_br = (2 * col + w) >> 1;
    roi->y_br = (2 * row + h) >> 1;

    /* Copyrighted ST API code end */
#endif /* IS_USED(MODULE_VL53L1X_ST_API) */

    return VL53L1X_OK;
}

#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

/** Functions for internal use only */

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(const vl53l1x_t *dev)
{
    DEBUG_DEV("", dev);

    uint8_t id;
    uint8_t rev;
    uint8_t mod_type;
    uint16_t mod_id = 0;

    /* read the chip id from VL53L1X_REG_ID_X */
    EXEC_RET(_read_byte(dev, VL53L1X_IDENTIFICATION__MODEL_ID, &id));

    if (id != VL53L1X_DEVICE_ID) {
        DEBUG_DEV("sensor is not available, wrong device id %02x, "
                  "should be %02x", dev, id, VL53L1X_DEVICE_ID);
        return -VL53L1X_ERROR_WRONG_ID;
    }

    EXEC_RET(_read_byte(dev, VL53L1X_IDENTIFICATION__REVISION_ID, &rev));
    EXEC_RET(_read_byte(dev, VL53L1X_IDENTIFICATION__MODULE_TYPE, &mod_type));
    EXEC_RET(_read_word(dev, VL53L1X_IDENTIFICATION__MODULE_ID, &mod_id));

    DEBUG_DEV("rev=%02x, module type=%02x id=%04x", dev, rev, mod_type, mod_id);
    return VL53L1X_OK;
}

#if IS_USED(MODULE_VL53L1X_BASIC)
/* The following block was recorded with the
 * [ST VL53L1X API implementation for Arduino]
 * (https://github.com/pololu/vl53l1x-st-api-arduino) with a timing budget of
 * 50 ms and a inter-measurement time of 100 ms. It is streamed to the sensor
 * as configuration.
 */
static const uint8_t _init_block[] = {
  /* address                                                   length */
  /* 0x0001 */ 0x29, 0x02, 0x10, 0x00, 0x27, 0xbd, 0x1e, 0x81, /*   8 */
  /* 0x0009 */ 0x80, 0x07, 0x94, 0x00, 0xff, 0xff, 0x9f, 0x9e, /*  16 */
  /* 0x0011 */ 0xff, 0x0f, 0x00, 0x0c, 0x01, 0x00, 0x00, 0x00, /*  24 */
  /* 0x0019 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*  32 */
  /* 0x0021 */ 0x22, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, /*  40 */
  /* 0x0029 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, /*  48 */
  /* 0x0031 */ 0x02, 0x00, 0x02, 0x08, 0x00, 0x08, 0x10, 0x01, /*  56 */
  /* 0x0039 */ 0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x02, /*  64 */
  /* 0x0041 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x0b, 0x00, /*  72 */
  /* 0x0049 */ 0x00, 0x02, 0x0a, 0x21, 0x00, 0x00, 0x02, 0x00, /*  80 */
  /* 0x0051 */ 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x38, 0xff, /*  88 */
  /* 0x0059 */ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe8, 0x0f, /*  96 */
  /* 0x0061 */ 0x01, 0x85, 0x0d, 0x01, 0x68, 0x00, 0xc0, 0x08, /* 104 */
  /* 0x0069 */ 0xb8, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x74, 0x00, /* 112 */
  /* 0x0071 */ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0f, /* 120 */
  /* 0x0079 */ 0x0d, 0x0e, 0x0e, 0x01, 0x00, 0x02, 0xc7, 0xff, /* 128 */
  /* 0x0081 */ 0x8b, 0x00, 0x00                                /* 131 */
};
#endif /* IS_USED(MODULE_VL53L1X_BASIC) */

static int _init(vl53l1x_t *dev)
{
    /* wait for 6 ms after power on reset */
    ztimer_sleep(ZTIMER_MSEC, 6);

    /* check availability of the sensor */
    EXEC_RET(_is_available(dev));

    /* reset, wait for 4 ms, and wait that devices has been booted */
    EXEC_RET(_reset(dev));

#if IS_USED(MODULE_VL53L1X_BASIC)

    /* stream the initialization block to the sensor to configure it */
    EXEC_RET(_write(dev, VL53L1X_I2C_SLAVE__DEVICE_ADDRESS,
                         _init_block, sizeof(_init_block)));

#else /* IS_USED(MODULE_VL53L1X_BASIC) */

#if IS_USED(MODULE_VL53L1X_ST_API)

    /* configure the device without calibration data */
    EXEC_RET(VL53L1_DataInit(&dev->dev));
    EXEC_RET(VL53L1_StaticInit(&dev->dev));

#else /* IS_USED(MODULE_VL53L1X_ST_API) */

    /* Copyrighted POLOLU / ST API code begin */
    /* store oscillator info for later use */
    EXEC_RET(_read_word(dev, VL53L1X_OSC_MEASURED__FAST_OSC__FREQUENCY, &dev->fast_osc_frequency));
    EXEC_RET(_read_word(dev, VL53L1X_RESULT__OSC_CALIBRATE_VAL, &dev->osc_calibrate_val));

    /* static config */
    EXEC_RET(_write_word(dev, VL53L1X_DSS_CONFIG__TARGET_TOTAL_RATE_MCPS, _target_rate));
    EXEC_RET(_write_byte(dev, VL53L1X_GPIO__TIO_HV_STATUS, 0x02));
    EXEC_RET(_write_byte(dev, VL53L1X_SIGMA_ESTIMATOR__EFFECTIVE_PULSE_WIDTH_NS, 8));
    EXEC_RET(_write_byte(dev, VL53L1X_SIGMA_ESTIMATOR__EFFECTIVE_AMBIENT_WIDTH_NS, 16));
    EXEC_RET(_write_byte(dev, VL53L1X_ALGO__CROSSTALK_COMPENSATION_VALID_HEIGHT_MM, 0x01));
    EXEC_RET(_write_byte(dev, VL53L1X_ALGO__RANGE_IGNORE_VALID_HEIGHT_MM, 0xff));
    EXEC_RET(_write_byte(dev, VL53L1X_ALGO__RANGE_MIN_CLIP, 0));
    EXEC_RET(_write_byte(dev, VL53L1X_ALGO__CONSISTENCY_CHECK__TOLERANCE, 2));

    /* general config */
    EXEC_RET(_write_word(dev, VL53L1X_SYSTEM__THRESH_RATE_HIGH, 0x0000));
    EXEC_RET(_write_word(dev, VL53L1X_SYSTEM__THRESH_RATE_LOW, 0x0000));
    EXEC_RET(_write_byte(dev, VL53L1X_DSS_CONFIG__APERTURE_ATTENUATION, 0x38));

    /*
     * timing config (most of these settings will be determined later by distance
     * and timing budget configuration
     */
    EXEC_RET(_write_word(dev, VL53L1X_RANGE_CONFIG__SIGMA_THRESH, 360));
    EXEC_RET(_write_word(dev, VL53L1X_RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS, 192));

    /* dynamic config */

    EXEC_RET(_write_byte(dev, VL53L1X_SYSTEM__GROUPED_PARAMETER_HOLD_0, 0x01));
    EXEC_RET(_write_byte(dev, VL53L1X_SYSTEM__GROUPED_PARAMETER_HOLD_1, 0x01));
    EXEC_RET(_write_byte(dev, VL53L1X_SD_CONFIG__QUANTIFIER, 2));

    /*
     * from VL53L1_preset_mode_timed_ranging_*
     * GPH is 0 after reset, but writing GPH0 and GPH1 above seem to set GPH
     * to 1, and things don't seem to work if we don't set GPH back to 0 (which
     * the API does here).
     */
    EXEC_RET(_write_byte(dev, VL53L1X_SYSTEM__GROUPED_PARAMETER_HOLD, 0x00));
    EXEC_RET(_write_byte(dev, VL53L1X_SYSTEM__SEED_CONFIG, 1));

    /* from VL53L1_config_low_power_auto_mode */
    EXEC_RET(_write_byte(dev, VL53L1X_SYSTEM__SEQUENCE_CONFIG, 0x8B));
    EXEC_RET(_write_word(dev, VL53L1X_DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT, 200 << 8));
    EXEC_RET(_write_byte(dev, VL53L1X_DSS_CONFIG__ROI_MODE_CONTROL, 2));

    /* default to long range, 50 ms timing budget */
    EXEC_RET(vl53l1x_set_distance_mode(dev, VL53L1X_DIST_LONG));
    EXEC_RET(vl53l1x_set_timing_budget(dev, 50000));

    /*
     * the API triggers this change in VL53L1_init_and_start_range() once a
     * measurement is started; assumes MM1 and MM2 are disabled
     */
    uint16_t word;
    EXEC_RET(_read_word(dev, VL53L1X_MM_CONFIG__OUTER_OFFSET_MM, &word));
    word *= 4;
    EXEC_RET(_write_word(dev, VL53L1X_ALGO__PART_TO_PART_RANGE_OFFSET_MM, word));

    /* Copyrighted POLOLU / ST API code end */

#endif /* IS_USED(MODULE_VL53L1X_ST_API) */

    /* set measurement parameters */
    EXEC_RET(vl53l1x_set_distance_mode(dev, dev->params.mode));
    EXEC_RET(vl53l1x_set_timing_budget(dev, dev->params.budget));
    EXEC_RET(vl53l1x_set_measurement_period(dev, dev->params.period));

#endif /* IS_USED(MODULE_VL53L1X_BASIC) */

    /* start measurement */
    EXEC_RET(_start(dev));

    return VL53L1X_OK;
}

#define VL53L1X_BOOT_TIMEOUT    10      /* 10 ms */

static int _reset(vl53l1x_t *dev)
{
#if IS_USED(MODULE_VL53L1X_ST_API)

    /* write reset impuls for 150 us */
    VL53L1_software_reset(&dev->dev);

    /* give the sensor some time to boot */
    ztimer_sleep(ZTIMER_MSEC, 4);

    /* wait until the sensor has been booted */
    EXEC_RET(VL53L1_WaitDeviceBooted(&dev->dev));

    /* set VDDIO */
    EXEC_RET(VL53L1_UpdateByte(&dev->dev,
                               VL53L1X_PAD_I2C_HV__EXTSUP_CONFIG,
                               0xfe, dev->params.vddio_2v8 ? 1 : 0));

#else /* IS_USED(MODULE_VL53L1X_ST_API) */

    /* write reset impuls of at least 100 us, we use 1 ms */
    _write_byte(dev, VL53L1X_SOFT_RESET, 0);
    ztimer_sleep(ZTIMER_MSEC, 1);
    _write_byte(dev, VL53L1X_SOFT_RESET, 1);

    /* wait until the sensor has been booted in 1 ms steps*/
    unsigned timeout = VL53L1X_BOOT_TIMEOUT;
    uint8_t res;
    while (timeout--) {
        if ((_read_byte(dev, VL53L1X_FIRMWARE__SYSTEM_STATUS, &res) == VL53L1X_OK) &&
            (res & 0x01) == 1) {
            break;
        }
        ztimer_sleep(ZTIMER_MSEC, 1);
    }
    if (timeout == 0) {
        DEBUG_DEV("could not boot the sensor", dev);
        return VL53L1X_ERROR_TIMEOUT;
    }

    /* set VDDIO */
    EXEC_RET(_update(dev, VL53L1X_PAD_I2C_HV__EXTSUP_CONFIG,
                          0xfe, dev->params.vddio_2v8 ? 1 : 0));

#endif /* IS_USED(MODULE_VL53L1X_ST_API) */

    return VL53L1X_OK;
}

#if !IS_USED(MODULE_VL53L1X_ST_API)

static int _start(const vl53l1x_t *dev)
{
    /* Copyrighted POLOLU / ST API code begin */

    /* from VL53L1_set_inter_measurement_period_ms() */
    EXEC_RET(_write_byte(dev, VL53L1X_SYSTEM__INTERRUPT_CLEAR, 0x01));
    EXEC_RET(_write_byte(dev, VL53L1X_SYSTEM__MODE_START, 0x40));

    /* Copyrighted POLOLU / ST API code end */
    return VL53L1X_OK;
}

/* Copyrighted POLOLU / ST API code begin */

#if !IS_USED(MODULE_VL53L1X_BASIC)

static uint32_t _dec_timeout(uint16_t val)
{
    /*
     * Decode sequence step timeout in MCLKs from register value based
     * on VL53L1_decode_timeout()
     */
    return ((uint32_t)(val & 0xff) << (val >> 8)) + 1;
}

static uint16_t _enc_timeout(uint32_t clks)
{
    /*
     * Encode sequence step timeout register value from timeout in MCLKs
     * based on VL53L1_enc_timeout()
     * encoded format: "(LSByte * 2^MSByte) + 1"
     */
    if (clks > 0) {
        uint32_t ls_byte = clks - 1;
        uint16_t ms_byte = 0;
        while ((ls_byte & 0xFFFFFF00) > 0) {
            ls_byte >>= 1;
            ms_byte++;
        }
        return (ms_byte << 8) | (ls_byte & 0xFF);
    }
    else {
        return 0;
    }
}

static uint32_t _timeout_clk_to_us(uint32_t clks, uint32_t period_us)
{
    /*
     * Convert sequence step timeout from macro periods to microseconds with
     * given macro period in microseconds (12.12 format) based on
     * VL53L1_calc_timeout_us()
     */
    return ((uint64_t)clks * period_us + 0x800) >> 12;
}

static uint32_t _timeout_us_to_clk(uint32_t us, uint32_t period_us)
{
    /*
     * Convert sequence step timeout from microseconds to macro periods with
     * given macro period in microseconds (12.12 format) based on
     * VL53L1_calc_timeout_mclks()
     */
    return (((uint32_t)us << 12) + (period_us >> 1)) / period_us;
}

static uint32_t _calc_macro_period(const vl53l1x_t *dev, uint8_t vcsel_period)
{
    /*
     * Calculate macro period in microseconds (12.12 format) with given VCSEL
     * period assumes fast_osc_frequency has been read and stored based on
     * VL53L1_calc_macro_period_us()
     */

    /* fast osc frequency in 4.12 format; PLL period in 0.24 format */
    uint32_t pll_period_us = ((uint32_t)0x01 << 30) / dev->fast_osc_frequency;

    /* from VL53L1_decode_vcsel_period() */
    uint8_t vcsel_period_pclks = (vcsel_period + 1) << 1;

    /* VL53L1_MACRO_PERIOD_VCSEL_PERIODS = 2304 */
    uint32_t period_us = (uint32_t)2304 * pll_period_us;
    period_us >>= 6;
    period_us *= vcsel_period_pclks;
    period_us >>= 6;

    return period_us;
}

static int _setup_manual_calibration(const vl53l1x_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    /*
     * Setup ranges after the first one in low power auto mode by turning off
     * FW calibration steps and programming static values
     * based on VL53L1_low_power_auto_setup_manual_calibration()
     */

    uint8_t  saved_vhv_init;
    uint8_t  saved_vhv_timeout;

    /* save original vhv configs */
    EXEC_RET(_read_byte(dev, VL53L1X_VHV_CONFIG__INIT, &saved_vhv_init));
    EXEC_RET(_read_byte(dev, VL53L1X_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND,
                             &saved_vhv_timeout));
    /* disable VHV init */
    EXEC_RET(_write_byte(dev, VL53L1X_VHV_CONFIG__INIT,
                              saved_vhv_init & 0x7f));

    /* set loop bound to tuning param */
    EXEC_RET(_write_byte(dev, VL53L1X_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND,
                              (saved_vhv_timeout & 0x03) + (3 << 2)));

    /* override phasecal */
    uint8_t reg;
    EXEC_RET(_write_byte(dev, VL53L1X_PHASECAL_CONFIG__OVERRIDE, 0x01));
    EXEC_RET(_read_byte(dev, VL53L1X_PHASECAL_RESULT__VCSEL_START, &reg));
    EXEC_RET(_write_byte(dev, VL53L1X_CAL_CONFIG__VCSEL_START, reg));

    return VL53L1X_OK;
}

#endif /* IS_USED(MODULE_VL53L1X_BASIC) */

static int _update_dss(const vl53l1x_t *dev, _vl53l1x_raw_data_t* raw)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL);
    DEBUG_DEV("raw=%p", dev, raw);

    uint16_t spad_count;
    uint32_t peak;
    uint32_t ambient;

    spad_count  = (uint16_t)raw->dss_actual_effective_spads_sd0_hi << 8;
    spad_count |= raw->dss_actual_effective_spads_sd0_lo;

    peak  = (uint32_t)raw->peak_signal_count_rate_crosstalk_corrected_mcps_sd0_hi << 8;
    peak |= raw->peak_signal_count_rate_crosstalk_corrected_mcps_sd0_lo;

    ambient  = (uint32_t)raw->ambient_count_rate_mcps_sd0_hi << 8;
    ambient |= raw->ambient_count_rate_mcps_sd0_lo;

    if (spad_count != 0) {
        uint32_t total_rate;
        /* Calc total rate per spad */
        total_rate = peak + ambient;
        /* clip to 16 bits */
        total_rate = (total_rate > 0xffff) ? 0xffff : total_rate;
        /* shift up to take advantage of 32 bits */
        total_rate <<= 16;

        total_rate /= spad_count;

        if (total_rate != 0) {
            /* get the target rate and shift up by 16 */
            uint32_t req_spads = ((uint32_t)_target_rate << 16) / total_rate;
            /* clip to 16 bits */
            req_spads = (req_spads > 0xffff) ? 0xffff : req_spads;

            /* override DSS config */
            EXEC_RET(_write_word(dev, VL53L1X_DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT,
                                      req_spads));
            /*
             * VL53L1X_DSS_CONFIG__ROI_MODE_CONTROL should already be set to
             * REQUESTED_EFFFECTIVE_SPADS
             */

             return VL53L1X_OK;
        }
    }

    /*
     * If we reached this point, it means something above would have resulted
     * in a divide by zero. We want to gracefully set a spad target, not just
     * exit with an error
     */
    /* set target to mid point */
    EXEC_RET(_write_word(dev, VL53L1X_DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT,
                              0x8000));
    return VL53L1X_OK;
}

/* Copyrighted POLOLU / ST API code end */

#define VL53L1X_BUFSIZ   32
static uint8_t _buffer[VL53L1X_BUFSIZ] = {};

static int _read_byte(const vl53l1x_t *dev, uint16_t index, uint8_t *data)
{
    return _read(dev, index, data, 1);
}

static int _read_word(const vl53l1x_t *dev, uint16_t index, uint16_t *data)
{
    uint8_t bytes[2];
    EXEC_RET(_read(dev, index, bytes, 2));
    *data = ((uint16_t)bytes[0] << 8) + (bytes[1]);

    return VL53L1X_OK;
}

static int _read(const vl53l1x_t *dev, uint16_t index, uint8_t *pdata, uint32_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(pdata != NULL);
    DEBUG_DEV("index=%04x pdata=%p len=%u", dev, index, pdata, len);

    /* acquire the I2C device */
    i2c_acquire(dev->params.i2c_dev);

    /* read blockwise with a maximum size of VL53L1X_BUFSIZ */
    while (len) {

        uint8_t bytes[2] = { (index >> 8) & 0xff, index & 0xff };

        /* write the register index for the block */
        if (i2c_write_bytes(dev->params.i2c_dev, VL53L1X_I2C_ADDRESS, bytes, 2, 0) != 0) {
            i2c_release(dev->params.i2c_dev);
            return VL53L1X_ERROR_I2C;
        }

        uint8_t cnt = (len < VL53L1X_BUFSIZ) ? len : VL53L1X_BUFSIZ;

        if (i2c_read_bytes(dev->params.i2c_dev, VL53L1X_I2C_ADDRESS, _buffer, cnt, 0) != 0) {
            i2c_release(dev->params.i2c_dev);
            return VL53L1X_ERROR_I2C;
        }

        memcpy(pdata, _buffer, cnt);
        len -= cnt;
        pdata += cnt;
        index += cnt;
    }

    /* release the I2C device */
    i2c_release(dev->params.i2c_dev);

    return VL53L1X_OK;
}

static int _write_byte(const vl53l1x_t *dev, uint16_t index, uint8_t data)
{
    return _write(dev, index, &data, 1);
}

static int _write_word(const vl53l1x_t *dev, uint16_t index, uint16_t data)
{
    uint8_t bytes[2] = { (data >> 8) & 0xff, data & 0xff };
    return _write(dev, index, (uint8_t*)&bytes, 2);
}

#if !IS_USED(MODULE_VL53L1X_BASIC)
static int _write_dword(const vl53l1x_t *dev, uint16_t index, uint32_t data)
{
    uint8_t bytes[4] = { (data >> 24) & 0xff, (data >> 16) & 0xff,
                         (data >> 8) & 0xff, data & 0xff };
    return _write(dev, index, (uint8_t*)&bytes, 4);
}
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

static int _write(const vl53l1x_t *dev, uint16_t index, const uint8_t *pdata, uint32_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(pdata != NULL);
    DEBUG_DEV("index=%04x pdata=%p len=%u", dev, index, pdata, len);

    assert(!irq_is_in());
    i2c_acquire(dev->params.i2c_dev);

    /* write blockwise with a maximum size of VL53L1X_BUFSIZ-2 */
    while (len) {

        uint8_t cnt = (len < (VL53L1X_BUFSIZ - 2)) ? len : VL53L1X_BUFSIZ - 2;

        /* fill the first 2 bytes of the buffer with the index */
        _buffer[0] = (index >> 8) & 0xff;
        _buffer[1] = index & 0xff;

        /* fill the buffer with data bytes */
        memcpy(_buffer + 2, pdata, cnt);

        if (i2c_write_bytes(dev->params.i2c_dev, VL53L1X_I2C_ADDRESS, _buffer, cnt + 2, 0) != 0) {
            i2c_release(dev->params.i2c_dev);
            return VL53L1X_ERROR_I2C;
        }
        len -= cnt;
        pdata += cnt;
        index += cnt;
    }

    /* release the I2C device */
    i2c_release(dev->params.i2c_dev);

    return VL53L1X_OK;
}

static int _update(const vl53l1x_t *dev, uint16_t index, uint8_t mask,
                                                         uint8_t data)
{
    uint8_t byte;
    EXEC_RET(_read_byte(dev, index, &byte));
    byte &= mask;
    byte |= data;
    EXEC_RET(_write_byte(dev, index, byte));

    return VL53L1X_OK;
}

#endif /* !IS_USED(MODULE_VL53L1X_ST_API) */
