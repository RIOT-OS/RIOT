/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_openmote
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the OpenMote board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000U)         /* desired core clock frequency, 32MHz */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (4U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1
#define TIMER_3_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         GPTIMER0
#define TIMER_0_CHANNELS    NUM_CHANNELS_PER_GPTIMER
#define TIMER_0_MAX_VALUE   0xffffffff
#define TIMER_0_IRQn_1      GPTIMER_0A_IRQn
#define TIMER_0_IRQn_2      GPTIMER_0B_IRQn
#define TIMER_0_ISR_1       isr_timer0_chan0
#define TIMER_0_ISR_2       isr_timer0_chan1

/* Timer 1 configuration */
#define TIMER_1_DEV         GPTIMER1
#define TIMER_1_CHANNELS    NUM_CHANNELS_PER_GPTIMER
#define TIMER_1_MAX_VALUE   0xffffffff
#define TIMER_1_IRQn_1      GPTIMER_1A_IRQn
#define TIMER_1_IRQn_2      GPTIMER_1B_IRQn
#define TIMER_1_ISR_1       isr_timer1_chan0
#define TIMER_1_ISR_2       isr_timer1_chan1

/* Timer 2 configuration */
#define TIMER_2_DEV         GPTIMER2
#define TIMER_2_CHANNELS    NUM_CHANNELS_PER_GPTIMER
#define TIMER_2_MAX_VALUE   0xffffffff
#define TIMER_2_IRQn_1      GPTIMER_2A_IRQn
#define TIMER_2_IRQn_2      GPTIMER_2B_IRQn
#define TIMER_2_ISR_1       isr_timer2_chan0
#define TIMER_2_ISR_2       isr_timer2_chan1

/* Timer 3 configuration */
#define TIMER_3_DEV         GPTIMER3
#define TIMER_3_CHANNELS    NUM_CHANNELS_PER_GPTIMER
#define TIMER_3_MAX_VALUE   0xffffffff
#define TIMER_3_IRQn_1      GPTIMER_3A_IRQn
#define TIMER_3_IRQn_2      GPTIMER_3B_IRQn
#define TIMER_3_ISR_1       isr_timer3_chan0
#define TIMER_3_ISR_2       isr_timer3_chan1
/** @} */


/**
 * @name Hardware Timer peripheral configuration
 * @{
 */
#define HWTIMER_IRQ_PRIO    1
/** @} */


/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          UART0
#define UART_0_IRQ          UART0_IRQn
#define UART_0_ISR          isr_uart0
/* UART 0 pin configuration */
#define UART_0_TX_PIN       GPIO_PA1
#define UART_0_RX_PIN       GPIO_PA0
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        1
/** @} */

/**
 * @name GPIO configuration
 * @{
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

/* GPIO 0 configuration */
#define GPIO_0_PORT         GPIO_A
#define GPIO_0_PIN          0
#define GPIO_0_OVER         PA_OVER
/* GPIO 1 configuration */
#define GPIO_1_PORT         GPIO_A
#define GPIO_1_PIN          1
#define GPIO_1_OVER         PA_OVER
/* GPIO 2 configuration */
#define GPIO_2_PORT         GPIO_A
#define GPIO_2_PIN          2
#define GPIO_2_OVER         PA_OVER
/* GPIO 3 configuration */
#define GPIO_3_PORT         GPIO_A
#define GPIO_3_PIN          3
#define GPIO_3_OVER         PA_OVER
/* GPIO 4 configuration */
#define GPIO_4_PORT         GPIO_A
#define GPIO_4_PIN          4
#define GPIO_4_OVER         PA_OVER
/* GPIO 5 configuration */
#define GPIO_5_PORT         GPIO_A
#define GPIO_5_PIN          5
#define GPIO_5_OVER         PA_OVER
/* GPIO 6 configuration */
#define GPIO_6_PORT         GPIO_A
#define GPIO_6_PIN          6
#define GPIO_6_OVER         PA_OVER
/* GPIO 7 configuration */
#define GPIO_7_PORT         GPIO_A
#define GPIO_7_PIN          7
#define GPIO_7_OVER         PA_OVER
/* GPIO 8 configuration */
#define GPIO_8_PORT         GPIO_B
#define GPIO_8_PIN          0
#define GPIO_8_OVER         PB_OVER
/* GPIO 9 configuration */
#define GPIO_9_PORT         GPIO_B
#define GPIO_9_PIN          1
#define GPIO_9_OVER         PB_OVER
/* GPIO 10 configuration */
#define GPIO_10_PORT        GPIO_B
#define GPIO_10_PIN         2
#define GPIO_10_OVER        PB_OVER
/* GPIO 11 configuration */
#define GPIO_11_PORT        GPIO_B
#define GPIO_11_PIN         3
#define GPIO_11_OVER        PB_OVER
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
