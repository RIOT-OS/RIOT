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

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "qma6100p_params.h"
#include "qma6100p_regs.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define BUS                   (dev->params.i2c)
#define ADDR                  (dev->params.addr)

/** Print out a message that function is not yet implemented */
#define NOT_YET_IMPLEMENTED() DEBUG("%s not yet implemented\n", __func__)

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
out:
    i2c_release(i2c);
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

    res = qma6100p_set_mode(dev, params->mode);
    if (res < 0) {
        return res;
    }

    DEBUG("[qma6100p] init: successful\n");
    return QMA6100P_OK;
}

/**
 * TODO: Implement this
 *
 * @brief Disable all configured interrupts on the device
 *
 * @param[in] dev  device descriptor
 *
 * @return  0 on success
 * @return  negative error code on failure
 *
 */
static int _disable_set_interrupt(const qma6100p_t *dev)
{
    (void)dev;
    NOT_YET_IMPLEMENTED();
    return 0;
}

/**
 * @brief Write register sequence to put device into Ultra Low Power State (ULPS)
 *
 * @param[in] dev  device descriptor
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _enter_ulps_mode(const qma6100p_t *dev)
{
    int res = QMA6100P_OK;

    WRITE_REG(QMA6100P_REG_PM, 0x87, out);
    WRITE_REG(QMA6100P_REG_ULPS, 0x0F, out);
    WRITE_REG(QMA6100P_REG_TST0_ANA, 0x00, out);

    res = _disable_set_interrupt(dev);
    if (res < 0) {
        DEBUG("[qma6100p] %s: failed to disable interrupt\n", __func__);
        goto out;
    }

out:
    return res;
}

/**
 * @brief Configure interrupt behavior in INT_CFG register
 *
 * Sets latch mode, status clear behavior, and data shadowing from @p int_params
 *
 * @param[in,out] dev         device descriptor
 * @param[in]     int_params  interrupt parameters
 *
 * @return  0 on success
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _set_int_params(qma6100p_t *dev, qma6100p_int_params_t int_params)
{
    int res = QMA6100P_OK;
    uint8_t int_reg;

    READ_REG(QMA6100P_REG_INT_CFG, int_reg, out);

    FIELD_SET(QMA6100P_INT_CFG_CLR_MASK, int_params.interrupt_clear_behavior, int_reg);
    FIELD_SET(QMA6100P_INT_CFG_LATCH_MASK, int_params.interrupt_latch, int_reg);
    FIELD_SET(QMA6100P_INT_CFG_SHADOW_MASK, int_params.interrupt_shadow, int_reg);

    WRITE_REG(QMA6100P_REG_INT_CFG, int_reg, out);

out:
    return res;
}

/**
 * @brief Set MODE bit to 1 in PM register to enter active mode
 *
 * @param[in] dev  device descriptor
 * @param[in] pm   current PM register
 *
 * @return  QMA6100P_OK on success
 * @return  negative error code on I2C failure
 */
static inline int qma6100p_enter_active_mode(const qma6100p_t *dev, uint8_t pm)
{
    FIELD_SET(QMA6100P_PM_MODE_MASK, 1, pm);
    return _write_reg(BUS, ADDR, QMA6100P_REG_PM, pm);
}

int qma6100p_set_mode(qma6100p_t *dev, qma6100p_mode_t mode)
{
    int res;
    uint8_t pm;

    assert(dev);

    i2c_acquire(BUS);

    READ_REG(QMA6100P_REG_PM, pm, out);

    switch (mode) {
    case QMA6100P_MODE_ULPS:
        res = _enter_ulps_mode(dev);
        if (res < 0) {
            DEBUG("[qma6100p] set_mode - error: failed to enter ulps mode\n");
            goto out;
        }
        break;

    case QMA6100P_MODE_ACTIVE:
        res = qma6100p_enter_active_mode(dev, pm);
        if (res < 0) {
            DEBUG("[qma6100p] set_mode - error: failed to enter active mode\n");
            goto out;
        }
        break;

    case QMA6100P_MODE_INTERMEDIATE:
    default:
        res = QMA6100P_INVALID_ARG;
        DEBUG("[qma6100p] set_mode: mode not supported\n");
        goto out;
    }

    dev->params.mode = mode;

out:
    i2c_release(BUS);
    return res;
}

/**
 * @brief Configure INTPIN_CONF register and resolve the MAP register for the selected INT pin
 *
 * Sets active level and output mode (push-pull/open-drain) for the chosen INT pin,
 * and writes @p map_reg with the corresponding INTx_MAP1 register address
 *
 * @param[in]  dev      device descriptor
 * @param[in]  params   interrupt parameters (@ref qma6100p_int_params_t)
 * @param[out] map_reg  INTx_MAP1 register address for the selected pin
 *
 * @return  0 on success
 * @return  QMA6100P_INVALID_ARG if interrupt_pin_num is invalid
 * @return  negative error code on I2C failure
 *
 * @warning I2C bus must be acquired by the caller
 */
static int _set_intpin_conf(const qma6100p_t *dev,
                            const qma6100p_int_params_t *params,
                            uint8_t *map_reg)
{
    int res;
    uint8_t reg;

    READ_REG(QMA6100P_REG_INTPIN_CONF, reg, out);

    switch (params->interrupt_pin_num) {
    case QMA6100P_INT1:
        *map_reg = QMA6100P_REG_INT1_MAP1;
        FIELD_SET(QMA6100P_INTPIN_INT1_LVL_MASK, params->active_level_int, reg);
        FIELD_SET(QMA6100P_INTPIN_INT1_OD_MASK, params->pin_mode_int, reg);
        break;
    case QMA6100P_INT2:
        *map_reg = QMA6100P_REG_INT2_MAP1;
        FIELD_SET(QMA6100P_INTPIN_INT2_LVL_MASK, params->active_level_int, reg);
        FIELD_SET(QMA6100P_INTPIN_INT2_OD_MASK, params->pin_mode_int, reg);
        break;
    default:
        res = QMA6100P_INVALID_ARG;
        DEBUG("[qma6100p] %s: pin_num %d invalid, choose %d or %d\n",
              __func__, params->interrupt_pin_num, QMA6100P_INT1, QMA6100P_INT2);
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

int qma6100p_set_data_ready_int(qma6100p_t *dev, const qma6100p_int_t *interrupt)
{
    assert(dev && interrupt->cb);

    if (!gpio_is_valid(interrupt->params.interrupt_pin)) {
        return QMA6100P_GPIO_ERROR;
    }

    uint8_t map_reg;

    i2c_acquire(BUS);

    int res = _set_int_params(dev, interrupt->params);
    if (res < 0) {
        return res;
    }

    res = _set_intpin_conf(dev, &interrupt->params, &map_reg);
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

    gpio_flank_t flank = (interrupt->params.active_level_int == QMA6100P_INTPIN_ACTIVE_HIGH) ? GPIO_RISING : GPIO_FALLING;
    if (gpio_init_int(interrupt->params.interrupt_pin, GPIO_IN, flank,
                      interrupt->cb, interrupt->arg) < 0) {
        return QMA6100P_GPIO_ERROR;
    }

    dev->interrupt = *interrupt;
    return QMA6100P_OK;
}
