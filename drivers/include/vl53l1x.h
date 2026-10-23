/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_vl53l1x VL53L1X Time-of-Flight (ToF) ranging sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the ST VL53L1X Time-of-Flight (ToF) ranging sensor
 *
 * \section drivers_vl53l1x VL53L1X Time-of-Flight (ToF) ranging sensor
 *
 * ## Driver Variants
 *
 * Using the according module, the driver can be used in two different variants
 * which differ in functionality and size:
 *
 * Module Name     | Driver             | Short Description
 * :---------------|:-------------------|:----------------------------
 * `vl53l1x_basic` | \ref vl53l1x_basic | Minimum functionality at small size
 * `vl53l1x`       | \ref vl53l1x       | Complete functionality at acceptable size
 *
 * If the driver variant is not specified, \ref vl53l1x is used by default.
 * Both driver variants provide \ref drivers_saul capabilities for distance.
 *
 * ## Basic Driver (vl53l1x_basic) {#vl53l1x_basic}
 *
 * This is the smallest driver variant that only provides some basic functions
 * such as
 *
 * - distance measurements at acceptable accuracy,
 * - data-ready interrupts, and
 * - SAUL capability.
 *
 * This driver should be used when memory requirements are an issue.
 *
 * ## Default Driver (vl53l1x) {#vl53l1x}
 *
 * Using \ref vl53l1x variant provides the application with the functionality
 * of the basic driver variant \ref vl53l1x_basic and additional functionality
 * such as
 *
 * - configuration of sensor parameters (distance mode, timing budget
 *   and inter-measurement period),
 * - configuration and use of distance threshold interrupts, or
 * - power handling.
 *
 * Additionally, it provides access to the complete functionality of the sensor
 * by using VL53L1X ULD API functions directly. This is for example necessary to
 * to calibrate the sensor. Even though the sensor is calibrated by ST during
 * the final module test and these calibration data are loaded from NVM, the
 * sensor should be calibrated on customers production line when soldered on
 * customer board or adding a cover glass.
 *
 * Please refer the [VL53L1X ULD API user manual]
 * (https://www.st.com/resource/en/user_manual/um2510-a-guide-to-using-the-vl53l1x-ultra-lite-driver-stmicroelectronics.pdf)
 * for detailed information on how to use the API to calibrate the sensor.
 *
 * @note    Since the ST STSW-IMG009 VL53L1X ULD API provides functions that are
 *          not required in most use cases, this driver variant should only be
 *          used when memory requirements are not an issue.
 *
 * ## Driver Comparison Sheet
 *
 * Function / Property                     | vl53l1x_basic | vl53l1x
 * :---------------------------------------|:-------------:|:--------------:
 * Distance results in mm                  | X             | X
 * Signal rate results in kcps             |               | X
 * Measurement status information          |               | X
 * SAUL capability                         | X             | X
 * Distance mode configuration             |               | X
 * Timing budget configuration             |               | X
 * Inter-measurement period configuration  |               | X
 * Region of Interest (ROI) configuration  |               | X
 * Data-ready interrupts (ranging mode)    | X             | X
 * Threshold interrupts (detection mode)   |               | X
 * Power on/off functions                  |               | X
 * SHUTDOWN pin support                    |               | X
 * Calibration functions                   |               | X [1]
 * Size on reference platform in kByte [2] | 1.0           | 2.9
 *
 * [1] These functions are available by using the ST VL53L1X ULD API directly.<br>
 * [2] Reference platform: STM32F411RE
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "kernel_defines.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#define VL53L1X_DEVICE_ID   (0xeacc)    /**< VL53L1X Model ID */
#define VL53L1X_I2C_ADDRESS (0x29)      /**< VL53L1X I2C address */

/**
 * @brief   VL53L1X distance modes
 *
 * The values are reused from the ST VL53L1X ULD API for compatibility.
 * Although the sensor supports 3 distance modes (short, medium, long),
 * the ST VL53L1X ULD API supports only two which should be sufficient in
 * most use cases.
 *
 * @note    Distance mode CANNOT be configured if module \ref vl53l1x_basic
 *          is enabled.
 */
typedef enum {
    VL53L1X_DIST_SHORT  = 1,        /**< up to 1.3 m */
    VL53L1X_DIST_LONG   = 2,        /**< up to 4 m */
} vl53l1x_dist_mode_t;

/**
 * @brief   Interrupt mode
 *
 * The interrupt mode defines when an interrupt is triggered. Interrupts can
 * be triggered in each measurement cycle, either when new data is available
 * (ranging mode) or when the distance threshold conditions defined by
 * \ref vl53l1x_thresh_config_t are met (detection mode). Use the function
 * \ref vl53l1x_int_config to configure these interrupts.
 *
 * @note    If the \ref vl53l1x_basic is used, only data-ready interrupts
 *          are supported.
 */
typedef enum {
    VL53L1X_INT_DATA_READY,  /**< interrupt on new data */
    VL53L1X_INT_DIST_THRESH, /**< interrupt when distance threshold conditions are met */
} vl53l1x_int_mode_t;

/**
 * @brief   Threshold modes used for detection mode
 *
 * The threshold mode defines how the upper and lower thresholds are used
 * to trigger an interrupt in detection mode.
 *
 * @note Although the VL53L1X allows the definition of thresholds for distance
 *       and signal rate, the ST VL53L1X ULD API only allows the definition
 *       of thresholds for the distance.
 */
typedef enum {
    VL53L1X_THRESH_HIGH = 0, /**< when the distance exceeds the upper threshold */
    VL53L1X_THRESH_LOW  = 1, /**< when the distance falls below the lower threshold */
    VL53L1X_THRESH_OUT  = 2, /**< when the distance exceeds the upper threshold or
                                  falls below the lower threshold (the distance
                                  leaves the threshold window) */
    VL53L1X_THRESH_IN   = 3, /**< when the distance exceeds the lower threshold or
                                  falls below the higher threshold (the distance
                                  enters the threshold window) */
} vl53l1x_thresh_mode_t;

/**
 * @brief   Distance threshold configuration
 *
 * The distance threshold configuration is used in function \ref vl53l1x_int_config
 * to configure the conditions when interrupts are triggered in detection mode.
 *
 * @note    If the \ref vl53l1x_basic is used, only data-ready interrupts can be
 *          used. The distance threshold configuration is not used in this case.
 */
typedef struct {
    vl53l1x_thresh_mode_t mode; /**< threshold mode for distance */
    uint16_t high;              /**< upper threshold for distance in mm */
    uint16_t low;               /**< lower threshold for distance in mm */
} vl53l1x_thresh_config_t;

/**
 * @brief   VL53L1X device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;      /**< I2C device, default I2C_DEV(0) */
    uint8_t i2c_addr;   /**< I2C slave address */
    bool vddio_2v8;     /**< if true, I/O voltage is 2.8 V, otherwise 1.8 V */
    gpio_t pin_int;     /**< Interrupt pin open drain, active low:
                             \ref GPIO_UNDEF if not used */
#if !IS_USED(MODULE_VL53L1X_BASIC)
    gpio_t pin_shutdown;/**< Shutdown pin low active:
                             \ref GPIO_UNDEF if not used */
    uint16_t budget;    /**< Timing budget given to the sensor to perform range
                             measurements in ms [15, 20, 33, 50, 100, 200, 500],
                             default: 50 ms */
    uint32_t period;    /**< Inter-measurement period in ms which has to be
                             at least vl53l1x_params_t::budget + 4ms,
                             default: 100 ms */
    vl53l1x_dist_mode_t mode; /**< Distance mode, default: VL53L1X_DIST_LONG */
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */
} vl53l1x_params_t;

/**
 * @brief   VL53L1X sensor device data structure type
 */
typedef struct {
    uint16_t addr;      /**< Device address as used by the ST VL53L1X ULD API,
                             format = I2C bus num << 8 | I2C slave address */
    bool int_init;      /**< Interrupt pin is already initialized */
#if !IS_USED(MODULE_VL53L1X_BASIC)
    uint16_t budget;    /**< Timing budget used */
    uint32_t period;    /**< Inter-measurement period used */
    vl53l1x_dist_mode_t mode;       /**< Distance mode used */
#endif
    const vl53l1x_params_t *params; /**< Device initialization parameters */
} vl53l1x_t;

/**
 * @brief   VL53L1X ranging data
 */
typedef struct {
    uint8_t  status;        /**< status for the current measurement */
    uint16_t distance;      /**< distance in millimeter */
    uint16_t signal_rate;   /**< signal rate in kcps (kilo count per seconds) */
    uint16_t ambient_rate;  /**< ambient rate in kcps (kilo count per seconds) */
} vl53l1x_data_t;

/**
 * @brief   VL53L1X region of interest (ROI)
 *
 * Defines the region of interest (ROI) within the 16 x 16 SPAD (single photon
 * avalanche diode) array. The ROI is a square or rectangle defined by
 * x and y dimension. The center is set automatically.
 */
typedef struct {
    uint16_t x_size;     /**< ROI x size [4...16] (default 16)  */
    uint16_t y_size;     /**< ROI y size [4...16] (default 16) */
} vl53l1x_roi_t;

/**
 * @brief   Initialize the VL53L1X sensor device
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor to be initialized
 * @param[in]   params  configuration parameters, see \ref vl53l1x_params_t
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 * @retval  -ENODEV no device found
 */
int vl53l1x_init(vl53l1x_t *dev, const vl53l1x_params_t *params);

/**
 * @brief   Data-ready status function
 *
 * The function can be used for polling to know when new ranging data are
 * available.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 *
 * @retval  0       new ranging data are ready
 * @retval  -EAGAIN no new ranging data available
 * @retval  -EIO    on I2C communication error
 */
int vl53l1x_data_ready(vl53l1x_t *dev);

/**
 * @brief   Read one ranging data sample in mm
 *
 * This function returns only the ranging data in millimeters.
 *
 * @note    Since reading any data resets the interrupt as well as the
 *          data-ready flag, either function \ref vl53l1x_read_mm or function
 *          \ref vl53l1x_read_data can be used in one measurement cycle.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 * @param[out]  mm      ranging data in mm
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 */
int vl53l1x_read_mm(vl53l1x_t *dev, uint16_t *mm);

#if !IS_USED(MODULE_VL53L1X_BASIC)
/**
 * @brief   Read one ranging data sample with status and signal information
 *
 * This function returns the ranging data distance together with additional
 * information about the measurement like the status and the signals.
 *
 * @note
 * - Since reading any data resets the interrupt as well as the data-ready
 *   flag, either function \ref vl53l1x_read_mm or function
*    \ref vl53l1x_read_data can be used in one measurement cycle.
 * - This function is NOT available if module \ref vl53l1x_basic is used.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 * @param[out]  data    ranging data
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 */
int vl53l1x_read_data(vl53l1x_t *dev, vl53l1x_data_t *data);

/**
 * @brief   Power down the sensor
 *
 * This function requires that a GPIO connected to sensor's /XSHUT pin is
 * defined by parameter vl53l1x_params_t::pin_shutdown.
 *
 * @note    This function is NOT available if module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 */
int vl53l1x_power_down(const vl53l1x_t *dev);

/**
 * @brief   Power up the sensor
 *
 * This function requires that a GPIO connected to sensor's /XSHUT pin is
 * defined by parameter vl53l1x_params_t::pin_shutdown.
 *
 * @note    This function is NOT available if module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 * @retval  -ENODEV no device found
 */
int vl53l1x_power_up(vl53l1x_t *dev);

/**
 * @brief   Change the timing budget in microseconds
 *
 * This function can be used to change the timing budget that was defined
 * by configuration parameter vl53l1x_params_t::budget during sensor
 * initialization.
 *
 * @note    This function is NOT available if module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev         device descriptor of VL53L1X sensor
 * @param[in]   budget_ms   new timing budget in ms
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 * @retval  -EINVAL if the current distance mode or the given budget_ms parameter is invalid
 */
int vl53l1x_set_timing_budget(vl53l1x_t *dev, uint16_t budget_ms);

/**
 * @brief   Change the inter-measurement period im ms
 *
 * This function can be used to change the the inter-measurement period that was
 * defined by configuration parameter vl53l1x_params_t::period during sensor
 * initialization.
 *
 * @pre     The inter-measurement period MUST be longer than the timing budget
 *          + 4 ms. Otherwise the function fails. Therefore, if the time
 *          budget is also changed, it should be done first.
 *
 * @note    This function is NOT available if module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev         device descriptor of VL53L1X sensor
 * @param[in]   period_ms   new measurement period in ms
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 */
int vl53l1x_set_measurement_period(vl53l1x_t *dev, uint32_t period_ms);

/**
 * @brief   Change the distance mode
 *
 * This function can be used to change the the distance mode that was
 * defined by configuration parameter vl53l1x_params_t::mode during sensor
 * initialization.
 *
 * @note    This function is NOT available if module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 * @param[in]   mode    new distance mode, see \ref vl53l1x_dist_mode_t
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 * @retval  -EINVAL if the given mode parameter or the current budget is invalid
 */
int vl53l1x_set_distance_mode(vl53l1x_t *dev, vl53l1x_dist_mode_t mode);

/**
 * @brief   Set the region of interest (ROI)
 *
 * Using this function, the region of interest (ROI) within the 16 x 16 SPAD
 * (single photon avalanche diode) array can be defined.
 *
 * @note    This function is NOT available if module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 * @param[in]   roi     new region of interest, see #vl53l1x_roi_t
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 */
int vl53l1x_set_roi(vl53l1x_t *dev, vl53l1x_roi_t *roi);

/**
 * @brief   Get the region of interest (ROI)
 *
 * @note    This function is NOT available if module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 * @param[out]  roi     current region of interest, see \ref vl53l1x_roi_t
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 */
int vl53l1x_get_roi(vl53l1x_t *dev, vl53l1x_roi_t *roi);

#endif /* IS_USED(MODULE_VL53L1X_BASIC) */

/**
 * @brief   Configure interrupts
 *
 * The function
 *
 * - configures the interrupts of the sensor,
 * - initializes the GPIO defined by vl53l1x_params_t::pin_int, and
 * - attaches the ISR specified by the @p isr parameter to the interrupt.
 *
 * @warning Since the ISR is executed in the interrupt context, it must not be
 *          blocking or time consuming. In addition, it must not access the
 *          sensor directly via I2C. It should only indicate to a waiting
 *          thread that an interrupt has occurred, which is then handled
 *          in the thread context.
 *          If it is tried to access the sensor in interrupt context, an
 *          assertion blows up (if assertions are enabled).
 *
 * @note    The configuration of distance threshold interrupts is ONLY
 *          used if the \ref vl53l1x driver variant is used. Otherwise, only
 *          data-ready interrupts are used independent on the configuration given
 *          by parameter \p cfg. In later case, parameter \p cfg can be NULL.
 *
 * @param[in]   dev     device descriptor of the VL53L1X sensor
 * @param[in]   mode    interrupt mode, see \ref vl53l1x_int_mode_t
 * @param[in]   cfg     threshold configuration, see \ref vl53l1x_thresh_config_t
 * @param[in]   isr     ISR called for all types of interrupts
 * @param[in]   isr_arg ISR argument, can be NULL
 *
 * @retval  0       on success
 * @retval  -EIO    on I2C communication error
 */
int vl53l1x_int_config(vl53l1x_t *dev, vl53l1x_int_mode_t mode,
                                       vl53l1x_thresh_config_t* cfg,
                                       void (*isr)(void *),
                                       void *isr_arg);
#ifdef __cplusplus
}
#endif

/** @} */
