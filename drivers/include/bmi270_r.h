#ifndef BMI270_R_H
#define BMI270_R_H

#include <stdint.h>
#include <stdbool.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#include "bmi2.h"

/* ----------------------------------------------------------------------------
 * Public types
 * --------------------------------------------------------------------------*/

/**
 * @brief BMI270 device descriptor (RIOT wrapper).
 *
 * has RIOT I2C information and the Bosch BMI2 device context
 */
typedef struct {
    struct bmi2_dev sensor;

    // RIOT I2C context
    i2c_t  i2c;
    uint8_t addr;

    // optional interrupt pins (GPIO_UNDEF if unused)
    gpio_t intpin1;
    gpio_t intpin2;
} bmi270_t;

/**
 * @brief Container for BMI270 sensor output data.
 */
typedef struct {
    struct bmi2_sens_data sensor_data;
} bmi270_data_t;

/* ----------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------*/

/**
 * @brief Initialize BMI270 over I2C.
 *
 * sets up the Bosch BMI2 driver callbacks and loads the BMI270 config
 *
 * @param[in,out] dev   Device descriptor (allocated by caller)
 * @param[in]     i2c   RIOT I2C bus
 * @param[in]     addr  7-bit I2C address (e.g. 0x68 or 0x69)
 *
 * @return 0 on success, negative error code on failure
 */
int bmi270_init_i2c(bmi270_t *dev, i2c_t i2c, uint8_t addr);

/**
 * @brief Apply default accelerometer + gyroscope configuration and enable sensors.
 *
 * @param[in,out] dev Device descriptor
 *
 * @return 0 on success, negative error code on failure
 */
int bmi270_config_default(bmi270_t *dev);

/**
 * @brief Read accelerometer and gyroscope data
 *
 * @param[in,out] dev   Device descriptor
 * @param[out]    data  Output data container
 *
 * @return 0 on success, negative error code on failure
 */
int bmi270_read(bmi270_t *dev, bmi270_data_t *data);

/**
 * @brief Run accelerometer and gyroscope self-tests
 *
 * @param[in,out] dev Device descriptor
 *
 * @return 0 on success, negative error code on failure
 */
int bmi270_selftest(bmi270_t *dev);

/**
 * @brief Configure BMI270 interrupt pin behavior.
 *
 * This only configures the BMI270 internal interrupt routing and pin settings.
 * GPIO interrupt setup is left to board / CPU code.
 *
 * @param[in,out] dev         Device descriptor
 * @param[in]     intsettings BMI270 interrupt pin configuration
 * @param[in]     pin         GPIO pin connected to INT1 or INT2
 *
 * @return 0 on success, negative error code on failure
 */
int bmi270_config_int(bmi270_t *dev,
                      struct bmi2_int_pin_config *intsettings,
                      gpio_t pin);

/**
 * @brief Release interrupt pin bookkeeping
 * @param[in,out] dev Device descriptor
 */
void bmi270_free_pin(bmi270_t *dev);

#endif /* BMI270_R_H */
