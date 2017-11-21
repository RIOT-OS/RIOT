 /*
 * Copyright (C) 2014-2015,2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_arduino_due
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for Arduino Due based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
/* targeted system core clock */
#define CLOCK_CORECLOCK     (84000000UL)
/* external oscillator clock */
#define CLOCK_EXT_OSC       (12000000UL)
/* define PLL configuration
 *
 * The values must fulfill this equation:
 * CORECLOCK = (EXT_OCS / PLL_DIV) * (PLL_MUL + 1)
 */
#define CLOCK_PLL_MUL       (83)
#define CLOCK_PLL_DIV       (12)

/* number of wait states before flash read and write operations */
#define CLOCK_FWS           (4)         /* 4 is save for 84MHz */
/** @} */

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    { .dev = TC0, .id_ch0 = ID_TC0 },
    { .dev = TC1, .id_ch0 = ID_TC3 }
};

#define TIMER_0_ISR         isr_tc0
#define TIMER_1_ISR         isr_tc3

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = (Uart *)UART,
        .rx_pin = GPIO_PIN(PA, 8),
        .tx_pin = GPIO_PIN(PA, 9),
        .mux    = GPIO_MUX_A,
        .pmc_id = ID_UART,
        .irqn   = UART_IRQn
    },
    {
        .dev    = (Uart *)USART0,
        .rx_pin = GPIO_PIN(PA, 10),
        .tx_pin = GPIO_PIN(PA, 11),
        .mux    = GPIO_MUX_A,
        .pmc_id = ID_USART0,
        .irqn   = USART0_IRQn
    },
    {
        .dev    = (Uart *)USART1,
        .rx_pin = GPIO_PIN(PA, 12),
        .tx_pin = GPIO_PIN(PA, 13),
        .mux    = GPIO_MUX_A,
        .pmc_id = ID_USART1,
        .irqn   = USART1_IRQn
    },
    {
        .dev    = (Uart *)USART3,
        .rx_pin = GPIO_PIN(PD, 5),
        .tx_pin = GPIO_PIN(PD, 4),
        .mux    = GPIO_MUX_B,
        .pmc_id = ID_USART3,
        .irqn   = USART3_IRQn
    }
};

/* define interrupt vectors */
#define UART_0_ISR          isr_uart
#define UART_1_ISR          isr_usart0
#define UART_2_ISR          isr_usart1
#define UART_3_ISR          isr_usart3

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
* @name     SPI configuration
* @{
*/
static const spi_conf_t spi_config[] = {
    {
        .dev   = SPI0,
        .id    = ID_SPI0,
        .clk   = GPIO_PIN(PA, 27),
        .mosi  = GPIO_PIN(PA, 26),
        .miso  = GPIO_PIN(PA, 25),
        .mux   = GPIO_MUX_A
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_chan_conf_t pwm_chan[] = {
    { .pin = GPIO_PIN(PC, 21), .hwchan = 4 },
    { .pin = GPIO_PIN(PC, 22), .hwchan = 5 },
    { .pin = GPIO_PIN(PC, 23), .hwchan = 6 },
    { .pin = GPIO_PIN(PC, 24), .hwchan = 7 }
};

#define PWM_NUMOF           (1U)
#define PWM_CHAN_NUMOF      (sizeof(pwm_chan) / sizeof(pwm_chan[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
