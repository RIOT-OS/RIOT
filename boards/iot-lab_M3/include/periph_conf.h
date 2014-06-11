/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
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
#define TIMER_1_EN          2

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
/* UART 1 pin configuration */
#define UART_1_PORT         GPIOA
#define UART_1_PORT_CLKEN() (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define UART_1_RX_PIN       3
#define UART_1_TX_PIN       2
#define UART_1_AF           1

/**
 * @brief GPIO configuration
 */
#define GPIO_NUMOF          12
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
#define GPIO_IRQ_12         GPIO_8
#define GPIO_IRQ_13         GPIO_9
#define GPIO_IRQ_14         GPIO_10
#define GPIO_IRQ_15         GPIO_11

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOA                   /* Used for user button 1 */
#define GPIO_0_PIN          0
#define GPIO_0_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_0_EXTI_CFG()   (AFIO->EXTICR[GPIO_0_PIN>>0x02] |= (((uint32_t)0x00) << (0x04 * (GPIO_0_PIN & (uint8_t)0x03))))
#define GPIO_0_IRQ          EXTI0_IRQn
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOA
#define GPIO_1_PIN          1
#define GPIO_1_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIO_1_EXTI_CFG()   (AFIO->EXTICR[GPIO_1_PIN>>0x02] |= (((uint32_t)0x00) << (0x04 * (GPIO_1_PIN & (uint8_t)0x03))))
#define GPIO_1_IRQ          EXTI0_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOF
#define GPIO_2_PIN          4
#define GPIO_2_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPFEN)
#define GPIO_2_EXTI_CFG()   (AFIO->EXTICR[GPIO_2_PIN>>0x02] |= (((uint32_t)0x05) << (0x04 * (GPIO_2_PIN & (uint8_t)0x03))))
#define GPIO_2_IRQ          EXTI4_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOF
#define GPIO_3_PIN          5
#define GPIO_3_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPFEN)
#define GPIO_3_EXTI_CFG()   (AFIO->EXTICR[GPIO_3_PIN>>0x02] |= (((uint32_t)0x05) << (0x04 * (GPIO_3_PIN & (uint8_t)0x03))))
#define GPIO_3_IRQ          EXTI4_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOF
#define GPIO_4_PIN          6
#define GPIO_4_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPFEN)
#define GPIO_4_EXTI_CFG()   (AFIO->EXTICR[GPIO_3_PIN>>0x02] |= (((uint32_t)0x05) << (0x04 * (GPIO_3_PIN & (uint8_t)0x03))))
#define GPIO_4_IRQ          EXTI4_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOF
#define GPIO_5_PIN          7
#define GPIO_5_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPFEN)
#define GPIO_5_EXTI_CFG()   (AFIO->EXTICR[GPIO_5_PIN>>0x02] |= (((uint32_t)0x05) << (0x04 * (GPIO_5_PIN & (uint8_t)0x03))))
#define GPIO_5_IRQ          EXTI4_IRQn
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOC
#define GPIO_6_PIN          4
#define GPIO_6_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_6_EXTI_CFG()   (AFIO->EXTICR[GPIO_6_PIN>>0x02] |= (((uint32_t)0x02) << (0x04 * (GPIO_6_PIN & (uint8_t)0x03))))
#define GPIO_6_IRQ          EXTI3_IRQn
/* GPIO channel 7 config */
#define GPIO_7_PORT         GPIOC
#define GPIO_7_PIN          11
#define GPIO_7_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_7_EXTI_CFG()   (AFIO->EXTICR[GPIO_7_PIN>>0x02] |= (((uint32_t)0x02) << (0x04 * (GPIO_7_PIN & (uint8_t)0x03))))
#define GPIO_7_IRQ          EXTI3_IRQn
/* GPIO channel 8 config */
#define GPIO_8_PORT         GPIOC
#define GPIO_8_PIN          12
#define GPIO_8_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_8_EXTI_CFG()   (AFIO->EXTICR[GPIO_8_PIN>>0x02] |= (((uint32_t)0x02) << (0x04 * (GPIO_8_PIN & (uint8_t)0x03))))
#define GPIO_8_IRQ          EXTI3_IRQn
/* GPIO channel 9 config */
#define GPIO_9_PORT         GPIOC
#define GPIO_9_PIN          13
#define GPIO_9_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_9_EXTI_CFG()   (AFIO->EXTICR[GPIO_9_PIN>>0x02] |= (((uint32_t)0x02) << (0x04 * (GPIO_9_PIN & (uint8_t)0x03))))
#define GPIO_9_IRQ          EXTI3_IRQn
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOC
#define GPIO_10_PIN         14
#define GPIO_10_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_10_EXTI_CFG()  (AFIO->EXTICR[GPIO_10_PIN>>0x02] |= (((uint32_t)0x02) << (0x04 * (GPIO_10_PIN & (uint8_t)0x03))))
#define GPIO_10_IRQ         EXTI3_IRQn
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOC
#define GPIO_11_PIN         15
#define GPIO_11_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIO_11_EXTI_CFG()  (AFIO->EXTICR[GPIO_11_PIN>>0x02] |= (((uint32_t)0x02) << (0x04 * (GPIO_11_PIN & (uint8_t)0x03))))
#define GPIO_11_IRQ         EXTI3_IRQn

/**
 * @brief SPI configuration
 */
#define SPI_NUM_OF      1
#define SPI_0_EN        1

#define SPI_IRQ_0       SPI_0

#define SPI_0_BR_PRESC  16

#define SPI_0_SCLK      GPIO_5_PIN
#define SPI_0_MISO      GPIO_6_PIN
#define SPI_0_MOSI      GPIO_7_PIN
#define SPI_0_CS        GPIO_4_PIN

#endif /* __PERIPH_CONF_H */
/** @} */
