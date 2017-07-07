/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc2538
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Starting offset of CPU_ID
 */
#define CPUID_ADDR          (&IEEE_ADDR_MSWORD)
/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (8U)

/**
 * @brief   Define a custom type for GPIO pins
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @name Internal GPIO shift and masking
 * @{
 */
#define PORTNUM_MASK        (0x00007000)    /**< bit mask for GPIO port [0-3] */
#define PORTNUM_SHIFT       (12U)           /**< bit shift for GPIO port      */
#define PIN_MASK            (0x00000007)    /**< bit mask for GPIO pin [0-7]  */
#define GPIO_MASK           (0xfffff000)    /**< bit mask for GPIO port addr  */
#define GPIO_BITS_PER_PORT  (8U)            /**< number of GPIO pins per port */
/** @} */

/**
 * @brief   Define a custom GPIO_PIN macro
 *
 * For the CC2538, we use OR the gpio ports base register address with the
 * actual pin number.
 */
#define GPIO_PIN(port, pin) (gpio_t)(((uint32_t)GPIO_A + \
                                      (port << PORTNUM_SHIFT)) | pin)

/**
 * @brief Access GPIO low-level device
 *
 * @param[in] pin   gpio pin
 *
 * @return          pointer to gpio low level device address
 */
static inline cc2538_gpio_t *gpio(gpio_t pin)
{
    return (cc2538_gpio_t *)(pin & GPIO_MASK);
}

/**
 * @brief   Helper function to get port number for gpio pin
 *
 * @param[in] pin   gpio pin
 *
 * @return          port number of gpio pin, [0=A - 3=D]
 */
static inline int gpio_port_num(gpio_t pin)
{
    return (int)((pin & PORTNUM_MASK) >> PORTNUM_SHIFT) - 1;
}

/**
 * @brief   Helper function to get pin number for gpio pin
 *
 * @param[in] pin   gpio pin
 *
 * @return          pin number of gpio pin, [0 - 7]
 */
static inline int gpio_pin_num(gpio_t pin)
{
    return (int)(pin & PIN_MASK);
}
/**
 * @brief   Helper function to get bit mask for gpio pin number
 *
 * @param[in] pin   gpio pin
 *
 * @return          bit mask for gpio pin number, 2^[0 - 7]
 */
static inline uint32_t gpio_pin_mask(gpio_t pin)
{
    return (1 << (pin & PIN_MASK));
}

/**
 * @brief   Helper function to get CC2538 gpio number from port and pin
 *
 * @param[in] pin   gpio pin
 *
 * @return          number of gpio pin, [0 - 31]
 */
static inline int gpio_pp_num(gpio_t pin)
{
    return (gpio_port_num(pin) * 8) + gpio_pin_num(pin);
}

/**
 * @brief   Helper function to enable gpio hardware control
 *
 * @param[in] pin   gpio pin
 */
static inline void gpio_hw_ctrl(gpio_t pin)
{
    gpio(pin)->AFSEL |= gpio_pin_mask(pin);
}

/**
 * @brief   Helper function to enable gpio software control
 *
 * @param[in] pin   gpio pin
 */
static inline void gpio_sw_ctrl(gpio_t pin)
{
    gpio(pin)->AFSEL &= ~gpio_pin_mask(pin);
}

/**
 * @brief   Define a custom GPIO_UNDEF value
 */
#define GPIO_UNDEF 99

/**
 * @brief   I2C configuration options
 */
typedef struct {
    gpio_t scl_pin;         /**< pin used for SCL */
    gpio_t sda_pin;         /**< pin used for SDA */
} i2c_conf_t;

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Override the default GPIO mode settings
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = ((uint8_t)0x00),               /**< input, no pull */
    GPIO_IN_PD = ((uint8_t)IOC_OVERRIDE_PDE),   /**< input, pull-down */
    GPIO_IN_PU = ((uint8_t)IOC_OVERRIDE_PUE),   /**< input, pull-up */
    GPIO_OUT   = ((uint8_t)IOC_OVERRIDE_OE),    /**< output */
    GPIO_OD    = (0xff),                        /**< not supported */
    GPIO_OD_PU = (0xff)                         /**< not supported */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override SPI mode settings
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                             /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = (SSI_CR0_SPH),                 /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (SSI_CR0_SPO),                 /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SSI_CR0_SPO | SSI_CR0_SPH)    /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @ */

/**
 * @brief   Override SPI clock settings
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 0,     /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 1,     /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = 2,     /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = 3,     /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 4      /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   Datafields for static SPI clock configuration values
 */
typedef struct {
    uint8_t cpsr;           /**< CPSR clock divider */
    uint8_t scr;            /**< SCR clock divider */
} spi_clk_conf_t;

/**
 * @brief   SPI configuration data structure
 * @{
 */
typedef struct {
    cc2538_ssi_t *dev;      /**< SSI device */
    gpio_t mosi_pin;        /**< pin used for MOSI */
    gpio_t miso_pin;        /**< pin used for MISO */
    gpio_t sck_pin;         /**< pin used for SCK */
    gpio_t cs_pin;          /**< pin used for CS */
} spi_conf_t;
/** @} */

/**
 * @brief   Timer configuration data
 */
typedef struct {
    cc2538_gptimer_t *dev;  /**< timer device */
    uint_fast8_t channels;  /**< number of channels */
    uint_fast8_t cfg;       /**< timer config word */
} timer_conf_t;

#ifdef __cplusplus
}
#endif

#include "periph/dev_enums.h"

#endif /* PERIPH_CPU_H */
/** @} */
