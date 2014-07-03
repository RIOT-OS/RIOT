/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_iot-lab_M3
 * @{
 *
 * @file        periph_conf.h
 * @brief       Peripheral MCU configuration for the iot-lab_M3 board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */
#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

/**
 * @brief Timer configuration
 * @{
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TIM2
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (36000U)
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define TIMER_0_ISR         isr_tim2
#define TIMER_0_IRQ_CHAN    TIM2_IRQn
#define TIMER_0_IRQ_PRIO    1

/* Timer 1 configuration */
#define TIMER_1_DEV         TIM3
#define TIMER_1_CHANNELS    2
#define TIMER_1_PRESCALER   (36000U)
#define TIMER_1_MAX_VALUE   (0xffff)
#define TIMER_1_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
#define TIMER_1_ISR         isr_tim3
#define TIMER_1_IRQ_CHAN    TIM3_IRQn
#define TIMER_1_IRQ_PRIO    1
/** @} */

/**
 * @brief UART configuration
 */
#define UART_NUMOF          (2U)
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
#define UART_0_PORT         GPIOA
#define UART_0_PORT_CLKEN() (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define UART_0_RX_PIN       10
#define UART_0_TX_PIN       9
#define UART_0_AF           0

/* UART 1 device configuration */
#define UART_1_DEV          USART2
#define UART_1_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_1_IRQ          USART2_IRQn
#define UART_1_ISR          isr_usart2
#define UART_1_BUS_FREQ     36000000
/* UART 1 pin configuration */
#define UART_1_PORT         GPIOA
#define UART_1_PORT_CLKEN() (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define UART_1_RX_PIN       3
#define UART_1_TX_PIN       2
#define UART_1_AF           1

/**
 * @brief GPIO configuration
 */
#define GPIO_NUMOF          16
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
#define GPIO_15_EN          0       /* not configured */
#define GPIO_IRQ_PRIO       1

/* IRQ config */
#define GPIO_IRQ_0          GPIO_0
#define GPIO_IRQ_1          GPIO_1
#define GPIO_IRQ_2          GPIO_0                  /* not configured */
#define GPIO_IRQ_3          GPIO_0                  /* not configured */
#define GPIO_IRQ_4          GPIO_2
#define GPIO_IRQ_5          GPIO_3
#define GPIO_IRQ_6          GPIO_4
#define GPIO_IRQ_7          GPIO_5
#define GPIO_IRQ_8          GPIO_0                  /* not configured */
#define GPIO_IRQ_9          GPIO_0                  /* not configured */
#define GPIO_IRQ_10         GPIO_6
#define GPIO_IRQ_11         GPIO_7
#define GPIO_IRQ_12         GPIO_4
#define GPIO_IRQ_13         GPIO_9
#define GPIO_IRQ_14         GPIO_10
#define GPIO_IRQ_15         GPIO_11

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOA                   /* Used for user button 1 */
#define GPIO_0_PIN          3
#define GPIO_0_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_0_EXTI_CFG()   (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PA)
#define GPIO_0_EXTI_LINE    4
#define GPIO_0_IRQ          EXTI4_IRQn
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOA
#define GPIO_1_PIN          8
#define GPIO_1_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_1_EXTI_CFG()   (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PA)
#define GPIO_1_EXTI_LINE    4
#define GPIO_1_IRQ          EXTI4_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOA
#define GPIO_2_PIN          12
#define GPIO_2_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_2_EXTI_CFG()   (AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI12_PA)
#define GPIO_2_EXTI_LINE    4
#define GPIO_2_IRQ          EXTI4_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOB
#define GPIO_3_PIN          8
#define GPIO_3_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIO_3_EXTI_CFG()   (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PB)
#define GPIO_3_EXTI_LINE    4
#define GPIO_3_IRQ          EXTI4_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOB
#define GPIO_4_PIN          9
#define GPIO_4_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIO_4_EXTI_CFG()   (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI9_PB)
#define GPIO_4_EXTI_LINE    4
#define GPIO_4_IRQ          EXTI4_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOC
#define GPIO_5_PIN          7
#define GPIO_5_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_5_EXTI_CFG()   (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI7_PC)
#define GPIO_5_EXTI_LINE    4
#define GPIO_5_IRQ          EXTI4_IRQn
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOC
#define GPIO_6_PIN          8
#define GPIO_6_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_6_EXTI_CFG()   (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PC)
#define GPIO_6_EXTI_LINE    4
#define GPIO_6_IRQ          EXTI3_IRQn
/* GPIO channel 7 config */
#define GPIO_7_PORT         GPIOC
#define GPIO_7_PIN          11
#define GPIO_7_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_7_EXTI_CFG()   (AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI11_PC)
#define GPIO_7_EXTI_LINE    4
#define GPIO_7_IRQ          EXTI3_IRQn
/* GPIO channel 8 config */
#define GPIO_8_PORT         GPIOA
#define GPIO_8_PIN          5
#define GPIO_8_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_8_EXTI_CFG()   (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI5_PA)
#define GPIO_8_EXTI_LINE    4
#define GPIO_8_IRQ          EXTI4_IRQn
/* GPIO channel 9 config */
#define GPIO_9_PORT         GPIOA
#define GPIO_9_PIN          6
#define GPIO_9_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_9_EXTI_CFG()   (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PA)
#define GPIO_9_EXTI_LINE    4
#define GPIO_9_IRQ          EXTI4_IRQn
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOA
#define GPIO_10_PIN         7
#define GPIO_10_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_10_EXTI_CFG()  (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI7_PA)
#define GPIO_10_EXTI_LINE   4
#define GPIO_10_IRQ         EXTI4_IRQn
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOA
#define GPIO_11_PIN         4
#define GPIO_11_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_11_EXTI_CFG()  (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI4_PA)
#define GPIO_11_EXTI_LINE   4
#define GPIO_11_IRQ         EXTI4_IRQn
/* GPIO channel 12 config */
#define GPIO_12_PORT        GPIOC
#define GPIO_12_PIN         4
#define GPIO_12_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_12_EXTI_CFG()  (AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI4_PC)
#define GPIO_12_EXTI_LINE   4
#define GPIO_12_IRQ         EXTI4_IRQn
/* GPIO channel 13 config */
#define GPIO_13_PORT        GPIOC
#define GPIO_13_PIN         1
#define GPIO_13_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_13_EXTI_CFG()  (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PC)
#define GPIO_13_EXTI_LINE   4
#define GPIO_13_IRQ         EXTI4_IRQn
/* GPIO channel 14 config */
#define GPIO_14_PORT        GPIOA
#define GPIO_14_PIN         2
#define GPIO_14_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_14_EXTI_CFG()  (AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI2_PA)
#define GPIO_14_EXTI_LINE   4
#define GPIO_14_IRQ         EXTI4_IRQn
/* GPIO channel 15 config */
#define GPIO_15_PORT        GPIOC
#define GPIO_15_PIN         15
#define GPIO_15_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_15_EXTI_CFG()  (AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI15_PC)
#define GPIO_15_EXTI_LINE   4
#define GPIO_15_IRQ         EXTI4_IRQn

/**
 * @brief SPI configuration
 */
#define SPI_NUM_OF      1
#define SPI_0_EN        1

#define SPI_0_DEV       SPI1
#define SPI_IRQ_0       SPI_0

#define SPI_0_BR_PRESC  16

#define SPI_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_0_SCLK_GPIO     GPIO_8
#define SPI_0_SCLK_PIN      GPIO_8_PIN
#define SPI_0_SCLK_PORT     GPIO_8_PORT
#define SPI_0_MISO_GPIO     GPIO_9
#define SPI_0_MISO_PIN      GPIO_9_PIN
#define SPI_0_MISO_PORT     GPIO_9_PORT
#define SPI_0_MOSI_GPIO     GPIO_10
#define SPI_0_MOSI_PIN      GPIO_10_PIN
#define SPI_0_MOSI_PORT     GPIO_10_PORT
#define SPI_0_CS_GPIO       GPIO_11
#define SPI_0_CS_PIN        GPIO_11_PIN
#define SPI_0_CS_PORT       GPIO_11_PORT
#define SPI_0_IRQ0_GPIO     GPIO_12
#define SPI_0_IRQ0_PIN      GPIO_12_PIN
#define SPI_0_IRQ0_PORT     GPIO_12_PORT
#define SPI_0_RESET_GPIO    GPIO_13
#define SPI_0_RESET_PIN     GPIO_13_PIN
#define SPI_0_RESET_PORT    GPIO_13_PORT
#define SPI_0_SLEEP_GPIO    GPIO_14
#define SPI_0_SLEEP_PIN     GPIO_14_PIN
#define SPI_0_SLEEP_PORT    GPIO_14_PORT

#define SPI_2_LINES_FULL_DUPLEX     (0x0000)
#define SPI_MASTER_MODE             (0x0104)
#define SPI_DATA_SIZE_8B            (0x0000)
#define SPI_CPOL_LOW                (0x0000)
#define SPI_CPHA_1_EDGE             (0x0000)
#define SPI_NSS_SOFT                (0x0200)
#define SPI_BR_PRESCALER_16         (0x0018)
#define SPI_1ST_BIT_MSB             (0x0000)

#endif /* __PERIPH_CONF_H */
/** @} */
