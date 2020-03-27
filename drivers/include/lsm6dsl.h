/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup    drivers_lsm6dsl LSM6DSL 3D accelerometer/gyroscope
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the LSM6DSL 3D accelerometer/gyroscope
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the LSM6DSL 3D
 *              accelerometer/gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 */

#ifndef LSM6DSL_H
#define LSM6DSL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

/**
 * @brief   Data rate settings
 */
enum {
    LSM6DSL_DATA_RATE_POWER_DOWN = 0x0,
    LSM6DSL_DATA_RATE_1_6HZ      = 0xB,
    LSM6DSL_DATA_RATE_12_5HZ     = 0x1,
    LSM6DSL_DATA_RATE_26HZ       = 0x2,
    LSM6DSL_DATA_RATE_52HZ       = 0x3,
    LSM6DSL_DATA_RATE_104HZ      = 0x4,
    LSM6DSL_DATA_RATE_208HZ      = 0x5,
    LSM6DSL_DATA_RATE_416HZ      = 0x6,
    LSM6DSL_DATA_RATE_833HZ      = 0x7,
    LSM6DSL_DATA_RATE_1_66KHZ    = 0x8,
    LSM6DSL_DATA_RATE_3_33KHZ    = 0x9,
    LSM6DSL_DATA_RATE_6_66KHZ    = 0xa,
};

/**
 * @brief   Decimation settings
 */
enum {
    LSM6DSL_DECIMATION_NOT_IN_FIFO = 0,
    LSM6DSL_DECIMATION_NO,
    LSM6DSL_DECIMATION_2,
    LSM6DSL_DECIMATION_3,
    LSM6DSL_DECIMATION_4,
    LSM6DSL_DECIMATION_8,
    LSM6DSL_DECIMATION_16,
    LSM6DSL_DECIMATION_32,
};

/**
 * @brief   Accelerometer full scale
 */
enum {
    LSM6DSL_ACC_FS_2G  = 0,
    LSM6DSL_ACC_FS_16G,
    LSM6DSL_ACC_FS_4G,
    LSM6DSL_ACC_FS_8G,
    LSM6DSL_ACC_FS_MAX,
};

/**
 * @brief   Accelerometer user weigh offset
 */
enum {
    LSM6DSL_ACC_SMALL_SCALE = 0,
    LSM6DSL_ACC_LARGE_SCALE,
};

/**
 * @brief   Gyroscope full scale
 */
enum {
    LSM6DSL_GYRO_FS_245DPS    = 0,
    LSM6DSL_GYRO_FS_500DPS,
    LSM6DSL_GYRO_FS_1000DPS,
    LSM6DSL_GYRO_FS_2000DPS,
    LSM6DSL_GYRO_FS_MAX,
};

/**
 * @brief   Interrupt pins
 */
enum {
    LSM6DSL_PIN_INT_1   = 0,
    LSM6DSL_PIN_INT_2,
    LSM6DSL_PIN_INT_MAX,
};

/**
 * @brief Acc alerts
 */
enum {
    LSM6DSL_INT_TIMER       = 0x01, /* Not used */
    LSM6DSL_INT_TILT        = 0x02, /* Not used */
    LSM6DSL_INT_6D_4D       = 0x04,
    LSM6DSL_INT_DOUBLE_TAP  = 0x08,
    LSM6DSL_INT_FREE_FALL   = 0x10,
    LSM6DSL_INT_WAKE_UP     = 0x20,
    LSM6DSL_INT_SINGLE_TAP  = 0x40,
    LSM6DSL_INT_SLEEP       = 0x80,
    LSM6DSL_INT_ALL         = 0xFF,
};

/**
 * @brief LSM6DSL TAP interruption parameters
 */
typedef struct {
    uint8_t tap_ths;        /**< [4:0] threshold for tap detection
                                (1 LSB = FS_XL/(2^5)) */
    uint8_t shock;          /**< [1:0] shock time window (1LSB = 8/ODR_XL) */
    uint8_t quiet;          /**< [1:0] quiet time (1LSB = 4/ODR_XL) */
    uint8_t duration;       /**< [3:0] max duration between two taps
                                (1LSB = 32/ODR_XL) */
    uint8_t dbl_tap;        /**< 1: double tap, 0: single tap */
} lsm6dsl_tap_params_t;

/**
 * @brief LSM6DSL 6D/4D interruption parameters
 */
typedef struct {
    uint8_t d4d_en;         /**< 1 to enable 4D orientation */
    uint8_t sixd_ths;       /**< Threshold for 4D/6D detection */
    uint8_t low_pass_on_6d; /**< use low pass filter */
} lsm6dsl_6d4d_params_t;

/**
 * @brief LSM6DSL WakeUp/Activity/Inactivity/FreeFall interruption parameters
 */
typedef struct {
    uint8_t slope_fds;  /**< 0 slope filter or 1 high-pass digital filter */
    uint8_t wk_ths;     /**< [5:0] threshold for wake up
                            (1 LSB = FS_XL/(2^6)) */
    uint8_t wake_dur;   /**< [1:0] Wake up duration event
                            (1LSB = 1 ODR_time) */
    uint8_t sleep_dur;  /**< [3:0] sleep duration before event
                            (1LSB = 512/ ODR_XL) */
    uint8_t ff_dur;     /**< Free fall duration event */
    uint8_t ff_ths;     /**< Free fall threshold */
} lsm6dsl_wkup_params_t;

/**
 * @brief LSM6DSL WakeUp/Activity/Inactivity/FreeFall sources
 */
typedef struct {
    bool free_fall; /**< true if Free Fall wake up detected, false otherwise*/
    bool inactive;  /**< true if inactive wake up detected, false otherwise*/
    bool active;    /**< true if active wake up detected, false otherwise*/
    bool x;         /**< true if x-axis wake up detected, false otherwise*/
    bool y;         /**< true if y-axis wake up detected, false otherwise*/
    bool z;         /**< true if z-axis wake up detected, false otherwise*/
} lsm6dsl_wkup_src_t;

/**
 * @brief LSM6DSL 6D/4D sources
 */
typedef struct {
    bool zh;    /**< true Z axis positive detected, false otherwise */
    bool zl;    /**< true Z axis negative detected, false otherwise */
    bool yh;    /**< true Y axis positive detected, false otherwise */
    bool yl;    /**< true Y axis negative detected, false otherwise */
    bool xh;    /**< true X axis positive detected, false otherwise */
    bool xl;    /**< true X axis negative detected, false otherwise */
} lsm6dsl_6d4d_src_t;

/**
 * @brief LSM6DSL TAP sources
 */
typedef struct {
    bool tap_ia;        /**< true when any type of TAP detected,
                            false otherwise */
    bool single_tap;    /**< true when single tap detected, false otherwise */
    bool double_tap;    /**< true when double tap detected, false otherwise */
    bool tap_sign;      /**< true when negative, false when positive*/
    bool x_tap;         /**< true X axis tap detected, false otherwise */
    bool y_tap;         /**< true Y axis tap detected, false otherwise */
    bool z_tap;         /**< true Z axis tap detected, false otherwise */
} lsm6dsl_tap_src_t;

/**
 * @brief   LSM6DSL driver parameters
 */
typedef struct {
    i2c_t i2c;                              /**< i2c bus */
    uint8_t addr;                           /**< i2c address */
    uint8_t acc_odr;                        /**< accelerometer output data
                                                rate */
    uint8_t gyro_odr;                       /**< gyroscope output data rate */
    uint8_t acc_fs;                         /**< accelerometer full scale */
    uint8_t gyro_fs;                        /**< gyroscope full scale */
    uint8_t acc_decimation;                 /**< accelerometer decimation */
    uint8_t gyro_decimation;                /**< gyroscope decimation */
    uint8_t acc_usr_ofs_w;                  /**< accelerometer user offset
                                                weigh */
    gpio_t alert_pins[LSM6DSL_PIN_INT_MAX]; /**< INT1 and INT2 alert pins */
} lsm6dsl_params_t;

/**
 * @brief  LSM6DSL alert callback
 */
typedef void (* lsm6dsl_cb_t)(void *arg);

/**
 * @brief   LSM6DSL device descriptor
 */
typedef struct {
    lsm6dsl_params_t params; /**< driver parameters */
} lsm6dsl_t;

/**
 * @brief   3D output data
 */
typedef struct {
    int16_t x;  /**< X axis */
    int16_t y;  /**< Y axis */
    int16_t z;  /**< Z axis */
} lsm6dsl_3d_data_t;

/**
 * @brief   Named return values
 */
enum {
    LSM6DSL_OK = 0,             /**< all good */
    LSM6DSL_ERROR_BUS,          /**< I2C bus error */
    LSM6DSL_ERROR_CNF,          /**< Config error */
    LSM6DSL_ERROR_DEV,          /**< device error */
};

/**
 * @brief   Initialize a LSM6DSL device
 *
 * @param[out]  dev         Device to initialize
 * @param[in]   params      Driver parameters
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_init(lsm6dsl_t *dev, const lsm6dsl_params_t *params);

/**
 * @brief   enable latched interrupt
 *
 * @param[in] dev   Targeted device
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_enable_latched_interrupt(const lsm6dsl_t *dev);

/**
 * @brief   disable latched interrupt
 *
 * @param[in] dev   Targeted device
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_disable_latched_interrupt(const lsm6dsl_t *dev);

/**
 * @brief   Read accelerometer data
 *
 * @param[in] dev    Device to read
 * @param[out] data  Accelerometer values
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_read_acc(const lsm6dsl_t *dev, lsm6dsl_3d_data_t *data);

/**
 * @brief   Read gyroscope data
 *
 * @param[in] dev    Device to read
 * @param[out] data  Gyroscope values
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_read_gyro(const lsm6dsl_t *dev, lsm6dsl_3d_data_t *data);

/**
 * @brief   Read temperature data
 *
 * @note To avoid floating point data types but still provide high resolution
 *       for temperature readings, resulting values are scale by factor 100.
 *
 *
 * @param[in] dev    Device to read
 * @param[out] data  Temperature value, in °C x 100
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_read_temp(const lsm6dsl_t *dev, int16_t *data);

/**
 * @brief   Power down accelerometer
 *
 * @param[in] dev    Device to power down
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_power_down(const lsm6dsl_t *dev);

/**
 * @brief   Power down gyroscope
 *
 * @param[in] dev    Device to power down
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_gyro_power_down(const lsm6dsl_t *dev);

/**
 * @brief   Power up accelerometer
 *
 * @param[in] dev    Device to power up
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_power_up(const lsm6dsl_t *dev);

/**
 * @brief   Power up gyroscope
 *
 * @param[in] dev    Device to power up
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_gyro_power_up(const lsm6dsl_t *dev);

/**
 * @brief Wake up alerts configuration
 *          * Activity/Inactivity recognition
 *          * wake up interrupt
 *          * Free Fall interrupt
 *
 * @param[in]   dev     Device to configure
 * @param[in]   params  Parameters to set
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_configure_wkup_alert(const lsm6dsl_t *dev,
                                    const lsm6dsl_wkup_params_t *params);

/**
 * @brief enable  alert :
 *          * Activity/Inactivity recognition
 *          * wake up interrupt
 *          * Free Fall interrupt
 *          * 6D/4D interrupt
 *          * TAP interrupt
 *
 * @note: enum {
 *          LSM6DSL_INT_TIMER       = 0x01, // Not used
 *          LSM6DSL_INT_TILT        = 0x02, // Not used
 *          LSM6DSL_INT_6D_4D       = 0x04,
 *          LSM6DSL_INT_DOUBLE_TAP  = 0x08,
 *          LSM6DSL_INT_FREE_FALL   = 0x10,
 *          LSM6DSL_INT_WAKE_UP     = 0x20,
 *          LSM6DSL_INT_SINGLE_TAP  = 0x40,
 *          LSM6DSL_INT_SLEEP       = 0x80,
 *          LSM6DSL_INT_ALL         = 0xFF,
 *        };
 *      To enable all alerts use LSM6DSL_INT_ALL
 *
 * @param[in,out]   dev     Targeted device
 * @param[in]       alert   Alert to enable
 * @param[in]       pin     Interrupt pin
 * @param[in]       cb      Callback
 * @param[in]       arg     Callback parameter
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_enable_alert(lsm6dsl_t *dev, uint8_t alert, uint8_t pin,
                            lsm6dsl_cb_t cb, void *arg);

/**
 * @brief Disable alert :
 *          * Activity/Inactivity recognition
 *          * wake up interrupt
 *          * Free Fall interrupt
 *          * 6D/4D interrupt
 *          * TAP interrupt
 *
 * @note: enum {
 *          LSM6DSL_INT_TIMER       = 0x01, // Not used
 *          LSM6DSL_INT_TILT        = 0x02, // Not used
 *          LSM6DSL_INT_6D_4D       = 0x04,
 *          LSM6DSL_INT_DOUBLE_TAP  = 0x08,
 *          LSM6DSL_INT_FREE_FALL   = 0x10,
 *          LSM6DSL_INT_WAKE_UP     = 0x20,
 *          LSM6DSL_INT_SINGLE_TAP  = 0x40,
 *          LSM6DSL_INT_SLEEP       = 0x80,
 *          LSM6DSL_INT_ALL         = 0xFF,
 *        };
 *      To disable all alerts use LSM6DSL_INT_ALL
 *
 * @param[in,out]   dev     Targeted device
 * @param[in]       alert   Alert to enable
 * @param[in]       pin     Interrupt pin
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_disable_alert(lsm6dsl_t *dev, uint8_t alert, uint8_t pin);

/**
 * @brief get wake up alert sources:
 *          * Activity/Inactivity recognition
 *          * wake up interrupt
 *          * Free Fall interrupt
 *
 * @param[in]   dev             Targeted device
 * @param[out]  reason          Wake up reason
 * @param[in]   disable_flag    Set to true to disable interrupts(*1)
 *
 * @note (*1) if set to true, the following interrupts will be disabled:
 *              * 6D/4D
 *              * free-fall
 *              * wake-up
 *              * tap
 *              * inactivity
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_get_wkup_src_alert(const lsm6dsl_t *dev,
                                    lsm6dsl_wkup_src_t *reason,
                                    bool disable_flag);

/**
 * @brief 4D/6D alerts configuration
 *
 * @param[in]   dev     Targeted device
 * @param[in]   params  Parameters to set
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_configure_6D4D_alert(const lsm6dsl_t *dev,
                                    const lsm6dsl_6d4d_params_t *params);

/**
 * @brief get 4D/6D alerts sources
 *
 * @param[in]   dev             Targeted device
 * @param[out]  reason          4D/6D reason
 * @param[in]   disable_flag    Set to true to disable interrupts(*1)
 *
 * @note (*1) if set to true, the following interrupts will be disabled:
 *              * 6D/4D
 *              * free-fall
 *              * wake-up
 *              * tap
 *              * inactivity
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_get_6D4D_src_alert(const lsm6dsl_t *dev,
                                    lsm6dsl_6d4d_src_t *reason,
                                    bool disable_flag);

/**
 * @brief Single/Double TAP alerts configuration
 *
 * @param[in]   dev     Device to configure
 * @param[in]   params  Parameters to set
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_configure_tap_alert(const lsm6dsl_t *dev,
                                    const lsm6dsl_tap_params_t *params);

/**
 * @brief get TAP alert sources:
 *
 * @param[in]   dev             Targeted device
 * @param[out]  reason          TAP reason
 * @param[in]   disable_flag    Set to true to disable interrupts(*1)
 *
 * @note (*1) if set to true, the following interrupts will be disabled:
 *              * 6D/4D
 *              * free-fall
 *              * wake-up
 *              * tap
 *              * inactivity
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_get_tap_src_alert(const lsm6dsl_t *dev,
                                    lsm6dsl_tap_src_t *reason,
                                    bool disable_flag);

/**
 * @brief set X-axis offset
 *
 * @note : This offset will be ADDED to
 *         the internal measured value
 *
 * @param[in]   dev         Targeted device
 * @param[in]   offset      Wanted offset
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_set_x_offset(const lsm6dsl_t *dev, int8_t offset);

/**
 * @brief get X-axis offset
 *
 * @param[in]   dev         Targeted device
 * @param[out]  offset      Applied offset
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_get_x_offset(const lsm6dsl_t *dev, int8_t *offset);

/**
 * @brief set Y-axis offset
 *
 * @note : This offset will be ADDED to
 *         the internal measured value
 *
 * @param[in]   dev         Targeted device
 * @param[in]   offset      Wanted offset
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_set_y_offset(const lsm6dsl_t *dev, int8_t offset);

/**
 * @brief get Y-axis offset
 *
 * @param[in]   dev         Targeted device
 * @param[out]  offset      Applied offset
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_get_y_offset(const lsm6dsl_t *dev, int8_t *offset);

/**
 * @brief set Z-axis offset
 *
 * @note : This offset will be SUBTRACTED to
 *         the internal measured value
 *
 * @param[in]   dev         Targeted device
 * @param[in]   offset      Wanted offset
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_set_z_offset(const lsm6dsl_t *dev, int8_t offset);

/**
 * @brief get Z-axis offset
 *
 * @param[in]   dev         Targeted device
 * @param[out]  offset      Applied offset
 *
 * @return LSM6DSL_OK on success
 * @return < 0 on error
 */
int lsm6dsl_acc_get_z_offset(const lsm6dsl_t *dev, int8_t *offset);

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSL_H */
/** @} */
