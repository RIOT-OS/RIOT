/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_remote
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the RE-Mote board revision B
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include "cc2538_gpio.h"
#include "periph_cpu.h"
#include "periph_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          UART0
#define UART_0_IRQ          UART0_IRQn
#define UART_0_ISR          isr_uart0
/* UART 0 pin configuration */
#define UART_0_TX_PIN       GPIO_PA1
#define UART_0_RX_PIN       GPIO_PA0

/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF               1
#define I2C_0_EN                1
#define I2C_IRQ_PRIO            1

/* I2C 0 device configuration */
#define I2C_0_DEV               0
#define I2C_0_IRQ               I2C_IRQn
#define I2C_0_IRQ_HANDLER       isr_i2c
#define I2C_0_SCL_PIN           GPIO_PC3
#define I2C_0_SDA_PIN           GPIO_PC2

static const i2c_conf_t i2c_config[I2C_NUMOF] = {
    {
        .scl_pin = I2C_0_SCL_PIN,
        .sda_pin = I2C_0_SDA_PIN,
    },
};
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           2
#define SPI_0_EN            1
#define SPI_1_EN            1

#ifdef HAVE_PERIPH_SPI_CONF_T
static const periph_spi_conf_t spi_config[SPI_NUMOF] = {
    {
        .dev      = SSI0,
        .mosi_pin = GPIO_PB1,
        .miso_pin = GPIO_PB3,
        .sck_pin  = GPIO_PB2,
        .cs_pin   = GPIO_PB5,
    },
    {
        .dev      = SSI1,
        .mosi_pin = GPIO_PC5,
        .miso_pin = GPIO_PC6,
        .sck_pin  = GPIO_PC4,
        .cs_pin   = GPIO_PA7,
    },
};
#endif
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_IRQ_PRIO       1

/* Enable only up to PD5 as PD6-PD7 are used with the 32KHz XOSC */
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           1
#define GPIO_7_EN           1
#define GPIO_8_EN           1
#define GPIO_9_EN           1
#define GPIO_10_EN          1
#define GPIO_11_EN          1
#define GPIO_12_EN          1
#define GPIO_13_EN          1
#define GPIO_14_EN          1
#define GPIO_15_EN          1
#define GPIO_16_EN          1
#define GPIO_17_EN          1
#define GPIO_18_EN          1
#define GPIO_19_EN          1
#define GPIO_20_EN          1
#define GPIO_21_EN          1
#define GPIO_22_EN          1
#define GPIO_23_EN          1
#define GPIO_24_EN          1
#define GPIO_25_EN          1
#define GPIO_26_EN          1
#define GPIO_27_EN          1
#define GPIO_28_EN          1
#define GPIO_29_EN          1

/**
 * @brief Port config
 *
 * These defines configures the port settings
 */
/* UART0 RX */
#define GPIO_0_PIN          GPIO_PA0
/* UART0 TX */
#define GPIO_1_PIN          GPIO_PA1
/* ADC3 */
#define GPIO_2_PIN          GPIO_PA2
/* User button/Bootloader */
#define GPIO_3_PIN          GPIO_PA3
/* ADC2 */
#define GPIO_4_PIN          GPIO_PA4
/* ADC1 */
#define GPIO_5_PIN          GPIO_PA5
/* SEL MicroSD or AIN6/ADC4 */
#define GPIO_6_PIN          GPIO_PA6
/* CSn MicroSD or AIN7/ADC5 */
#define GPIO_7_PIN          GPIO_PA7
/* CC1200 GDO2 */
#define GPIO_8_PIN          GPIO_PB0
/* SSI0 CC1200 MOSI */
#define GPIO_9_PIN          GPIO_PB1
/* SSI0 CC1200 CLK */
#define GPIO_10_PIN         GPIO_PB2
/* SSI0 CC1200 MISO */
#define GPIO_11_PIN         GPIO_PB3
/* CC1200 GDO0 */
#define GPIO_12_PIN         GPIO_PB4
/* CC1200 CSn */
#define GPIO_13_PIN         GPIO_PB5
/* JTAG TDI & LED3 */
#define GPIO_14_PIN         GPIO_PB6
/* JTAG TDO & LED2 */
#define GPIO_15_PIN         GPIO_PB7
/* UART1 TX */
#define GPIO_16_PIN         GPIO_PC0
/* UART1 RX */
#define GPIO_17_PIN         GPIO_PC1
/* I2C SDA */
#define GPIO_18_PIN         GPIO_PC2
/* I2C SCL */
#define GPIO_19_PIN         GPIO_PC3
/* SSI1 MicroSD CLK */
#define GPIO_20_PIN         GPIO_PC4
/* SSI1 MicroSD MOSI */
#define GPIO_21_PIN         GPIO_PC5
/* SSI1 MicroSD MISO */
#define GPIO_22_PIN         GPIO_PC6
/* CC1200 Reset */
#define GPIO_23_PIN         GPIO_PC7
/* GPIO */
#define GPIO_24_PIN         GPIO_PD0
/* Power Management enable */
#define GPIO_25_PIN         GPIO_PD1
/* RF SWITCH */
#define GPIO_26_PIN         GPIO_PD2
/* RTCC INT1 */
#define GPIO_27_PIN         GPIO_PD3
/* LED1 */
#define GPIO_28_PIN         GPIO_PD4
/* External WDT */
#define GPIO_29_PIN         GPIO_PD5

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
