/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_paa5100je PAA5100JE/PMW3901 Driver
 * @ingroup     drivers_sensors
 * @brief       Driver for the PAA5100JE/PMW3901 Optical Flow Sensor
 *
 * ## Description
 *
 * The PAA5100JE and PMW3901 sensors are accessed in the same way over SPI
 * but require slightly different initialization code.
 * Since the datasheets are not very detailed, I created this driver based on a reference
 * implementation by Pimoroni, who designed a breakout board for the PMW3901.
 *
 * The motion data read represents the relative motion since the last readout.
 * It depends on the surface, lighting conditions, and the sensor’s distance from the ground.
 *
 * The readings are scaled according to the configuration parameter `PAA5100JE_SCALE_DENOMINATOR`.
 * The readings are multiplied by 100 and divided by the denominator.
 *
 * The scaling factor, the quality threshold, and the timeout can be configured via Kconfig.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * Datasheets:
 * * [PAA5100JE](https://cdn.shopify.com/s/files/1/0174/1800/files/PAA5100JE-Q-GDS-R1.00_25072018.pdf)
 * * [PMW3901](https://wiki.bitcraze.io/_media/projects:crazyflie2:expansionboards:pot0189-pmw3901mb-txqt-ds-r1.00-200317_20170331160807_public.pdf)
 *
 * Reference Implementation:
 * * [Pomoroni](https://github.com/pimoroni/pmw3901-python)
 *
 * @{
 *
 * @file
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#include <periph/gpio.h>
#include <periph/spi.h>

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Variant of the sensor
 */
typedef enum {
    PAA5100JE = 0,
    PMW3901   = 1,
} paa5100je_variant_t;

/**
 * @brief PAA5100JE LED brightness levels
 */
typedef enum {
    PAA5100JE_LED_OFF    = 0,
    PAA5100JE_LED_MEDIUM = 1,
    PAA5100JE_LED_MAX    = 2,
} paa5100je_led_brightness_t;

/**
 * @brief Device initialization parameters
 */
typedef struct {
    spi_t spi;                /**< SPI bus used */
    spi_clk_t clk;            /**< clock speed used on the selected SPI bus */
    gpio_t cs;                /**< pin connected to the chip select line */
    paa5100je_variant_t var;  /**< variant of the sensor used */
} paa5100je_params_t;

/**
 * @brief Device descriptor for the driver
 */
typedef struct {
    const paa5100je_params_t *params;  /**< Device initialization parameters */
} paa5100je_t;

/**
 * @brief Initialize the given device
 *
 * @param[in,out] dev      Device descriptor of the driver
 * @param[in]     params   Initialization parameters
 *
 * @retval        0        on success
 * @retval        -ENXIO   invalid SPI device
 * @retval        -ENODEV  invalid SPI CS pin/line or wrong device id or revision
 */
int paa5100je_init(paa5100je_t *dev, const paa5100je_params_t *params);

/**
 * @brief Reads the relative motition vector from the device using burst read.
 *
 * Reads twelve bytes from the burst register.
 * Repeats the read when the data is not ready, the quality is too low
 * or the shutter values is too high.
 * The data being read from the sensor represents the relative motion since the last readout.
 * It depends on the surface, lighting conditions, and the sensor’s distance from the ground.
 * The quality threshold, the timeout, and a scaling factor can be configured via Kconfig.
 *
 * @param[in]     dev      device descriptor
 * @param[out]    x        x component in millimeters
 * @param[out]    y        y component in millimeters
 *
 * @retval        0        on success
 * @retval        -ETIME   data was not ready in time, the quality was not good enough,
 *                         or the shutter to high
 */
int paa5100je_get_motion_burst(const paa5100je_t *dev, int16_t *x, int16_t *y);

/**
 * @brief Sets the LED brightness level.
 *
 * @param[in]   dev        device descriptor
 * @param[in]   level      brightness level
 *
 * @retval      0          on success
 * @retval      -EINVAL    invalid brightness level
 */
int paa5100je_set_led_brightness(const paa5100je_t *dev, const paa5100je_led_brightness_t level);

#ifdef __cplusplus
}
#endif

/** @} */
