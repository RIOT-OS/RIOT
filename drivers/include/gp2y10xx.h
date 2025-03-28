/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GP2Y10XX_H
#define GP2Y10XX_H

/**
 * @defgroup   drivers_gp2y10xx GP2Y10xx Optical Dust Sensor device driver
 * @ingroup    drivers_sensors
 * @ingroup    drivers_saul
 * @brief      GP2Y10xx Optical Dust Sensor Converter device driver
 *
 * This driver works with GP2Y1010AU0F and GP2Y1014AU0F versions.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * # Usage
 *
 * ```make
 * USEMODULE += gp2y10xx
 * ```
 *
 * The device can be initialized with @ref gp2y10xx_init and
 * @ref gp2y10xx_read_density is used to read the current dust density that
 * the sensor can detect.
 *
 * @{
 *
 * @file
 * @brief      GP2Y10xx device driver
 *
 * @author     Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#include <stdint.h>
#include <stdbool.h>

#include "periph/adc.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   This configures the maximum number of ADC readings stored to
 *          calculate the average ADC value.
 *
 * The bigger the number of readings the bigger each device descriptor will be.
 */
#ifndef CONFIG_GP2Y10XX_MAX_READINGS
#define CONFIG_GP2Y10XX_MAX_READINGS (10)
#endif

/**
 * @brief   Driver error values
 */
enum {
    GP2Y10XX_OK         = 0,    /**< Everything is ok */
    GP2Y10XX_ERR_ADC    = -1,   /**< ADC error */
    GP2Y10XX_ERR_ILED   = -2,   /**< ILED pin error */
};

/**
 * @brief   ILED pin level.
 *
 * This specifies how the ILED pin behaves, if it's on when it's
 * active-low/high. Waveshare breakout board is active-high, that is, that the
 * voltage is 3.3v to turn ILED on (logic 1) and 0v to turn it off (logic 0).
 */
typedef enum {
    GP2Y10XX_ILED_LEVEL_HIGH,   /**< Active high */
    GP2Y10XX_ILED_LEVEL_LOW,    /**< Active low */
} gp2y10xx_level_t;

/**
 * @brief   GP2Y10xx device parameters
 */
typedef struct {
    adc_t aout;         /**< ADC line connected to device AOUT pin. */
    adc_res_t adc_res;  /**< ADC line resolution. */
    uint16_t vref;      /**< Reference voltage used for the VCC supply of the
                             sensor, in mV. */
    gpio_t iled_pin;    /**< ILED pin */
    gp2y10xx_level_t iled_level; /**< ILED pin level */
} gp2y10xx_params_t;

/**
 * @brief   GP2Y10xx device descriptor
 */
typedef struct {
    gp2y10xx_params_t params;   /**< device driver configuration */
} gp2y10xx_t;

/**
 * @brief   Initialize an GP2Y10xx device.
 *
 * @param[in,out]   dev     Device descriptor.
 * @param[in]       params  Device configuration.
 *
 * @return GP2Y10XX_OK on successful initialization.
 * @return GP2Y10XX_ERR_ADC if ADC line initialization failed.
 * @return GP2Y10XX_ERR_ILED if the ILED pin initialization failed.
 */
int gp2y10xx_init(gp2y10xx_t *dev, const gp2y10xx_params_t *params);

/**
 * @brief   Read a raw ADC value
 *
 * @param[in]       dev     Device descriptor.
 * @param[out]      density Dust density value in ug/m3.
 *
 * @return GP2Y10XX_OK on successful read.
 * @return Any other value on error.
 */
int gp2y10xx_read_density(const gp2y10xx_t *dev, uint16_t *density);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GP2Y10XX_H */
