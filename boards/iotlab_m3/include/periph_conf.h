/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_iotlab_m3
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the iotlab_m3 board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 **/
#define CLOCK_HSE           (16000000U)             /* frequency of external oscillator */
#define CLOCK_CORECLOCK     (72000000U)             /* targeted core clock frequency */
/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := HSE / PLL_HSE_DIV * PLL_HSE_MUL */
#define CLOCK_PLL_HSE_DIV   RCC_CFGR_PLLXTPRE_HSE_Div2
#define CLOCK_PLL_HSE_MUL   RCC_CFGR_PLLMULL9
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 72MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 72MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2     /* APB1 clock -> 36MHz */
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_2
/** @} */

/**
 * @brief Timer configuration
 * @{
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV_0       TIM2
#define TIMER_0_DEV_1       TIM3
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (72U)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN))
#define TIMER_0_ISR_0       isr_tim2
#define TIMER_0_ISR_1       isr_tim3
#define TIMER_0_IRQ_CHAN_0  TIM2_IRQn
#define TIMER_0_IRQ_CHAN_1  TIM3_IRQn
#define TIMER_0_IRQ_PRIO    1
#define TIMER_0_TRIG_SEL    TIM_SMCR_TS_0

/* Timer 1 configuration */
#define TIMER_1_DEV_0       TIM4
#define TIMER_1_DEV_1       TIM5
#define TIMER_1_CHANNELS    4
#define TIMER_1_PRESCALER   (72U)
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_CLKEN()     (RCC->APB1ENR |= (RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM5EN))
#define TIMER_1_ISR_0       isr_tim4
#define TIMER_1_ISR_1       isr_tim5
#define TIMER_1_IRQ_CHAN_0  TIM4_IRQn
#define TIMER_1_IRQ_CHAN_1  TIM5_IRQn
#define TIMER_1_IRQ_PRIO    1
#define TIMER_1_TRIG_SEL    TIM_SMCR_TS_1
/** @} */

/**
 * @brief UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART1
#define UART_0_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define UART_0_IRQ          USART1_IRQn
#define UART_0_ISR          isr_usart1
#define UART_0_BUS_FREQ     72000000
/* UART 0 pin configuration */
#define UART_0_RX_PIN       GPIO(PORT_A,10)
#define UART_0_TX_PIN       GPIO(PORT_A,9)

/* UART 1 device configuration */
#define UART_1_DEV          USART2
#define UART_1_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_1_IRQ          USART2_IRQn
#define UART_1_ISR          isr_usart2
#define UART_1_BUS_FREQ     36000000
/* UART 1 pin configuration */
#define UART_1_RX_PIN       GPIO(PORT_A,3)
#define UART_1_TX_PIN       GPIO(PORT_A,2)
/** @} */

/**
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)
#define SPI_0_EN            1

/* SPI 0 device configuration */
#define SPI_0_DEV               SPI1
#define SPI_0_CLKEN()           (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()          (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_0_BUS_DIV           1   /* 1 -> SPI runs with full CPU clock, 0 -> half CPU clock */
/* SPI 0 pin configuration */
#define SPI_0_CLK_PIN           GPIO(PORT_A,5)
#define SPI_0_MOSI_PIN          GPIO(PORT_A,7)
#define SPI_0_MISO_PIN          GPIO(PORT_A,6)
/** @} */

/**
 * @name Real time counter configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_IRQ_PRIO        1

#define RTT_DEV             RTC
#define RTT_IRQ             RTC_IRQn
#define RTT_ISR             isr_rtc
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (1)             /* in Hz */
#define RTT_PRESCALER       (0x7fff)        /* run with 1 Hz */
/** @} */

/**
 * @name I2C configuration
  * @{
 */
#define I2C_NUMOF           (1U)
#define I2C_0_EN            1
#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (36000000U)

/* I2C 0 device configuration */
#define I2C_0_DEV           I2C1
#define I2C_0_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN)
#define I2C_0_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))
#define I2C_0_EVT_IRQ       I2C1_EV_IRQn
#define I2C_0_EVT_ISR       isr_i2c1_ev
#define I2C_0_ERR_IRQ       I2C1_ER_IRQn
#define I2C_0_ERR_ISR       isr_i2c1_er
/* I2C 0 pin configuration */
#define I2C_0_SCL_PIN       GPIO(PORT_B,6)
#define I2C_0_SDA_PIN       GPIO(PORT_B,7)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
