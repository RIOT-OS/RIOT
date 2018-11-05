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
#include "board.h"
#include "periph_cpu.h"

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
#define ADC_NUMOF   1   /**< number of ADC channels */
/** @} */


/**
 * @name    PWM configuration
 *
 * The hardware implementation of ESP8266 PWM supports only frequencies as power of
 * two. Therefore a software implementation of one PWM device PWM_DEV(0) with up to
 * 8 PWM channels (PWM_CHANNEL_NUM_MAX) is used.
 *
 * @note   The minumum PWM period that can be realized is 10 us or 100.000 PWM
 * clock cycles per second. Therefore, the product of frequency and resolution
 * should not be greater than 100.000. Otherwise the frequency is scaled down
 * automatically.
 *
 * @{
 */

#define PWM_NUMOF           (1)     /**< Number of PWM devices */

/**
 * @brief   Maximum number of channels per PWM device.
 */
#define PWM_CHANNEL_NUM_MAX (8)

/**
 * @brief   Definition of GPIOs that can be used as PWM channels
 *          of device PWM_DEV(0).
 *
 * The following definition is just an example configuration. Declare up to
 * \ref PWM_CHANNEL_NUM_MAX GPIOs as PWM channels. GPIOs with a duty cycle
 * value of 0 can be used as normal GPIOs for other purposes. GPIOs in the
 * list that are used for other purposes, e.g., I2C or SPI, are then not
 * available as PWM channels.
 */
#ifndef PWM0_CHANNEL_GPIOS
#define PWM0_CHANNEL_GPIOS { GPIO2, GPIO4, GPIO5 }
#endif

/** Alternative device definition */
#define PWM0_DEV    PWM_DEV(0)
/** @} */


/**
 * @name    SPI configuration
 *
 * ESP8266 provides two hardware SPI interfaces:
 *
 * _FSPI_ for flash memory and usually simply referred to as _SPI_<br>
 * _HSPI_ for peripherals
 *
 * Even though _FSPI_ (or simply _SPI_) is a normal SPI interface, it is not
 * possible to use it for peripherals. _HSPI_ is therefore the only usable
 * SPI interface available for peripherals as RIOT's SPI_DEV(0).
 *
 * The pin configuration of the _HSPI_ interface SPI_DEV(0) is fixed. The
 * only pin definition that can be overridden by an application-specific
 * board configuration is the CS signal defined by SPI0_CS0_GPIO.
 *
 * Signal          | Pin
 * ----------------|-------
 * SPI_DEV(0).MISO | GPIO12
 * SPI_DEV(0).MOSI | GPIO13
 * SPI_DEV(0).SCK  | GPIO14
 * SPI_DEV(0).CS   | GPIOn with n = 0, 2, 4, 5, 15, 16 (additionally 9, 10 in DOUT flash mode)
 * @{
 */
#if defined(MODULE_PERIPH_SPI) || defined(DOXYGEN)

#define SPI_NUMOF   1                       /**< Number of SPI interfaces */
#define SPI_DEV(x)  ((unsigned int)(x+1))   /**< SPI_DEV to SPI hardware mapping */

#define SPI0_DEV         SPI_DEV(0) /**< HSPI / SPI_DEV(0) device */
#define SPI0_MISO_GPIO   GPIO12     /**< HSPI / SPI_DEV(0) MISO pin */
#define SPI0_MOSI_GPIO   GPIO13     /**< HSPI / SPI_DEV(0) MOSI pin */
#define SPI0_SCK_GPIO    GPIO14     /**< HSPI / SPI_DEV(0) SCK pin */

#ifndef SPI0_CS0_GPIO
#define SPI0_CS0_GPIO    GPIO15  /**< HSPI / SPI_DEV(0) CS default pin, only used when cs
                                      parameter in spi_acquire is GPIO_UNDEF */
#endif
#endif /* defined(MODULE_PERIPH_SPI) || defined(DOXYGEN) */
/** @} */

/**
 * @name    Timer configuration
 * @{
 */
#if defined(MODULE_ESP_SW_TIMER)

/* software timer */
#define TIMER_NUMOF         (1U)    /**< number of timer devices */
#define TIMER_CHANNELS      (10U)   /**< number of channels per timer device */

#else /* MODULE_ESP_SW_TIMER */

/* hardware timer */
#define TIMER_NUMOF         (1U)    /**< number of timer devices */
#define TIMER_CHANNELS      (1U)    /**< number of channels per timer device */

#endif /* MODULE_ESP_SW_TIMER */
/** @} */


/**
 * @name   UART configuration
 *
 * All ESP8266 boards have exactly one UART device with fixed pin mapping.
 * This UART device is used for flashing and as a console interface.
 * Therefore, the number of UART devices is fixed and can not be overridden.
 * Used pins are determined by the MCU implementation and are defined here
 * only for documentation reasons.
 *
 * @{
 */
#define UART_NUMOF  1                   /**< Number of UART devices */
#define UART0_TXD   GPIO1               /**< TxD pin of UART_DEV(0) */
#define UART0_RXD   GPIO3               /**< RxD pin of UART_DEV(0) */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
