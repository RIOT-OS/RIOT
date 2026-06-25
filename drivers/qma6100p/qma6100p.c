/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-FileCopyrightText: 2026 Léandre Le Duc <leandre.leduc38@gmail.com>
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
 * @author      Léandre Le Duc <leandre.leduc38@gmail.com>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "qma6100p.h"
#include "qma6100p_params.h"
#include "qma6100p_regs.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define BUS                       (dev->params.i2c)
#define ADDR                      (dev->params.addr)

#define QMA6100P_OTP_LOAD_RETRIES (10U)

/**
 * @brief   Read a register and jump to a label on failure
 *
 * @warning Requires in calling scope: `int res`, label `out_label`, macros `BUS` and `ADDR`
 *          On failure, `res` holds the error code and execution jumps to `out_label`
*/
#define READ_REG(reg_addr, reg_val, out_label)                                     \
    do {                                                                           \
        res = _read_reg(BUS, ADDR, (reg_addr), &(reg_val));                        \
        if (res < 0) {                                                             \
            DEBUG("[qma6100p] %s: failed to read 0x%02x\n", __func__, (reg_addr)); \
            goto out_label;                                                        \
        }                                                                          \
    } while (0)

/**
 * @brief   Write a register and jump to a label on failure
 *
 * @warning Requires in calling scope: `int res`, label `out_label`, macros `BUS` and `ADDR`
 *          On failure, `res` holds the error code and execution jumps to `out_label`
*/
#define WRITE_REG(reg_addr, reg_val, out_label)                                     \
    do {                                                                            \
        res = _write_reg(BUS, ADDR, (reg_addr), (reg_val));                         \
        if (res < 0) {                                                              \
            DEBUG("[qma6100p] %s: failed to write 0x%02x\n", __func__, (reg_addr)); \
            goto out_label;                                                         \
        }                                                                           \
    } while (0)

/**
 * @brief Read one register over I2C, mapping errors to QMA6100P return codes
 *
 * @param[in]  i2c        I2C device
 * @param[in]  addr       I2C address
 * @param[in]  reg_addr   register address to read
 * @param[out] reg_value  value read from the register
 *
 * @return  QMA6100P_OK on success
 * @return  QMA6100P_NODEV if device not found
 * @return  QMA6100P_INVALID_ARG on invalid argument
 * @return  QMA6100P_NOI2C on other I2C error
 */
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

/**
 * @brief Write one register over I2C, mapping errors to QMA6100P return codes
 *
 * @param[in] i2c        I2C device
 * @param[in] addr       I2C address
 * @param[in] reg_addr   register address to write
 * @param[in] reg_value  value to write
 *
 * @return  QMA6100P_OK on success
 * @return  QMA6100P_NODEV if device not found
 * @return  QMA6100P_INVALID_ARG on invalid argument
 * @return  QMA6100P_NOI2C on other I2C error
 */
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

    if ((reg & QMA6100P_CHIP_ID_MASK) != QMA6100P_CHIP_ID_VAL) {
        DEBUG("[qma6100p] init - error: wrong chip id 0x%02x\n", reg);
        res = QMA6100P_NODEV;
        goto out;
    }
out:
    i2c_release(i2c);
    return res;
}

/**
 * @brief Execute the device soft reset
 *
 * @param[in] dev  device descriptor
 *
 * @return  0 on success
 * @return  negative error code on failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _soft_reset(const qma6100p_t *dev)
{
    int res;

    WRITE_REG(QMA6100P_REG_SW_RESET, QMA6100P_SW_RESET_VAL, out);

    ztimer_sleep(ZTIMER_MSEC, 1);

    WRITE_REG(QMA6100P_REG_SW_RESET, 0x00, out);

    uint8_t nvm_status;

    /* Poll for OTP load completion, datasheet specifies no explicit timeout.
     * Loading completes within a few I2C reads after reset */
    unsigned retries = QMA6100P_OTP_LOAD_RETRIES;

    do {
        READ_REG(QMA6100P_REG_NVM, nvm_status, out);
        if ((nvm_status & (QMA6100P_NVM_LOAD_DONE | QMA6100P_NVM_RDY)) ==
            (QMA6100P_NVM_LOAD_DONE | QMA6100P_NVM_RDY)) {
            break;
        }
    } while (--retries);

    if (!retries) {
        res = QMA6100P_TIMEOUT;
        goto out;
    }

out:
    return res;
}

/**
 * @brief Execute the init sequence as described in the section 6.3 of the spec.
 *
 * @param[in] dev  device descriptor
 *
 * @return  0 on success
 * @return  negative error code on failure
 */
static int _qma6100p_run_init_seq(const qma6100p_t *dev)
{
    int res;

    i2c_acquire(BUS);

    uint8_t chip_state;
    do {
        res = _soft_reset(dev);
        if (res < 0) {
            goto out;
        }
        READ_REG(QMA6100P_REG_CHIP_STATE, chip_state, out);
    } while ((chip_state >> 4) != 0x0C);

    uint8_t pm = 0;

    /* Enters Active Mode */
    FIELD_SET(QMA6100P_PM_MODE_MASK, 1, pm);
    WRITE_REG(QMA6100P_REG_PM, pm, out);

    FIELD_SET(QMA6100P_PM_MCLK_MASK, QMA6100P_PM_MCLK_51K2, pm);
    WRITE_REG(QMA6100P_REG_PM, pm, out);

    WRITE_REG(QMA6100P_REG_TST0_ANA, 0x20, out);
    WRITE_REG(QMA6100P_REG_AFE_ANA, 0x01, out);
    WRITE_REG(QMA6100P_REG_TST1_ANA, 0x80, out);

    ztimer_sleep(ZTIMER_MSEC, 1);

    WRITE_REG(QMA6100P_REG_TST1_ANA, 0x00, out);

out:
    i2c_release(BUS);
    return res;
}

/**
 * @brief Configure full scale range of the device
 *
 *
 * @param[in,out] dev         device descriptor
 * @param[in]     range       requested scale range
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _qma6100p_set_range(const qma6100p_t *dev, qma6100p_range_t range)
{
    int res;
    uint8_t range_reg;

    READ_REG(QMA6100P_REG_RANGE, range_reg, out);

    FIELD_SET(QMA6100P_RANGE_MASK, range, range_reg);

    WRITE_REG(QMA6100P_REG_RANGE, range_reg, out);

out:
    return res;
}

/**
 * @brief Configure output data rate.
 *
 *
 * @param[in,out] dev         device descriptor
 * @param[in]     odr         requested output data rate
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _qma6100p_set_odr(const qma6100p_t *dev, qma6100p_odr_t odr)
{
    int res;
    uint8_t odr_reg;

    READ_REG(QMA6100P_REG_ODR, odr_reg, out);

    FIELD_SET(QMA6100P_ODR_MASK, odr, odr_reg);

    WRITE_REG(QMA6100P_REG_ODR, odr_reg, out);

out:
    return res;
}

/**
 * @brief Configure master clock frequency.
 *
 * @param[in,out] dev         device descriptor
 * @param[in]     mclk        requested master clock
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _qma6100p_set_mclk(const qma6100p_t *dev, qma6100p_mclk_t mclk)
{
    int res;
    uint8_t pm_reg;

    READ_REG(QMA6100P_REG_PM, pm_reg, out);

    FIELD_SET(QMA6100P_PM_MCLK_MASK, mclk, pm_reg);

    WRITE_REG(QMA6100P_REG_PM, pm_reg, out);

out:
    return res;
}

/**
 * @brief Set all the common parameter requested by the user
 *
 * Sets full scale range, output data rate and master clock from @p params
 *
 * @param[in,out] dev         device descriptor
 * @param[in]     params      configuration parameters
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 */
static int _qma6100p_set_common_params(const qma6100p_t *dev, const qma6100p_params_t *params)
{
    int res;

    i2c_acquire(BUS);

    res = _qma6100p_set_odr(dev, params->rate);
    if (res < 0) {
        goto out;
    }

    res = _qma6100p_set_range(dev, params->range);
    if (res < 0) {
        goto out;
    }

    res = _qma6100p_set_mclk(dev, params->mclk);
    if (res < 0) {
        goto out;
    }

out:
    i2c_release(BUS);
    return res;
}

int qma6100p_init(qma6100p_t *dev, const qma6100p_params_t *params)
{
    assert(dev && params);
    assert(params->addr == QMA6100P_I2C_ADDR_LOW || params->addr == QMA6100P_I2C_ADDR_HIGH);

    DEBUG("[qma6100p] init - i2c=%d, addr=0x%02x\n", params->i2c, params->addr);

    int res = _qma6100p_init_test(params->i2c, params->addr);
    if (res < 0) {
        return res;
    }
    dev->params = *params;

    res = _qma6100p_run_init_seq(dev);
    if (res < 0) {
        DEBUG("[qma6100p] init: init sequence failed (%d)\n", res);
        return res;
    }

    res = _qma6100p_set_common_params(dev, params);
    if (res < 0) {
        DEBUG("[qma6100p] init: set common parameters failed (%d)\n", res);
        return res;
    }

    return QMA6100P_OK;
}

/**
 * @brief Convert read output data into a 14 bit signed data.
 *
 * Merge two uint8_t register getting all [7:0] from @p msb and
 * [7:2] from @p lsb and convert it into a int16_t signed value.
 *
 * @param[in]  lsb      lsb of the data register
 * @param[in]  msb      msb of the data register
 *
 * @return 14 bits signed data
 */
static inline int16_t _to_signed14(uint8_t lsb, uint8_t msb)
{
    uint16_t raw = (uint16_t)msb << 8 | lsb;
    return (int16_t)((int16_t)raw >> 2);
}

/**
 * @brief Read the raw output data of the device and write them in the given buffer
 *
 * @param[in,out] dev         device descriptor
 * @param[out]    data        raw data buffer
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 */
int qma6100p_read_raw(const qma6100p_t *dev, qma6100p_raw_data_t *data)
{
    assert(dev && data);

    int res;
    uint8_t buf[6];
    uint8_t new_data;

    i2c_acquire(BUS);

    /* Burst read 6 bytes to ensure atomic update of X/Y/Z samples (avoid LSB/MSB mismatch) */
    res = i2c_read_regs(BUS, ADDR, QMA6100P_REG_DX_LSB, buf, 6, 0);
    if (res < 0) {
        goto out;
    }

    new_data =
        (buf[0] | buf[2] | buf[4]) & QMA6100P_NEWDATA_FLAG_MASK;

    if (!new_data) {
        res = QMA6100P_NO_NEW_DATA;
        goto out;
    }

    data->x = _to_signed14(buf[0], buf[1]);
    data->y = _to_signed14(buf[2], buf[3]);
    data->z = _to_signed14(buf[4], buf[5]);
    res = QMA6100P_DATA_READY;

out:
    i2c_release(BUS);
    return res;
}

/**
 * @brief Converts to ug a raw data value given the full scale range
 *
 * Multiplies the raw 14-bit signed ADC value by the resolution (ug/LSB)
 * corresponding to the configured full scale range.
 *
 * @param raw_value raw data to convert in ug
 * @param range     full scale range used
 *
 * @return converted to ug value
 */
static int32_t _convert_to_ug(int16_t raw_value, qma6100p_range_t range)
{
    int32_t resolution;

    switch (range) {
    case (QMA6100P_RANGE_2G):
        resolution = QMA6100P_2G_RESOLUTION;
        break;

    case (QMA6100P_RANGE_4G):
        resolution = QMA6100P_4G_RESOLUTION;
        break;

    case (QMA6100P_RANGE_8G):
        resolution = QMA6100P_8G_RESOLUTION;
        break;

    case (QMA6100P_RANGE_16G):
        resolution = QMA6100P_16G_RESOLUTION;
        break;

    case (QMA6100P_RANGE_32G):
        resolution = QMA6100P_32G_RESOLUTION;
        break;
    default:
        resolution = QMA6100P_2G_RESOLUTION;
        break;
    }
    return raw_value * resolution;
}

int qma6100p_read(const qma6100p_t *dev, qma6100p_data_t *data)
{
    assert(dev && data);

    int res;
    qma6100p_raw_data_t raw_data;

    res = qma6100p_read_raw(dev, &raw_data);

    if (res == QMA6100P_NO_NEW_DATA) {
        return res;
    }

    if (res < 0) {
        DEBUG("[qma6100p] read: read raw data failed (%d)\n", res);
        return res;
    }

    data->x = _convert_to_ug(raw_data.x, dev->params.range);
    data->y = _convert_to_ug(raw_data.y, dev->params.range);
    data->z = _convert_to_ug(raw_data.z, dev->params.range);

    return res;
}

/**
 * @brief Disable all configured interrupts on the device
 *
 * @param[in] dev  device descriptor
 *
 * @return  0 on success
 * @return  negative error code on failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _disable_all_interrupt(const qma6100p_t *dev)
{
    int res;
    WRITE_REG(QMA6100P_REG_INT_EN1, 0x00, out);
out:
    return res;
}

/**
 * @brief Write register sequence to put device into Ultra Low Power State (ULPS)
 *
 * @param[in] dev  device descriptor
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 */
static int _enter_ulps_mode(const qma6100p_t *dev)
{
    int res = QMA6100P_OK;

    i2c_acquire(BUS);

    WRITE_REG(QMA6100P_REG_PM, 0x87, out);
    WRITE_REG(QMA6100P_REG_ULPS, 0x0F, out);
    WRITE_REG(QMA6100P_REG_TST0_ANA, 0x00, out);

    res = _disable_all_interrupt(dev);
    if (res < 0) {
        DEBUG("[qma6100p] %s: failed to disable interrupt\n", __func__);
        goto out;
    }

out:
    i2c_release(BUS);
    return res;
}

/**
 * @brief Configure interrupt behavior in INT_CFG register
 *
 * Sets the data shadowing mode from @ref qma6100p_params_t::interrupt_shadow
 *
 * @param[in,out] dev  device descriptor
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _set_int_params(qma6100p_t *dev)
{
    int res = QMA6100P_OK;
    uint8_t int_reg;

    READ_REG(QMA6100P_REG_INT_CFG, int_reg, out);

    FIELD_SET(QMA6100P_INT_CFG_SHADOW_MASK, dev->params.interrupt_shadow, int_reg);

    WRITE_REG(QMA6100P_REG_INT_CFG, int_reg, out);

out:
    return res;
}

/**
 * @brief Switch the device between Ultra-Low Power State (ULPS) and active mode
 *
 * @param[in,out] dev        device descriptor
 * @param[in]     low_power  true to enter ULPS, false to enter active mode
 *
 * @return  0 on success
 * @return  negative error code on failure
 */
static int _qma6100p_set_power_mode(qma6100p_t *dev, bool low_power)
{
    assert(dev);

    int res;

    if (low_power) {
        res = _enter_ulps_mode(dev);
        if (res < 0) {
            DEBUG("[qma6100p] set_power_mode - error: failed to enter ulps (%d)\n", res);
        }
        goto out;
    }

    /* Exiting ULPS requires a full soft reset according to the spec */
    res = _qma6100p_run_init_seq(dev);
    if (res < 0) {
        DEBUG("[qma6100p] set_power_mode - error: init sequence failed (%d)\n", res);
        goto out;
    }

    res = _qma6100p_set_common_params(dev, &dev->params);
    if (res < 0) {
        DEBUG("[qma6100p] set_power_mode - error: failed to restore params (%d)\n", res);
    }

out:
    return res;
}

int qma6100p_set_low_power(qma6100p_t *dev)
{
    return _qma6100p_set_power_mode(dev, true);
}

int qma6100p_set_active_mode(qma6100p_t *dev)
{
    return _qma6100p_set_power_mode(dev, false);
}

/**
 * @brief Configure INTPIN_CONF register and resolve the MAP register for the selected INT pin
 *
 * Sets active level and output mode (push-pull/open-drain) for the chosen INT pin,
 * and writes @p map_reg with the corresponding INTx_MAP1 register address
 *
 * @param[in]  dev      device descriptor
 * @param[in]  pin_num  QMA6100P INT pin to configure (@ref QMA6100P_INT1 or @ref QMA6100P_INT2)
 * @param[out] map_reg  INTx_MAP1 register address for the selected pin
 *
 * @return  0 on success
 * @return  QMA6100P_INVALID_ARG if pin_num is invalid
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _set_intpin_conf(const qma6100p_t *dev,
                            qma6100p_int_pin_num_t pin_num,
                            uint8_t *map_reg)
{
    int res;
    uint8_t reg;

    READ_REG(QMA6100P_REG_INTPIN_CONF, reg, out);

    switch (pin_num) {
    case QMA6100P_INT1:
        *map_reg = QMA6100P_REG_INT1_MAP1;
        FIELD_SET(QMA6100P_INTPIN_INT1_LVL_MASK, QMA6100P_PARAM_INT_ACTIVE_LEVEL, reg);
        FIELD_SET(QMA6100P_INTPIN_INT1_OD_MASK, QMA6100P_PARAM_INT_PIN_MODE, reg);
        break;
    case QMA6100P_INT2:
        *map_reg = QMA6100P_REG_INT2_MAP1;
        FIELD_SET(QMA6100P_INTPIN_INT2_LVL_MASK, QMA6100P_PARAM_INT_ACTIVE_LEVEL, reg);
        FIELD_SET(QMA6100P_INTPIN_INT2_OD_MASK, QMA6100P_PARAM_INT_PIN_MODE, reg);
        break;
    default:
        res = QMA6100P_INVALID_ARG;
        DEBUG("[qma6100p] %s: pin_num %d invalid, choose %d or %d\n",
              __func__, pin_num, QMA6100P_INT1, QMA6100P_INT2);
        goto out;
    }

    WRITE_REG(QMA6100P_REG_INTPIN_CONF, reg, out);

out:
    return res;
}

/**
 * @brief Route data-ready interrupt to the selected INT pin via INTx_MAP1 register
 *
 * @param[in] dev      device descriptor
 * @param[in] map_reg  INTx_MAP1 register address (from @ref _set_intpin_conf)
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _route_data_ready_int(const qma6100p_t *dev, uint8_t map_reg)
{
    int res;
    uint8_t reg;

    READ_REG(map_reg, reg, out);
    FIELD_SET(QMA6100P_INT_MAP1_DATA_MASK, 1, reg);
    WRITE_REG(map_reg, reg, out);

out:
    return res;
}

/**
 * @brief Enable data-ready interrupt in INT_EN1 register
 *
 * @param[in] dev  device descriptor
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _enable_data_ready_int(const qma6100p_t *dev)
{
    int res;
    uint8_t reg;

    READ_REG(QMA6100P_REG_INT_EN1, reg, out);
    FIELD_SET(QMA6100P_INT_EN1_DATA_MASK, 1, reg);
    WRITE_REG(QMA6100P_REG_INT_EN1, reg, out);

out:
    return res;
}

int qma6100p_set_data_ready_int(qma6100p_t *dev, qma6100p_int_pin_num_t line,
                                qma6100p_int_cb_t cb, void *arg)
{
    assert(dev && cb);

    gpio_t pin = (line == QMA6100P_INT2) ? dev->params.int2_pin : dev->params.int1_pin;

    if (!gpio_is_valid(pin)) {
        return QMA6100P_GPIO_ERROR;
    }

    uint8_t map_reg = 0;

    i2c_acquire(BUS);

    int res = _set_int_params(dev);
    if (res < 0) {
        goto out;
    }

    res = _set_intpin_conf(dev, line, &map_reg);
    if (res < 0) {
        goto out;
    }

    res = _route_data_ready_int(dev, map_reg);
    if (res < 0) {
        goto out;
    }

    res = _enable_data_ready_int(dev);

out:
    i2c_release(BUS);
    if (res < 0) {
        return res;
    }

    gpio_flank_t flank = (QMA6100P_PARAM_INT_ACTIVE_LEVEL == QMA6100P_INTPIN_ACTIVE_HIGH) ? GPIO_RISING : GPIO_FALLING;
    if (gpio_init_int(pin, GPIO_IN, flank, cb, arg) < 0) {
        return QMA6100P_GPIO_ERROR;
    }

    return QMA6100P_OK;
}
