/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_limifrog-v1
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the limifrog-v1 board
 *
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
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
/** @} */

/**
 * @brief Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    /* device, RCC bit, IRQ bit */
    {TIM5, 3, TIM5_IRQn},
};
/* interrupt routines */
#define TIMER_0_ISR         (isr_tim5)
/* number of defined timers */
#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief UART configuration
 * @{
 */
#define UART_NUMOF          (2U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART3
#define UART_0_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART3EN)
#define UART_0_CLK          (CLOCK_CORECLOCK)
#define UART_0_IRQ          USART3_IRQn
#define UART_0_ISR          isr_usart3
#define UART_0_BUS_FREQ     32000000
/* UART 0 pin configuration */
#define UART_0_RX_PIN       GPIO_PIN(PORT_C, 11)
#define UART_0_TX_PIN       GPIO_PIN(PORT_C, 10)
#define UART_0_AF           GPIO_AF7

/* UART 1 device configuration */
#define UART_1_DEV          USART1        /* Panasonic PAN1740 BLE module */
#define UART_1_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define UART_1_CLK          (CLOCK_CORECLOCK)
#define UART_1_IRQ          USART1_IRQn
#define UART_1_ISR          isr_usart1
#define UART_0_BUS_FREQ     32000000
/* UART 1 pin configuration */
#define UART_1_RX_PIN       GPIO_PIN(PORT_A, 10)
#define UART_1_TX_PIN       GPIO_PIN(PORT_A, 9)
#define UART_1_AF           GPIO_AF7
/** @} */

/**
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN            1

/* SPI 0 device configuration */
#define SPI_0_DEV           SPI1  /* Densitron DD-160128FC-1a OLED display; external pins */
#define SPI_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_0_IRQ           SPI1_IRQn
#define SPI_0_ISR           isr_spi1
/* SPI 0 pin configuration */
#define SPI_0_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define SPI_0_PORT          GPIOA
#define SPI_0_PIN_SCK       5
#define SPI_0_PIN_MOSI      7
#define SPI_0_PIN_MISO      6
#define SPI_0_PIN_AF        5

/* SPI 1 device configuration */
#define SPI_1_DEV           SPI3          /*  Adesto AT45DB641E data flash */
#define SPI_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_SPI3EN)
#define SPI_1_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI3EN))
#define SPI_1_IRQ           SPI3_IRQn
#define SPI_1_ISR           isr_spi3
/* SPI 1 pin configuration */
#define SPI_1_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define SPI_1_PORT          GPIOB
#define SPI_1_PIN_SCK       3
#define SPI_1_PIN_MOSI      5
#define SPI_1_PIN_MISO      4
#define SPI_1_PIN_AF        6
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF           (2U)
#define I2C_0_EN            1
#define I2C_1_EN            1

static const i2c_conf_t i2c_cfg[] = {
    /* Device, SCL-, SDA-Pin, AF, Event-, Error-IRQ, Clock enable */
    {I2C1, GPIO_PIN(PORT_B, 8), GPIO_PIN(PORT_B, 9), 4,
     I2C1_EV_IRQn, I2C1_ER_IRQn, RCC_APB1ENR_I2C1EN},
    {I2C2, GPIO_PIN(PORT_B, 10), GPIO_PIN(PORT_B, 11), 4,
     I2C2_EV_IRQn, I2C2_ER_IRQn, RCC_APB1ENR_I2C2EN},
};

#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (36000000U)

/* I2C 0 device configuration */
#define I2C_0_EVT_ISR       isr_i2c1_ev
#define I2C_0_ERR_ISR       isr_i2c1_er

/* I2C 1 device configuration [ST VL6180X, ST LSM6DS3, ST LIS3MDL, ST SLPS25H]*/
#define I2C_1_EVT_ISR       isr_i2c2_ev
#define I2C_1_ERR_ISR       isr_i2c2_er
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
