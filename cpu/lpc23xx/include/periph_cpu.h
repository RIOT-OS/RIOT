/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_lpc23xx
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "cpu.h"

/**
 * @brief   LPC2387 MCU defines
 * @{
 */
#define __IO volatile
/** @} */

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (4)
/** @} */

/**
 * @brief Fast GPIO register definition struct
 */
typedef struct {
    /** @brief Direction: Output if corresponding bit is set, otherwise input */
    __IO uint32_t DIR;
    /** @brief 12 bytes of reserved memory we don't need to access */
    uint32_t _reserved[3];
    /** @brief Set bits to ignore corresponding bits when accessing `PIN`, `SET`
     *         or `CLR` register of this port
     */
    __IO uint32_t MASK;
    /** @brief The current state of each pin of this port is accessible here
     *         (regardless of direction): If bit is set input is high
     */
    __IO uint32_t PIN;
    /** @brief Output pins are set to high by setting the corresponding bit */
    __IO uint32_t SET;
    /** @brief Output pins are set to low by setting the corresponding bit */
    __IO uint32_t CLR;
} FIO_PORT_t;

/**
 * @brief   Pointer to the Fast GPIO register
 */
#define FIO_PORTS   ((FIO_PORT_t*)FIO_BASE_ADDR)

/**
 * @brief   Pointer to the PINSEL register
 */
#define PINSEL      ((__IO uint32_t *)(PINSEL_BASE_ADDR))

/**
 * @brief   Pointer to the PINMODE register
 */
#define PINMODE     ((__IO uint32_t *)(PINSEL_BASE_ADDR + 0x40))

/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
int gpio_init_mux(unsigned pin, unsigned mux);

/**
 * @brief   Macro for accessing GPIO pins
 */
#define GPIO_PIN(port, pin) (port<<5 | pin)

#ifndef DOXYGEN
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 1,       /**< emit interrupt on falling flank */
    GPIO_RISING = 2,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif /* ifndef DOXYGEN */

/**
 * @brief   UART device configuration
 */
typedef struct {
    lpc23xx_uart_t *dev;    /**< pointer to the UART device */
    uint8_t irq_prio_rx;    /**< priority of the RX IRQ */
    uint8_t pinsel_rx;      /**< PINSEL# of the RX pin  */
    uint8_t pinsel_tx;      /**< PINSEL# of the TX pin  */
    uint32_t pinsel_msk_rx; /**< RX PINSEL Mask         */
    uint32_t pinsel_msk_tx; /**< TX PINSEL Mask         */
} uart_conf_t;

/**
 * @brief   SPI device configuration
 */
typedef struct {
    lpc23xx_spi_t *dev;         /**< pointer to the SPI device */
    uint8_t pinsel_mosi;        /**< PINSEL# of the MOSI pin   */
    uint8_t pinsel_miso;        /**< PINSEL# of the MISO pin   */
    uint8_t pinsel_clk;         /**< PINSEL# of the CLK pin    */
    uint32_t pinsel_msk_mosi;   /**< MOSI PINSEL Mask          */
    uint32_t pinsel_msk_miso;   /**< MISO PINSEL Mask          */
    uint32_t pinsel_msk_clk;    /**< CLK PINSEL Mask           */
} spi_conf_t;

/**
 * @brief   Number of available timer channels
 */
#define TIMER_CHANNEL_NUMOF (4U)

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   Declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 100,   /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 400,   /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = 1000,  /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = 5000,  /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000  /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   DAC configuration, valid for all boards using this CPU
 *
 * lpc23xx has a fixed mapping of DAC pins and a fixed number of DAC channels,
 * so this DAC configuration is valid for all boards using this CPU. No need for
 * any board specific configuration.
 *
 * The DAC of the lpc23xx is mapped to the following fixed pin:
 * - line 0 (ch0): P0.26
 */
#define DAC_NUMOF           (1U)

#ifndef DOXYGEN
/**
 * @brief   Possible ADC resolution settings
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_16BIT = 0xff,   /**< not applicable         */
    ADC_RES_14BIT = 0xfe,   /**< not applicable         */
    ADC_RES_12BIT = 0xfd,   /**< not applicable         */
    ADC_RES_10BIT = 0b000,  /**< ADC resolution: 10 bit */
    ADC_RES_9BIT  = 0b001,  /**< ADC resolution:  9 bit */
    ADC_RES_8BIT  = 0b010,  /**< ADC resolution:  8 bit */
    ADC_RES_7BIT  = 0b011,  /**< ADC resolution:  7 bit */
    ADC_RES_6BIT  = 0b100,  /**< ADC resolution:  6 bit */
    ADC_RES_5BIT  = 0b101,  /**< ADC resolution:  5 bit */
    ADC_RES_4BIT  = 0b110,  /**< ADC resolution:  4 bit */
    ADC_RES_3BIT  = 0b111,  /**< ADC resolution:  3 bit */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   ADC device configuration
 */
typedef struct {
    uint8_t  chan;          /**< which ADC to use (0…7)  */
    uint8_t  pinsel;        /**< PINSEL# of the ADC pin  */
    uint32_t pinsel_msk;    /**< PINSEL Mask for ADC pin */
} adc_conf_t;

#ifndef DOXYGEN
/**
 * @brief   Override I2C clock speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW    =  10000,  /**< low speed mode:  ~10 kbit/s */
    I2C_SPEED_NORMAL = 100000,  /**< normal mode:    ~100 kbit/s */
    I2C_SPEED_FAST   = 400000,  /**< fast mode:      ~400 kbit/s */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   I2C device configuration
 */
typedef struct {
    lpc23xx_i2c_t *dev;     /**< pointer to the I2C device  */
    i2c_speed_t speed;      /**< I2C bus speed              */
    uint8_t irq_prio;       /**< priority of the I2C IRQ    */
    uint8_t pinsel_sda;     /**< PINSEL# of the SDA pin     */
    uint8_t pinsel_scl;     /**< PINSEL# of the SCL pin     */
    uint32_t pinsel_msk_sda;/**< SDA PINSEL Mask            */
    uint32_t pinsel_msk_scl;/**< SCL PINSEL Mask            */
} i2c_conf_t;

/**
 * @name   Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_WRITE_REG
/** @} */

/**
 * Enable yday and wday calculation in rtc_normalize().
 * Those fields are used by the RTC.
 * @{
 */
#if defined(MODULE_PERIPH_RTC) && !defined(RTC_NORMALIZE_COMPAT)
#define RTC_NORMALIZE_COMPAT    (1)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
