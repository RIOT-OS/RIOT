/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_pba-d-01-kw2x
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the phyWAVE-KW22 Board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
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
#define KINETIS_MCG_USE_PLL               1
#define KINETIS_MCG_DCO_RANGE             (24000000U)
#define KINETIS_MCG_ERC_OSCILLATOR        0
#define KINETIS_MCG_ERC_FRDIV             2
#define KINETIS_MCG_ERC_RANGE             1
#define KINETIS_MCG_ERC_FREQ              4000000
#define KINETIS_MCG_PLL_PRDIV             1
#define KINETIS_MCG_PLL_VDIV0             0
#define KINETIS_MCG_PLL_FREQ              48000000

#define CLOCK_CORECLOCK                   KINETIS_MCG_PLL_FREQ
/** @} */


/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF                       (1U)
#define TIMER_0_EN                        1
#define TIMER_1_EN                        0
#define TIMER_IRQ_PRIO                    1
#define TIMER_DEV                         PIT
#define TIMER_MAX_VALUE                   (0xffffffff)
#define TIMER_CLOCK                       CLOCK_CORECLOCK
#define TIMER_CLKEN()                     (SIM->SCGC6 |= (SIM_SCGC6_PIT_MASK))

/* Timer 0 configuration */
#define TIMER_0_PRESCALER_CH              0
#define TIMER_0_COUNTER_CH                1
#define TIMER_0_ISR                       isr_pit1
#define TIMER_0_IRQ_CHAN                  PIT1_IRQn

/* Timer 1 configuration */
#define TIMER_1_PRESCALER_CH              2
#define TIMER_1_COUNTER_CH                3
#define TIMER_1_ISR                       isr_pit3
#define TIMER_1_IRQ_CHAN                  PIT3_IRQn
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
#define KINETIS_UART        UART_Type
#define UART_0_DEV          UART2
#define UART_0_CLKEN()      (SIM->SCGC4 |= (SIM_SCGC4_UART2_MASK))
#define UART_0_CLK          UART_CLK
#define UART_0_IRQ_CHAN     UART2_RX_TX_IRQn
#define UART_0_ISR          isr_uart2_rx_tx
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define UART_0_PORT         PORTD
#define UART_0_RX_PIN       2
#define UART_0_TX_PIN       3
#define UART_0_AF           3

/* UART 1 device configuration */
#define UART_1_DEV          UART0
#define UART_1_CLKEN()      (SIM->SCGC4 |= (SIM_SCGC4_UART0_MASK))
#define UART_1_CLK          UART_CLK
#define UART_1_IRQ_CHAN     UART0_RX_TX_IRQn
#define UART_1_ISR          isr_uart0_rx_tx
/* UART 1 pin configuration */
#define UART_1_PORT_CLKEN() (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define UART_1_PORT         PORTD
#define UART_1_RX_PIN       6
#define UART_1_TX_PIN       7
#define UART_1_AF           3
/** @} */


/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (1U)
#define ADC_0_EN            1
#define ADC_MAX_CHANNELS    6

/* ADC 0 configuration */
#define ADC_0_DEV           ADC0
#define ADC_0_MODULE_CLOCK  CLOCK_CORECLOCK
#define ADC_0_CHANNELS      6
#define ADC_0_CLKEN()       (SIM->SCGC6 |= (SIM_SCGC6_ADC0_MASK))
#define ADC_0_CLKDIS()      (SIM->SCGC6 &= ~(SIM_SCGC6_ADC0_MASK))
#define ADC_0_PORT_CLKEN()  (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK))
/* ADC 0 channel 0 pin config */
#define ADC_0_CH0           1
#define ADC_0_CH0_PIN       2
#define ADC_0_CH0_PIN_AF    0
#define ADC_0_CH0_PORT      PORTE
/* ADC 0 channel 1 pin config */
#define ADC_0_CH1           1 /* PTE3 uses the same ADC_CH as PTE2, in single channel mode only one of them can be selected */
#define ADC_0_CH1_PIN       3
#define ADC_0_CH1_PIN_AF    0
#define ADC_0_CH1_PORT      PORTE
/* ADC 0 channel 2 pin config */
#define ADC_0_CH2           22
#define ADC_0_CH2_PIN       7
#define ADC_0_CH2_PIN_AF    0
#define ADC_0_CH2_PORT      PORTD
/* ADC 0 channel 3 pin config */
#define ADC_0_CH3           6
#define ADC_0_CH3_PIN       5
#define ADC_0_CH3_PIN_AF    0
#define ADC_0_CH3_PORT      PORTD
/* ADC 0 channel 4 pin config */
#define ADC_0_CH4           10
#define ADC_0_CH4_PIN       0
#define ADC_0_CH4_PIN_AF    0
#define ADC_0_CH4_PORT      PORTE
/* ADC 0 channel 5 pin config */
#define ADC_0_CH5           11
#define ADC_0_CH5_PIN       1
#define ADC_0_CH5_PIN_AF    0
#define ADC_0_CH5_PORT      PORTE
/** @} */


/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (1U)
#define PWM_0_EN            1
#define PWM_MAX_CHANNELS    4

/* PWM 0 device configuration */
#define PWM_0_DEV           FTM0
#define PWM_0_CHANNELS      3
#define PWM_0_CLK           (48e6)
#define PWM_0_CLKEN()       (SIM->SCGC6 |= (SIM_SCGC6_FTM0_MASK))
#define PWM_0_CLKDIS()      (SIM->SCGC6 &= ~(SIM_SCGC6_FTM0_MASK))
/* PWM 0 pin configuration */
#define PWM_0_PORT_CLKEN()  (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTA_MASK))

#define PWM_0_PIN_CH0       4
#define PWM_0_FTMCHAN_CH0   1
#define PWM_0_PORT_CH0      PORTA
#define PWM_0_PIN_AF_CH0    3

#define PWM_0_PIN_CH1       4
#define PWM_0_FTMCHAN_CH1   4
#define PWM_0_PORT_CH1      PORTD
#define PWM_0_PIN_AF_CH1    4

#define PWM_0_PIN_CH2       6
#define PWM_0_FTMCHAN_CH2   6
#define PWM_0_PORT_CH2      PORTD
#define PWM_0_PIN_AF_CH2    4

#define PWM_0_PIN_CH3       1
#define PWM_0_FTMCHAN_CH3   1
#define PWM_0_PORT_CH3      PORTA
#define PWM_0_PIN_AF_CH3    3
/** @} */


/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF               (2U)
#define SPI_0_EN                1
#define SPI_1_EN                1
#define SPI_IRQ_PRIO            1
#define KINETIS_SPI_USE_HW_CS   1

/* SPI 0 device config */
#define SPI_0_DEV               SPI0
#define SPI_0_INDEX             0
#define SPI_0_CTAS              0
#define SPI_0_CLKEN()           (SIM->SCGC6 |= (SIM_SCGC6_SPI0_MASK))
#define SPI_0_CLKDIS()          (SIM->SCGC6 &= ~(SIM_SCGC6_SPI0_MASK))
#define SPI_0_IRQ               SPI0_IRQn
#define SPI_0_IRQ_HANDLER       isr_spi0
#define SPI_0_FREQ              (48e6)

/* SPI 0 pin configuration */
#define SPI_0_PORT              PORTC
#define SPI_0_PORT_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK))
#define SPI_0_AF                2

#define SPI_0_PCS0_PIN          4
#define SPI_0_SCK_PIN           5
#define SPI_0_SOUT_PIN          6
#define SPI_0_SIN_PIN           7

#define SPI_0_PCS0_ACTIVE_LOW   1

/* SPI 1 device config */
#define SPI_1_DEV               SPI1
#define SPI_1_INDEX             1
#define SPI_1_CTAS              0
#define SPI_1_CLKEN()           (SIM->SCGC6 |= (SIM_SCGC6_SPI1_MASK))
#define SPI_1_CLKDIS()          (SIM->SCGC6 &= ~(SIM_SCGC6_SPI1_MASK))
#define SPI_1_IRQ               SPI1_IRQn
#define SPI_1_IRQ_HANDLER       isr_spi1
#define SPI_1_FREQ              (48e6)

/* SPI 1 pin1configuration */
#define SPI_1_PORT              KW2XDRF_PORT
#define SPI_1_PORT_CLKEN()      KW2XDRF_PORT_CLKEN();
#define SPI_1_AF                KW2XDRF_PIN_AF

#define SPI_1_PCS0_PIN          KW2XDRF_PCS0_PIN
#define SPI_1_SCK_PIN           KW2XDRF_SCK_PIN
#define SPI_1_SOUT_PIN          KW2XDRF_SOUT_PIN
#define SPI_1_SIN_PIN           KW2XDRF_SIN_PIN

#define SPI_1_PCS0_ACTIVE_LOW   1

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
#define I2C_0_PORT              PORTE
#define I2C_0_PORT_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define I2C_0_PIN_AF            6
#define I2C_0_SDA_PIN           0
#define I2C_0_SCL_PIN           1
#define I2C_0_PORT_CFG          (PORT_PCR_MUX(I2C_0_PIN_AF) | PORT_PCR_ODE_MASK)

/** @} */


/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          8
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           0
#define GPIO_7_EN           0
#define GPIO_8_EN           0        /* I2CSDA */
#define GPIO_9_EN           0        /* I2CSCL */
#define GPIO_10_EN          0
#define GPIO_11_EN          0
#define GPIO_12_EN          0
#define GPIO_13_EN          0        /* USB VOUT 3V3 */
#define GPIO_14_EN          0        /* USB VREGIN */
#define GPIO_15_EN          0
#define GPIO_16_EN          0
#define GPIO_17_EN          0
#define GPIO_18_EN          0
#define GPIO_19_EN          0        /* SPI0_CS0 */
#define GPIO_20_EN          0        /* SPI0_CLK */
#define GPIO_21_EN          0        /* SPI0_MOSI */
#define GPIO_22_EN          0        /* SPI0_MISO */
#define GPIO_23_EN          1        /* KW2XRF INT */
#define GPIO_24_EN          1        /* KW2XRF CS */
#define GPIO_IRQ_PRIO       1
#define ISR_PORT_A          isr_porta
#define ISR_PORT_B          isr_portb
#define ISR_PORT_C          isr_portc
#define ISR_PORT_D          isr_portd

/* GPIO channel 0 config */
#define GPIO_0_DEV          GPIOD   /* DIO9; extension connecotr D9; LED_G */
#define GPIO_0_PORT         PORTD
#define GPIO_0_PORT_BASE    PORTD_BASE
#define GPIO_0_PIN          4
#define GPIO_0_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define GPIO_0_IRQ          PORTD_IRQn
/* GPIO channel 1 config */
#define GPIO_1_DEV          GPIOD   /* DIO11; extension connecotr D5; LED_R */
#define GPIO_1_PORT         PORTD
#define GPIO_1_PORT_BASE    PORTD_BASE
#define GPIO_1_PIN          6
#define GPIO_1_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define GPIO_1_IRQ          PORTD_IRQn
/* GPIO channel 2 config */
#define GPIO_2_DEV          GPIOA   /* DIO27; extension connecotr D3; LED_B */
#define GPIO_2_PORT         PORTA
#define GPIO_2_PORT_BASE    PORTA_BASE
#define GPIO_2_PIN          4
#define GPIO_2_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK))
#define GPIO_2_IRQ          PORTA_IRQn
/* GPIO channel 3 config */
#define GPIO_3_DEV          GPIOD   /* DIO06; extension connecotr --; User_Button */
#define GPIO_3_PORT         PORTD
#define GPIO_3_PORT_BASE    PORTD_BASE
#define GPIO_3_PIN          1
#define GPIO_3_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define GPIO_3_IRQ          PORTD_IRQn
/* GPIO channel 4 config */
#define GPIO_4_DEV          GPIOE   /* DIO17; extension connecotr D2 */
#define GPIO_4_PORT         PORTE
#define GPIO_4_PORT_BASE    PORTE_BASE
#define GPIO_4_PIN          4
#define GPIO_4_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define GPIO_4_IRQ          PORTE_IRQn
/* GPIO channel 5 config */
#define GPIO_5_DEV          GPIOA   /* DIO29; extension connecotr D4 */
#define GPIO_5_PORT         PORTA
#define GPIO_5_PORT_BASE    PORTA_BASE
#define GPIO_5_PIN          19
#define GPIO_5_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK))
#define GPIO_5_IRQ          PORTA_IRQn
/* GPIO channel 6 config */
#define GPIO_6_DEV          GPIOD    /* DIO10; extension connecotr A3 */
#define GPIO_6_PORT         PORTD
#define GPIO_6_PORT_BASE    PORTD_BASE
#define GPIO_6_PIN          5
#define GPIO_6_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define GPIO_6_IRQ          PORTD_IRQn
/* GPIO channel 7 config */
#define GPIO_7_DEV          GPIOD    /* DIO12; extension connecotr A2 */
#define GPIO_7_PORT         PORTD
#define GPIO_7_PORT_BASE    PORTD_BASE
#define GPIO_7_PIN          7
#define GPIO_7_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define GPIO_7_IRQ          PORTD_IRQn
/* GPIO channel 8 config */
#define GPIO_8_DEV          GPIOE    /* DIO13; extension connecotr A4;  I2CSDA */
#define GPIO_8_PORT         PORTE
#define GPIO_8_PORT_BASE    PORTE_BASE
#define GPIO_8_PIN          0
#define GPIO_8_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define GPIO_8_IRQ          PORTE_IRQn
/* GPIO channel 9 config */
#define GPIO_9_DEV          GPIOE    /* DIO14; extension connecotr A5;  I2CSCL */
#define GPIO_9_PORT         PORTE
#define GPIO_9_PORT_BASE    PORTE_BASE
#define GPIO_9_PIN          1
#define GPIO_9_CLKEN()      (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define GPIO_9_IRQ          PORTE_IRQn
/* GPIO channel 10 config */
#define GPIO_10_DEV         GPIOE   /* DIO15; extension connecotr A0 */
#define GPIO_10_PORT        PORTE
#define GPIO_10_PORT_BASE   PORTE_BASE
#define GPIO_10_PIN         2
#define GPIO_10_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define GPIO_10_IRQ         PORTE_IRQn
/* GPIO channel 11 config */
#define GPIO_11_DEV         GPIOE   /* DIO16; extension connecotr A1 */
#define GPIO_11_PORT        PORTE
#define GPIO_11_PORT_BASE   PORTE_BASE
#define GPIO_11_PIN         3
#define GPIO_11_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define GPIO_11_IRQ         PORTE_IRQn
/* GPIO channel 12 config */
#define GPIO_12_DEV         GPIOD    /* DIO7; extension connecotr D0;  UART2_RX */
#define GPIO_12_PORT        PORTD
#define GPIO_12_PORT_BASE   PORTD_BASE
#define GPIO_12_PIN         2
#define GPIO_12_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define GPIO_12_IRQ         PORTD_IRQn
/* GPIO channel 13 config */
#define GPIO_13_DEV         GPIOE   /* DIO20; extension connecotr D14, USB OUT3V3 */
#define GPIO_13_PORT        PORTE
#define GPIO_13_PORT_BASE   PORTE_BASE
#define GPIO_13_PIN         18
#define GPIO_13_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define GPIO_13_IRQ         PORTE_IRQn
/* GPIO channel 14 config */
#define GPIO_14_DEV         GPIOE   /* DIO21; extension connecotr D15, USB VREGIN */
#define GPIO_14_PORT        PORTE
#define GPIO_14_PORT_BASE   PORTE_BASE
#define GPIO_14_PIN         19
#define GPIO_14_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK))
#define GPIO_14_IRQ         PORTE_IRQn
/* GPIO channel 15 config */
#define GPIO_15_DEV         GPIOA   /* DIO24; extension connecotr D7 */
#define GPIO_15_PORT        PORTA
#define GPIO_15_PORT_BASE   PORTA_BASE
#define GPIO_15_PIN         1
#define GPIO_15_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK))
#define GPIO_15_IRQ         PORTA_IRQn
/* GPIO channel 16 config */
#define GPIO_16_DEV         GPIOA   /* DIO25; extension connecotr D6 */
#define GPIO_16_PORT        PORTA
#define GPIO_16_PORT_BASE   PORTA_BASE
#define GPIO_16_PIN         2
#define GPIO_16_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK))
#define GPIO_16_IRQ         PORTA_IRQn
/* GPIO channel 17 config */
#define GPIO_17_DEV         GPIOA   /* DIO28; extension connecotr D8 */
#define GPIO_17_PORT        PORTA
#define GPIO_17_PORT_BASE   PORTA_BASE
#define GPIO_17_PIN         18
#define GPIO_17_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK))
#define GPIO_17_IRQ         PORTA_IRQn
/* GPIO channel 18 config */
#define GPIO_18_DEV         GPIOD    /* DIO8; extension connecotr D1;  UART2_TX */
#define GPIO_18_PORT        PORTD
#define GPIO_18_PORT_BASE   PORTD_BASE
#define GPIO_18_PIN         3
#define GPIO_18_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK))
#define GPIO_18_IRQ         PORTD_IRQn
/* GPIO channel 19 config */
#define GPIO_19_DEV         GPIOC    /* DIO2; extension connecotr D10; SPI0_CS0 */
#define GPIO_19_PORT        PORTC
#define GPIO_19_PORT_BASE   PORTC_BASE
#define GPIO_19_PIN         4
#define GPIO_19_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK))
#define GPIO_19_IRQ         PORTC_IRQn
/* GPIO channel 20 config */
#define GPIO_20_DEV         GPIOC    /* DIO3; extension connecotr D13; SPI0_CLK */
#define GPIO_20_PORT        PORTC
#define GPIO_20_PORT_BASE   PORTC_BASE
#define GPIO_20_PIN         5
#define GPIO_20_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK))
#define GPIO_20_IRQ         PORTC_IRQn
/* GPIO channel 21 config */
#define GPIO_21_DEV         GPIOC    /* DIO4; extension connecotr D11;  SPI0_MOSI */
#define GPIO_21_PORT        PORTC
#define GPIO_21_PORT_BASE   PORTC_BASE
#define GPIO_21_PIN         6
#define GPIO_21_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK))
#define GPIO_21_IRQ         PORTC_IRQn
/* GPIO channel 22 config */
#define GPIO_22_DEV         GPIOC    /* DIO5; extension connecotr D12;  SPI0_MISO */
#define GPIO_22_PORT        PORTC
#define GPIO_22_PORT_BASE   PORTC_BASE
#define GPIO_22_PIN         7
#define GPIO_22_CLKEN()     (SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK))
#define GPIO_22_IRQ         PORTC_IRQn
/* GPIO channel 23 config */
#define GPIO_23_DEV         KW2XDRF_GPIO
#define GPIO_23_PORT        KW2XDRF_PORT
#define GPIO_23_PORT_BASE   KW2XDRF_PORT_BASE
#define GPIO_23_PIN         KW2XDRF_IRQ_PIN
#define GPIO_23_CLKEN()     KW2XDRF_PORT_CLKEN()
#define GPIO_23_IRQ         KW2XDRF_PORT_IRQn
#define GPIO_KW2XDRF        GPIO_23
/* GPIO channel 24 config */
#define GPIO_24_DEV         KW2XDRF_GPIO
#define GPIO_24_PORT        KW2XDRF_PORT
#define GPIO_24_PORT_BASE   KW2XDRF_PORT_BASE
#define GPIO_24_PIN         KW2XDRF_PCS0_PIN
#define GPIO_24_CLKEN()     KW2XDRF_PORT_CLKEN()
#define GPIO_24_IRQ         KW2XDRF_PORT_IRQn
#define KW2XRF_CS_GPIO      GPIO_24
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

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF         (1U)
#define KINETIS_RNGA         RNG
#define RANDOM_CLKEN()       (SIM->SCGC6 |= (1 << 9))
#define RANDOM_CLKDIS()      (SIM->SCGC6 &= ~(1 << 9))

/** @} */

/**
 * @name Radio configuration (kw2xrf)
 * @{
 */
#define KW2XRF_SHARED_SPI       0
#define KW2XRF_SPI              SPI_1
#define KW2XRF_SPI_SPEED        SPI_SPEED_10MHZ

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
