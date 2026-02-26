/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_amg88xx AMG88xx Infrared Array Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for Panasonic AMG88xx infrared array sensors.
 *
 * The AMG88xx (Grid-EYE) is a family of 8x8 infrared (thermal) array sensors
 * connected via I2C. It measures temperatures in the range of 0 to 80 or -20
 * to 100 degrees Celsius (depends on the variant) with a resolution of 0.25
 * degrees Celsius per pixel. The on-chip thermistor provides the ambient
 * temperature with a resolution of 0.0625 degrees Celsius.
 *
 * The sensor outputs a frame of 8x8 pixels, where each pixel represents the
 * temperature of the corresponding area in the field of view. The sensors
 * support configurable frame rates (1 or 10 FPS), multiple power modes, a
 * moving average filter, and per-pixel interrupt thresholds.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * ## Usage
 *
 * Add one of the following variants to the application Makefile:
 *
 * ```makefile
 * USEMODULE += amg8831
 * USEMODULE += amg8832
 * USEMODULE += amg8833
 * USEMODULE += amg8834
 * USEMODULE += amg8851
 * USEMODULE += amg8852
 * USEMODULE += amg8853
 * USEMODULE += amg8854
 * ```
 *
 * Support for interrupts is available and included if GPIO interrupts are
 * supported. Interrupts can be used to trigger processing when a pixel exceeds
 * a certain temperature threshold. Note that the AMG88xx requires a pull-up
 * resistor on the interrupt line, which may not be present on all
 * off-the-shelve (evaluation) boards.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the AMG88xx infrared array sensor driver
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#include "saul.h"

#include "amg88xx_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Dimensions of a frame
 */
#define AMG88XX_PIXELS_HORIZONTAL   (8)     /**< Number of horizontal pixels */
#define AMG88XX_PIXELS_VERTICAL     (8)     /**< Number of vertical pixels */
#define AMG88XX_PIXELS_COUNT        (64)    /**< Number of pixels per frame */

/**
 * @brief   Size of pixel data
 */
#define AMG88XX_BYTES_PER_PIXEL     (2)     /**< Bytes per pixel */
#define AMG88XX_BYTES_PER_FRAME     (128)   /**< Bytes per frame */

/**
 * @brief   Frame rate options
 */
typedef enum {
    AMG88XX_FPS_10  = 0x00,     /**< 10 frames per second */
    AMG88XX_FPS_1   = 0x01,     /**< 1 frame per second */
} amg88xx_fps_t;

/**
 * @brief   Operating mode options
 */
typedef enum {
    AMG88XX_MODE_NORMAL         = 0x00,     /**< Normal mode */
    AMG88XX_MODE_SLEEP          = 0x10,     /**< Sleep mode */
    AMG88XX_MODE_STANDBY_60S    = 0x20,     /**< Standby with 60s intermittence */
    AMG88XX_MODE_STANDBY_10S    = 0x21,     /**< Standby with 10s intermittence */
} amg88xx_mode_t;

/**
 * @brief   Interrupt mode options
 */
typedef enum {
    AMG88XX_INT_DIFFERENCE  = 0x00,     /**< Difference interrupt mode */
    AMG88XX_INT_ABSOLUTE    = 0x01,     /**< Absolute value interrupt mode */
} amg88xx_interrupt_mode_t;

#if IS_USED(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN)
/**
 * @brief   Interrupt callback type
 */
typedef void (*amg88xx_int_cb_t)(void *);
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C bus the sensor is connected to */
    uint8_t address;            /**< sensor address */
#if IS_USED(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN)
    gpio_t int_pin;             /**< interrupt pin (GPIO_UNDEF if not used) */
#endif
} amg88xx_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    amg88xx_params_t params;    /**< device initialization parameters */
    amg88xx_mode_t mode;        /**< last set operating mode */
} amg88xx_t;

/**
 * @brief   Export of SAUL interface for temperature sensor
 */
extern const saul_driver_t amg88xx_temperature_saul_driver;

/**
 * @brief   Set the frame rate
 *
 * @param[in] dev           Device descriptor
 * @param[in] fps           Frame rate setting
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_set_fps(const amg88xx_t *dev, amg88xx_fps_t fps);

/**
 * @brief   Get the current frame rate
 *
 * @param[in]  dev          Device descriptor
 * @param[out] fps          Current frame rate setting
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_fps(const amg88xx_t *dev, amg88xx_fps_t *fps);

/**
 * @brief   Set the operating mode
 *
 * When transitioning from sleep mode to any other mode, the driver
 * automatically performs the required wake-up sequence (set normal mode,
 * wait 50 ms, initial reset, wait 2 ms, flag reset) as specified by the
 * datasheet.
 *
 * @note    When the sensor is in sleep mode, all register values are
 *          volatile and unreliable.
 *
 * @param[in] dev           Device descriptor
 * @param[in] mode          Operating mode
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_set_mode(amg88xx_t *dev, amg88xx_mode_t mode);

/**
 * @brief   Get the current operating mode
 *
 * @param[in]  dev          Device descriptor
 * @param[out] mode         Current operating mode
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_mode(const amg88xx_t *dev, amg88xx_mode_t *mode);

/**
 * @brief   Enable or disable twice moving average mode
 *
 * When enabled, the sensor applies a twice moving average filter to the
 * pixel output, reducing noise at the cost of slower response.
 *
 * @note    The frame rate must be set to 10 FPS (@ref AMG88XX_FPS_10) before
 *          enabling averaging. Behavior is undefined at 1 FPS.
 *
 * @param[in] dev           Device descriptor
 * @param[in] enabled       @c true to enable, @c false to disable
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_set_averaging(const amg88xx_t *dev, bool enabled);

/**
 * @brief   Get the current averaging mode
 *
 * @param[in]  dev          Device descriptor
 * @param[out] enabled      Current averaging state
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_averaging(const amg88xx_t *dev, bool *enabled);

/**
 * @brief   Read the thermistor (chip) temperature
 *
 * @param[in] dev           Device descriptor
 * @param[out] temperature  Current thermistor temperature in centi-degrees Celsius
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_temperature(const amg88xx_t *dev, int16_t *temperature);

/**
 * @brief   Read a 8x8 frame of thermal pixel data
 *
 * Each pixel value is a signed 12-bit value in units of 0.25 deg C.
 *
 * Use @ref amg88xx_raw_to_temperature to convert a raw pixel value to
 * centi-degrees Celsius.
 *
 * @param[in]  dev          Device descriptor
 * @param[out] pixels       Buffer for @ref AMG88XX_PIXELS_COUNT pixel values
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_frame(const amg88xx_t *dev, int16_t pixels[AMG88XX_PIXELS_COUNT]);

#if IS_USED(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN)
/**
 * @brief   Initialize the interrupt pin
 *
 * The interrupt pin can be used to signal the CPU when the sensor detects
 * a pixel temperature that exceeds the configured thresholds. The callback
 * function is called when the interrupt is triggered, with the provided
 * argument.
 *
 * Use @ref amg88xx_set_interrupt and @ref amg88xx_set_interrupt_levels
 * to enable and configure the interrupt behavior.
 *
 * @pre     The device must be initialized with @ref amg88xx_init first.
 *
 * @param[in] dev           Device descriptor
 * @param[in] cb            Callback function when the interrupt is triggered
 * @param[in] arg           Callback argument
 *
 * @retval                  0 on success
 * @retval                  -ENODEV if no valid interrupt pin is configured
 * @retval                  -EIO on error
 */
int amg88xx_init_int(amg88xx_t *dev, amg88xx_int_cb_t cb, void *arg);
#endif

/**
 * @brief   Enable or disable the interrupt with the given mode
 *
 * @note    Use @ref amg88xx_init_int to initialize the interrupt pin and
 *          callback.
 *
 * @param[in] dev           Device descriptor
 * @param[in] mode          Interrupt mode (absolute or difference)
 * @param[in] enabled       @c true to enable, @c false to disable
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_set_interrupt(const amg88xx_t *dev, amg88xx_interrupt_mode_t mode,
                          bool enabled);

/**
 * @brief   Get the current interrupt configuration
 *
 * @param[in]  dev          Device descriptor
 * @param[out] mode         Current interrupt mode (absolute or difference)
 * @param[out] enabled      Current interrupt enabled state
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_interrupt(const amg88xx_t *dev, amg88xx_interrupt_mode_t *mode,
                          bool *enabled);

/**
 * @brief   Set the interrupt threshold levels
 *
 * All values are in raw sensor units (0.25 deg C per LSB), using 12-bit
 * sign-magnitude format.
 *
 * Use @ref amg88xx_temperature_to_raw to convert a temperature in
 * centi-degrees Celsius to a raw value suitable for the @p upper, @p lower,
 * and @p hysteresis parameters.
 *
 * @note    The interrupt must be enabled with @ref amg88xx_set_interrupt
 *          before the thresholds have any effect.
 *
 * @param[in] dev           Device descriptor
 * @param[in] upper         Upper threshold level
 * @param[in] lower         Lower threshold level
 * @param[in] hysteresis    Hysteresis level
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_set_interrupt_levels(const amg88xx_t *dev, int16_t upper,
                                 int16_t lower, int16_t hysteresis);

/**
 * @brief   Get the current interrupt threshold levels
 *
 * All values are in raw sensor units (0.25 deg C per LSB), using 12-bit
 * sign-magnitude format.
 *
 * Use @ref amg88xx_raw_to_temperature to convert the raw values read into
 * @p upper, @p lower, and @p hysteresis to temperatures in centi-degrees
 * Celsius.
 *
 * @param[in]  dev          Device descriptor
 * @param[out] upper        Upper threshold level
 * @param[out] lower        Lower threshold level
 * @param[out] hysteresis   Hysteresis level
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_interrupt_levels(const amg88xx_t *dev, int16_t *upper,
                                 int16_t *lower, int16_t *hysteresis);

/**
 * @brief   Read the interrupt table
 *
 * The table contains 8 bytes (64 bits), one bit per pixel. A bit value of 1
 * indicates that the corresponding pixel has triggered an interrupt.
 *
 * @param[in]  dev          Device descriptor
 * @param[out] table        Buffer for 8 bytes of interrupt flags
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_interrupt_table(const amg88xx_t *dev, uint8_t table[8]);

/**
 * @brief   Read the status register
 *
 * Returns the raw value of the status register (STAT). The following bits
 * can be checked using the AMG88XX_STAT_* masks:
 *
 *  - @ref AMG88XX_STAT_INTF - interrupt triggered
 *  - @ref AMG88XX_STAT_OVF_THS - thermistor temperature overflow
 *  - @ref AMG88XX_STAT_OVF_IRS - pixel temperature overflow
 *
 * @param[in]  dev          Device descriptor
 * @param[out] status       Raw status register value
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_get_status(const amg88xx_t *dev, uint8_t *status);

/**
 * @brief   Clear all status and interrupt flags
 *
 * @param[in] dev           Device descriptor
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int amg88xx_clear_status(const amg88xx_t *dev);

/**
 * @brief   Initialize the given sensor
 *
 * The function resets the sensor by performing a wake-up routine.
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 * @retval                  -ENODEV on sensor not found
 */
int amg88xx_init(amg88xx_t *dev, const amg88xx_params_t *params);

/**
 * @brief   Convert a raw pixel value to temperature in centi-degrees Celsius
 *
 * Pixel values from @ref amg88xx_get_frame are in units of 0.25 deg C per LSB.
 * This function converts such a value to centi-degrees Celsius (1/100 deg C).
 *
 * For example, a value of 100 corresponds to 25.0 deg C and a value of
 * 40 corresponds to 10.0 deg C.
 *
 * @param[in] raw           Raw pixel value (0.25 deg C per LSB)
 *
 * @return                  Temperature in centi-degrees Celsius
 */
static inline int16_t amg88xx_raw_to_temperature(int16_t raw)
{
    return (int16_t)raw * 25;
}

/**
 * @brief   Convert a temperature in centi-degrees Celsius to a raw pixel value
 *
 * This is the inverse of @ref amg88xx_raw_to_temperature. The result can be
 * used with @ref amg88xx_set_interrupt_levels.
 *
 * @param[in] temperature   Temperature in centi-degrees Celsius
 *
 * @return                  Raw pixel value (0.25 deg C per LSB)
 */
static inline int16_t amg88xx_temperature_to_raw(int16_t temperature)
{
    return (int16_t)(temperature / 25);
}

#ifdef __cplusplus
}
#endif

/** @} */
