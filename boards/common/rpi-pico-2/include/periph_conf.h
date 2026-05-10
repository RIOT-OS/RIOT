/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         boards_rpi_pico_2
 * @{
 *
 * @file
 * @brief           Board periph definitions for the Raspberry Pi Pico 2
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include <stdint.h>

#include "RP2350.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configuration details for an UART interface
 */
typedef struct {
    UART0_Type *dev;    /**< Base address of the I/O registers of the device */
    gpio_t rx_pin;      /**< GPIO pin to use for RX */
    gpio_t tx_pin;      /**< GPIO pin to use for TX */
    IRQn_Type irqn;     /**< IRQ number of the UART interface */
} uart_conf_t;

static const uart_conf_t uart_config[] = {
    {
        .dev = UART0,
        .rx_pin = GPIO_PIN(0, 1),
        .tx_pin = GPIO_PIN(0, 0),
        .irqn = UART0_IRQ_IRQn
    },
    {
        .dev = UART1,
        .rx_pin = GPIO_PIN(0, 9),
        .tx_pin = GPIO_PIN(0, 8),
        .irqn = UART1_IRQ_IRQn
    }
};

#define UART_0_ISR      (isr_uart0)
#define UART_1_ISR      (isr_uart1)

#define UART_NUMOF      ARRAY_SIZE(uart_config)

/**
 * @name    Timer configuration
 *
 * The RP2350 provides two independent 64-bit microsecond timers (TIMER0,
 * TIMER1). Each has four compare channels (ALARM0..3) wired to separate
 * NVIC IRQs. We expose only TIMER0 here; alarm matches are on the lower
 * 32 bits of the counter, so timer_read() / timer_set_absolute() use a
 * 32-bit wraparound window of ~71 minutes at 1 MHz.
 * @{
 */
#define PERIPH_TIMER_PROVIDES_SET   /**< use the driver-provided timer_set */

/**
 * @brief   Per-channel timer configuration (one IRQ per ALARM)
 */
typedef struct {
    IRQn_Type irqn;                 /**< NVIC IRQ for this alarm channel */
} timer_channel_conf_t;

/**
 * @brief   Timer device configuration
 */
typedef struct {
    TIMER0_Type *dev;               /**< pointer to TIMER register block */
    const timer_channel_conf_t *ch; /**< per-channel configuration array */
    uint8_t ch_numof;               /**< number of compare channels */
} timer_conf_t;

static const timer_channel_conf_t timer0_channel_config[] = {
    { .irqn = TIMER0_IRQ_0_IRQn },
    { .irqn = TIMER0_IRQ_1_IRQn },
    { .irqn = TIMER0_IRQ_2_IRQn },
    { .irqn = TIMER0_IRQ_3_IRQn },
};

static const timer_conf_t timer_config[] = {
    {
        .dev = TIMER0,
        .ch = timer0_channel_config,
        .ch_numof = ARRAY_SIZE(timer0_channel_config),
    },
};

#define TIMER_0_ISRA    isr_timer0_0
#define TIMER_0_ISRB    isr_timer0_1
#define TIMER_0_ISRC    isr_timer0_2
#define TIMER_0_ISRD    isr_timer0_3

#define TIMER_NUMOF     ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
