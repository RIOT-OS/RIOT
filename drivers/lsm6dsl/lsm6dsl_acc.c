/*
 * Copyright (C) 2020 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_lsm6dsl
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LSM6DSL 3D accelerometer.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Sylvain Fabre <sylvain.fabre-ext@continental-its.com>
 *
 * @}
 */

#include "xtimer.h"

#include "lsm6dsl.h"
#include "lsm6dsl_internal.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define BUS         (dev->params.i2c)
#define ADDR        (dev->params.addr)

/**
 * order in array [0, 1, 2, 3] is
 * LSM6DSL_ACC_FS_2G, LSM6DSL_ACC_FS_16G, LSM6DSL_ACC_FS_4G, LSM6DSL_ACC_FS_8G
 */
static const int16_t range_acc[] = { 2000, 16000, 4000, 8000 };

int lsm6dsl_read_acc(const lsm6dsl_t *dev, lsm6dsl_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, LSM6DSL_REG_STATUS_REG, &tmp, 0);
    DEBUG("lsm6dsl status: %x\n", tmp);

    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTX_L_XL, &tmp, 0);
    data->x = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTX_H_XL, &tmp, 0);
    data->x |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTY_L_XL, &tmp, 0);
    data->y = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTY_H_XL, &tmp, 0);
    data->y |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTZ_L_XL, &tmp, 0);
    data->z = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTZ_H_XL, &tmp, 0);
    data->z |= tmp << 8;
    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_read_acc\n");
        return -LSM6DSL_ERROR_BUS;
    }

    assert(dev->params.acc_fs < LSM6DSL_ACC_FS_MAX);
    data->x = ((int32_t)data->x * range_acc[dev->params.acc_fs]) / INT16_MAX;
    data->y = ((int32_t)data->y * range_acc[dev->params.acc_fs]) / INT16_MAX;
    data->z = ((int32_t)data->z * range_acc[dev->params.acc_fs]) / INT16_MAX;

    return LSM6DSL_OK;
}

int lsm6dsl_acc_power_down(const lsm6dsl_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_CTRL1_XL, &tmp, 0);
    if (res < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dsl_acc_power_down\n");
        return -LSM6DSL_ERROR_BUS;
    }

    tmp &= ~(LSM6DSL_CTRL_ODR_MASK);
    res = i2c_write_reg(BUS, ADDR, LSM6DSL_REG_CTRL1_XL, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_power_down\n");
        return -LSM6DSL_ERROR_BUS;
    }

    return LSM6DSL_OK;
}

int lsm6dsl_acc_power_up(const lsm6dsl_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_CTRL1_XL, &tmp, 0);
    if (res < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dsl_acc_power_up\n");
        return -LSM6DSL_ERROR_BUS;
    }

    tmp &= ~(LSM6DSL_CTRL_ODR_MASK);
    tmp |= dev->params.acc_odr << LSM6DSL_CTRL_ODR_SHIFT;
    res = i2c_write_reg(BUS, ADDR, LSM6DSL_REG_CTRL1_XL, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_power_up\n");
        return -LSM6DSL_ERROR_BUS;
    }

    return LSM6DSL_OK;
}

int lsm6dsl_acc_configure_wkup_alert(const lsm6dsl_t *dev,
                                    const lsm6dsl_wkup_params_t *params)
{
    assert(params);
    int res = 0;
    uint8_t tmp;

    i2c_acquire(BUS);
    /* set filtering type */
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, &tmp, 0);
    tmp &= (~LSM6DSL_TAP_CFG_SLOPE_FDS_MASK);
    tmp |= ((params->slope_fds << LSM6DSL_TAP_CFG_SLOPE_FDS_SHIFT)
                  & LSM6DSL_TAP_CFG_SLOPE_FDS_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, tmp, 0);

    /* set wake up duration and sleep duration */
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_WAKE_UP_DUR, &tmp, 0);
    tmp &= (~LSM6DSL_WAKE_UP_DUR_TIMER_HR_MASK);
    tmp |= ((params->wake_dur << LSM6DSL_WAKE_UP_DUR_WAKE_DUR_SHIFT)
            & LSM6DSL_WAKE_UP_DUR_WAKE_DUR_MASK);
    tmp |= (params->sleep_dur & LSM6DSL_WAKE_UP_DUR_SLEEP_DUR_MASK);
    /* FF_DUR is split over 2 registers, get the 5th bit*/
    uint8_t ff_dur5_bit = params->ff_dur & (0x10);
    tmp |= ((ff_dur5_bit << LSM6DSL_WAKE_UP_DUR_FF_DUR5_SHIFT)
            & LSM6DSL_WAKE_UP_DUR_FF_DUR5_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_WAKE_UP_DUR, tmp, 0);

    /* set wake up threshold */
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_WAKE_UP_THS, &tmp, 0);
    tmp = (tmp & (~LSM6DSL_WAKE_UP_THS_WK_TH_MASK)) | (params->wk_ths &
            LSM6DSL_WAKE_UP_THS_WK_TH_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_WAKE_UP_THS, tmp, 0);

    /* set free fall duration (4 bits over 5) and threshold */
    tmp = ((params->ff_dur << LSM6DSL_FREE_FALL_FF_DUR_SHIFT)
            & LSM6DSL_FREE_FALL_FF_DUR_MASK) |
            (params->ff_ths & LSM6DSL_FREE_FALL_FF_THS_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_FREE_FALL, tmp, 0);

    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_configure_wkup_alert\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_enable_alert(lsm6dsl_t *dev, uint8_t alert, uint8_t pin,
                            lsm6dsl_cb_t cb,
                            void *arg)
{
    (void) cb;
    (void) arg;

    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    /* check pin */
    if (pin >= LSM6DSL_PIN_INT_MAX || dev->params.alert_pins[pin]
        == GPIO_UNDEF) {
        i2c_release(BUS);
        goto pin_error;
    }

    /* enable basic interrupt */
    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, &tmp, 0);
    tmp = LSM6DSL_TAP_CFG_INT_ENABLE_MASK;
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, tmp, 0);

    /* root alert on selected pin */
    uint16_t mdx_cfg_reg;
    mdx_cfg_reg = (pin == LSM6DSL_PIN_INT_1 ?
                    LSM6DSL_REG_MD1_CFG:LSM6DSL_REG_MD2_CFG);
    res += i2c_read_reg(BUS, ADDR, mdx_cfg_reg, &tmp, 0);
    tmp |= alert;
    res += i2c_write_reg(BUS, ADDR, mdx_cfg_reg, tmp, 0);

    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_enable_alert\n");
        return -LSM6DSL_ERROR_BUS;
    }

    #ifdef MODULE_PERIPH_GPIO_IRQ
    /* set gpio interrupt pin */
    if (cb) {
        gpio_init_int(dev->params.alert_pins[pin], GPIO_IN, GPIO_RISING,
                        cb, arg);
    }
    #else
        DEBUG("[WARNING] GPIO IRQ not defined\n");
    #endif
    return LSM6DSL_OK;

pin_error:
    DEBUG("[ERROR] lsm6dsl_acc_enable_alert: selected pin (%d) not "  \
            "available\n", pin);
    return -LSM6DSL_ERROR_CNF;
}

int lsm6dsl_acc_disable_alert(lsm6dsl_t *dev, uint8_t alert, uint8_t pin)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    /* check pin */
    if (pin >= LSM6DSL_PIN_INT_MAX || dev->params.alert_pins[pin]
        == GPIO_UNDEF) {
      i2c_release(BUS);
      goto pin_error;
    }

    /* unroot alert on selected pin */
    uint16_t mdx_cfg_reg;
    mdx_cfg_reg = (pin == LSM6DSL_PIN_INT_1 ?
                 LSM6DSL_REG_MD1_CFG:LSM6DSL_REG_MD2_CFG);
    res = i2c_read_reg(BUS, ADDR, mdx_cfg_reg, &tmp, 0);
    tmp &= (~alert);
    res += i2c_write_reg(BUS, ADDR, mdx_cfg_reg, tmp, 0);

    i2c_release(BUS);

    #ifdef MODULE_PERIPH_GPIO_IRQ
    if (tmp == 0) {
        /* No other interrupts enable, clear the pin */
        gpio_irq_disable(dev->params.alert_pins[pin]);
    }
    #else
        DEBUG("[WARNING] GPIO IRQ not defined\n");
    #endif

    return LSM6DSL_OK;

pin_error:
    DEBUG("[ERROR] lsm6dsl_acc_disable_alert: selected pin (%d) " \
            "not available\n", pin);
    return -LSM6DSL_ERROR_CNF;
}

int lsm6dsl_acc_get_wkup_src_alert(const lsm6dsl_t *dev,
                                    lsm6dsl_wkup_src_t *reason,
                                    bool disable_flag)
{
    int res = 0;
    uint8_t tmp;
    /* read wake up source */
    i2c_acquire(BUS);

    if (reason) {
        res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_WAKE_UP_SRC, &tmp, 0);
        reason->free_fall = (tmp & LSM6DSL_WAKE_UP_SRC_FF_IA_MASK)
                            >> LSM6DSL_WAKE_UP_SRC_FF_IA_SHIFT;
        reason->inactive = (tmp & LSM6DSL_WAKE_UP_SRC_SLEEP_MASK)
                            >> LSM6DSL_WAKE_UP_SRC_SLEEP_SHIFT;
        reason->active = (tmp & LSM6DSL_WAKE_UP_SRC_WU_IA_MASK)
                            >> LSM6DSL_WAKE_UP_SRC_WU_IA_SHIFT;
        reason->x = (tmp & LSM6DSL_WAKE_UP_SRC_X_WU_MASK)
                    >> LSM6DSL_WAKE_UP_SRC_X_WU_SHIFT;
        reason->y = (tmp & LSM6DSL_WAKE_UP_SRC_Y_WU_MASK)
                    >> LSM6DSL_WAKE_UP_SRC_Y_WU_SHIFT;
        reason->z = (tmp & LSM6DSL_WAKE_UP_SRC_Z_WU_MASK);
    }

    if (disable_flag) {
        res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, &tmp, 0);
        tmp &= (~LSM6DSL_TAP_CFG_INT_ENABLE_MASK);
        res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, tmp, 0);
    }

    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_get_wkup_alert\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_configure_6D4D_alert(const lsm6dsl_t *dev,
                                    const lsm6dsl_6d4d_params_t *params)
{
    assert(params);
    int res = 0;
    uint8_t tmp;
    i2c_acquire(BUS);

    /* set filtering type */
    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_CTRL8_XL, &tmp, 0);
    tmp &= (~LSM6DSL_CTRL8_XL_LP_ON_D6_MASK);
    tmp |= (params->low_pass_on_6d & LSM6DSL_CTRL8_XL_LP_ON_D6_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_CTRL8_XL, tmp, 0);

    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_THS_6D, &tmp, 0);
    tmp &= (LSM6DSL_TAP_THS_6D_TAP_THS_MASK);
    tmp |= ((params->d4d_en << LSM6DSL_TAP_THS_6D_D4D_EN_SHIFT)
            & LSM6DSL_TAP_THS_6D_D4D_EN_MASK);
    tmp |= ((params->sixd_ths << LSM6DSL_TAP_THS_6D_SIXD_THS_SHIFT)
            & LSM6DSL_TAP_THS_6D_SIXD_THS_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_THS_6D, tmp, 0);

    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_configure_6D4D_alert\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_get_6D4D_src_alert(const lsm6dsl_t *dev,
                                    lsm6dsl_6d4d_src_t *reason,
                                    bool disable_flag)
{
    int res = 0;
    uint8_t tmp;
    i2c_acquire(BUS);
    if (reason) {
        res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_D6D_SRC, &tmp, 0);
        reason->zh = (tmp & LSM6DSL_D6D_SRC_ZH_MASK)
                        >> LSM6DSL_D6D_SRC_ZH_SHIFT;
        reason->zl = (tmp & LSM6DSL_D6D_SRC_ZL_MASK)
                        >> LSM6DSL_D6D_SRC_ZL_SHIFT;
        reason->yh = (tmp & LSM6DSL_D6D_SRC_YH_MASK)
                        >> LSM6DSL_D6D_SRC_YH_SHIFT;
        reason->yl = (tmp & LSM6DSL_D6D_SRC_YL_MASK)
                        >> LSM6DSL_D6D_SRC_YL_SHIFT;
        reason->xh = (tmp & LSM6DSL_D6D_SRC_XH_MASK)
                        >> LSM6DSL_D6D_SRC_XH_SHIFT;
        reason->xl = (tmp & LSM6DSL_D6D_SRC_XL_MASK);
    }

    if (disable_flag) {
        res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, &tmp, 0);
        tmp &= (~LSM6DSL_TAP_CFG_INT_ENABLE_MASK);
        res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, tmp, 0);
    }

    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_get_6D4D_src_alert\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_configure_tap_alert(const lsm6dsl_t *dev,
                                    const lsm6dsl_tap_params_t *params)
{
    assert(params);
    int res = 0;
    uint8_t tmp;
    i2c_acquire(BUS);

    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_THS_6D, &tmp, 0);
    tmp &= (~LSM6DSL_TAP_THS_6D_TAP_THS_MASK);
    tmp |= (params->tap_ths & LSM6DSL_TAP_THS_6D_TAP_THS_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_THS_6D, tmp, 0);

    tmp = (params->shock & LSM6DSL_INT_DUR2_SHOCK_MASK);
    tmp |= ((params->quiet << LSM6DSL_INT_DUR2_QUIET_SHIFT)
            & LSM6DSL_INT_DUR2_QUIET_MASK);
    tmp |= ((params->duration << LSM6DSL_INT_DUR2_DUR_SHIFT)
            & LSM6DSL_INT_DUR2_DUR_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_INT_DUR2, tmp, 0);

    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_WAKE_UP_THS, &tmp, 0);
    tmp &= (~LSM6DSL_WAKE_UP_THS_SD_TAP_MASK);
    tmp |= ((params->dbl_tap << LSM6DSL_WAKE_UP_THS_SD_TAP_SHIFT)
            & LSM6DSL_WAKE_UP_THS_SD_TAP_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_WAKE_UP_THS, tmp, 0);

    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_configure_tap_alert\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_get_tap_src_alert(const lsm6dsl_t *dev,
                                    lsm6dsl_tap_src_t *reason,
                                    bool disable_flag)
{
    int res = 0;
    uint8_t tmp;
    i2c_acquire(BUS);
    if (reason) {
        res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_D6D_SRC, &tmp, 0);
        reason->tap_ia = (tmp & LSM6DSL_TAP_SRC_TAP_IA_MASK)
                            >> LSM6DSL_TAP_SRC_TAP_IA_SHIFT;
        reason->single_tap = (tmp & LSM6DSL_TAP_SRC_S_TAP_MASK)
                                >> LSM6DSL_TAP_SRC_S_TAP_SHIFT;
        reason->double_tap = (tmp & LSM6DSL_TAP_SRC_D_TAP_MASK)
                                >> LSM6DSL_TAP_SRC_D_TAP_SHIFT;
        reason->tap_sign = (tmp & LSM6DSL_TAP_SRC_TAP_SIGN_MASK)
                                >> LSM6DSL_TAP_SRC_TAP_SIGN_SHIFT;
        reason->x_tap = (tmp & LSM6DSL_TAP_SRC_X_TAP_MASK)
                            >> LSM6DSL_TAP_SRC_X_TAP_SHIFT;
        reason->y_tap = (tmp & LSM6DSL_TAP_SRC_Y_TAP_MASK)
                            >> LSM6DSL_TAP_SRC_Y_TAP_SHIFT;
        reason->z_tap = tmp & LSM6DSL_TAP_SRC_Z_TAP_MASK;
    }

    if (disable_flag) {
        res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, &tmp, 0);
        tmp &= (~LSM6DSL_TAP_CFG_INT_ENABLE_MASK);
        res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, tmp, 0);
    }

    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_get_tap_src_alert\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_get_x_offset(const lsm6dsl_t *dev, int8_t *offset)
{
    assert(offset);
    i2c_acquire(BUS);
    int res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_X_OFS_USR, offset, 0);
    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_get_x_offset\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_set_x_offset(const lsm6dsl_t *dev, int8_t offset)
{
    i2c_acquire(BUS);
    int res = i2c_write_reg(BUS, ADDR, LSM6DSL_REG_X_OFS_USR, offset, 0);
    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_set_x_offset\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_get_y_offset(const lsm6dsl_t *dev, int8_t *offset)
{
    assert(offset);
    i2c_acquire(BUS);
    int res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_Y_OFS_USR, offset, 0);
    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_get_y_offset\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_set_y_offset(const lsm6dsl_t *dev, int8_t offset)
{
    i2c_acquire(BUS);
    int res = i2c_write_reg(BUS, ADDR, LSM6DSL_REG_Y_OFS_USR, offset, 0);
    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_set_y_offset\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_get_z_offset(const lsm6dsl_t *dev, int8_t *offset)
{
    assert(offset);
    i2c_acquire(BUS);
    int res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_Z_OFS_USR, offset, 0);
    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_get_z_offset\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_acc_set_z_offset(const lsm6dsl_t *dev, int8_t offset)
{
    i2c_acquire(BUS);
    int res = i2c_write_reg(BUS, ADDR, LSM6DSL_REG_Z_OFS_USR, offset, 0);
    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_acc_set_z_offset\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}
