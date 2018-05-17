/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
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

/* Included for PCA95XX_MAX_INTS */
#include "board.h"

/**
 * @brief  PCA95xx default address (PCA9539 address, A0 = 0, A1 = 0)
 *
 * @note   Many of the supported devices have different addresses.
 */
#ifndef PCA95XX_I2C_ADDRESS
#define PCA95XX_I2C_ADDRESS  (0x74)
#endif

/**
 * @brief   PCA95xx default maximum number of interrupts
 */
#ifndef PCA95XX_MAX_INTS
#define PCA95XX_MAX_INTS  (2)
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
 * @brief   Device flags
 */
typedef enum {
    PCA95XX_16PIN_DEV    /**< device has 16 pins */
} pca95xx_dflags_t;

/**
 * @brief   PCA95xx params
 */
typedef struct pca95xx_params {
    i2c_t i2c;                  /**< i2c device */
    uint8_t addr;               /**< i2c address */
    gpio_t int_pin;             /**< interrupt pin */
    pca95xx_dflags_t dflags;    /**< device flags */
} pca95xx_params_t;

/**
 * @brief   Pin flags
 */
enum {
    PCA95XX_LOW_DRIVE     = 0,    /**< drive pin when set low */
    PCA95XX_HIGH_DRIVE    = 1,    /**< drive pin when set high */
};

/**
 * @brief   Interrupt info
 *
 * @note    The first nibble of dev.int_info[n] contains the pin number
 */
enum {
    PCA95XX_INT_EN      = 4,    /**< Interrupt is enabled */
    PCA95XX_INT_LASTVAL = 5,    /**< Last value of the interrupt pin */
    PCA95XX_INT_FALL    = 6,    /**< Run callback on falling input */
    PCA95XX_INT_RISE    = 7     /**< Run callback on rising input */
};

/**
 * @brief   PCA95xx device descriptor
 */
typedef struct pca95xx {
    pca95xx_params_t params;                 /**< device driver configuration */
    uint32_t pflags;                         /**< pin flags */
    gpio_isr_ctx_t cbs[PCA95XX_MAX_INTS];    /**< array of callbacks */
    uint8_t int_info[PCA95XX_MAX_INTS];      /**< array of interrupt pin info */
} pca95xx_t;

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
 * @brief   Initialize the PCA95xx pin as general purpose input or output
 *
 * @param[in] dev   device descriptor
 * @param[in] pin   pin to initialize
 * @param[in] mode  mode of the pin, see @c gpio_mode_t
 *
 * @return zero on successful initialization, non zero on error
 */
int pca95xx_init_pin(pca95xx_t *dev, uint8_t pin, gpio_mode_t mode);

/**
 * @brief   Initialize a GPIO pin for external interrupt usage
 *
 * The registered callback function will be called in interrupt context every
 * time the defined flank(s) are detected.
 *
 * The interrupt is activated automatically after the initialization.
 *
 * @param[in,out] dev   device descriptor
 * @param[in] pin       pin to initialize
 * @param[in] mode      mode of the pin, see @c gpio_mode_t
 * @param[in] flank     define the active flank(s)
 * @param[in] cb        callback that is called from interrupt context
 * @param[in] arg       optional argument passed to the callback
 *
 * @return zero on successful initialization, non zero on error
 */
int pca95xx_init_int(pca95xx_t *dev, uint8_t pin, gpio_mode_t mode,
                     gpio_flank_t flank, gpio_cb_t cb, void *arg);

/**
 * @brief   Enable pin interrupt if configured as interrupt source
 *
 * @param[in] dev    device descriptor
 * @param[in] pin    the pin to enable the interrupt for
 */
void pca95xx_irq_enable(pca95xx_t *dev, uint8_t pin);

/**
 * @brief   Disable the pin interrupt if configured as interrupt source
 *
 * @param[in] dev    device descriptor
 * @param[in] pin    the pin to disable the interrupt for
 */
void pca95xx_irq_disable(pca95xx_t *dev, uint8_t pin);

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
