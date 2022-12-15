/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
 * Using the according module, the driver can be used in different variants
 * which differ in functionality and size:
 *
 * Module Name         | Driver                    | Short Description
 * :-------------------|:--------------------------|:----------------------------
 * \ref vl53l1x_std    | Standard driver (default) | Common functionality and medium size
 * \ref vl53l1x_st_api | ST VL53L1X API driver     | Complete functionality and large size
 * \ref vl53l1x_basic  | Basic driver              | Minimum functionality and small size
 *
 * If no driver variant is explicitly specified, \ref vl53l1x_std is used by
 * default. All driver variants provide \ref drivers_saul capabilities for distance.
 *
 * ## ST VL53L1X API driver (vl53l1x_st_api) {#vl53l1x_st_api}
 *
 * This variant of the driver uses ST STSW-IMG007 VL53L1X Full API as package.
 * In this case, the driver is simply a wrapper for the API which provides a
 * driver interface that is compatible with other driver variants.
 *
 * Using vl53l1x_st_api variant provides the application with the functionality
 * of other driver variants. Additionally, it provides access to the complete
 * functionality of the sensor by using API functions directly. This is for
 * example necessary to
 *
 * - to configure and use threshold interrupts, or
 * - to calibrate the sensor.
 *
 * Even though the sensor is calibrated by ST during the final module test and
 * these calibration data are loaded from NVM, the sensor should be calibrated
 * on customers production line when soldered on customer board or adding a
 * cover glass.
 *
 * Please refer the [VL53L1X API user manual]
 * (https://www.st.com/resource/en/user_manual/dm00474730.pdf) for detailed
 * information on how to use the API to calibrate the sensor.
 *
 * @note    Since the ST STSW-IMG007 VL53L1X API package is quite complex and
 *          very large, this driver variant should only be used when memory
 *          requirements are not an issue.
 *
 * @warning The ST STSW-IMG007 VL53L1X API package functions use a significant
 *          amount of memory in the stack. If you have crashes due to memory
 *          access errors, try increasing the default thread stack size
 *          \ref THREAD_STACKSIZE_DEFAULT.
 *
 * ## Standard driver (vl53l1x_std) {#vl53l1x_std}
 *
 * This driver variant is a compromise of size and functionality. It provides
 * the application with most functionality of the sensor. The driver can be
 * used when memory requirements are important and most of the functionality
 * should be used.
 *
 * Since the sensor loads its calibration data from NVM during boot, this
 * driver variant gives results with same accuracy as with module
 * \ref vl53l1x_st_api once the calibration of the sensor is done on customers
 * production line with module \ref vl53l1x_st_api.
 *
 * ## Basic driver (vl53l1x_basic) {#vl53l1x_basic}
 *
 * This is the smallest driver variant that only provides some basic functions
 * such as
 *
 * - the distance measurements and
 * - the data-ready interrupt
 *
 * at acceptable accuracy. This driver should be used when memory requirements
 * are an issue.
 *
 * ## Driver Comparison Sheet
 *
 * Function / Property                     | vl53l1x_basic | vl53l1x_std | vl53l1x_st_api
 * :---------------------------------------|:-------------:|:-----------:|:--------------:
 * Distance results in mm                  | X             | X           | X
 * Signal rate results in MCPS             |               | X           | X
 * Measurement status information          |               | X           | X
 * SAUL capability                         | X             | X           | X
 * Distance mode configuration             |               | X           | X
 * Timing budget configuration             |               | X           | X
 * Inter-measurement period configuration  |               | X           | X
 * Region of Interest (ROI) configuration  |               | X           | X
 * Data-ready interrupts                   | X             | X           | X
 * Threshold interrupts                    |               |             | X
 * Calibration functions                   |               |             | X [1]
 * Limit check configuration               |               |             | X [1]
 * Accuracy                                | medium        | high        | high
 * Size on reference platform in kByte [2] | 1.0           | 2.9         | 19.8
 *
 * [1] These functions are available by using the ST VL53L1X API directly.<br>
 * [2] Reference platform: STM32F411RE
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef VL53L1X_H
#define VL53L1X_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "kernel_defines.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#if IS_USED(MODULE_VL53L1X_ST_API)

#include "vl53l1_api.h"

/* to be able to use also VL53L1_ instead of VL53L1X_ for consistency */
#define VL53L1X_ VL53L1_
#define vl53l1x_ vl53l1_
/* platform specific error codes are starting at VL53L1X_ERROR_BASE */
#define VL53L1X_ERROR_BASE          VL53L1_ERROR_PLATFORM_SPECIFIC_START

#else /* IS_USED(MODULE_VL53L1X_ST_API) */

#define VL53L1X_ERROR_BASE          (0) /**< base value of driver error codes */

#define VL53L1X_RANGESTATUS_RANGE_VALID                    0   /**< range is valid */
#define VL53L1X_RANGESTATUS_SIGMA_FAIL                     1   /**< sigma failure */
#define VL53L1X_RANGESTATUS_SIGNAL_FAIL                    2   /**< signal failure */
#define VL53L1X_RANGESTATUS_RANGE_VALID_MIN_RANGE_CLIPPED  3   /**< target is below minimum
                                                                    detection threshold */
#define VL53L1X_RANGESTATUS_OUTOFBOUNDS_FAIL               4   /**< phase out of valid limits,
                                                                    different to a wrap exit. */
#define VL53L1X_RANGESTATUS_HARDWARE_FAIL                  5   /**< hardware failure */
#define VL53L1X_RANGESTATUS_RANGE_VALID_NO_WRAP_CHECK_FAIL 6   /**< range is valid but the wrap
                                                                    around check has not been
                                                                    done */
#define VL53L1X_RANGESTATUS_WRAP_TARGET_FAIL               7   /**< wrapped target, no matching
                                                                    phase in other VCSEL period
                                                                    timing */
#define VL53L1X_RANGESTATUS_XTALK_SIGNAL_FAIL              9   /**< specific to lite ranging */
#define VL53L1X_RANGESTATUS_SYNCRONISATION_INT             10  /**< first interrupt when starting
                                                                    ranging in back to back mode */
#define VL53L1X_RANGESTATUS_MIN_RANGE_FAIL                 13  /**< user ROI input is not valid */
#define VL53L1X_RANGESTATUS_NONE                           255 /**< No Update. */

#define VL53L1_DISTANCEMODE_SHORT   1  /**< distances up to 1.3 m */
#define VL53L1_DISTANCEMODE_MEDIUM  2  /**< distances up to 2 m */
#define VL53L1_DISTANCEMODE_LONG    3  /**< distances up to 4 m */

#endif /* IS_USED(MODULE_VL53L1X_ST_API) */

/**
 * @name Definition of error codes
 * @{
 */
#define VL53L1X_OK              (0)                      /**< success */
#define VL53L1X_ERROR_I2C       (VL53L1X_ERROR_BASE - 1) /**< I2C communication error */
#define VL53L1X_ERROR_WRONG_ID  (VL53L1X_ERROR_BASE - 2) /**< wrong id read */
#define VL53L1X_ERROR_NO_DATA   (VL53L1X_ERROR_BASE - 3) /**< no data available */
#define VL53L1X_ERROR_RAW_DATA  (VL53L1X_ERROR_BASE - 4) /**< read raw data failed */
#define VL53L1X_ERROR_NO_PIN    (VL53L1X_ERROR_BASE - 5) /**< pin not defined */
#define VL53L1X_ERROR_TIMEOUT   (VL53L1X_ERROR_BASE - 6) /**< timeout happened */
#define VL53L1X_ERROR_INV_ARG   (VL53L1X_ERROR_BASE - 7) /**< invalid argument */
#define VL53L1X_ERROR_INV_PARAM (VL53L1X_ERROR_BASE - 8) /**< invalid params */
/** @} */

#if !IS_USED(MODULE_VL53L1X_BASIC) || DOXYGEN
/**
 * @brief   VL53L1X distance modes
 *
 * The values are reused from the ST VL53L1X API for compatibility.
 *
 * @note    Distance mode CANNOT be configured when module \ref vl53l1x_basic
 *          is enabled.
 */
typedef enum {
    VL53L1X_DIST_SHORT  = VL53L1_DISTANCEMODE_SHORT,    /**< up to 1.3 m */
    VL53L1X_DIST_MEDIUM = VL53L1_DISTANCEMODE_MEDIUM,   /**< up to 2 m */
    VL53L1X_DIST_LONG   = VL53L1_DISTANCEMODE_LONG,     /**< up to 4 m */
} vl53l1x_dist_mode_t;
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

#if IS_USED(MODULE_VL53L1X_ST_API) || DOXYGEN
/**
 * @brief   Threshold modes used for threshold interrupts
 *
 * For distances and signal rates, upper and lower thresholds can be defined,
 * with the upper and lower thresholds defining a threshold window. These
 * thresholds can be used to trigger threshold interrupts.
 *
 * #vl53l1x_thresh_mode_t is used by interrupt configuration to define the
 * conditions under which an threshold interrupt will be triggered.
 *
 * @note    Threshold interrupts are ONLY available when module
 *          \ref vl53l1x_st_api is enabled. Otherwise only data-ready
 *          interrupts can be used.
 */
typedef enum {
    VL53L1X_THRESH_HIGH = 0, /**< only when the value exceeds the upper
                                  threshold */
    VL53L1X_THRESH_LOW  = 1, /**< only when the value falls below the lower
                                  threshold */
    VL53L1X_THRESH_OUT  = 2, /**< when the value exceeds the upper threshold or
                                  falls below the lower threshold (the value
                                  leaves the threshold window) */
    VL53L1X_THRESH_IN   = 3, /**< when value exceeds the lower threshold or
                                  falls below the higher threshold (the value
                                  enters the threshold window) */
} vl53l1x_thresh_mode_t;

/**
 * @brief   Interrupt mode
 *
 * It defines when an interrupt is triggered. Interrupts can be triggered
 * either on new data in each measurement cycly or when the threshold condition
 * defined by the vl53l1x_thresh_mode_t is fulfilled.
 *
 * @note    The interrupt mode can be configured ONLY if \ref vl53l1x_st_api
 *          is enabled. Otherwise only data-ready interrupts can be used.
 */
typedef enum {
    VL53L1X_INT_DATA_READY = 0,     /**< on new data in each measurement cycle */
    VL53L1X_INT_DIST = 1,           /**< on distance threshold condition */
    VL53L1X_INT_RATE = 2,           /**< on signal rate threshold condition */
    VL53L1X_INT_DIST_OR_RATE  = 3,  /**< on distance or signal rate threshold condition */
    VL53L1X_INT_DIST_AND_RATE  = 4, /**< on distance and signal rate threshold condition */
} vl53l1x_int_mode_t;
#endif  /* IS_USED(MODULE_VL53L1X_ST_API) || DOXYGEN */

/**
 * @brief   Interrupt configuration
 *
 * @note    Interrupts can be configured only if \ref vl53l1x_st_api
 *          is enabled. Otherwise only data-ready interrupts can be used.
 */
typedef struct {
#if IS_USED(MODULE_VL53L1X_ST_API) || DOXYGEN
    vl53l1x_int_mode_t mode;         /**< defines when interrupts are triggered */

    vl53l1x_thresh_mode_t dist_mode; /**< threshold mode for distance */
    uint16_t dist_high;              /**< upper threshold for distance in mm */
    uint16_t dist_low;               /**< lower threshold for distance in mm */

    vl53l1x_thresh_mode_t rate_mode; /**< threshold mode for signal rates */
    uint32_t rate_high;              /**< upper threshold for signal rate
                                          as fixed point 16.16 */
    uint32_t rate_low;               /**< lower threshold for signal rate
                                          as fixed point 16.16 */
#endif  /* IS_USED(MODULE_VL53L1X_ST_API) || DOXYGEN */
} vl53l1x_int_config_t;

/**
 * @brief   VL53L1X device initialization parameters
 */
typedef struct {

    unsigned i2c_dev;   /**< I2C device, default I2C_DEV(0) */
    bool vddio_2v8;     /**< if true, I/O voltage is 2.8 V, otherwise 1.8 V */

    gpio_t pin_int;     /**< Interrupt pin open drain, active low:
                             #GPIO_UNDEF if not used */
#if !IS_USED(MODULE_VL53L1X_BASIC)
    gpio_t pin_shutdown;/**< Shutdown pin low active:
                             #GPIO_UNDEF if not used */
    uint32_t budget;    /**< Timing budget given to the sensor to perform range
                             measurements in us [20000 ... 1000000],
                             default: 50 ms */
    uint32_t period;    /**< Inter-measurement period in ms, has to be
                             vl53l1x_params_t::t_budget + 4ms,
                             default: 100 ms */
    vl53l1x_dist_mode_t mode; /**< Distance mode, default: VL53L1X_DIST_LONG */
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

} vl53l1x_params_t;

/**
 * @brief   VL53L1X sensor device data structure type
 */
typedef struct {
#if IS_USED(MODULE_VL53L1X_ST_API)
    VL53L1_Dev_t dev;   /**< ST VL53L1 API device structure, must be the first
                             member for compatibility with ST VL53L1 API. The
                             driver device structure is just an extension. */
#elif !IS_USED(MODULE_VL53L1X_BASIC)
    bool     calibrated;         /**< Device is already calibrated */
    uint16_t fast_osc_frequency; /**< Fast OSC Frequency in fixed point 4.12 format */
    uint16_t osc_calibrate_val;  /**< OSC calibration value */
#endif

    vl53l1x_params_t params;     /**< Device initialization parameters */
    bool int_init;               /**< Interrupt pin is already initialized */

} vl53l1x_t;

/**
 * @brief   VL53L1X ranging data
 *
 * Please refer the [VL53L1X API user manual]
 * (https://www.st.com/resource/en/user_manual/dm00474730.pdf)
 * section 2.6 for detailed information about the meaning of ranging data
 * values.
 */
typedef struct {
    uint16_t distance;      /**< distance in millimeter */
    uint8_t  status;        /**< status for the current measurement */
    uint32_t signal_rate;   /**< signal rate in MCPS as fixed point 16.16 */
    uint32_t ambient_rate;  /**< ambient rate in MCPS as fixed point 16.16 */
} vl53l1x_data_t;

#if !IS_USED(MODULE_VL53L1X_BASIC)
/**
 * @brief   VL53L1X region of interest (ROI)
 *
 * Defines the region of interest (ROI) within the 16 x 16 SPAD (single photon
 * avalanche diode) array. The ROI is a square or rectangle defined by two
 * corners: top left and bottom right.
 */
typedef struct {
    uint8_t x_tl; /**< ROI top left x coordinate [0...15] (default 0)  */
    uint8_t y_tl; /**< ROI top left y coordinate [0...15] (default 15) */
    uint8_t x_br; /**< ROI bottom right x coordinate [0...15] (default 15) */
    uint8_t y_br; /**< ROI bottom right x coordinate [0...15] (default 0)  */
} vl53l1x_roi_t;
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

/**
 * @brief   Initialize the VL53L1X sensor device
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor to be initialized
 * @param[in]   params  configuration parameters, see #vl53l1x_params_t
 *
 * @retval  VL53L1X_OK      on success
 * @retval  VL53L1X_ERROR_* a negative error code on error
 */
int vl53l1x_init(vl53l1x_t *dev, const vl53l1x_params_t *params);

/**
 * @brief   Data-ready status function
 *
 * The function can be used for polling to know when new ranging data are ready.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 *
 * @retval  VL53L1X_OK             new ranging data are ready
 * @retval  VL53L1X_ERROR_NO_DATA  no new ranging data available
 * @retval  VL53L1X_ERROR_*        negative error code
 */
int vl53l1x_data_ready(vl53l1x_t *dev);

/**
 * @brief   Read one ranging data sample in mm
 *
 * This function returns only the ranging data in millimeters.
 *
 * @note    Since reading any data resets the interrupt as well as the
 *          data-ready flag, either function #vl53l1x_read_mm, function
 *          #vl53l1x_read_data, or function #vl53l1x_read_details can be
 *          used in one measurement cycle.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 * @param[out]  mm      ranging data in mm
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_read_mm(vl53l1x_t *dev, int16_t *mm);

#if !IS_USED(MODULE_VL53L1X_BASIC)
/**
 * @brief   Read one ranging data sample with status and signal information
 *
 * This function returns the ranging data distance together with additional
 * information about the measurement like the status and the signals.
 *
 * Please refer the [VL53L1X API user manual]
 * (https://www.st.com/resource/en/user_manual/dm00474730.pdf)
 * section 2.6 for detailed information about the meaning of ranging data
 * values.
 *
 * @note
 * - Since reading any data resets the interrupt as well as the data-ready
 *   flag, either function #vl53l1x_read_mm, function #vl53l1x_read_data, or
 *   function #vl53l1x_read_details can be used in one measurement cycle.
 * - This function is NOT available when module \ref vl53l1x_basic is used.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 * @param[out]  data    ranging data
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_read_data(vl53l1x_t *dev, vl53l1x_data_t *data);

#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

#if IS_USED(MODULE_VL53L1X_ST_API) || DOXYGEN
/**
 * @brief   Read one ranging data sample with detailed information
 *
 * This function can only be used with module \ref vl53l1x_st_api and the
 * ST STSW-IMG007 VL53L1X API to get detailed information about the measurement.
 *
 * Please refer the [VL53L1X API user manual]
 * (https://www.st.com/resource/en/user_manual/dm00474730.pdf)
 * section 2.6 for detailed information about the meaning of detailed
 * ranging data.
 *
 * @note
 * - Since reading any data resets the interrupt as well as the data-ready
 *   flag, either function #vl53l1x_read_mm, function #vl53l1x_read_data, or
 *   function #vl53l1x_read_details can be used in one measurement cycle.
 * - This function is ONLY available when module \ref vl53l1x_basic is used.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 * @param[out]  details ranging data
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_read_details(vl53l1x_t *dev,
                         VL53L1_RangingMeasurementData_t *details);

#endif /* IS_USED(MODULE_VL53L1X_ST_API) || DOXYGEN */

#if !IS_USED(MODULE_VL53L1X_BASIC)
/**
 * @brief   Power down the sensor
 *
 * This function requires that a GPIO connected to sensor's /XSHUT pin is
 * defined by parameter vl53l1x_params_t::pin_shutdown.
 *
 * @note    This function is NOT available when module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_power_down(const vl53l1x_t *dev);

/**
 * @brief   Power down the sensor
 *
 * This function requires that a GPIO connected to sensor's /XSHUT pin is
 * defined by parameter vl53l1x_params_t::pin_shutdown.
 *
 * @note    This function is NOT available when module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_power_up(vl53l1x_t *dev);

/**
 * @brief   Change the timing budget in microseconds
 *
 * This function can be used to change the timing budget that was defined
 * by configuration parameter vl53l1x_params_t::budget during sensor
 * initialization.
 *
 * @note    This function is NOT available when module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev         device descriptor of VL53L1X sensor
 * @param[in]   budget_us   new timing budget in us
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_set_timing_budget(vl53l1x_t *dev, uint32_t budget_us);

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
 * @note    This function is NOT available when module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev         device descriptor of VL53L1X sensor
 * @param[in]   period_ms   new measurement period in ms
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_set_measurement_period(vl53l1x_t *dev, uint32_t period_ms);

/**
 * @brief   Change the distance mode
 *
 * This function can be used to change the the distance mode that was
 * defined by configuration parameter vl53l1x_params_t::mode during sensor
 * initialization.
 *
 * @note    This function is NOT available when module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 * @param[in]   mode    new distance mode, see #vl53l1x_dist_mode_t
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_set_distance_mode(vl53l1x_t *dev, vl53l1x_dist_mode_t mode);

/**
 * @brief   Set the region of interest (ROI)
 *
 * Using this function, the region of interest (ROI) within the 16 x 16 SPAD
 * (single photon avalanche diode) array can be defined.
 *
 * @note    This function is NOT available when module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 * @param[in]   roi     new region of interest, see #vl53l1x_roi_t
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_set_roi(vl53l1x_t *dev, vl53l1x_roi_t *roi);

/**
 * @brief   Get the region of interest (ROI)
 *
 * @note    This function is NOT available when module \ref vl53l1x_basic
 *          is used.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 * @param[out]  roi     region of interest, see #vl53l1x_roi_t
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code
 */
int vl53l1x_get_roi(vl53l1x_t *dev, vl53l1x_roi_t *roi);

#endif /* IS_USED(MODULE_VL53L1X_BASIC) */

/**
 * @brief   Configure interrupts
 *
 * The function
 *
 * - configures the interrupts of the sensor
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
 * @note    The interrupt configuration for threshold interrupts is ONLY
 *          used when module \ref vl53l1x_st_api is enabled. Otherwise, only
 *          data-ready interrupts are used independent on the configuration given
 *          by parameter \p cfg. In later case, parameter \p cfg can be NULL.
 *
 * @param[in]   dev     device descriptor of VL53L1X sensor
 * @param[in]   cfg     interrupt configuration, see #vl53l1x_int_config_t
 * @param[in]   isr     ISR called for all types of interrupts
 * @param[in]   isr_arg ISR argument, can be NULL
 *
 * @retval  VL53L1X_OK         on success
 * @retval  VL53L1X_ERROR_*    negative error code on error
 */
int vl53l1x_int_config(vl53l1x_t *dev, vl53l1x_int_config_t* cfg,
                                       void (*isr)(void *),
                                       void *isr_arg);
#ifdef __cplusplus
}
#endif

#endif /* VL53L1X_H */
/** @} */
