/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Atmel SAM R21 Xplained
 *              Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>s
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   External oscillator and clock configuration
 *
 * For selection of the used CORECLOCK, we have implemented two choices:
 *
 * - usage of the PLL fed by the internal 8MHz oscillator divided by 8
 * - usage of the internal 8MHz oscillator directly, divided by N if needed
 *
 *
 * The PLL option allows for the usage of a wider frequency range and a more
 * stable clock with less jitter. This is why we use this option as default.
 *
 * The target frequency is computed from the PLL multiplier and the PLL divisor.
 * Use the following formula to compute your values:
 *
 * CORECLOCK = ((PLL_MUL + 1) * 1MHz) / PLL_DIV
 *
 * NOTE: The PLL circuit does not run with less than 32MHz while the maximum PLL
 *       frequency is 96MHz. So PLL_MULL must be between 31 and 95!
 *
 *
 * The internal Oscillator used directly can lead to a slightly better power
 * efficiency to the cost of a less stable clock. Use this option when you know
 * what you are doing! The actual core frequency is adjusted as follows:
 *
 * CORECLOCK = 8MHz / DIV
 *
 * NOTE: A core clock frequency below 1MHz is not recommended
 *
 * @{
 */
#define CLOCK_USE_PLL       (1)

#if CLOCK_USE_PLL
/* edit these values to adjust the PLL output frequency */
#define CLOCK_PLL_MUL       (47U)               /* must be >= 31 & <= 95 */
#define CLOCK_PLL_DIV       (1U)                /* adjust to your needs */
/* generate the actual used core clock frequency */
#define CLOCK_CORECLOCK     (((CLOCK_PLL_MUL + 1) * 1000000U) / CLOCK_PLL_DIV)
#else
/* edit this value to your needs */
#define CLOCK_DIV           (1U)
/* generate the actual core clock frequency */
#define CLOCK_CORECLOCK     (8000000 / CLOCK_DIV)
#endif
/** @} */

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TC3->COUNT16
#define TIMER_0_CHANNELS    2
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_ISR         isr_tc3

/* Timer 1 configuration */
#define TIMER_1_DEV         TC4->COUNT32
#define TIMER_1_CHANNELS    2
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_ISR         isr_tc4
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          SERCOM0->USART
#define UART_0_IRQ          SERCOM0_IRQn
#define UART_0_ISR          isr_sercom0
/* UART 0 pin configuration */
#define UART_0_PORT         (PORT->Group[0])
#define UART_0_TX_PIN       (4)
#define UART_0_RX_PIN       (5)
#define UART_0_PINS         (PORT_PA04 | PORT_PA05)
/** @} */


/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF          (2)
#define SPI_0_EN           1
#define SPI_1_EN           1

/*      SPI0             */
#define SPI_0_DEV          SERCOM4->SPI
#define SPI_IRQ_0          SERCOM4_IRQn
#define SPI_0_DOPO         (1)
#define SPI_0_DIPO         (0)

#define SPI_0_SCLK_DEV     PORT->Group[2]
#define SPI_0_SCLK_PIN     (18)

#define SPI_0_MISO_DEV     PORT->Group[2]
#define SPI_0_MISO_PIN     (19)

#define SPI_0_MOSI_DEV     PORT->Group[1]
#define SPI_0_MOSI_PIN     (30)

/*      SPI1             */
#define SPI_1_DEV          SERCOM5->SPI
#define SPI_IRQ_1          SERCOM5_IRQn
#define SPI_1_DOPO         (1)
#define SPI_1_DIPO         (2)

#define SPI_1_SCLK_DEV     PORT->Group[1]
#define SPI_1_SCLK_PIN     (23)

#define SPI_1_MISO_DEV     PORT->Group[1]
#define SPI_1_MISO_PIN     (02)

#define SPI_1_MOSI_DEV     PORT->Group[1]
#define SPI_1_MOSI_PIN     (22)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF          (1U)
#define I2C_0_EN            1
#define I2C_1_EN            0
#define I2C_2_EN            0
#define I2C_3_EN            0
#define I2C_IRQ_PRIO        1

#define I2C_0_DEV           SERCOM3->I2CM
#define I2C_0_IRQ           SERCOM3_IRQn
#define I2C_0_ISR           isr_sercom3
/* I2C 0 pin configuration */
#define I2C_0_PORT          (PORT->Group[0])
#define I2C_SDA             PIN_PA16
#define I2C_SCL             PIN_PA17
#define I2C_0_PINS          (PORT_PA16 | PORT_PA17)

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF       (0U)
/** @} */

/**
 * @name RTC configuration
 * @{
 */
#define RTC_NUMOF           (1U)
#define RTC_DEV             RTC->MODE2
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_DEV             RTC->MODE0
#define RTT_IRQ             RTC_IRQn
#define RTT_IRQ_PRIO        10
#define RTT_ISR             isr_rtc
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (32768U)    /* in Hz. For changes see `rtt.c` */
#define RTT_RUNSTDBY        (1)         /* Keep RTT running in sleep states */
/** @} */


/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF         (9U)
#define GPIO_0_EN          1
#define GPIO_1_EN          1
#define GPIO_2_EN          1
#define GPIO_3_EN          1
/*4-7 -> internal */
#define GPIO_4_EN          1
#define GPIO_5_EN          1
#define GPIO_6_EN          1
#define GPIO_7_EN          1
#define GPIO_8_EN          1
#define GPIO_9_EN          0
#define GPIO_10_EN         0
#define GPIO_11_EN         0

#define GPIO_12_EN         0
#define GPIO_13_EN         0
#define GPIO_14_EN         0
#define GPIO_15_EN         0

#define GPIO_NO_EXTINT     (18)

/* GPIO channel 0 config */
#define GPIO_0_DEV         PORT->Group[0]
#define GPIO_0_PIN         (13)
#define GPIO_0_EXTINT      (13)
/* GPIO channel 1 config */
#define GPIO_1_DEV         PORT->Group[0]
#define GPIO_1_PIN         (28)
#define GPIO_1_EXTINT      (8)
/* GPIO channel 2 config */
#define GPIO_2_DEV         PORT->Group[0]
#define GPIO_2_PIN         (15)
#define GPIO_2_EXTINT      (15)
/* GPIO channel 3 config */
#define GPIO_3_DEV         PORT->Group[0]
#define GPIO_3_PIN         (19)
#define GPIO_3_EXTINT      (3)
/* GPIO 4-7 Internal radio pins*/
/* GPIO channel 4 config  radio CS*/
#define GPIO_4_DEV         PORT->Group[1]
#define GPIO_4_PIN         (31)
#define GPIO_4_EXTINT      GPIO_NO_EXTINT
/* GPIO channel 5 config  radio IRQ0*/
#define GPIO_5_DEV         PORT->Group[1]
#define GPIO_5_PIN         (0)
#define GPIO_5_EXTINT      (0)
/* GPIO channel 6 config  radio reset*/
#define GPIO_6_DEV         PORT->Group[1]
#define GPIO_6_PIN         (15)
#define GPIO_6_EXTINT      GPIO_NO_EXTINT
/* GPIO channel 7 config radio sleep*/
#define GPIO_7_DEV         PORT->Group[0]
#define GPIO_7_PIN         (20)
#define GPIO_7_EXTINT      GPIO_NO_EXTINT
/* GPIO channel 8 config */
#define GPIO_8_DEV         PORT->Group[0]
#define GPIO_8_PIN         (27)
#define GPIO_8_EXTINT      GPIO_NO_EXTINT
/* GPIO channel 9 config */
#define GPIO_9_DEV
#define GPIO_9_PIN
#define GPIO_9_EXTINT
/* GPIO channel 10 config */
#define GPIO_10_DEV
#define GPIO_10_PIN
#define GPIO_10_EXTINT
/* GPIO channel 11 config */
#define GPIO_11_DEV
#define GPIO_11_PIN
#define GPIO_11_EXTINT
/* GPIO channel 12 config */
#define GPIO_12_PIN
#define GPIO_12_EXTINT
/* GPIO channel 13 config */
#define GPIO_13_PIN
#define GPIO_13_EXTINT
/* GPIO channel 14 config */
#define GPIO_14_PIN
#define GPIO_14_EXTINT
/* GPIO channel 15 config */
#define GPIO_15_PIN
#define GPIO_15_EXTINT
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
