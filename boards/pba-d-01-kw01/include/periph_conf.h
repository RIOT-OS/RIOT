/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_pba-d-01-kw01
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the phyWAVE-KW01
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#include "cpu-conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define KINETIS_CPU_USE_MCG               1

#define KINETIS_MCG_USE_ERC               1
#define KINETIS_MCG_ERC_FREQ              32768
#define KINETIS_MCG_ERC_FRDIV             0
#define KINETIS_MCG_ERC_RANGE             0
#define KINETIS_MCG_ERC_OSCILLATOR        1

#define KINETIS_MCG_USE_PLL               0

#define KINETIS_MCG_USE_FAST_IRC          0

#define CLOCK_CORECLOCK                   (48e6)        /* core clock frequency */
/** @} */


/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (0U)
#define TIMER_0_EN          0
#define TIMER_1_EN          0
#define TIMER_2_EN          0
#define TIMER_IRQ_PRIO      1
/** @} */


/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_IRQ_PRIO       1
#define UART_CLK            (48e6)

/* UART 0 device configuration */
#define KINETIS_UART        UART0_Type
#define UART_0_DEV          UART0
#define UART_0_CLKEN()      (SIM->SCGC4 |= (SIM_SCGC4_UART0_MASK))
#define UART_0_CLK          UART_CLK
#define UART_0_IRQ_CHAN     UART0_IRQn
#define UART_0_ISR          isr_uart0
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK))
#define UART_0_PORT         PORTA
#define UART_0_RX_PIN       1
#define UART_0_TX_PIN       2
#define UART_0_AF           2
/** @} */


/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (0U)
#define ADC_0_EN            0
#define ADC_MAX_CHANNELS    0
/** @} */


/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (0U)
#define PWM_0_EN            0
#define PWM_MAX_CHANNELS    0
/** @} */


/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF               (0U)
#define SPI_CLK                 (48e6)
#define SPI_0_EN                0
#define SPI_IRQ_PRIO            1

/* SPI 0 device config */
#define SPI_0_DEV               SPI0
#define SPI_0_CLKEN()           (SIM->SCGC6 |= (SIM_SCGC4_SPI1_MASK))
#define SPI_0_CLKDIS()          (SIM->SCGC6 &= ~(SIM_SCGC4_SPI1_MASK))
#define SPI_0_IRQ               SPI0_IRQn
#define SPI_0_IRQ_HANDLER       isr_spi0
/* SPI 0 pin configuration */
#define SPI_0_PORT              PORTD
#define SPI_0_PORT_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define SPI_0_PIN_AF            2
#define SPI_0_PCS0_PIN          4
#define SPI_0_SCK_PIN           5
#define SPI_0_SOUT_PIN          6
#define SPI_0_SIN_PIN           7
/** @} */


/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF               (1U)
#define I2C_CLK                 (48e6)
#define I2C_0_EN                1
#define I2C_IRQ_PRIO            1
/* Low (10 kHz): MUL = 4, SCL divider = 2560, total: 10240 */
#define KINETIS_I2C_F_ICR_LOW        (0x3D)
#define KINETIS_I2C_F_MULT_LOW       (2)
/* Normal (100 kHz): MUL = 2, SCL divider = 240, total: 480 */
#define KINETIS_I2C_F_ICR_NORMAL     (0x1F)
#define KINETIS_I2C_F_MULT_NORMAL    (1)
/* Fast (400 kHz): MUL = 1, SCL divider = 128, total: 128 */
#define KINETIS_I2C_F_ICR_FAST       (0x17)
#define KINETIS_I2C_F_MULT_FAST      (0)
/* Fast plus (1000 kHz): MUL = 1, SCL divider = 48, total: 48 */
#define KINETIS_I2C_F_ICR_FAST_PLUS  (0x10)
#define KINETIS_I2C_F_MULT_FAST_PLUS (0)

/* I2C 0 device configuration */
#define I2C_0_DEV               I2C1
#define I2C_0_CLKEN()           (SIM->SCGC4 |= (SIM_SCGC4_I2C1_MASK))
#define I2C_0_CLKDIS()          (SIM->SCGC4 &= ~(SIM_SCGC4_I2C1_MASK))
#define I2C_0_IRQ               I2C1_IRQn
#define I2C_0_IRQ_HANDLER       isr_i2c1
/* I2C 0 pin configuration */
#define I2C_0_PORT              PORTC
#define I2C_0_PORT_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK))
#define I2C_0_PIN_AF            2
#define I2C_0_SDA_PIN           2
#define I2C_0_SCL_PIN           1
#define I2C_0_PORT_CFG          (PORT_PCR_MUX(I2C_0_PIN_AF))

/** @} */


/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          3
#define GPIO_0_EN           1    /* DIO11 */
#define GPIO_1_EN           1    /* DIO27 */
#define GPIO_2_EN           1    /* DIO2 */
#define GPIO_IRQ_PRIO       1

/* GPIO channel 0 config */
#define GPIO_0_DEV          GPIOA
#define GPIO_0_PORT         PORTA
#define GPIO_0_PIN          4
#define GPIO_0_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK))
#define GPIO_0_IRQ          PORTA_IRQn
#define GPIO_0_ISR          isr_porta
/* GPIO channel 1 config */
#define GPIO_1_DEV          GPIOC
#define GPIO_1_PORT         PORTC
#define GPIO_1_PIN          3
#define GPIO_1_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK))
#define GPIO_1_IRQ          PORTC_PORTD_IRQn
#define GPIO_1_ISR          isr_portc
/* GPIO channel 3 config */
#define GPIO_2_DEV          GPIOD
#define GPIO_2_PORT         PORTD
#define GPIO_2_PIN          4
#define GPIO_2_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define GPIO_2_IRQ          PORTC_PORTD_IRQn
#define GPIO_2_ISR          isr_portd
/** @} */

/**
* @name RTT and RTC configuration
* @{
*/
#define RTT_NUMOF            (1U)
#define RTC_NUMOF            (1U)
#define RTT_DEV              RTC
#define RTT_IRQ              RTC_IRQn
#define RTT_IRQ_PRIO         10
#define RTT_UNLOCK()         (SIM->SCGC6 |= (SIM_SCGC6_RTC_MASK))
#define RTT_ISR              isr_rtc
#define RTT_FREQUENCY        (1)
#define RTT_MAX_VALUE        (0xffffffff)
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
