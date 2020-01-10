/*
 * Copyright (C) 2019 Noel Le Moult <noel.lemoult@dfxlab.fr>
 * Copyright (C) 2019 William MARTIN <william.martin@power-lan.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
  * @ingroup     drivers_actuators
  * @ingroup     drivers_saul
  * @defgroup    drivers_tca6507 TCA6507 I2C Led controller
  * @brief       Public API for TCA6507 I2C Led controller
  * @author      Noel Le Moult <noel.lemoult@dfxlab.fr>
  * @author      William MARTIN <william.martin@power-lan.com>
  * @file
  * @{
  */

#ifndef TCA6507_H
#define TCA6507_H

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Driver return codes
 */
enum {
    TCA6507_OK,                 /**< All OK */
    TCA6507_ERR_NODEV,          /**< No valid device found on I2C bus */
    TCA6507_ERR_I2C             /**< An error occurred when reading/writing on I2C bus */
};


/**
 * @brief Bank define
 *
 * Each bank can be configured to a specific brightness with the function
 * tca6507_intensity and TCA6507_BRIGHTNESS_*_PCENT defines. Then, you can
 * select the bank to use when you use function: tca6507_set_led,
 * tca6507_toggle_led, and tca6507_blink_led.
 */
enum {
    TCA6507_BANK0,
    TCA6507_BANK1,
    TCA6507_BANK_COUNT
};

/**
 * @brief PWM Operating mode
 * Table 19. One-Shot / Master Intensity Register
 * The one-shot mode may be bugged by crappy design of the hardware chip, do not use it
 */
#define TCA6507_BANK0_MODE_NORMAL              (0 << 6)
#define TCA6507_BANK0_MODE_ONESHOT             (1 << 6)
#define TCA6507_BANK0_BRIGHTNESS_MASTER        (1 << 4)
#define TCA6507_BANK0_BRIGHTNESS_BANK          (0 << 4)
#define TCA6507_BANK1_MODE_NORMAL              (0 << 7)
#define TCA6507_BANK1_MODE_ONESHOT             (1 << 7)
#define TCA6507_BANK1_BRIGHTNESS_MASTER        (1 << 5)
#define TCA6507_BANK1_BRIGHTNESS_BANK          (0 << 5)

/**
 * @brief Fadding times
 */
enum {
    TCA6507_TIME_0_MS,
    TCA6507_TIME_64_MS,
    TCA6507_TIME_128_MS,
    TCA6507_TIME_192_MS,
    TCA6507_TIME_256_MS,
    TCA6507_TIME_384_MS,
    TCA6507_TIME_512_MS,
    TCA6507_TIME_768_MS,
    TCA6507_TIME_1024_MS,
    TCA6507_TIME_1536_MS,
    TCA6507_TIME_2048_MS,
    TCA6507_TIME_3072_MS,
    TCA6507_TIME_4096_MS,
    TCA6507_TIME_5760_MS,
    TCA6507_TIME_8128_MS,
    TCA6507_TIME_16320_MS,
    TCA6507_TIME_COUNT
};

/**
 * @brief Brightness
 */
enum {
    TCA6507_BRIGHTNESS_0_PCENT,
    TCA6507_BRIGHTNESS_6_PCENT,
    TCA6507_BRIGHTNESS_13_PCENT,
    TCA6507_BRIGHTNESS_20_PCENT,
    TCA6507_BRIGHTNESS_26_PCENT,
    TCA6507_BRIGHTNESS_33_PCENT,
    TCA6507_BRIGHTNESS_40_PCENT,
    TCA6507_BRIGHTNESS_46_PCENT,
    TCA6507_BRIGHTNESS_53_PCENT,
    TCA6507_BRIGHTNESS_60_PCENT,
    TCA6507_BRIGHTNESS_66_PCENT,
    TCA6507_BRIGHTNESS_73_PCENT,
    TCA6507_BRIGHTNESS_80_PCENT,
    TCA6507_BRIGHTNESS_86_PCENT,
    TCA6507_BRIGHTNESS_93_PCENT,
    TCA6507_BRIGHTNESS_100_PCENT,
    TCA6507_BRIGHTNESS_COUNT

};

/**
 * @brief Device initialization parameters.
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C bus the sensor is connected to */
    uint8_t address;            /**< sensor address */
} tca6507_params_t;

/**
 * @brief TCA6507 device descriptor.
 */
typedef struct {
    tca6507_params_t params;     /**< Device parameters */
} tca6507_t;

/**
 * @brief   Initialize and reset the sensor.
 *
 * @param[in] dev           device descriptor
 * @param[in] params        initialization parameters
 *
 * @return                  TCA6507_OK on successful initialization
 * @return                  TCA6507_ERR_NODEV on device test error
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_init(tca6507_t *dev, const tca6507_params_t *params);

/**
 * @brief   Turn on a led
 *
 * @param[in] dev           device descriptor
 * @param[in] led           The led index
 * @param[in] bank          The bank to use (intensity)
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_set_led(const tca6507_t *dev, uint8_t led, uint8_t bank);

/**
 * @brief   Turn off a led
 *
 * @param[in] dev           device descriptor
 * @param[in] led           The led index
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_clear_led(const tca6507_t *dev, uint8_t led);

/**
 * @brief   Turn off all leds
 *
 * @param[in] dev           device descriptor
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_clear_all(const tca6507_t *dev);

/**
 * @brief   Toggle a led on
 *
 * @param[in] dev           device descriptor
 * @param[in] led           The led index
 * @param[in] bank          The bank to use (intensity)
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_toggle_led(const tca6507_t *dev, uint8_t led, uint8_t bank);

/**
 * @brief   Enable the blink mode for a led
 *
 * @param[in] dev           device descriptor
 * @param[in] led           The led index
 * @param[in] bank          The bank to use (intensity)
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_blink_led(const tca6507_t *dev, uint8_t led, uint8_t bank);

/**
 * @brief   Configure the intensity of a bank
 *
 * @param[in] dev           device descriptor
 * @param[in] intensity     Intensity of the led in percent
 * @param[in] bank          The bank to configure
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_intensity(const tca6507_t *dev, uint8_t intensity, uint8_t bank);

/**
 * @brief   Configure the master intensity
 *
 * @param[in] dev           device descriptor
 * @param[in] intensity     Intensity of the led in percent
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_master_intensity(const tca6507_t *dev, uint8_t brightness);

/**
 * @brief   Configure the fade on time of a bank
 *
 * @param[in] dev           device descriptor
 * @param[in] time          Duration define by TCA6507_TIME_*
 * @param[in] bank          The bank to configure
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_fade_on_time(const tca6507_t *dev, uint8_t time, uint8_t bank);

/**
 * @brief   Configure the fade off time of a bank
 *
 * @param[in] dev           device descriptor
 * @param[in] time          Duration define by TCA6507_TIME_*
 * @param[in] bank          The bank to configure
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_fade_off_time(const tca6507_t *dev, uint8_t time, uint8_t bank);

/**
 * @brief   Configure the fully on time of a bank
 *
 * @param[in] dev           device descriptor
 * @param[in] time          Duration define by TCA6507_TIME_*
 * @param[in] bank          The bank to configure
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_fully_on_time(const tca6507_t *dev, uint8_t time, uint8_t bank);

/**
 * @brief   Configure the first fully off time of a bank
 *
 * @param[in] dev           device descriptor
 * @param[in] time          Duration define by TCA6507_TIME_*
 * @param[in] bank          The bank to configure
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_first_fully_off_time(const tca6507_t *dev, uint8_t time, uint8_t bank);

/**
 * @brief   Configure the second fully off time of a bank
 *
 * @param[in] dev           device descriptor
 * @param[in] time          Duration define by TCA6507_TIME_*
 * @param[in] bank          The bank to configure
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_second_fully_off_time(const tca6507_t *dev, uint8_t time, uint8_t bank);

/**
 * @brief   Advance configuration for banks
 *
 * @param[in] dev           device descriptor
 * @param[in] mask          Bits mask for register 0x09 (One-Shot / Master Intensity)
 *                          See TCA6507_BANK0_* and TCA6507_BANK1_* definitions
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_bank_setup(const tca6507_t *dev, uint8_t mask);

/**
 * @brief   Show content of hardware registers (for debug)
 *
 * @param[in] dev           device descriptor
 *
 * @return                  TCA6507_OK on success
 * @return                  TCA6507_ERR_I2C on I2C bus error
 */
int tca6507_dump_registers(const tca6507_t *dev);


#ifdef __cplusplus
}
#endif

#endif /* TCA6507_H */
/** @} */
