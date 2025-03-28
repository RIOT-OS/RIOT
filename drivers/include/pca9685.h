/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef PCA9685_H
#define PCA9685_H

/**
 * @defgroup    drivers_pca9685 PCA9685 I2C PWM controller
 * @ingroup     drivers_actuators
 * @ingroup     drivers_saul
 * @brief       Device driver for the NXP PCA9685
 *
 * ## Overview
 *
 * The driver supports the NXP PCA9685 16-channel, 12-bit PWM LED controller
 * connected to I2C. Although the controller is optimized for LED control,
 * the 12-bit resolution also allows the control of servos with a resolution
 * of 4 us at 60 Hz refresh signal.
 *
 * The following features of the PCA9685 are supported by the driver:
 *
 * - 16 channels with 12-bit resolution
 * - Refresh rates from 24 Hz to 1526 Hz with internal 25 MHz oscillator
 * - Totem pole outputs with 25 mA as sink and 10 mA as source at 5V
 * - Software programmable open-drain output selection
 * - Inverted outputs
 * - Active LOW Output Enable (OE) input pin
 * - External clock input with max. 50 MHz
 *
 * ## Usage
 *
 * The driver interface is kept as compatible as possible with the peripheral
 * PWM interface. The only differences are that
 *
 * - functions have the prefix `pca9685_` and
 * - functions require an additional parameter, the pointer to the PWM
 *   device of type #pca9685_t.
 *
 * Please refer the test application in `tests/drivers/pca9685` for an example
 * on how to use the driver.
 *
 * ## SAUL Capabilities
 *
 * The driver provides SAUL capabilities that are compatible to the SAUL
 * actuators of type #SAUL_ACT_SERVO.
 *
 * Each PCA9685 channel can be mapped directly to SAUL by defining an
 * according entry in \c PCA9685_SAUL_PWM_PARAMS. Please refer file
 * `$RIOTBASE/drivers/pca9685/include/pca9685_params.h` for an example.
 *
 *      pca9685_saul_pwm_params_t pca9685_saul_pwm_params[] = {
 *          {
 *              .name = "PCA9685-0:0",
 *              .dev = 0,
 *              .channel = 0,
 *              .initial = (PCA9685_PARAM_RES >> 1),
 *           },
 *          {
 *              .name = "PCA9685-0:1",
 *              .dev = 0,
 *              .channel = 1,
 *              .initial = (PCA9685_PARAM_RES >> 2),
 *           },
 *          {
 *              .name = "PCA9685-0:2",
 *              .dev = 0,
 *              .channel = 2,
 *              .initial = (PCA9685_PARAM_RES >> 3),
 *           },
 *      };
 *
 * For each PWM channel that should be used with SAUL, an entry with a name,
 * the device, the channel, and the initial value has to be defined as shown
 * above.
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

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/pwm.h"

/**
 * @name    PCA9685 I2C slave addresses
 *
 * PCA9685 offers 64 possible hardware-programmable I2C slave addresses.
 * Therefore the I2C slave address is defined as an offset in the range
 * from 0 to 63 to a base address #PCA9685_I2C_BASE_ADDR. PCA9685 I2C
 * slave addresses are then in the range from #PCA9685_I2C_BASE_ADDR + 0 to
 * #PCA9685_I2C_BASE_ADDR + 63
 *
 * Four I2C slave addresses have special meaning when they are enabled, the
 * All Call address (enabled by default) and three Sub Call addresses
 * disabled by default). These addresses can be used to address either all or
 * sub groups of PCF9695 controllers at the same time.
 *
 * @{
 */
#define PCA9685_I2C_BASE_ADDR   (0x40)  /**< I2C slave base address */
#define PCA9685_I2C_ALLCALLADDR (0x70)  /**< Default All Call address */
#define PCA9685_I2C_SUBADR1     (0x71)  /**< Default Sub Call address 1 */
#define PCA9685_I2C_SUBADR2     (0x72)  /**< Default Sub Call address 2 */
#define PCA9685_I2C_SUBADR3     (0x73)  /**< Default Sub Call address 3 */
/** @} */

/**
 * @brief   Number of PWM channels provided by PCA9685
 */
#define PCA9685_CHANNEL_NUM     (16U)

/**
 * @brief   Internal PCA9685 channel resolution is 12-bit
 */
#define PCA9685_RESOLUTION      (1 << 12)

/**
 * @brief   Internal PCA9685 oscillator frequency is 25 MHz
 */
#define PCA9685_OSC_FREQ        (25000000)

/**
 * @brief   Maximum external clock frequency is 50 MHz
 */
#define PCA9685_EXT_FERQ_MAX    (50000000)

/**
 * @brief   PCA9685 driver error codes
 */
typedef enum {
    PCA9685_OK              = 0,    /**< Success */
    PCA9685_ERROR_I2C       = 1,    /**< I2C communication error */
} pca9685_error_t;

/**
 * @brief   PCA9685 output driver mode
 *
 * The output driver mode defines how the outputs are configured.
 */
typedef enum {
    PCA9685_OPEN_DRAIN = 0,     /**< Open-drain structure output */
    PCA9685_TOTEM_POLE = 1,     /**< Totem pole structure output */
} pca9685_out_drv_t;

/**
 * @brief   PCA9685 output-not-enabled mode
 *
 * The output-not-enabled mode defines how the outputs behave when the
 * active LOW output enable pin /OE is HIGH.
 */
typedef enum {
    PCA9685_OFF = 0,      /**< If /OE pin is HIGH, outputs are LOW */
    PCA9685_OUT_DRV = 1,  /**< Outputs depend on the output driver mode
                               pca9685_params_t::out_drv. If /OE pin is HIGH,
                               it is high-impedance for PCA9685_OPEN_DRAIN,
                               and HIGH for PCA9685_TOTEM_POLE */
    PCA9685_HIHGH_Z = 2,  /**< If /OE pin is HIGH, outputs are high-impedance */
} pca9685_out_ne_t;

/**
 * @brief   PCA9685 device initialization parameters
 */
typedef struct {

    i2c_t i2c_dev;     /**< I2C device, default I2C_DEV(0) */
    uint8_t i2c_addr;  /**< I2C slave address */

    pwm_mode_t mode;    /**< PWM mode for all channels: #PWM_LEFT, #PWM_CENTER,
                             #PWM_RIGHT supported, (default PWM_CENTER) */
    uint32_t freq;      /**< PWM frequency in Hz (default 100) */
    uint16_t res;       /**< PWM resolution (default 4096) */

    bool inv;           /**< Invert outputs, e.g., for LEDs (default yes) */

    uint32_t ext_freq;  /**< If not 0, EXTCLK pin is used with this frequency */
    gpio_t oe_pin;      /**< Active LOW output enable pin /OE. If #GPIO_UNDEF,
                             the pin is not used. (default #GPIO_UNDEF). */
    pca9685_out_drv_t out_drv; /**< Output driver mode */
    pca9685_out_ne_t  out_ne;  /**< Output-not-enabled mode */

} pca9685_params_t;

/**
 * @brief   PCA9685 PWM device data structure type
 */
typedef struct {

    pca9685_params_t params;     /**< Device initialization parameters */
    bool powered_on;             /**< Devices is powered on if true */

} pca9685_t;

#if MODULE_SAUL || DOXYGEN
/**
 * @brief   PCA9685 configuration structure for mapping PWM channels to SAUL
 */
typedef struct {
    const char *name;   /**< name of the PCA9685 device */
    unsigned int dev;   /**< index of the PCA9685 device */
    uint8_t channel;    /**< channel of the PCA9685 device */
    uint16_t initial;   /**< initial duty-cycle value*/
} pca9685_saul_pwm_params_t;
#endif

/**
 * @brief   Initialize the PCA9685 PWM device driver
 *
 * The function initializes the driver. After calling this function, the PWM
 * device is in low-power sleep mode (powered off), all outputs off. Before
 * the PWM device can be used, it has to be initialized with #pca9685_pwm_init.
 *
 * @param[in]   dev     Device descriptor of the PCA9685 to be initialized
 * @param[in]   params  Configuration parameters, see #pca9685_params_t
 *
 * @retval  PCA9685_OK      on success
 * @retval  PCA9685_ERROR_* a negative error code on error, see
 *                          #pca9685_error_t
 */
int pca9685_init(pca9685_t *dev, const pca9685_params_t *params);

/**
 * @brief   Initialize the PCA9685 PWM device
 *
 * The function initializes the PWM device with the given parameters that are
 * used for all channels. After calling this function, the PWM device is
 * operational (powered on). That is, all outputs are active with the given
 * parameters and the same duty cycle value as before the call.
 *
 * @note
 * - PCA9685 works with internally with a resolution of 12 bit = 4096.
 *   Using a resolution that is not a power of two, will cause inaccuracy due
 *   to alignment errors when scaling down the internal resolution to the
 *   configured resolution.
 * - Frequencies from 24 Hz to 1526 Hz can be used with PCF9865.
 *
 * @param[in]   dev     Device descriptor of the PCA9685
 * @param[in]   mode    PWM mode, left, right or center aligned
 * @param[in]   freq    PWM frequency in Hz [24...1526]
 * @param[in]   res     PWM resolution [2...4096], should be a power of two
 *
 * @retval  >0   actual frequency on success
 * @retval  0    on error
 */
uint32_t pca9685_pwm_init(pca9685_t *dev, pwm_mode_t mode, uint32_t freq,
                                                           uint16_t res);

/**
 * @brief   Set the duty-cycle for a given channel or all channels of the
 *          given PCA9685 PWM device
 *
 * The duty-cycle is set in relation to the chosen resolution of the given
 * device. If value > resolution, value is set to resolution.
 *
 * If the given channel is #PCA9685_CHANNEL_NUM, all channels are set to the
 * same duty cycle at the same time with only one I2C bus access.
 *
 * @param[in] dev       Device descriptor of the PCA9685
 * @param[in] channel   Channel of PCA9685 to set, if #PCA9685_CHANNEL_NUM
 *                      all channels are set to the desired duty-cycle
 * @param[in] value     Desired duty-cycle to set
 */
void pca9685_pwm_set(pca9685_t *dev, uint8_t channel, uint16_t value);

/**
 * @brief   Resume PWM generation on the given PCA9685 device
 *
 * When this function is called, the given PWM device is powered on and
 * continues its previously configured operation. The duty cycle of each channel
 * will be the value that was last set.
 *
 * This function must not be called before the PWM device was initialized.
 *
 * @param[in] dev       Device descriptor of the PCA9685
 */
void pca9685_pwm_poweron(pca9685_t *dev);

/**
 * @brief   Stop the PWM generation on the given PCA9685 device
 *
 * This function switches the PCA9685 into sleep mode which turns off the
 * internal oscillator. This disables the PWM generation on all configured.
 * If the active LOW output enable pin /OE is used, the signal is set to HIGH.
 * Dependent on the pca9685_params_t::out_drv and pca9685_params_t::out_ne
 * parameters, the outputs are set 0, 1 or high-impedance. All channel
 * duty-cycle values are preserved.
 *
 * @param[in]   dev     Device descriptor of the PCA9685
 */
void pca9685_pwm_poweroff(pca9685_t *dev);

/**
 * @brief   Get the number of available channels of the given PCA9685 device
 * @param[in]  dev      Device descriptor of the PCA9685
 * @return     Number of channels available
 */
static inline uint8_t pca9685_pwm_channels(pca9685_t *dev)
{
    (void)dev;
    return PCA9685_CHANNEL_NUM;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PCA9685_H */
