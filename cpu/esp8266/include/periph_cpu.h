/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       CPU specific definitions and functions for peripheral handling
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#include "eagle_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (4U)

/**
 * @name   GPIO configuration
 * @{
 */

/**
 * @brief   Override the default gpio_t type definition
 *
 * This is required here to have gpio_t defined in this file.
 * @{
 */
#define HAVE_GPIO_T
typedef unsigned int gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF ((gpio_t)(UINT_MAX))

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      ((x & 0) | y)

/**
 * @brief   Available GPIO ports on ESP8266
 */
#define PORT_GPIO           (0)

/**
 * @brief   Define CPU specific number of GPIO pins
 */
#define GPIO_PIN_NUMOF  (17)

#ifndef DOXYGEN
/**
 * @brief   Override flank selection values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_NONE    = 0,
    GPIO_RISING  = 1,        /**< emit interrupt on rising flank  */
    GPIO_FALLING = 2,        /**< emit interrupt on falling flank */
    GPIO_BOTH    = 3,        /**< emit interrupt on both flanks   */
    GPIO_LOW     = 4,        /**< emit interrupt on low level     */
    GPIO_HIGH    = 5         /**< emit interrupt on low level     */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */
/** @} */

/**
 * @name   Predefined GPIO names
 * @{
 */
#define GPIO0       (GPIO_PIN(PORT_GPIO,0))
#define GPIO1       (GPIO_PIN(PORT_GPIO,1))
#define GPIO2       (GPIO_PIN(PORT_GPIO,2))
#define GPIO3       (GPIO_PIN(PORT_GPIO,3))
#define GPIO4       (GPIO_PIN(PORT_GPIO,4))
#define GPIO5       (GPIO_PIN(PORT_GPIO,5))
#define GPIO6       (GPIO_PIN(PORT_GPIO,6))
#define GPIO7       (GPIO_PIN(PORT_GPIO,7))
#define GPIO8       (GPIO_PIN(PORT_GPIO,8))
#define GPIO9       (GPIO_PIN(PORT_GPIO,9))
#define GPIO10      (GPIO_PIN(PORT_GPIO,10))
#define GPIO11      (GPIO_PIN(PORT_GPIO,11))
#define GPIO12      (GPIO_PIN(PORT_GPIO,12))
#define GPIO13      (GPIO_PIN(PORT_GPIO,13))
#define GPIO14      (GPIO_PIN(PORT_GPIO,14))
#define GPIO15      (GPIO_PIN(PORT_GPIO,15))
#define GPIO16      (GPIO_PIN(PORT_GPIO,16))
/** @} */

/**
 * @name   I2C configuration
 *
 * ESP8266 provides up to two bit-banging I2C interfaces.
 *
 * The board-specific configuration of the I2C interface I2C_DEV(n) requires
 * the definition of
 *
 * I2Cn_SPEED, the bus speed,
 * I2Cn_SCL, the GPIO used as SCL signal, and
 * I2Cn_SDA, the GPIO used as SDA signal,
 *
 * where n can be 0 or 1. If they are not defined, the I2C interface
 * I2C_DEV(n) is not used.
 *
 * @note The configuration of the I2C interfaces I2C_DEV(n) must be in
 * continuous ascending order of n.
 *
 * I2C_NUMOF is determined automatically from board-specific peripheral
 * definitions of I2Cn_SPEED, I2Cn_SCK, and I2Cn_SDA.
 *
 * @{
 */
#ifndef DOXYGEN
/**
 * @brief    Override I2C clock speed values
 *
 * This is required here to have i2c_speed_t defined in this file.
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW = 0,      /**< 10 kbit/s */
    I2C_SPEED_NORMAL,       /**< 100 kbit/s */
    I2C_SPEED_FAST,         /**< 400 kbit/s */
    I2C_SPEED_FAST_PLUS,    /**< 1 Mbit/s */
    I2C_SPEED_HIGH,         /**< not supported */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */
/**
 * @brief   I2C configuration structure type
 */
typedef struct {
    i2c_speed_t speed;      /**< I2C bus speed */
    gpio_t scl;             /**< GPIO used as SCL pin */
    gpio_t sda;             /**< GPIO used as SDA pin */
} i2c_conf_t;

/**
 * @brief   Maximum number of I2C interfaces that can be used by board definitions
 */
#define I2C_NUMOF_MAX   (2)

#define PERIPH_I2C_NEED_READ_REG    /**< i2c_read_reg required */
#define PERIPH_I2C_NEED_READ_REGS   /**< i2c_read_regs required */
#define PERIPH_I2C_NEED_WRITE_REG   /**< i2c_write_reg required */
#define PERIPH_I2C_NEED_WRITE_REGS  /**< i2c_write_regs required */
/** @} */

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
#define PROVIDES_PM_RESTART
#define PROVIDES_PM_OFF
/** @} */

/**
 * @name    PWM configuration
 *
 * The hardware implementation of ESP8266 PWM supports only frequencies as
 * power of two. Therefore a software implementation of one PWM device
 * PWM_DEV(0) with up to 8 PWM channels (#PWM_CHANNEL_NUM_MAX) is used. The
 * GPIOs that can be used as PWM channels are defined by #PWM0_GPIOS in board
 * definition.
 *
 * @note The minimum PWM period that can be realized is 10 us or 100.000 PWM
 * clock cycles per second. Therefore, the product of frequency and resolution
 * should not be greater than 100.000. Otherwise the frequency is scaled down
 * automatically.
 *
 * @{
 */

/**
 * @brief   Maximum number of PWM devices.
 */
#define PWM_NUMOF_MAX       (1)

/**
 * @brief   Maximum number of channels per PWM device.
 */
#define PWM_CHANNEL_NUM_MAX (8)

/** @} */

/**
 * @name    SPI configuration
 *
 * ESP8266 has two SPI controllers:
 *
 * - _CSPI_ for caching and accessing the flash memory<br>
 * - _HSPI_ for peripherals
 *
 * Thus, _HSPI_ is the only SPI interface that is available for peripherals.
 * It is exposed as RIOT's SPI_DEV(0). The pin configuration of the _HSPI_
 * interface is fixed as shown in following table.
 *
 * Signal     | Pin
 * -- --------|-------
 * #SPI0_MISO | GPIO12
 * #SPI0_MOSI | GPIO13
 * #SPI0_SCK  | GPIO14
 * #SPI0_CS0  | GPIOn with n = 0, 2, 4, 5, 15, 16 (additionally 9, 10 in DOUT flash mode)
 *
 * The only pin definition that can be overridden by an application-specific
 * board configuration is the CS signal defined by #SPI0_CS0.
 *
 * @{
 */

/**
 * @brief   SPI controllers that can be used for peripheral interfaces
 */
typedef enum {
    HSPI = 1,         /**< HSPI interface controller */
} spi_ctrl_t;

/**
 * @brief   SPI configuration structure type
 */
typedef struct {
    spi_ctrl_t ctrl;        /**< SPI controller used for the interface */
    gpio_t sck;             /**< GPIO used as SCK pin */
    gpio_t mosi;            /**< GPIO used as MOSI pin */
    gpio_t miso;            /**< GPIO used as MISO pin */
    gpio_t cs;              /**< GPIO used as CS0 pin */
} spi_conf_t;

/**
 * @brief   Maximum number of SPI interfaces
 */
#define SPI_NUMOF_MAX   (1)

#define PERIPH_SPI_NEEDS_TRANSFER_BYTE  /**< requires function spi_transfer_byte */
#define PERIPH_SPI_NEEDS_TRANSFER_REG   /**< requires function spi_transfer_reg */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS  /**< requires function spi_transfer_regs */
/** @} */

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @name    UART configuration
 *
 * All ESP8266 boards have exactly one UART device with fixed pin mapping.
 *
 * @{
 */

/**
 * @brief   UART configuration structure type
 */
typedef struct {
    gpio_t txd;             /**< GPIO used as TxD pin */
    gpio_t rxd;             /**< GPIO used as RxD pin */
} uart_conf_t;

/**
 * @brief   Maximum number of UART interfaces
 */
#define UART_NUMOF_MAX  (1)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
