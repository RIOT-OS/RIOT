/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_frdm-k64f
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the FRDM-K64F
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define KINETIS_CPU_USE_MCG          1

#define KINETIS_MCG_USE_ERC          1
#define KINETIS_MCG_USE_PLL          1
#define KINETIS_MCG_DCO_RANGE        (24000000U)
#define KINETIS_MCG_ERC_OSCILLATOR   0
#define KINETIS_MCG_ERC_FRDIV        6           /* ERC devider = 1280 */
#define KINETIS_MCG_ERC_RANGE        2
#define KINETIS_MCG_ERC_FREQ         50000000
#define KINETIS_MCG_PLL_PRDIV        19          /* divide factor = 20 */
#define KINETIS_MCG_PLL_VDIV0        0           /* multiply factor = 24 */
#define KINETIS_MCG_PLL_FREQ         60000000

#define CLOCK_CORECLOCK              KINETIS_MCG_PLL_FREQ
/** @} */


/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF                  (1U)
#define TIMER_0_EN                   1
#define TIMER_1_EN                   0
#define TIMER_IRQ_PRIO               1
#define TIMER_BASE                   PIT
#define TIMER_MAX_VALUE              (0xffffffff)
#define TIMER_CLOCK                  CLOCK_CORECLOCK
#define TIMER_CLKEN()                (SIM->SCGC6 |= (SIM_SCGC6_PIT_MASK))

/* Timer 0 configuration */
#define TIMER_0_PRESCALER_CH         0
#define TIMER_0_COUNTER_CH           1
#define TIMER_0_ISR                  isr_pit1
#define TIMER_0_IRQ_CHAN             PIT1_IRQn

/* Timer 1 configuration */
#define TIMER_1_PRESCALER_CH         2
#define TIMER_1_COUNTER_CH           3
#define TIMER_1_ISR                  isr_pit3
#define TIMER_1_IRQ_CHAN             PIT3_IRQn
/** @} */

/**
* @name UART configuration
* @{
*/
#define UART_NUMOF                   (1U)
#define UART_0_EN                    1
#define UART_IRQ_PRIO                1
#define UART_CLK                     CLOCK_CORECLOCK

/* UART 0 device configuration */
#define KINETIS_UART                 UART_Type
#define UART_0_DEV                   UART0
#define UART_0_CLKEN()               (SIM->SCGC4 |= (SIM_SCGC4_UART0_MASK))
#define UART_0_CLK                   UART_CLK
#define UART_0_IRQ_CHAN              UART0_RX_TX_IRQn
#define UART_0_ISR                   isr_uart0_rx_tx
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN()          (SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK))
#define UART_0_PORT                  PORTB
#define UART_0_RX_PIN                16
#define UART_0_TX_PIN                17
#define UART_0_AF                    3
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF                    (1U)
#define ADC_0_EN                     1
#define ADC_MAX_CHANNELS             6

/* ADC 0 configuration */
#define ADC_0_DEV                    ADC1
#define ADC_0_MODULE_CLOCK           CLOCK_CORECLOCK
#define ADC_0_CHANNELS               6
#define ADC_0_CLKEN()                (SIM->SCGC3 |= (SIM_SCGC3_ADC1_MASK))
#define ADC_0_CLKDIS()               (SIM->SCGC3 &= ~(SIM_SCGC3_ADC1_MASK))
#define ADC_0_PORT_CLKEN()           (SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK))
/* ADC 0 channel 0 pin config */
#define ADC_0_CH0_PORT               PORTB
#define ADC_0_CH0_PIN                10
#define ADC_0_CH0_PIN_AF             0
#define ADC_0_CH0                    14
/* ADC 0 channel 1 pin config */
#define ADC_0_CH1_PORT               PORTB
#define ADC_0_CH1_PIN                11
#define ADC_0_CH1_PIN_AF             0
#define ADC_0_CH1                    15
/* ADC 0 channel 2 pin config */
#define ADC_0_CH2_PORT               PORTC
#define ADC_0_CH2_PIN                11
#define ADC_0_CH2_PIN_AF             0
#define ADC_0_CH2                    7
/* ADC 0 channel 3 pin config */
#define ADC_0_CH3_PORT               PORTC
#define ADC_0_CH3_PIN                10
#define ADC_0_CH3_PIN_AF             0
#define ADC_0_CH3                    6
/* ADC 0 channel 4 pin config */
#define ADC_0_CH4_PORT               PORTC
#define ADC_0_CH4_PIN                8
#define ADC_0_CH4_PIN_AF             0
#define ADC_0_CH4                    4
/* ADC 0 channel 5 pin config */
#define ADC_0_CH5_PORT               PORTC
#define ADC_0_CH5_PIN                9
#define ADC_0_CH5_PIN_AF             0
#define ADC_0_CH5                    5
/** @} */

/**
* @name PWM configuration
* @{
*/
#define PWM_NUMOF                    (1U)
#define PWM_0_EN                     1
#define PWM_MAX_CHANNELS             8
#define PWM_MAX_VALUE                0xffff

/* PWM 0 device configuration */
#define PWM_0_DEV                    FTM0
#define PWM_0_CHANNELS               4
#define PWM_0_CLK                    CLOCK_CORECLOCK
#define PWM_0_CLKEN()                (SIM->SCGC6 |= (SIM_SCGC6_FTM0_MASK))
#define PWM_0_CLKDIS()               (SIM->SCGC6 &= ~(SIM_SCGC6_FTM0_MASK))
/* PWM 0 pin configuration */
#define PWM_0_PORT_CLKEN()           (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK))
/* Arduino Connector D3 */
#define PWM_0_PORT_CH0               PORTA
#define PWM_0_PIN_CH0                1
#define PWM_0_FTMCHAN_CH0            6
#define PWM_0_PIN_AF_CH0             3
/* Arduino Connector D5 */
#define PWM_0_PORT_CH1               PORTA
#define PWM_0_PIN_CH1                2
#define PWM_0_FTMCHAN_CH1            7
#define PWM_0_PIN_AF_CH1             3
/* Arduino Connector D6 */
#define PWM_0_PORT_CH2               PORTC
#define PWM_0_PIN_CH2                2
#define PWM_0_FTMCHAN_CH2            1
#define PWM_0_PIN_AF_CH2             4
/* Arduino Connector D7 */
#define PWM_0_PORT_CH3               PORTC
#define PWM_0_PIN_CH3                3
#define PWM_0_FTMCHAN_CH3            2
#define PWM_0_PIN_AF_CH3             4
/** @} */


/**
* @name SPI configuration
* @{
*/
#define SPI_NUMOF                    (1U)
#define SPI_0_EN                     1
#define SPI_IRQ_PRIO                 1
#define KINETIS_SPI_USE_HW_CS        1

/* SPI 0 device config */
#define SPI_0_DEV                    SPI0
#define SPI_0_INDEX                  0
#define SPI_0_CTAS                   0
#define SPI_0_CLKEN()                (SIM->SCGC6 |= (SIM_SCGC6_SPI0_MASK))
#define SPI_0_CLKDIS()               (SIM->SCGC6 &= ~(SIM_SCGC6_SPI0_MASK))
#define SPI_0_IRQ                    SPI0_IRQn
#define SPI_0_IRQ_HANDLER            isr_spi0
#define SPI_0_FREQ                   CLOCK_CORECLOCK

/* SPI 0 pin configuration */
#define SPI_0_PORT                   PORTD
#define SPI_0_PORT_CLKEN()           (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define SPI_0_AF                     2

#define SPI_0_PCS0_PIN               0
#define SPI_0_SCK_PIN                1
#define SPI_0_SOUT_PIN               2
#define SPI_0_SIN_PIN                3

#define SPI_0_PCS0_ACTIVE_LOW        1
/** @} */


/**
* @name I2C configuration
* @{
*/
#define I2C_NUMOF                    (1U)
#define I2C_CLK                      CLOCK_CORECLOCK
#define I2C_0_EN                     1
#define I2C_IRQ_PRIO                 1
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
#define I2C_0_DEV                    I2C0
#define I2C_0_CLKEN()                (SIM->SCGC4 |= (SIM_SCGC4_I2C0_MASK))
#define I2C_0_CLKDIS()               (SIM->SCGC4 &= ~(SIM_SCGC4_I2C0_MASK))
#define I2C_0_IRQ                    I2C0_IRQn
#define I2C_0_IRQ_HANDLER            isr_i2c0
/* I2C 0 pin configuration */
#define I2C_0_PORT                   PORTE
#define I2C_0_PORT_CLKEN()           (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define I2C_0_PIN_AF                 5
#define I2C_0_SDA_PIN                25
#define I2C_0_SCL_PIN                24
#define I2C_0_PORT_CFG               (PORT_PCR_MUX(I2C_0_PIN_AF) | PORT_PCR_ODE_MASK)
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_IRQ_PRIO                CPU_DEFAULT_IRQ_PRIO
/** @} */

/**
* @name RTT and RTC configuration
* @{
*/
#define RTT_NUMOF                    (1U)
#define RTC_NUMOF                    (1U)
#define RTT_DEV                      RTC
#define RTT_IRQ                      RTC_IRQn
#define RTT_IRQ_PRIO                 10
#define RTT_UNLOCK()                 (SIM->SCGC6 |= (SIM_SCGC6_RTC_MASK))
#define RTT_ISR                      isr_rtc
#define RTT_FREQUENCY                (1)
#define RTT_MAX_VALUE                (0xffffffff)
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF                 (1U)
#define KINETIS_RNGA                 RNG
#define RANDOM_CLKEN()               (SIM->SCGC6 |= (1 << 9))
#define RANDOM_CLKDIS()              (SIM->SCGC6 &= ~(1 << 9))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
