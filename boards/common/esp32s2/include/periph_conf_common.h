/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp32s2
 * @brief       Common peripheral configurations for ESP32-S2 boards
 *
 * This file contains the peripheral configurations that are valid for all
 * ESP32-S2 boards.
 *
 * For detailed information about the peripheral configuration for ESP32-S2
 * boards, see section \ref esp32_peripherals "Common Peripherals".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

/* include periph_cpu.h to make it visible in any case */
#include "periph_cpu.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC configuration
 * @{
 */

/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * ADC_GPIOS is defined in board-specific peripheral configuration. Since
 * ADC_GPIOS must be defined even if there are no ADC channels, an empty
 * list definition is done here as fallback configuration.
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { }
#endif

/**
 * @brief   Static array with declared ADC channels
 */
static const gpio_t adc_channels[] = ADC_GPIOS;

/**
 * @brief Number of GPIOs declared as ADC channels
 *
 * The number of GPIOs that are declared as ADC channels is determined from
 * the ADC_GPIOS definition.
 *
 * @note ADC_NUMOF definition must not be changed.
 */
#define ADC_NUMOF   ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name    DAC configuration
 * @{
 */

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 *
 * DAC_GPIOS is defined in board-specific peripheral configuration. Since
 * DAC_GPIOS must be defined even if there are no DAC channels, an empty
 * list definition is done here as fallback configuration.
 */
#ifndef DAC_GPIOS
#define DAC_GPIOS   { }
#endif

/**
 * @brief   Static array with declared DAC channels
 */
static const gpio_t dac_channels[] = DAC_GPIOS;

/**
 * @brief Number of GPIOs declared as DAC channels
 *
 * The number of GPIOs that are declared as DAC channels is determined from
 * the DAC_GPIOS definition.
 *
 * @note DAC_NUMOF definition must not be changed.
 */
#define DAC_NUMOF   ARRAY_SIZE(dac_channels)
/** @} */

/**
 * @name   I2C configuration
 * @{
 */

#if defined(I2C0_SCL) && !defined(I2C0_SCL_PULLUP)
/** Define SCL pullup enabled by default */
#define I2C0_SCL_PULLUP true
#endif
#if defined(I2C0_SDA) && !defined(I2C0_SDA_PULLUP)
/** Define SDA pullup enabled by default */
#define I2C0_SDA_PULLUP true
#endif

#if defined(I2C1_SCL) && !defined(I2C1_SCL_PULLUP)
/** Define SCL pullup enabled by default */
#define I2C1_SCL_PULLUP true
#endif
#if defined(I2C1_SDA) && !defined(I2C1_SDA_PULLUP)
/** Define SDA pullup enabled by default */
#define I2C1_SDA_PULLUP true
#endif

/**
 * @brief   Static array with configuration for declared I2C devices
 */
static const i2c_conf_t i2c_config[] = {
#if defined(I2C0_SCL) && defined(I2C0_SDA) && defined(I2C0_SPEED)
    {
        .module = PERIPH_I2C0_MODULE,
        .speed = I2C0_SPEED,
        .scl = I2C0_SCL,
        .sda = I2C0_SDA,
        .scl_pullup = I2C0_SCL_PULLUP,
        .sda_pullup = I2C0_SCL_PULLUP,
    },
#endif
#if defined(I2C1_SCL) && defined(I2C1_SDA) && defined(I2C1_SPEED)
    {
        .module = PERIPH_I2C1_MODULE,
        .speed = I2C1_SPEED,
        .scl = I2C1_SCL,
        .sda = I2C1_SDA,
        .scl_pullup = I2C1_SCL_PULLUP,
        .sda_pullup = I2C1_SCL_PULLUP,
    },
#endif
};

/**
 * @brief Number of I2C interfaces
 *
 * The number of I2C interfaces is determined from board-specific peripheral
 * definitions of I2Cn_SPEED, I2Cn_SCK, and I2Cn_SDA.
 *
 * @note I2C_NUMOF definition must not be changed.
 */
#define I2C_NUMOF   ARRAY_SIZE(i2c_config)

/** @} */

/**
 * @name   PWM configuration
 * @{
 */

/**
 * @brief   GPIOs used as channels for the according PWM device
 */
#ifdef PWM0_GPIOS
static const gpio_t pwm0_gpios[] = PWM0_GPIOS;
#endif

/**
 * @brief   GPIOs used as channels for the according PWM device
 */
#ifdef PWM1_GPIOS
static const gpio_t pwm1_gpios[] = PWM1_GPIOS;
#endif

/**
 * @brief   GPIOs used as channels for the according PWM device
 */
#ifdef PWM2_GPIOS
static const gpio_t pwm2_gpios[] = PWM2_GPIOS;
#endif

/**
 * @brief   GPIOs used as channels for the according PWM device
 */
#ifdef PWM3_GPIOS
static const gpio_t pwm3_gpios[] = PWM3_GPIOS;
#endif

/**
 * @brief   PWM device configuration based on defined PWM channel GPIOs
 */
static const pwm_config_t pwm_config[] =
{
#ifdef PWM0_GPIOS
    {
        .module = PERIPH_LEDC_MODULE,
        .group = LEDC_LOW_SPEED_MODE,
        .timer = LEDC_TIMER_0,
        .ch_numof = ARRAY_SIZE(pwm0_gpios),
        .gpios = pwm0_gpios,
    },
#endif
#ifdef PWM1_GPIOS
    {
        .module = PERIPH_LEDC_MODULE,
#ifdef SOC_LEDC_SUPPORT_HS_MODE
        .group = LEDC_HIGH_SPEED_MODE,
#else
        .group = LEDC_LOW_SPEED_MODE,
#endif
        .timer = LEDC_TIMER_1,
        .ch_numof = ARRAY_SIZE(pwm1_gpios),
        .gpios = pwm1_gpios,
    },
#endif
#ifdef PWM2_GPIOS
    {
        .module = PERIPH_LEDC_MODULE,
        .group = LEDC_LOW_SPEED_MODE,
        .timer = LEDC_TIMER_2,
        .ch_numof = ARRAY_SIZE(pwm2_gpios),
        .gpios = pwm2_gpios,
    },
#endif
#ifdef PWM3_GPIOS
    {
        .module = PERIPH_LEDC_MODULE,
#ifdef SOC_LEDC_SUPPORT_HS_MODE
        .group = LEDC_HIGH_SPEED_MODE,
#else
        .group = LEDC_LOW_SPEED_MODE,
#endif
        .timer = LEDC_TIMER_3,
        .ch_numof = ARRAY_SIZE(pwm3_gpios),
        .gpios = pwm3_gpios,
    },
#endif
};

/**
 * @brief   Number of PWM devices
 *
 * The number of PWM devices is determined from the PWM device configuration.
 *
 * @note PWM_NUMOF definition must not be changed.
 */
#define PWM_NUMOF   ARRAY_SIZE(pwm_config)

/** @} */

/**
 * @name    RMT configuration
 * @{
 */

/**
 * @brief   RMT channel configuration
 *
 * For the moment, the only RMT channel configuration used is for the WS2812
 * driver. To override this default configuration by a board definition, add
 * ```
 * #define HAVE_RMT_CHANNEL_CONFIG
 * ```
 * before including this file and define an according `rmt_channel_config`
 * after including this file.
 */
#ifndef HAVE_RMT_CHANNEL_CONFIG
static const rmt_channel_config_t rmt_channel_config[] = {
#ifdef WS281X_PARAM_PIN
    { 0, WS281X_PARAM_PIN },
#else
    /* default for boards that don't define WS281X_PARAM_PIN */
    { 0, GPIO0 },
#endif
};
#endif

/**
 * @brief   Number of RMT channels
 */
#define RMT_CH_NUMOF    ARRAY_SIZE(rmt_channel_config)

/** @} */

/**
 * @name   SPI configuration
 * @{
 */

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
#ifdef SPI1_CTRL
    {
        .ctrl = SPI1_CTRL,
        .sck = SPI1_SCK,
        .mosi = SPI1_MOSI,
        .miso = SPI1_MISO,
        .cs = SPI1_CS0,
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
 * @name   UART configuration
 * @{
 */

#ifndef UART0_TXD
#define UART0_TXD   (GPIO43)  /**< TxD of UART_DEV(0) used on all ESP32-S2 boards */
#endif
#ifndef UART0_RXD
#define UART0_RXD   (GPIO44)  /**< RxD of UART_DEV(0) used on all ESP32-S2 boards */
#endif

/**
 * @brief   Static array with configuration for declared UART devices
 */
static const uart_conf_t uart_config[] = {
    {
        .txd = UART0_TXD,
        .rxd = UART0_RXD,
    },
#if defined(UART1_TXD) && defined(UART1_RXD)
    {
        .txd = UART1_TXD,
        .rxd = UART1_RXD,
    },
#endif
#if defined(UART2_TXD) && defined(UART2_RXD)
    {
        .txd = UART2_TXD,
        .rxd = UART2_RXD,
    },
#endif
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

/**
 * @name   USB device configuration
 *
 * @{
 */

#include "usbdev_esp32.h"

/**
 * @brief   Static array with USB OTG FS configuration
 */
static const dwc2_usb_otg_fshs_config_t dwc2_usb_otg_fshs_config[] = {
    {
        .periph = USB_OTG_FS_PERIPH_BASE,
        .type = DWC2_USB_OTG_FS,
        .phy = DWC2_USB_OTG_PHY_BUILTIN,
    }
};

/**
 * @brief Number of USB OTG FS interfaces
 */
#define USBDEV_NUMOF    ARRAY_SIZE(dwc2_usb_otg_fshs_config)

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
