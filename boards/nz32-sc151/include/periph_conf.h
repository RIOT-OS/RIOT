/*
 * Copyright (C) 2016 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     nz32-sc151
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the limifrog-v1 board
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 **/
#define CLOCK_HSI           (16000000U)         /* internal oscillator */
#define CLOCK_CORECLOCK     (32000000U)         /* desired core clock frequency */

/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := HSI / CLOCK_PLL_DIV * CLOCK_PLL_MUL */
#define CLOCK_PLL_DIV       RCC_CFGR_PLLDIV2
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMUL4
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 32MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 32MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV1     /* APB1 clock -> 32MHz */
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @brief   ADC configuration
 * @{
 */
#define ADC_CONFIG {            \
    { GPIO_PIN(PORT_C, 0), 10 },\
    { GPIO_PIN(PORT_C, 1), 11 },\
    { GPIO_PIN(PORT_C, 2), 12 },\
    { GPIO_PIN(PORT_C, 5), 15},\
    { GPIO_UNDEF, ADC_VREF_CHANNEL}, \
    { GPIO_UNDEF, ADC_TEMPERATURE_CHANNEL}, \
}

#define ADC_NUMOF           (6)
/** @} */

/**
 * @brief   DAC configuration
 * @{
 */
#define DAC_CONFIG {            \
    { GPIO_PIN(PORT_A, 4), 1},  \
    { GPIO_PIN(PORT_A, 5), 2},  \
}

#define DAC_NUMOF           (2)
/** @} */

/**
 * @brief Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    },
    {
        .dev      = TIM3,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .bus      = APB1,
        .irqn     = TIM3_IRQn
    }
};

/* interrupt routines */
#define TIMER_0_ISR         (isr_tim5)
#define TIMER_1_ISR         (isr_tim3)
/* number of defined timers */
#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = USART3,
        .rcc_mask = RCC_APB1ENR_USART3EN,
        .rx_pin   = GPIO_PIN(PORT_B, 11),
        .tx_pin   = GPIO_PIN(PORT_B, 10),
        .rx_af    = GPIO_AF7,
        .tx_af    = GPIO_AF7,
        .bus      = APB1,
        .irqn     = USART3_IRQn
    }/*,
    {
        .dev      = USART2,
        .rcc_mask = RCC_APB1ENR_USART2EN,
        .rx_pin   = GPIO_PIN(PORT_A, 3),
        .tx_pin   = GPIO_PIN(PORT_A, 2),
        .rx_af    = GPIO_AF7,
        .tx_af    = GPIO_AF7,
        .bus      = APB1,
        .irqn     = USART2_IRQn
    },
    {
        .dev      = USART1,
        .rcc_mask = RCC_APB2ENR_USART1EN,
        .rx_pin   = GPIO_PIN(PORT_A, 10),
        .tx_pin   = GPIO_PIN(PORT_A, 9),
        .rx_af    = GPIO_AF7,
        .tx_af    = GPIO_AF7,
        .bus      = APB2,
        .irqn     = USART1_IRQn
    }*/
};


#define UART_0_ISR          (isr_usart3)
#define UART_1_ISR          (isr_usart2)
#define UART_2_ISR          (isr_usart1)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))

/**
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (SPI_0_EN + SPI_1_EN)
#define SPI_0_EN            1
#define SPI_1_EN            0

/* SPI 0 device configuration */
#define SPI_0_DEV           SPI1  
#define SPI_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_0_IRQ           SPI1_IRQn
#define SPI_0_ISR           isr_spi1
/* SPI 0 pin configuration */
#define SPI_0_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define SPI_0_PORT          GPIOB
#define SPI_0_PIN_SCK       3
#define SPI_0_PIN_MOSI      5
#define SPI_0_PIN_MISO      4
#define SPI_0_PIN_AF        5

/* SPI 1 device configuration */
#define SPI_1_DEV           SPI2          
#define SPI_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#define SPI_1_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN))
#define SPI_1_IRQ           SPI2_IRQn
#define SPI_1_ISR           isr_spi2
/* SPI 1 pin configuration */
#define SPI_1_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define SPI_1_PORT          GPIOB
#define SPI_1_PIN_SCK       13
#define SPI_1_PIN_MOSI      15
#define SPI_1_PIN_MISO      14
#define SPI_1_PIN_AF        5
/** @} */

/**
 * @name I2C configuration
  * @{
 */
#define I2C_NUMOF           (I2C_0_EN + I2C_1_EN)
#define I2C_0_EN            0
#define I2C_1_EN            0
#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (36000000U)          // Configurable from 2MHz to 50Mhz, steps of 2Mhz     

/* I2C 0 device configuration */
#define I2C_0_EVT_ISR       isr_i2c1_ev
#define I2C_0_ERR_ISR       isr_i2c1_er

/* I2C 1 device configuration */
#define I2C_1_EVT_ISR       isr_i2c2_ev
#define I2C_1_ERR_ISR       isr_i2c2_er

static const i2c_conf_t i2c_config[] = {
    /* device, port, scl-, sda-pin-number, I2C-AF, ER-IRQn, EV-IRQn */
    {I2C1, GPIO_PIN(PORT_B,  8), GPIO_PIN(PORT_B,  9),
     GPIO_OD_PU, GPIO_AF4, I2C1_ER_IRQn, I2C1_EV_IRQn},
    {I2C2, GPIO_PIN(PORT_B, 10), GPIO_PIN(PORT_B, 11),
     GPIO_OD_PU, GPIO_AF4, I2C2_ER_IRQn, I2C2_EV_IRQn},
};

/** @} */

/**
 * @name RTC configuration
 * @{
 */
#define RTC_NUMOF           (1U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
