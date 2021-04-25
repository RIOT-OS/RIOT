/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp8266
 * @brief       Configurations of the MCU periphery that are common for all
 *              ESP8266 boards
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

/* include board.h and periph_cpu.h to make them visible in any case */
#include "periph_cpu.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BIT
#define BIT(X) (1<<(X))
#endif

/**
 * @name    ADC configuration
 *
 * ESP8266 provides one ADC pin that is broken out on all boards.
 * @{
 */
#define ADC_NUMOF   (1)     /**< number of ADC channels */
/** @} */


/**
 * @name    DAC configuration
 *
 * ESP8266 provides no DAC.
 * @{
 */
#define DAC_NUMOF   (0)     /**< number of DAC channels */
/** @} */


/**
 * @name   I2C configuration
 * @{
 */

/**
 * @brief   Static array with configuration for declared I2C devices
 */
static const i2c_conf_t i2c_config[] = {
    #if defined(I2C0_SCL) && defined(I2C0_SDA) && defined(I2C0_SPEED)
    {
        .speed = I2C0_SPEED,
        .scl = I2C0_SCL,
        .sda = I2C0_SDA,
    },
    #endif
    #if defined(I2C1_SCL) && defined(I2C1_SDA) && defined(I2C1_SPEED)
    {
        .speed = I2C1_SPEED,
        .scl = I2C1_SCL,
        .sda = I2C1_SDA,
    },
    #endif
};

/**
 * @brief Number of I2C interfaces
 *
 * The number of I2C interfaces is determined automatically from board-specific
 * peripheral definitions.
 *
 * @note I2C_NUMOF definition must not be changed.
 */
#define I2C_NUMOF   ARRAY_SIZE(i2c_config)

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
 * @note   The minimum PWM period that can be realized is 10 us or 100.000 PWM
 * clock cycles per second. Therefore, the product of frequency and resolution
 * should not be greater than 100.000. Otherwise the frequency is scaled down
 * automatically.
 *
 * @{
 */

#if defined(PWM0_GPIOS) || defined(DOXYGEN)
/**
 * @brief   Static array of GPIOs that can be used as channels of PWM_DEV(0)
 */
static const gpio_t pwm0_channels[] = PWM0_GPIOS;

/**
 * @brief   Number of PWM devices
 *
 * The number of PWM devices is determined from the #PWM0_GPIOS definition.
 *
 * @note #PWM_NUMOF definition must not be changed.
 */
#define PWM_NUMOF           (1)     /**< Number of PWM devices */

#endif /* defined(PWM0_GPIOS) || defined(DOXYGEN) */
/** @} */

/**
 * @name   SPI configuration
 *
 * ESP8266 has two SPI controllers:
 *
 * - _CSPI_ for caching and accessing the flash memory<br>
 * - _HSPI_ for peripherals
 *
 * Thus, _HSPI_ is the only SPI interface that is available for peripherals.
 * It is exposed as RIOT's SPI_DEV(0). Furthermore, the pin configuration of
 * the _HSPI_ interface is fixed as shown in following table.
 *
 * Signal     | Pin
 * -----------|-------
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

#define SPI0_DEV    SPI_DEV(0)  /**< HSPI / SPI_DEV(0) device */
#define SPI0_CTRL   HSPI        /**< HSPI / SPI_DEV(0) controller */
#define SPI0_MISO   GPIO12      /**< HSPI / SPI_DEV(0) MISO pin */
#define SPI0_MOSI   GPIO13      /**< HSPI / SPI_DEV(0) MOSI pin */
#define SPI0_SCK    GPIO14      /**< HSPI / SPI_DEV(0) SCK pin */

#ifndef SPI0_CS0
#define SPI0_CS0    GPIO15  /**< HSPI / SPI_DEV(0) CS default pin, only used when cs
                                 parameter in spi_acquire is #GPIO_UNDEF */
#endif

/**
 * @brief   Static array with configuration for declared SPI devices
 */
static const spi_conf_t spi_config[] = {
#ifdef SPI0_CTRL
    {
        .ctrl = SPI0_CTRL,
        .sck = SPI0_SCK,
        .mosi = SPI0_MOSI,
        .miso = SPI0_MISO,
        .cs = SPI0_CS0,
    },
#endif
};

/**
 * @brief Number of SPI interfaces
 *
 * The number of SPI interfaces is determined from board-specific peripheral
 * definitions of SPIn_*.
 *
 * @note SPI_NUMOF definition must not be changed.
 */
#define SPI_NUMOF   ARRAY_SIZE(spi_config)

/** @} */

/**
 * @name    Timer configuration
 * @{
 */
#if defined(MODULE_ESP_SW_TIMER)

/* software timer */
#define TIMER_NUMOF         (1U)    /**< number of timer devices */
#define TIMER_CHANNEL_NUMOF (10U)   /**< number of channels per timer device */

#else /* MODULE_ESP_SW_TIMER */

/* hardware timer */
#define TIMER_NUMOF         (1U)    /**< number of timer devices */
#define TIMER_CHANNEL_NUMOF (1U)    /**< number of channels per timer device */

#endif /* MODULE_ESP_SW_TIMER */
/** @} */


/**
 * @name   UART configuration
 *
 * All ESP8266 boards have two UART devices with two options of pin mappings
 * each, however most board will only expose only UART0 and in the GPIO1 and
 * GPIO3 pins, although other combinations are possible. In particular, the
 * boot ROM will map both GPIO1 and GPIO2 as TX for UART0 on boot, so either
 * one can be used for serial communication or flashing the device.
 * While UART1 is also available, the only option for UART1 RX pin (GPIO8) is
 * used for the board flash, but UART1 TX can be used separately
 *
 * Pin mapping available:
 *  UART0 TX:  GPIO1 and GPIO2  (both enabled by the boot ROM)
 *  UART0 RX:  GPIO3
 *  UART1 TX:  GPIO2 and GPIO7  (GPIO7 is used by the flash)
 *  UART0 RX:  GPIO8            (GPIO8 is used by the flash)
 *
 * UART0 device is used for flashing and as a console interface. UART1 if
 * UART1_TXD is defined can be used for communication with other peripherals at
 * a different baudrate if desired.
 *
 * @{
 */
#ifndef UART0_TXD
#define UART0_TXD   GPIO1               /**< TxD pin of UART_DEV(0) */
#endif /* UART0_TXD */

#ifndef UART0_RXD
#define UART0_RXD   GPIO3               /**< RxD pin of UART_DEV(0) */
#endif /* UART0_RXD */

#ifdef DOXYGEN
#define UART1_TXD   GPIO2               /**< TxD pin of UART_DEV(1) */
#endif /* DOXYGEN */

#ifndef UART1_RXD
#define UART1_RXD   GPIO_UNDEF          /**< RxD pin of UART_DEV(1) */
#endif /* UART1_RXD */

/**
 * @brief   Static array with configuration for declared UART devices
 */
static const uart_conf_t uart_config[] = {
    {
        .txd = UART0_TXD,
        .rxd = UART0_RXD,
    },
#ifdef UART1_TXD
    {
        .txd = UART1_TXD,
        .rxd = UART1_RXD,
    },
#endif /* UART1_TXD */
};

/**
 * @brief Number of UART interfaces
 *
 * The number of UART interfaces is determined from board-specific peripheral
 * definitions of UARTn_*.
 *
 * @note UART_NUMOF definition must not be changed.
 */
#define UART_NUMOF  ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
