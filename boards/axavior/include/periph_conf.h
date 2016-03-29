/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_axavior
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the AxAvior module
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
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
#define CLOCK_HSI           (8000000U)              /* frequency of external oscillator */
#define CLOCK_CORECLOCK     (36000000U)             /* targeted core clock frequency */
/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := CLOCK_SOURCE / PLL_DIV * PLL_MUL */
#define CLOCK_PLL_DIV       (0)
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMULL9
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 36MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 36MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV1     /* APB1 clock -> 36MHz */
/* resulting bus clocks */
#define CLOCK_APB1          (CLOCK_CORECLOCK)
#define CLOCK_APB2          (CLOCK_CORECLOCK)
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_1
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_CONFIG          {       \
    { GPIO_UNDEF        , 0, 16 },  \
    { GPIO_UNDEF        , 0, 17 } }

#define ADC_NUMOF           (3)
/** @} */

/**
 * @name DAC configuration
 * @{
 */
#define DAC_CONFIG          {       \
    { GPIO_PIN(PORT_A, 5), 1 } }

#define DAC_NUMOF           (1)
/** @} */

/**
 * @brief Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    /* device, APB bus, rcc_bit */
    { TIM4, APB1, RCC_APB1ENR_TIM4EN, TIM4_IRQn },
    { TIM5, APB1, RCC_APB1ENR_TIM5EN, TIM5_IRQn }
};

#define TIMER_0_ISR         isr_tim4
#define TIMER_1_ISR         isr_tim5

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev     = USART3,
        .rx_pin  = GPIO_PIN(PORT_B,11),
        .tx_pin  = GPIO_PIN(PORT_B,10),
        .rcc_pin = RCC_APB1ENR_USART3EN,
        .bus     = APB1,
        .irqn    = USART3_IRQn
    },
    {
        .dev     = USART1,
        .rx_pin  = GPIO_PIN(PORT_A,10),
        .tx_pin  = GPIO_PIN(PORT_A,9),
        .rcc_pin = RCC_APB2ENR_USART1EN,
        .bus     = APB2,
        .irqn    = USART1_IRQn
    }
};

#define UART_0_ISR          isr_usart3
#define UART_1_ISR          isr_usart1

#define UART_1_RTS_PIN      GPIO_PIN(PORT_A,12)
#define UART_1_CTS_PIN      GPIO_PIN(PORT_A,11)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN            1

/* SPI 0 device configuration */
#define SPI_0_DEV           SPI2
#define SPI_0_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#define SPI_0_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN))
#define SPI_0_BUS_DIV       1   /* 1 -> SPI runs with full CPU clock, 0 -> half CPU clock */
/* SPI 0 pin configuration */
#define SPI_0_CLK_PIN       GPIO_PIN(PORT_B,13)
#define SPI_0_MOSI_PIN      GPIO_PIN(PORT_B,15)
#define SPI_0_MISO_PIN      GPIO_PIN(PORT_B,14)

/* SPI 1 device configuration */
#define SPI_1_DEV           SPI1
#define SPI_1_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_1_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_1_BUS_DIV       1   /* 1 -> SPI runs with full CPU clock, 0 -> half CPU clock */
/* SPI 1 pin configuration */
#define SPI_1_CLK_PIN       GPIO_PIN(PORT_A,5)
#define SPI_1_MOSI_PIN      GPIO_PIN(PORT_A,7)
#define SPI_1_MISO_PIN      GPIO_PIN(PORT_A,6)
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
#define I2C_0_SCL_PIN       GPIO_PIN(PORT_B,6)
#define I2C_0_SDA_PIN       GPIO_PIN(PORT_B,7)
/** @} */

/**
 * @brief PWM configuration
 * @{
 */
#define PWM_EN              1

static const pwm_conf_t pwm_config[] =
{
    { TIM3, AFIO_MAPR_TIM3_REMAP_PARTIALREMAP, 1, 1, {
            { GPIO_PIN(PORT_B, 4), 0 },
            { GPIO_PIN(PORT_B, 5), 1 },
            { GPIO_PIN(PORT_B, 0), 2 },
            { GPIO_PIN(PORT_B, 1), 3 },
    } }
};
#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
