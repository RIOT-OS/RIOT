/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_arduino-due
 * @{
 *
 * @file        periph_conf.h
 * @brief       Peripheral MCU configuration for the Arduino Due board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#include "registers/regsepit.h"
#include "registers/regsgpt.h"
#include "registers/regsuart.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

#define STDIO_UART					HW_UART2

// This epit-instance is used for the hal_delay-function:
#define SYSTIMER						HW_EPIT2

/**
 * @name Timer peripheral configuration
 * @{
 */
#define HWTIMER_CLKSRC			CLKSRC_IPG_CLK
#define HWTIMER_PRESCALE		(get_main_clock(IPG_CLK) / 1000000) // is this 12 bits big?

#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1

// This epit-instance is used for the hal_delay-function:
#define TIMER_2_EN          0
/* The EPIT2 is used for the hal_delay-function. See imx6's hwtimer_cpu.h */
#define SYSTIMER						HW_EPIT2

#define HWTIMER_CLKSRC		CLKSRC_IPG_CLK
#define HWTIMER_PRESCALE 	(get_main_clock(IPG_CLK) / 1000000) // is this 12 bits long?

/* Timer 0 configuration */
#define TIMER_0_ISEPIT			0
#define TIMER_0_CHANNELS    3
#define TIMER_0_MAX_VALUE   (0xffffffff)

/* Timer 1 configuration */
#define TIMER_1_EPITDEV     HW_EPIT1
#define TIMER_1_ISEPIT			1
#define TIMER_1_CHANNELS    1
#define TIMER_1_MAX_VALUE   (0xffffffff)

/* Timer 2 configuration */
#define TIMER_2_EPITDEV     HW_EPIT2
#define TIMER_2_ISEPIT			1
#define TIMER_2_CHANNELS    1
#define TIMER_2_MAX_VALUE   (0xffffffff)

#define TIMER_0_OFFSET			0
#define TIMER_1_OFFSET			(TIMER_0_EN * TIMER_0_CHANNELS)
#define TIMER_2_OFFSET			(TIMER_1_OFFSET + (TIMER_1_EN * TIMER_1_CHANNELS))
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (0U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_2_EN           0
#define UART_3_EN           0
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          UART
#define UART_0_IRQ          UART_IRQn
#define UART_0_ISR          isr_uart
/* UART 0 pin configuration */
#define UART_0_PORT         PIOA
#define UART_0_PINS         (PIO_PA8 | PIO_PA9)
#define UART_0_PORT_CLKEN() RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE)
#define UART_0_RX_AFCFG()   (GPIO_PinAFConfig(UART_0_PORT, GPIO_PinSource6, GPIO_AF_0))
#define UART_0_TX_AFCFG()   (GPIO_PinAFConfig(UART_0_PORT, GPIO_PinSource7, GPIO_AF_0))

/* UART 1 device configuration */
#define UART_1_DEV          USART2
#define UART_1_CLKEN()      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART2, ENABLE)
#define UART_1_IRQ          USART2_IRQn
#define UART_1_ISR          isr_usart2
/* UART 1 pin configuration */
#define UART_1_PORT         GPIOA
#define UART_1_PINS         (GPIO_Pin_2 | GPIO_Pin_3)
#define UART_1_PORT_CLKEN() RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE)
#define UART_1_RX_AFCFG()   GPIO_PinAFConfig(UART_1_PORT, GPIO_PinSource2, GPIO_AF_1)
#define UART_1_TX_AFCFG()   GPIO_PinAFConfig(UART_1_PORT, GPIO_PinSource3, GPIO_AF_1)
/** @} */


/**
 * @brief ADC configuration
 */
#define ADC_NUMOF           (0U)
#define ADC_0_EN            0
#define ADC_1_EN            0

/* ADC 0 configuration */
#define ADC_0_DEV           ADC1                                                    /* TODO */
#define ADC_0_SAMPLE_TIMER
/* ADC 0 channel 0 pin config */
#define ADC_0_C0_PORT
#define ADC_0_C0_PIN
#define ADC_0_C0_CLKEN()
#define ADC_0_C0_AFCFG()
/* ADC 0 channel 1 pin config */
#define ADC_0_C1_PORT
#define ADC_0_C1_PIN
#define ADC_0_C1_CLKEN()
#define ADC_0_C1_AFCFG()
/* ADC 0 channel 2 pin config */
#define ADC_0_C2_PORT
#define ADC_0_C2_PIN
#define ADC_0_C2_CLKEN()
#define ADC_0_C2_AFCFG()
/* ADC 0 channel 3 pin config */
#define ADC_0_C3_PORT
#define ADC_0_C3_PIN
#define ADC_0_C3_CLKEN()
#define ADC_0_C3_AFCFG()

/* ADC 0 configuration */
#define ADC_1_DEV           ADC2                                                    /* TODO */
#define ADC_1_SAMPLE_TIMER
/* ADC 0 channel 0 pin config */
#define ADC_1_C0_PORT
#define ADC_1_C0_PIN
#define ADC_1_C0_CLKEN()
#define ADC_1_C0_AFCFG()
/* ADC 0 channel 1 pin config */
#define ADC_1_C1_PORT
#define ADC_1_C1_PIN
#define ADC_1_C1_CLKEN()
#define ADC_1_C1_AFCFG()
/* ADC 0 channel 2 pin config */
#define ADC_1_C2_PORT
#define ADC_1_C2_PIN
#define ADC_1_C2_CLKEN()
#define ADC_1_C2_AFCFG()
/* ADC 0 channel 3 pin config */
#define ADC_1_C3_PORT
#define ADC_1_C3_PIN
#define ADC_1_C3_CLKEN()
#define ADC_1_C3_AFCFG()


/**
 * @brief PWM configuration
 */
#define PWM_NUMOF           (0U)                                                    /* TODO */
#define PWM_0_EN            0
#define PWM_1_EN            0

/* PWM 0 device configuration */
#define PWM_0_DEV           TIM1
#define PWM_0_CHANNELS      4
/* PWM 0 pin configuration */
#define PWM_0_PORT
#define PWM_0_PINS
#define PWM_0_PORT_CLKEN()
#define PWM_0_CH1_AFCFG()
#define PWM_0_CH2_AFCFG()
#define PWM_0_CH3_AFCFG()
#define PWM_0_CH4_AFCFG()

/* PWM 1 device configuration */
#define PWM_1_DEV           TIM3
#define PWM_1_CHANNELS      4
/* PWM 1 pin configuration */
#define PWM_1_PORT
#define PWM_1_PINS
#define PWM_1_PORT_CLKEN()
#define PWM_1_CH1_AFCFG()
#define PWM_1_CH2_AFCFG()
#define PWM_1_CH3_AFCFG()
#define PWM_1_CH4_AFCFG()



/**
 * @brief SPI configuration
 */
#define SPI_NUMOF           (0U)                                                    /* TODO */
#define SPI_0_EN            0
#define SPI_1_EN            0

/* SPI 0 device config */
#define SPI_0_DEV           SPI1
#define SPI_0_CLKEN()
#define SPI_0_IRQ           SPI1_IRQn
#define SPI_0_IRQ_HANDLER
#define SPI_0_IRQ_PRIO      1
/* SPI 1 pin configuration */
#define SPI_0_PORT
#define SPI_0_PINS          ()
#define SPI_1_PORT_CLKEN()
#define SPI_1_SCK_AFCFG()
#define SPI_1_MISO_AFCFG()
#define SPI_1_MOSI_AFCFG()

/* SPI 1 device config */
#define SPI_1_DEV           SPI2
#define SPI_1_CLKEN()
#define SPI_1_IRQ           SPI2_IRQn
#define SPI_1_IRQ_HANDLER
#define SPI_1_IRQ_PRIO      1
/* SPI 1 pin configuration */
#define SPI_1_PORT
#define SPI_1_PINS          ()
#define SPI_1_PORT_CLKEN()
#define SPI_1_SCK_AFCFG()
#define SPI_1_MISO_AFCFG()
#define SPI_1_MOSI_AFCFG()


/**
 * @brief I2C configuration
 */
#define I2C_NUMOF           (0U)                                                    /* TODO */
#define I2C_0_EN            0
#define I2C_0_EN            0

/* SPI 0 device configuration */
#define I2C_0_DEV           I2C1
#define I2C_0_CLKEN()
#define I2C_0_ISR           isr_i2c1
#define I2C_0_IRQ           I2C1_IRQn
#define I2C_0_IRQ_PRIO      1
/* SPI 0 pin configuration */
#define I2C_0_PORT          GPIOB
#define I2C_0_PINS          (GPIO_Pin_6 | GPIO_Pin_7)
#define I2C_0_PORT_CLKEN()
#define I2C_0_SCL_AFCFG()
#define I2C_0_SDA_AFCFG()

/* SPI 1 device configuration */
#define I2C_1_DEV           I2C2
#define I2C_1_CLKEN()
#define I2C_1_ISR           isr_i2c2
#define I2C_1_IRQ           I2C2_IRQn
#define I2C_1_IRQ_PRIO      1
/* SPI 1 pin configuration */
#define I2C_1_PORT          GPIOF
#define I2C_1_PINS          (GPIO_Pin_0 | GPIO_Pin_1)
#define I2C_1_PORT_CLKEN()
#define I2C_1_SCL_AFCFG()
#define I2C_1_SDA_AFCFG()


/**
 * @brief GPIO configuration
 * This really depends on how you chose to configure the IOMux
 */
#define GPIO_NUMOF          (0U)
#define GPIO_0_EN           0

#endif /* __PERIPH_CONF_H */
