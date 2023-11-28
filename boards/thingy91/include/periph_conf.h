/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_thingy91
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Thingy:91
 *
 * @author      Thanh-Viet Nguyen <topologicrose@protonmail.com>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "board.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

  /**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0_S,
        .rx_pin     = GPIO_PIN(0, 2),
        .tx_pin     = GPIO_PIN(0, 3),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQn,
    },
};

#define UART_0_ISR          (isr_uarte0_spim0_spis0_twim0_twis0) /**< UART0_IRQ */

#define UART_NUMOF          ARRAY_SIZE(uart_config) /**< UART confgiguration NUMOF */
/**
 * @name    I2C configuration
 * @{
 */
  
/*static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM1,
        .scl = GPIO_PIN(0, ???), //
        .sda = GPIO_PIN(0, ???),
        .speed = I2C_SPEED_NORMAL
    }
};*/
  
//#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER0_S,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER0_IRQn
    },
    {
        .dev      = NRF_TIMER1_S,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_08Bit,
        .irqn     = TIMER1_IRQn
    },
};

#define TIMER_0_ISR         isr_timer0 /**< Timer0 IRQ*/
#define TIMER_1_ISR         isr_timer1 /**< Timer1 IRQ */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config) /**< Timer configuration NUMOF */
/** @} */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
