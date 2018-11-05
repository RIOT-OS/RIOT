/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf52
 * @{
 *
 * @file
 * @brief           nRF52 specific definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed, fixed to 64MHz for all NRF52x CPUs
 */
#define CLOCK_CORECLOCK     (64000000U)

/**
 * @name    Peripheral clock speed (fixed to 16MHz for nRF52 based CPUs)
 */
#define PERIPH_CLOCK        (16000000U)

/**
 * @brief   Redefine some peripheral names to unify them between nRF51 and 52
 * @{
 */
#define UART_IRQN           (UARTE0_UART0_IRQn)
#define SPI_SCKSEL          (dev(bus)->PSEL.SCK)
#define SPI_MOSISEL         (dev(bus)->PSEL.MOSI)
#define SPI_MISOSEL         (dev(bus)->PSEL.MISO)
/** @} */

/**
 * @brief   The nRF52 family of CPUs provides a fixed number of 9 ADC lines
 */
#define ADC_NUMOF           (9U)

/**
 * @brief   nRF52 specific naming of ADC lines (for convenience)
 */
enum {
    NRF52_AIN0 = 0,         /**< Analog Input 0 */
    NRF52_AIN1 = 1,         /**< Analog Input 1 */
    NRF52_AIN2 = 2,         /**< Analog Input 2 */
    NRF52_AIN3 = 3,         /**< Analog Input 3 */
    NRF52_AIN4 = 4,         /**< Analog Input 4 */
    NRF52_AIN5 = 5,         /**< Analog Input 5 */
    NRF52_AIN6 = 6,         /**< Analog Input 6 */
    NRF52_AIN7 = 7,         /**< Analog Input 7 */
    NRF52_VDD  = 8,         /**< VDD, not useful if VDD is reference... */
};

/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xf0,   /**< not supported by hardware */
    ADC_RES_8BIT  = 0x00,   /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x01,   /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0x02,   /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xf1,   /**< supported with oversampling, not implemented */
    ADC_RES_16BIT = 0xf2    /**< not supported by hardware */
} adc_res_t;
/** @} */

/**
 * @brief   Override I2C speed settings
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 0xff,                         /**< not supported */
    I2C_SPEED_NORMAL    = TWIM_FREQUENCY_FREQUENCY_K100,    /**< 100kbit/s */
    I2C_SPEED_FAST      = TWIM_FREQUENCY_FREQUENCY_K400,    /**< 400kbit/s */
    I2C_SPEED_FAST_PLUS = 0xfe,                         /**< not supported */
    I2C_SPEED_HIGH      = 0xfd,                         /**< not supported */
} i2c_speed_t;
/** @} */

/**
 * @brief   I2C (TWI) configuration options
 */
typedef struct {
    NRF_TWIM_Type *dev;         /**< TWIM hardware device */
    uint8_t scl;                /**< SCL pin */
    uint8_t sda;                /**< SDA pin */
    i2c_speed_t speed;          /**< Bus speed */
} i2c_conf_t;
/** @} */

/**
 * @name   Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_WRITE_REG
/** @} */

/**
 * @name    The PWM unit on the nRF52 supports 4 channels per device
 */
#define PWM_CHANNELS        (4U)

/**
 * @name    Generate PWM mode values
 *
 * To encode the PWM mode, we use two bit:
 * - bit  0: select up or up-and-down counting
 * - bit 15: select polarity
 */
#define PWM_MODE(ud, pol)   (ud | (pol << 15))

/**
 * @brief   Override the PWM mode definitions
 * @{
 */
#define HAVE_PWM_MODE_T
typedef enum {
    PWM_LEFT       = PWM_MODE(0, 1),    /**< left aligned PWM */
    PWM_RIGHT      = PWM_MODE(0, 0),    /**< right aligned PWM */
    PWM_CENTER     = PWM_MODE(1, 1),    /**< not supported */
    PWM_CENTER_INV = PWM_MODE(1, 0)     /**< not supported */
} pwm_mode_t;
/** @} */

/**
 * @brief   PWM configuration options
 *
 * Each device supports up to 4 channels. If you want to use less than 4
 * channels, just set the unused pins to GPIO_UNDEF.
 *
 * @note    define unused pins only from right to left, so the defined channels
 *          always start with channel 0 to x and the undefined ones are from x+1
 *          to PWM_CHANNELS.
 */
typedef struct {
    NRF_PWM_Type *dev;                  /**< PWM device descriptor */
    uint32_t pin[PWM_CHANNELS];         /**< PWM out pins */
} pwm_conf_t;


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
