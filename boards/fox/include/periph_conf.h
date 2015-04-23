/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_fox
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the fox board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
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
#define TIMER_0_MAX_VALUE   (0xffff)
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
#define TIMER_1_MAX_VALUE   (0xffff)
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
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART2
#define UART_0_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_0_IRQ          USART2_IRQn
#define UART_0_ISR          isr_usart2
#define UART_0_BUS_FREQ     36000000
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOA
#define UART_0_PORT_CLKEN() (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define UART_0_RX_PIN       3
#define UART_0_TX_PIN       2
#define UART_0_AF           1

/* UART 1 device configuration */
#define UART_1_DEV          USART1
#define UART_1_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define UART_1_IRQ          USART1_IRQn
#define UART_1_ISR          isr_usart1
#define UART_1_BUS_FREQ     72000000
/* UART 1 pin configuration */
#define UART_1_PORT         GPIOA
#define UART_1_PORT_CLKEN() (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define UART_1_RX_PIN       10
#define UART_1_TX_PIN       9
#define UART_1_AF           0

/**
 * @brief GPIO configuration
 */
#define GPIO_NUMOF          13
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
#define GPIO_IRQ_PRIO       1

/* IRQ config */
#define GPIO_IRQ_0          GPIO_7
#define GPIO_IRQ_2          GPIO_12
#define GPIO_IRQ_4          GPIO_6
#define GPIO_IRQ_5          GPIO_3
#define GPIO_IRQ_6          GPIO_5
#define GPIO_IRQ_7          GPIO_8
#define GPIO_IRQ_8          GPIO_2
#define GPIO_IRQ_9          GPIO_4
#define GPIO_IRQ_11         GPIO_1
#define GPIO_IRQ_13         GPIO_0

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOC                   /* user button */
#define GPIO_0_PIN          13
#define GPIO_0_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_0_EXTI_CFG()   (AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI13_PC)
#define GPIO_0_IRQ          EXTI15_10_IRQn
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOB                   /* l3g4200d: int1 */
#define GPIO_1_PIN          11
#define GPIO_1_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIO_1_EXTI_CFG()   (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI11_PC)
#define GPIO_1_IRQ          EXTI15_10_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOB                   /* l3g4200d: int2/drdy */
#define GPIO_2_PIN          8
#define GPIO_2_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIO_2_EXTI_CFG()   (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PB)
#define GPIO_2_IRQ          EXTI9_5_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOB                   /* lsm303dlhc: accelerometer: int2 */
#define GPIO_3_PIN          5
#define GPIO_3_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIO_3_EXTI_CFG()   (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI5_PB)
#define GPIO_3_IRQ          EXTI9_5_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOB                   /* lsm303dlhc: accelerometer: int1*/
#define GPIO_4_PIN          9
#define GPIO_4_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIO_4_EXTI_CFG()   (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI9_PB)
#define GPIO_4_IRQ          EXTI9_5_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOC                   /* battery: high power */
#define GPIO_5_PIN          6
#define GPIO_5_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_5_EXTI_CFG()   (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PC)
#define GPIO_5_IRQ          EXTI9_5_IRQn
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOC                   /* battery: enable feedback */
#define GPIO_6_PIN          4
#define GPIO_6_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_6_EXTI_CFG()   (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI4_PC)
#define GPIO_6_IRQ          EXTI4_IRQn
/* GPIO channel 7 config */
#define GPIO_7_PORT        GPIOC                   /* battery: feedback */
#define GPIO_7_PIN         0
#define GPIO_7_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_7_EXTI_CFG()  (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PC)
#define GPIO_7_IRQ         EXTI0_IRQn
/* GPIO channel 8 config */
#define GPIO_8_PORT        GPIOA                    /* extension header */
#define GPIO_8_PIN         7
#define GPIO_8_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_8_EXTI_CFG()  (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI7_PA)
#define GPIO_8_IRQ         EXTI9_5_IRQn
/* GPIO channel 9 config */
#define GPIO_9_PORT        GPIOA                    /* lsm303dlhc: magnetometer: drdy */
#define GPIO_9_PIN         9
#define GPIO_9_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_9_EXTI_CFG()  (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI9_PA)
#define GPIO_9_IRQ         EXTI9_5_IRQn
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOB                    /* extension header, don't use as EXTI */
#define GPIO_10_PIN         0
#define GPIO_10_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIO_10_EXTI_CFG()  (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB)
#define GPIO_10_IRQ         EXTI0_IRQn
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOA                    /* radio: cs */
#define GPIO_11_PIN         1
#define GPIO_11_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_11_EXTI_CFG()  (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PA)
#define GPIO_11_IRQ         EXTI1_IRQn
/* GPIO channel 12 config */
#define GPIO_12_PORT        GPIOC                    /* radio: INT */
#define GPIO_12_PIN         2
#define GPIO_12_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_12_EXTI_CFG()  (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI2_PC)
#define GPIO_12_IRQ         EXTI2_IRQn
/* GPIO channel 13 config */
#define GPIO_13_PORT        GPIOC                    /* radio: reset */
#define GPIO_13_PIN         1
#define GPIO_13_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_13_EXTI_CFG()  (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PC)
#define GPIO_13_IRQ         EXTI1_IRQn
/* GPIO channel 14 config */
#define GPIO_14_PORT        GPIOA                    /* radio: sleep */
#define GPIO_14_PIN         0
#define GPIO_14_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_14_EXTI_CFG()  (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA)
#define GPIO_14_IRQ         EXTI0_IRQn
/**
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)
#define SPI_0_EN            1

/* SPI 0 device configuration */
#define SPI_0_DEV               SPI2
#define SPI_0_CLKEN()           (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#define SPI_0_CLKDIS()          (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN))
#define SPI_0_BUS_DIV           0   /* 1 -> SPI runs with full CPU clock, 0 -> half CPU clock */
/* SPI 0 pin configuration */
#define SPI_0_CLK_PORT          GPIOB
#define SPI_0_CLK_PIN           13
#define SPI_0_CLK_PORT_CLKEN()  (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define SPI_0_MOSI_PORT         GPIOB
#define SPI_0_MOSI_PIN          15
#define SPI_0_MOSI_PORT_CLKEN() (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define SPI_0_MISO_PORT         GPIOB
#define SPI_0_MISO_PIN          14
#define SPI_0_MISO_PORT_CLKEN() (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
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
#define I2C_0_SCL_PORT      GPIOB
#define I2C_0_SCL_PIN       6
#define I2C_0_SCL_CLKEN()   (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define I2C_0_SDA_PORT      GPIOB
#define I2C_0_SDA_PIN       7
#define I2C_0_SDA_CLKEN()   (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
