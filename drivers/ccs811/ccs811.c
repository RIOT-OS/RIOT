/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ccs811
 * @brief       Device Driver for AMS CCS811 digital gas sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "xtimer.h"

#include "ccs811_regs.h"
#include "ccs811.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * Internal macro definitions
 */

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[ccs811] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#define DEBUG_DEV(f, d, ...) \
    DEBUG("[ccs811] %s dev=%d addr=%02x: " f "\n", \
          __func__, d->params.i2c_dev, d->params.i2c_addr, ## __VA_ARGS__)

#define ERROR_DEV(f, d, ...) \
    LOG_ERROR("[ccs811] dev=%d addr=%x: " f "\n", \
              d->params.i2c_dev, d->params.i2c_addr, ## __VA_ARGS__)

/**
 * Internal type declarations
 */

typedef struct {
    uint8_t reserved_1 : 2;
    uint8_t int_thresh : 1; /**< interrupt if new ALG_RESULT_DAT crosses on of the thresholds */
    uint8_t int_datardy: 1; /**< interrupt if new sample is ready in ALG_RESULT_DAT  */
    uint8_t drive_mode : 3; /**< mode number binary coded */
} ccs811_meas_mode_reg_t;

/**
 * forward declaration of functions for internal use only
 */
static int _reg_read(const ccs811_t *dev, uint8_t reg, uint8_t *data, uint32_t len);
static int _reg_write(const ccs811_t *dev, uint8_t reg, uint8_t *data, uint32_t len);
static int _check_error_status(const ccs811_t *dev);
static int _error_code(const ccs811_t *dev, uint8_t err_reg);
static int _is_available(const ccs811_t *dev);

int ccs811_init(ccs811_t *dev, const ccs811_params_t *params)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);

    /* init sensor data structure */
    dev->params = *params;

    int res = CCS811_OK;

    if (dev->params.reset_pin != GPIO_UNDEF &&
        gpio_init(dev->params.reset_pin, GPIO_OUT) == 0) {
        DEBUG_DEV("nRESET pin configured", dev);
        /* enable low active reset signal */
        gpio_clear(dev->params.reset_pin);
        /* t_RESET (reset impuls) has to be at least 20 us, we wait 1 ms */
        xtimer_usleep(1000);
        /* disable low active reset signal */
        gpio_set(dev->params.reset_pin);
        /* t_START after reset is 1 ms, we wait 1 further ms */
        xtimer_usleep(1000);
    }

    if (dev->params.wake_pin != GPIO_UNDEF &&
        gpio_init(dev->params.wake_pin, GPIO_OUT) == 0) {
        gpio_clear(dev->params.wake_pin);
        DEBUG_DEV("nWAKE pin configured", dev);
    }

    /* check whether sensor is available including the check of the hardware id */
    if ((res = _is_available(dev)) != CCS811_OK) {
        return res;
    }

    static const uint8_t sw_reset[4] = { 0x11, 0xe5, 0x72, 0x8a };

    /* doing a software reset first */
    if (_reg_write(dev, CCS811_REG_SW_RESET, (uint8_t *)sw_reset, 4) != CCS811_OK) {
        DEBUG_DEV("could not write software reset command "
                  "to register CCS811_REG_SW_RESET", dev);
        return -CCS811_ERROR_I2C;
    }

    uint8_t status;

    /* wait 100 ms after the reset */
    xtimer_usleep(100000);

    /* get the status to check whether sensor is in bootloader mode */
    if (_reg_read(dev, CCS811_REG_STATUS, &status, 1) != CCS811_OK) {
        DEBUG_DEV("could not read register CCS811_REG_STATUS", dev);
        return -CCS811_ERROR_I2C;
    }

    /*
     * if sensor is in bootloader mode (FW_MODE == 0), it has to switch
     * to the application mode first
     */
    if (!(status & CCS811_STATUS_FW_MODE)) {
        /* check whether valid application firmware is loaded */
        if (!(status & CCS811_STATUS_APP_VALID)) {
            DEBUG_DEV("sensor is in boot mode, but has no app", dev);
            return -CCS811_ERROR_NO_APP;
        }

        /* switch to application mode */
        if (_reg_write(dev, CCS811_REG_APP_START, 0, 0) != CCS811_OK) {
            DEBUG_DEV("could not write app start command "
                      "to register CCS811_REG_APP_START", dev);
            return -CCS811_ERROR_I2C;
        }

        /* wait 100 ms after starting the app */
        xtimer_usleep(100000);

        /* get the status to check whether sensor switched to application mode */
        if (_reg_read(dev, CCS811_REG_STATUS, &status, 1) != CCS811_OK) {
            DEBUG_DEV("could not read register CCS811_REG_STATUS", dev);
            return -CCS811_ERROR_I2C;
        }
        if ((status & CCS811_STATUS_FW_MODE) == 0) {
            DEBUG_DEV("could not start application", dev);
            return -CCS811_ERROR_NO_APP;
        }
    }

#if MODULE_CCS811_FULL
    /* try to set interrupt mode */
    if (dev->params.int_mode != CCS811_INT_NONE &&
        (res = ccs811_set_int_mode (dev, dev->params.int_mode)) != CCS811_OK) {
        return res;
    }
#endif /* MODULE_CCS811_FULL */

    /* try to set default measurement mode */
    return ccs811_set_mode(dev, dev->params.mode);
}


int ccs811_set_mode(ccs811_t *dev, ccs811_mode_t mode)
{
    ASSERT_PARAM(dev != NULL);

    ccs811_meas_mode_reg_t reg;

    /* read measurement mode register value */
    if (_reg_read(dev, CCS811_REG_MEAS_MODE, (uint8_t *)&reg, 1) != CCS811_OK) {
        DEBUG_DEV("could not read current measurement mode "
                  "from register CCS811_REG_MEAS_MODE", dev);
        return -CCS811_ERROR_I2C;
    }

    reg.drive_mode = mode;

    /* write back measurement mode register */
    if (_reg_write(dev, CCS811_REG_MEAS_MODE, (uint8_t *)&reg, 1) != CCS811_OK) {
        DEBUG_DEV("could not write new measurement mode "
                  "to register CCS811_REG_MEAS_MODE", dev);
        return -CCS811_ERROR_I2C;
    }

    /* check whether setting measurement mode were successful */
    if (_reg_read(dev, CCS811_REG_MEAS_MODE, (uint8_t *)&reg, 1) != CCS811_OK) {
        DEBUG_DEV("could not read new measurement mode "
                  "from register CCS811_REG_MEAS_MODE", dev);
        return -CCS811_ERROR_I2C;
    }
    if (reg.drive_mode != mode) {
        DEBUG_DEV("could not set measurement mode to %d", dev, mode);
        return -CCS811_ERROR_MEASMODE_INV;
    }

    dev->params.mode = mode;

    return CCS811_OK;
}

#if MODULE_CCS811_FULL

int ccs811_set_int_mode(ccs811_t *dev, ccs811_int_mode_t mode)
{
    ASSERT_PARAM(dev != NULL);

    if (dev->params.int_pin == GPIO_UNDEF) {
        DEBUG_DEV("nINT pin not configured", dev);
        return CCS811_ERROR_NO_INT_PIN;
    }

    ccs811_meas_mode_reg_t reg;

    /* read measurement mode register value */
    if (_reg_read(dev, CCS811_REG_MEAS_MODE, (uint8_t *)&reg, 1) != CCS811_OK) {
        DEBUG_DEV("could not set interrupt mode, could not read register "
                  "CCS811_REG_MEAS_MODE", dev);
        return CCS811_ERROR_I2C;
    }

    reg.int_datardy = mode != CCS811_INT_NONE;
    reg.int_thresh  = mode == CCS811_INT_THRESHOLD;

    /* write back measurement mode register */
    if (_reg_write(dev, CCS811_REG_MEAS_MODE, (uint8_t *)&reg, 1) != CCS811_OK) {
        DEBUG_DEV("could not set interrupt mode, could not write register "
                  "CCS811_REG_MEAS_MODE", dev);
        return CCS811_ERROR_I2C;
    }

    dev->params.int_mode = mode;

    return CCS811_OK;
}

#endif /* MODULE_CCS811_FULL */

int ccs811_data_ready(const ccs811_t *dev)
{
    uint8_t status;

    /* check status register */
    if (_reg_read(dev, CCS811_REG_STATUS, &status, 1) != CCS811_OK) {
        DEBUG_DEV("could not read CCS811_REG_STATUS", dev);
        return -CCS811_ERROR_I2C;
    }

    if ((status & CCS811_STATUS_DATA_RDY)) {
        /* new data available */
        return CCS811_OK;
    }

    return CCS811_ERROR_NO_NEW_DATA;
}

#define CCS811_ALG_DATA_ECO2_HB   0
#define CCS811_ALG_DATA_ECO2_LB   1
#define CCS811_ALG_DATA_TVOC_HB   2
#define CCS811_ALG_DATA_TVOC_LB   3
#define CCS811_ALG_DATA_STATUS    4
#define CCS811_ALG_DATA_ERROR_ID  5
#define CCS811_ALG_DATA_RAW_HB    6
#define CCS811_ALG_DATA_RAW_LB    7

int ccs811_read_iaq(const ccs811_t *dev,
                    uint16_t *iaq_tvoc, uint16_t *iaq_eco2,
                    uint16_t *raw_i, uint16_t *raw_v)
{
    ASSERT_PARAM(dev != NULL);

    int res = CCS811_OK;

    if (dev->params.mode == CCS811_MODE_IDLE) {
        DEBUG_DEV("sensor is in idle mode and not performing "
                  "measurements", dev);
        return -CCS811_ERROR_MEASMODE_INV;
    }

    if (dev->params.mode == CCS811_MODE_250MS && (iaq_tvoc || iaq_eco2)) {
        DEBUG_DEV("sensor is in constant power mode, only raw data "
                  "are available every 250ms", dev);
        return -CCS811_ERROR_NO_IAQ_DATA;
    }

    uint8_t data[8];

    /* read IAQ sensor values and RAW sensor data including status and error id */
    if (_reg_read(dev, CCS811_REG_ALG_RESULT_DATA, data, 8) != CCS811_OK) {
        DEBUG_DEV("could not read sensor data from "
                  "register CCS811_REG_ALG_RESULT_DATA", dev);
        return -CCS811_ERROR_I2C;
    }

    /* check for errors */
    if (data[CCS811_ALG_DATA_STATUS] & CCS811_STATUS_ERROR) {
        return _error_code(dev, data[CCS811_ALG_DATA_ERROR_ID]);
    }

    /*
     * check whether new data are ready to read; if not, latest values read
     * from sensor are used and error code CCS811_ERROR_NO_NEW_DATA is returned
     */
    if (!(data[CCS811_ALG_DATA_STATUS] & CCS811_STATUS_DATA_RDY)) {
        DEBUG_DEV("no new data", dev);
        res = -CCS811_ERROR_NO_NEW_DATA;
    }

    /* if *iaq* is not NULL return IAQ sensor values */
    if (iaq_tvoc) {
        *iaq_tvoc  = data[CCS811_ALG_DATA_TVOC_HB] << 8;
        *iaq_tvoc |= data[CCS811_ALG_DATA_TVOC_LB];
    }
    if (iaq_eco2) {
        *iaq_eco2  = data[CCS811_ALG_DATA_ECO2_HB] << 8;
        *iaq_eco2 |= data[CCS811_ALG_DATA_ECO2_LB];
    }

    /* if *raw* is not NULL return RAW sensor data */
    if (raw_i) {
        *raw_i = data[CCS811_ALG_DATA_RAW_HB] >> 2;
    }
    if (raw_v) {
        *raw_v  = (data[CCS811_ALG_DATA_RAW_HB] & 0x03) << 8;
        *raw_v |= data[CCS811_ALG_DATA_RAW_LB];
    }

    return res;
}

#if MODULE_CCS811_FULL

int ccs811_read_ntc(const ccs811_t *dev, uint32_t r_ref, uint32_t *r_ntc)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(r_ntc != NULL);

    uint8_t data[4];

    /* read baseline register */
    if (_reg_read(dev, CCS811_REG_NTC, data, 4) != CCS811_OK) {
        DEBUG_DEV("could not read the V_REF and V_NTC "
                  "from register CCS811_REG_NTC", dev);
        return -CCS811_ERROR_I2C;
    }

    /* calculation from application note ams AN000372 */
    uint32_t v_ref = (uint16_t)(data[0]) << 8 | data[1];
    uint32_t v_ntc = (uint16_t)(data[2]) << 8 | data[3];

    *r_ntc = v_ntc * r_ref / v_ref;
    return CCS811_OK;
}

#endif /* MODULE_CCS811_FULL */

int ccs811_power_down (ccs811_t *dev)
{
    ASSERT_PARAM(dev != NULL);

    ccs811_mode_t tmp_mode = dev->params.mode;
    int res = ccs811_set_mode(dev, CCS811_MODE_IDLE);
    dev->params.mode = tmp_mode;

    if (dev->params.wake_pin != GPIO_UNDEF) {
        DEBUG_DEV("Setting nWAKE pin high", dev);
        gpio_set(dev->params.wake_pin);
    }

    return res;
}

int ccs811_power_up (ccs811_t *dev)
{
    ASSERT_PARAM(dev != NULL);

    if (dev->params.wake_pin != GPIO_UNDEF) {
        DEBUG_DEV("Setting nWAKE pin low", dev);
        gpio_clear(dev->params.wake_pin);
    }

    return ccs811_set_mode(dev, dev->params.mode);
}

#if MODULE_CCS811_FULL

int ccs811_set_environmental_data(const ccs811_t *dev,
                                  int16_t temp, int16_t hum)
{
    ASSERT_PARAM(dev != NULL);

    temp = (((uint32_t)temp + 2500) << 9) / 100; /* -25 °C maps to 0 */
    hum  = ((uint32_t)hum << 9) / 100;

    /* fill environmental data */
    uint8_t data[4]  = { temp >> 8, temp & 0xff,
                         hum  >> 8, hum  & 0xff  };

    /* send environmental data to the sensor */
    if (_reg_write(dev, CCS811_REG_ENV_DATA, data, 4) != CCS811_OK) {
        DEBUG_DEV("could not write environmental data "
                  "to register CCS811_REG_ENV_DATA", dev);
        return CCS811_ERROR_I2C;
    }

    return CCS811_OK;
}

int ccs811_set_eco2_thresholds(const ccs811_t *dev,
                               uint16_t low, uint16_t high, uint8_t hyst)
{
    ASSERT_PARAM(dev != NULL);

    /* check parameters */
    if (low < CCS811_ECO2_RANGE_MIN ||
        high > CCS811_ECO2_RANGE_MAX || low > high || !hyst) {
        DEBUG_DEV("wrong threshold parameters", dev);
        return CCS811_ERROR_THRESH_INV;
    }

    /* fill the threshold data */
    uint8_t data[5] = { low  >> 8, low  & 0xff,
                        high >> 8, high & 0xff,
                        hyst };

    /* write threshold data to the sensor */
    if (_reg_write(dev, CCS811_REG_THRESHOLDS, data, 5) != CCS811_OK) {
        DEBUG_DEV("could not set threshold interrupt parameters, "
                  "could not write register CCS811_REG_THRESHOLDS", dev);
        return CCS811_ERROR_I2C;
    }

    return CCS811_OK;
}

int ccs811_get_baseline(const ccs811_t *dev, uint16_t *base)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(base != NULL);

    uint8_t data[2];

    /* read baseline register */
    if (_reg_read(dev, CCS811_REG_BASELINE, data, 2) != CCS811_OK) {
        DEBUG_DEV("could not get current baseline value, "
                  "could not read register CCS811_REG_BASELINE", dev);
        return CCS811_ERROR_I2C;
    }

    *base = (uint16_t)(data[0]) << 8 | data[1];

    return CCS811_OK;
}

int ccs811_set_baseline(const ccs811_t *dev, uint16_t baseline)
{
    ASSERT_PARAM(dev != NULL);

    uint8_t data[2] = { baseline >> 8, baseline & 0xff };

    /* write baseline register */
    if (_reg_write(dev, CCS811_REG_THRESHOLDS, data, 5) != CCS811_OK) {
        DEBUG_DEV("could not set baseline value, "
                  "could not write register CCS811_REG_BASELINE", dev);
        return CCS811_ERROR_I2C;
    }

    return CCS811_OK;
}

#endif /* MODULE_CCS811_FULL */

/**
 * function for internal use only
 */

static int _reg_read(const ccs811_t *dev, uint8_t reg, uint8_t *data, uint32_t len)
{
    DEBUG_DEV("read %"PRIu32" bytes from sensor registers starting at addr %02x",
              dev, len, reg);

    int res = CCS811_OK;

    if (i2c_acquire(dev->params.i2c_dev) != CCS811_OK) {
        DEBUG_DEV("could not acquire I2C bus", dev);
        return -CCS811_ERROR_I2C;
    }

#if MODULE_CCS811_FULL
    if (dev->params.wake_pin != GPIO_UNDEF) {
        /* wake the sensor with low active WAKE signal */
        gpio_clear(dev->params.wake_pin);
        /* t_WAKE is 50 us */
        xtimer_usleep(50);
    }
#endif

    res = i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr, reg, data, len, 0);
    i2c_release(dev->params.i2c_dev);

#if MODULE_CCS811_FULL
    if (dev->params.wake_pin != GPIO_UNDEF) {
        /* let the sensor enter to sleep mode */
        gpio_set(dev->params.wake_pin);
        /* minimum t_DWAKE is 20 us */
        xtimer_usleep(20);
    }
#endif

    if (res == CCS811_OK) {
        if (ENABLE_DEBUG) {
            printf("[ccs811] %s dev=%d addr=%02x: read following bytes: ",
                   __func__, dev->params.i2c_dev, dev->params.i2c_addr);
            for (unsigned i = 0; i < len; i++) {
                    printf("%02x ", data[i]);
            }
            printf("\n");
        }
    }
    else {
        DEBUG_DEV("could not read %"PRIu32" bytes from sensor registers "
                  "starting at addr %02x, reason %i", dev, len, reg, res);
        return -CCS811_ERROR_I2C;
    }

    return CCS811_OK;
}

static int _reg_write(const ccs811_t *dev, uint8_t reg, uint8_t *data, uint32_t len)
{
    DEBUG_DEV("write %"PRIu32" bytes to sensor registers starting at addr %02x",
              dev, len, reg);

    int res = CCS811_OK;

    if (ENABLE_DEBUG && data && len) {
        printf("[css811] %s dev=%d addr=%02x: write following bytes: ",
               __func__, dev->params.i2c_dev, dev->params.i2c_addr);
        for (unsigned i = 0; i < len; i++) {
            printf("%02x ", data[i]);
        }
        printf("\n");
    }

    if (i2c_acquire(dev->params.i2c_dev)) {
        DEBUG_DEV("could not acquire I2C bus", dev);
        return -CCS811_ERROR_I2C;
    }

#if MODULE_CCS811_FULL
    if (dev->params.wake_pin != GPIO_UNDEF) {
        /* wake the sensor with low active WAKE signal */
        gpio_clear(dev->params.wake_pin);
        /* t_WAKE is 50 us */
        xtimer_usleep(50);
    }
#endif

    if (!data || !len) {
        res = i2c_write_byte(dev->params.i2c_dev, dev->params.i2c_addr, reg, 0);
    }
    else {
        res = i2c_write_regs(dev->params.i2c_dev, dev->params.i2c_addr, reg, data, len, 0);
    }
    i2c_release(dev->params.i2c_dev);

#if MODULE_CCS811_FULL
    if (dev->params.wake_pin != GPIO_UNDEF) {
        /* let the sensor enter to sleep mode */
        gpio_set(dev->params.wake_pin);
        /* minimum t_DWAKE is 20 us */
        xtimer_usleep(20);
    }
#endif

    if (res != CCS811_OK) {
        DEBUG_DEV("could not write %"PRIu32" bytes to sensor registers "
                  "starting at addr %02x, reason %i", dev, len, reg, res);
        return -CCS811_ERROR_I2C;
    }

    return CCS811_OK;
}

static int _error_code(const ccs811_t *dev, uint8_t err_reg)
{
    if (err_reg & CCS811_ERR_WRITE_REG_INV) {
        DEBUG_DEV("invalid register address on write", dev);
        return -CCS811_ERROR_WRITE_REG_INV;
    }

    if (err_reg & CCS811_ERR_READ_REG_INV) {
        DEBUG_DEV("invalid register address on read", dev);
        return -CCS811_ERROR_READ_REG_INV;
    }

    if (err_reg & CCS811_ERR_MEASMODE_INV) {
        DEBUG_DEV("invalid requested measurement mode", dev);
        return -CCS811_ERROR_MEASMODE_INV;
    }

    if (err_reg & CCS811_ERR_MAX_RESISTANCE) {
        DEBUG_DEV("sensor resistance measurement has reached "
                  "or exceeded the maximum range", dev);
        return -CCS811_ERROR_MAX_RESISTANCE;
    }

    if (err_reg & CCS811_ERR_HEATER_FAULT) {
        DEBUG_DEV("heater current not in range", dev);
        return -CCS811_ERROR_HEATER_FAULT;
    }

    if (err_reg & CCS811_ERR_HEATER_SUPPLY) {
        DEBUG_DEV("heater voltage is not being applied correctly", dev);
        return -CCS811_ERROR_HEATER_SUPPLY;
    }

    return CCS811_OK;
}

static int _check_error_status(const ccs811_t *dev)
{
    uint8_t status;
    uint8_t err_reg;

    /* check status register */
    if (_reg_read(dev, CCS811_REG_STATUS, &status, 1) != CCS811_OK) {
        DEBUG_DEV("could not read CCS811_REG_STATUS", dev);
        return -CCS811_ERROR_I2C;
    }

    if (!(status & CCS811_STATUS_ERROR)) {
        /* everything is OK */
        return CCS811_OK;
    }

    /* Check the error id register */
    if (_reg_read(dev, CCS811_REG_ERROR_ID, &err_reg, 1) != CCS811_OK) {
        DEBUG_DEV("could not read CCS811_REG_ERROR_ID", dev);
        return -CCS811_ERROR_I2C;
    }

    if (err_reg != 0) {
        return _error_code(dev, err_reg);
    }

    return CCS811_OK;
}

static int _is_available(const ccs811_t *dev)
{
    uint8_t reg_data[5];

    /* check hardware id (register 0x20) and hardware version (register 0x21) */
    if (_reg_read(dev, CCS811_REG_HW_ID, reg_data, 5) != CCS811_OK) {
        DEBUG_DEV("could not read CCS811_REG_HW_ID", dev);
        return -CCS811_ERROR_I2C;
    }

    if (reg_data[0] != CCS811_HW_ID) {
        DEBUG_DEV("wrong hardware ID %02x, should be %02x",
                  dev, reg_data[0], CCS811_HW_ID);
        return -CCS811_ERROR_NO_DEV;
    }

    DEBUG_DEV("hardware version:      %02x", dev, reg_data[1]);
    DEBUG_DEV("firmware boot version: %02x", dev, reg_data[3]);
    DEBUG_DEV("firmware app version:  %02x", dev, reg_data[4]);

    return _check_error_status(dev);
}
