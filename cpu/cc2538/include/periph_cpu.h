/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal peripheral handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>
#include <stdio.h>

#include "vendor/hw_soc_adc.h"

#include "cpu.h"
#include "vendor/hw_ssi.h"
#include "vendor/hw_uart.h"

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
 * @name    Define a custom type for GPIO pins
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST_CORTEXM
/** @} */

/**
 * @brief Define custom value to specify undefined or unused GPIOs
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief Custom value to indicate unused parameter in gpio_init_mux
 */
#define GPIO_MUX_NONE       (0xff)
/**
 * @brief   Define a custom GPIO_PIN macro
 *
 * For the CC2538, we use OR the gpio ports base register address with the
 * actual pin number.
 */
#define GPIO_PIN(port, pin) (gpio_t)(((uint32_t)GPIO_BASE + \
                                      (port << GPIO_PORTNUM_SHIFT)) | pin)

/**
 * @brief   Configure an alternate function for the given pin
 *
 * @param[in] pin   gpio pin
 * @param[in] sel   Select pin peripheral function
 * @param[in] over  Override pin configuration
 */
void gpio_init_af(gpio_t pin, uint8_t sel, uint8_t over);

/**
 * @brief   Configure an alternate function for the given pin
 *
 * @param[in] pin   gpio pin
 * @param[in] over  Override pin configuration
 * @param[in] sel   Set peripheral function for pin (output)
 * @param[in] func  Set pin for peripheral function (input)
 */
void gpio_init_mux(gpio_t pin, uint8_t over, uint8_t sel, uint8_t func);

/**
 * @name   Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @name   Override I2C clock speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 0x01,     /**< not supported */
    I2C_SPEED_NORMAL    = 100000U,  /**< normal mode:   ~100kbit/s */
    I2C_SPEED_FAST      = 400000U,  /**< fast mode:     ~400kbit/s */
    I2C_SPEED_FAST_PLUS = 0x02,     /**< not supported */
    I2C_SPEED_HIGH      = 0x03,     /**< not supported */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   I2C configuration options
 */
typedef struct {
    i2c_speed_t speed;      /**< baudrate used for the bus */
    gpio_t scl_pin;         /**< pin used for SCL */
    gpio_t sda_pin;         /**< pin used for SDA */
} i2c_conf_t;

/**
 * @name declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @name   Override the default GPIO mode settings
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN         = ((uint8_t)OVERRIDE_DISABLE),      /**< input, no pull */
    GPIO_IN_ANALOG  = ((uint8_t)OVERRIDE_ANALOG),       /**< input, analog */
    GPIO_IN_PD      = ((uint8_t)OVERRIDE_PULLDOWN),     /**< input, pull-down */
    GPIO_IN_PU      = ((uint8_t)OVERRIDE_PULLUP),       /**< input, pull-up */
    GPIO_OUT        = ((uint8_t)OVERRIDE_ENABLE),       /**< output */
    GPIO_OD         = (0xff),                           /**< not supported */
    GPIO_OD_PU      = (0xff)                            /**< not supported */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @name   UART device configuration
 * @{
 */
typedef struct {
    cc2538_uart_t *dev;       /**< pointer to the used UART device */
    gpio_t rx_pin;            /**< pin used for RX */
    gpio_t tx_pin;            /**< pin used for TX */
    gpio_t cts_pin;           /**< CTS pin - set to GPIO_UNDEF when not using */
    gpio_t rts_pin;           /**< RTS pin - set to GPIO_UNDEF when not using */
} uart_conf_t;
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override parity values
 * @{
 */
#define HAVE_UART_PARITY_T
typedef enum {
   UART_PARITY_NONE = 0,                                                /**< no parity */
   UART_PARITY_EVEN = (UART_LCRH_PEN | UART_LCRH_EPS),                  /**< even parity */
   UART_PARITY_ODD = UART_LCRH_PEN,                                     /**< odd parity */
   UART_PARITY_MARK = (UART_LCRH_PEN | UART_LCRH_SPS),                  /**< mark */
   UART_PARITY_SPACE = (UART_LCRH_PEN | UART_LCRH_EPS | UART_LCRH_SPS)  /**< space */
} uart_parity_t;
/** @} */

 /**
 * @brief   Override data bits length values
 * @{
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5 = (0 << UART_LCRH_WLEN_S),     /**< 5 data bits */
    UART_DATA_BITS_6 = (1 << UART_LCRH_WLEN_S),     /**< 6 data bits */
    UART_DATA_BITS_7 = (2 << UART_LCRH_WLEN_S),     /**< 7 data bits */
    UART_DATA_BITS_8 = (3 << UART_LCRH_WLEN_S),     /**< 8 data bits */
} uart_data_bits_t;
/** @} */

/**
 * @brief   Override stop bits length values
 * @{
 */
#define HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1 = 0,                  /**< 1 stop bit */
   UART_STOP_BITS_2 = UART_LCRH_STP2,     /**< 2 stop bits */
} uart_stop_bits_t;
/** @} */

/**
 * @name   Override SPI mode settings
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
 * @name   Override SPI clock settings
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
#endif /* ndef DOXYGEN */

/**
 * @brief   Datafields for static SPI clock configuration values
 */
typedef struct {
    uint8_t cpsr;           /**< CPSR clock divider */
    uint8_t scr;            /**< SCR clock divider */
} spi_clk_conf_t;

#ifndef BOARD_HAS_SPI_CLK_CONF
/**
 * @brief   Pre-calculated clock divider values based on a CLOCK_CORECLOCK (32MHz)
 *
 * SPI bus frequency =  CLOCK_CORECLOCK / (CPSR * (SCR + 1)), with
 * CPSR = 2..254 and even,
 *  SCR = 0..255
 */
static const spi_clk_conf_t spi_clk_config[] = {
    { .cpsr = 64, .scr =  4 },  /* 100khz */
    { .cpsr = 16, .scr =  4 },  /* 400khz */
    { .cpsr = 32, .scr =  0 },  /* 1.0MHz */
    { .cpsr =  2, .scr =  2 },  /* 5.3MHz */
    { .cpsr =  2, .scr =  1 }   /* 8.0MHz */
};
#endif /* BOARD_HAS_SPI_CLK_CONF */

/**
 * @name    SPI configuration data structure
 * @{
 */
typedef struct {
    uint8_t num;            /**< number of SSI device, i.e. 0 or 1 */
    gpio_t mosi_pin;        /**< pin used for MOSI */
    gpio_t miso_pin;        /**< pin used for MISO */
    gpio_t sck_pin;         /**< pin used for SCK */
    gpio_t cs_pin;          /**< pin used for CS */
} spi_conf_t;
/** @} */

/**
 * @brief   Timer configuration
 *
 * General purpose timers (GPT[0-3]) are configured consecutively and in order
 * (without gaps) starting from GPT0, i.e. if multiple timers are enabled.
 */
typedef struct {
    uint_fast8_t chn;   /**< number of channels */
    uint_fast8_t cfg;   /**< timer config word */
} timer_conf_t;

#ifndef DOXYGEN
/**
 * @name   Override resolution options
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  =             (0xa00),    /**< not supported by hardware */
    ADC_RES_7BIT  =             (0 << 4),   /**< ADC resolution: 7 bit */
    ADC_RES_8BIT  =             (0xb00),    /**< not supported by hardware */
    ADC_RES_9BIT  =             (1 << 4),   /**< ADC resolution: 9 bit */
    ADC_RES_10BIT =             (2 << 4),   /**< ADC resolution: 10 bit */
    ADC_RES_12BIT =             (3 << 4),   /**< ADC resolution: 12 bit */
    ADC_RES_14BIT =             (0xc00),    /**< not supported by hardware */
    ADC_RES_16BIT =             (0xd00),    /**< not supported by hardware */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief ADC configuration wrapper
 */
typedef gpio_t adc_conf_t;

/**
 * @name SOC_ADC_ADCCON3_EREF registers field values
 * @{
 */
#define SOC_ADC_ADCCON3_EREF_INT      (0 << SOC_ADC_ADCCON3_EREF_S)    /**< Internal reference */
#define SOC_ADC_ADCCON3_EREF_EXT      (1 << SOC_ADC_ADCCON3_EREF_S)    /**< External reference on AIN7 pin */
#define SOC_ADC_ADCCON3_EREF_AVDD5    (2 << SOC_ADC_ADCCON3_EREF_S)    /**< AVDD5 pin */
#define SOC_ADC_ADCCON3_EREF_DIFF     (3 << SOC_ADC_ADCCON3_EREF_S)    /**< External reference on AIN6-AIN7 differential input */
/** @} */

/**
 * @name Bit shift for data per ADC resolution
 * @{
 */
#define SOCADC_7_BIT_RSHIFT     (9U) /**< Mask for getting data( 7 bits ENOB) */
#define SOCADC_9_BIT_RSHIFT     (7U) /**< Mask for getting data( 9 bits ENOB) */
#define SOCADC_10_BIT_RSHIFT    (6U) /**< Mask for getting data(10 bits ENOB) */
#define SOCADC_12_BIT_RSHIFT    (4U) /**< Mask for getting data(12 bits ENOB) */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
