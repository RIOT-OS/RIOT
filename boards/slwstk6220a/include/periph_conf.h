/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_wstk6220
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the WSTK6220 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>
#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_RCOSC         (14000000)      /* internal RC oscillator speed */
/* external oscillator speed, comment out if you want to use the internal RC
 * oscillator circuit as a clock source */
#define CLOCK_HFXO          (48000000U)
/* define clock dividers */
#define CLOCK_HFCORECLKDIV  (1U)            /* core clock divider */
#define CLOCK_HFPERCLKDIV   (1U)            /* peripheral clock divider */

/* generate the actual clock values */
#ifdef CLOCK_HFXO
#define CLOCK_CORECLOCK     (CLOCK_HFXO / CLOCK_HFCORECLKDIV)
#else
#define CLOCK_CORECLOCK     (CLOCK_RCOSC / CLOCK_HFCORECLKDIV)
#endif
#define CLOCK_HFPERCLK      (CLOCK_CORECLOCK / CLOCK_HFPERCLKDIV)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        TIMER0,             /* lower numbered timer, used as pre-scaler */
        TIMER1,             /* higher numbered timer, this is the one */
        5,                  /* pre-scaler bit in the CMU register */
        TIMER1_IRQn,        /* IRQn of the higher numbered driver */
    }
};

#define TIMER_0_ISR         isr_timer1
#define TIMER_0_MAX_VALUE   (0xffff)            /* 16-bit timer */
#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        USART2,             /* device */
        GPIO_PIN(PB,4),     /* RX pin */
        GPIO_PIN(PB,3),     /* TX pin */
        1,                  /* AF location */
        2,                  /* bit in CMU enable register */
        USART2_RX_IRQn      /* IRQ base channel */
    },
};

#define UART_0_ISR_RX       isr_usart2_rx
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
