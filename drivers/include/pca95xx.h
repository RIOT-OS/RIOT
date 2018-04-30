/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup   drivers_pca95xx PCA95xx I2C GPIO expanders
 * @ingroup    drivers_actuators
 * @brief      PCA95xx I2C GPIO expanders
 *
 *             This driver works with many PCA95xx and TCA95xx components,
 *             including PCA9534, PCA9535, PCA9536, PCA9538, PCA9539, PCA9554,
 *             PCA9555, PCA9957, and variants such as PCA6408 and  PCA6416, as
 *             well as TCA versions of the components named. There may be other
 *             components that work with this driver that are not listed.
 * @{
 *
 * @file
 * @brief      PCA95xx I2C GPIO expander device driver
 *
 * GPIO and interrupt functionality are separated into two devices to
 * prevent wasteful representations.
 *
 * @author     Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef PCA95XX_H
#define PCA95XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"

/**
 * @brief  PCA95xx default address (PCA9539 address, A0 = 0, A1 = 0)
 *
 * Note that many of the supported devices have different addresses.
 */
#ifndef PCA95XX_I2C_ADDRESS
#define PCA95XX_I2C_ADDRESS  (0x74)
#endif

/**
 * @brief   Named return values
 */
enum {
    PCA95XX_OK          =  0,       /**< everything was fine */
    PCA95XX_NOI2C       = -1,       /**< I2C communication failed */
    PCA95XX_NODEV       = -2,       /**< no PCA95xx device found on the bus */
    PCA95XX_NODATA      = -3        /**< no data available */
};

/**
 * @brief   Pin flags
 */
typedef enum {
    PCA95XX_INIT_SET     = (1 << 0),    /**< initialize pin high, otherwise low */
    PCA95XX_16PIN_DEV    = (1 << 1),    /**< device has 16 pins */
    PCA95XX_HIGH_DRIVE   = (1 << 2),    /**< drive pin when set high */
    PCA95XX_LOW_DRIVE    = (1 << 3),    /**< drive pin when set low */
    PCA95XX_INPUT_INVERT = (1 << 4)     /**< invert input */
} pca95xx_flags_t;

/**
 * @brief   PCA95xx params
 */
typedef struct pca95xx_params {
    i2c_t i2c;                /**< i2c device */
    uint8_t addr;             /**< i2c address */
    uint8_t pin;              /**< pin number */
    pca95xx_flags_t flags;    /**< flags of the specified pin */
} pca95xx_params_t;

/**
 * @brief   PCA95xx device descriptor
 */
typedef struct pca95xx {
    pca95xx_params_t params;    /**< device driver configuration */
} pca95xx_t;

/**
 * @brief   PCA95xx interrupt params
 */
typedef struct pca95xx_int_params {
    i2c_t i2c;         /**< i2c device */
    uint8_t addr;      /**< i2c address */
    gpio_t int_pin;    /**< interrupt pin (GPIO_UNDEF if not connected) */
} pca95xx_int_params_t;

/**
 * @brief   PCA95xx interrupt callback
 */
typedef void (*pca95xx_int_cb_t)(void *);

/**
 * @brief   PCA95xx interrupt device descriptor
 */
typedef struct pca95xx_int {
    pca95xx_int_params_t params;    /**< device driver configuration */
    pca95xx_int_cb_t cb;            /**< alert callback */
    void *arg;                      /**< alert callback param */
} pc95xx_int_t;

/**
 * @brief   Initialize a PCA95xx GPIO device
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return zero on successful initialization, non zero on error
 */
int pca95xx_init(pca95xx_t *dev, const pca95xx_params_t *params);

/**
 * @brief   Initialize a PCA95xx interrupt device
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return zero on successful initialization, non zero on error
 */
int pca95xx_int_init(pca95xx_int_t *dev, const pca95xx_int_params_t *params);

/**
 * @brief   Enable interrupt
 *
 * @param[in] dev   device descriptor
 * @param[in] cb    callback called when the alert fires
 * @param[in] arg   callback argument
 *
 * @return zero on success, non zero on error
 */
int pca95xx_enable_int(pca95xx_int_t *dev, pca95xx_int_cb_t cb, void *arg);

/**
 * @brief   Get the current value of the device's pin
 *
 * @param[in] dev       device descriptor
 * @param[in] pin       pin number
 *
 * @return              0 when pin is LOW
 * @return              >0 for HIGH
 */
int pca95xx_read(const pca95xx_t *dev, uint8_t pin);

/**
 * @brief   Set the device's pin to HIGH
 *
 * @param[in] dev       device descriptor
 * @param[in] pin       pin number
 */
void pca95xx_set(const pca95xx_t *dev, uint8_t pin);

/**
 * @brief   Set the device's pin to LOW
 *
 * @param[in] dev       device descriptor
 * @param[in] pin       pin number
 */
void pca95xx_clear(const pca95xx_t *dev, uint8_t pin);

/**
 * @brief   Toggle device's pin
 *
 * @param[in] dev       device descriptor
 * @param[in] pin       pin number
 */
void pca95xx_toggle(const pca95xx_t *dev, uint8_t pin);

/**
 * @brief   Set the device's pin to the given value
 *
 * @param[in] dev       device descriptor
 * @param[in] pin       pin number
 * @param[in] value     value to set the pin to, 0 for LOW, HIGH otherwise
 */
void pca95xx_write(const pca95xx_t *dev, uint8_t pin, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* PCA95XX_H */
/** @} */
