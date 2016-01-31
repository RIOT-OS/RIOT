/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ryan Kurte <ryankurte@gmail.com>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"

#include "periph/uart.h"
#include "periph/gpio.h"

#include "em_usart.h"

#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
#include "em_leuart.h"
#endif

/* guard file in case no UART device is defined */
#if UART_NUMOF

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

/**
 * @brief   Check if device is a U(S)ART device.
 */
static inline bool _is_usart(uart_t dev)
{
    return ((uint32_t) uart_config[dev].dev) < LEUART0_BASE;
}

int uart_init(uart_t dev, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* check if device is valid */
    if (dev >= UART_NUMOF) {
        return -1;
    }

    /* save interrupt callback context */
    isr_ctx[dev].rx_cb = rx_cb;
    isr_ctx[dev].arg = arg;

    /* initialize the pins */
    gpio_init(uart_config[dev].rx_pin, GPIO_DIR_IN, GPIO_NOPULL);
    gpio_init(uart_config[dev].tx_pin, GPIO_DIR_OUT, GPIO_NOPULL);

    /* initialize the UART/USART/LEUART device */
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
    if (_is_usart(dev)) {
#endif
        /* enable clocks */
        CMU_ClockEnable(cmuClock_HFPER, true);
        CMU_ClockEnable(uart_config[dev].cmu, true);

        /* initialize device (reset is performed in USART_InitAsync) */
        USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

        init.enable = usartDisable;
        init.baudrate = baudrate;

        USART_InitAsync(uart_config[dev].dev, &init);

        /* configure pins */
#ifdef _SILICON_LABS_32B_PLATFORM_1
        ((USART_TypeDef *) uart_config[dev].dev)->ROUTE = (
            uart_config[dev].loc | USART_ROUTE_RXPEN | USART_ROUTE_TXPEN);
#else
        ((USART_TypeDef *) uart_config[dev].dev)->ROUTEPEN =
            USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
        ((USART_TypeDef *) uart_config[dev].dev)->ROUTELOC0 =
            uart_config[dev].loc;
#endif

        /* enable receive interrupt */
        USART_IntEnable(uart_config[dev].dev, USART_IEN_RXDATAV);

        /* enable it */
        USART_Enable(uart_config[dev].dev, usartEnable);
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
    } else {
        /* enable clocks */
        CMU_ClockEnable(cmuClock_CORELE, true);
        CMU_ClockEnable(uart_config[dev].cmu, true);

        /* initialize device (reset is performed in LEUART_Init) */
        LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;

        init.enable = leuartDisable;
        init.baudrate = baudrate;

        LEUART_Init(uart_config[dev].dev, &init);

        /* configure pins */
#ifdef _SILICON_LABS_32B_PLATFORM_1
        ((LEUART_TypeDef *) uart_config[dev].dev)->ROUTE = (
            uart_config[dev].loc | LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN);
#else
        ((LEUART_TypeDef *) uart_config[dev].dev)->ROUTEPEN =
            LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
        ((LEUART_TypeDef *) uart_config[dev].dev)->ROUTELOC0 =
            uart_config[dev].loc;
#endif

        /* enable receive interrupt */
        LEUART_IntEnable(uart_config[dev].dev, LEUART_IEN_RXDATAV);

        /* enable it */
        LEUART_Enable(uart_config[dev].dev, leuartEnable);
    }
#endif

    /* enable the interrupt */
    NVIC_ClearPendingIRQ(uart_config[dev].irq);
    NVIC_EnableIRQ(uart_config[dev].irq);

    return 0;
}

void uart_write(uart_t dev, const uint8_t *data, size_t len)
{
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
    if (_is_usart(dev)) {
#endif
        while (len--) {
            USART_Tx(uart_config[dev].dev, *(data++));
        }
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
    } else {
        while (len--) {
            LEUART_Tx(uart_config[dev].dev, *(data++));
        }
    }
#endif
}

void uart_poweron(uart_t dev)
{
    CMU_ClockEnable(uart_config[dev].cmu, true);
}

void uart_poweroff(uart_t dev)
{
    CMU_ClockEnable(uart_config[dev].cmu, false);
}

static inline void rx_irq(uart_t dev)
{
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
    if (_is_usart(dev)) {
#endif
        if (USART_IntGet(uart_config[dev].dev) & USART_IF_RXDATAV) {
            isr_ctx[dev].rx_cb(isr_ctx[dev].arg, USART_RxDataGet(uart_config[dev].dev));
        }
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
    } else {
        if (LEUART_IntGet(uart_config[dev].dev) & LEUART_IF_RXDATAV) {
            isr_ctx[dev].rx_cb(isr_ctx[dev].arg, LEUART_RxDataGet(uart_config[dev].dev));
        }
    }
#endif
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef UART_0_ISR_RX
void UART_0_ISR_RX(void)
{
    rx_irq(0);
}
#endif

#ifdef UART_1_ISR_RX
void UART_1_ISR_RX(void)
{
    rx_irq(1);
}
#endif

#ifdef UART_2_ISR_RX
void UART_2_ISR_RX(void)
{
    rx_irq(2);
}
#endif

#ifdef UART_3_ISR_RX
void UART_3_ISR_RX(void)
{
    rx_irq(3);
}
#endif

#ifdef UART_4_ISR_RX
void UART_4_ISR_RX(void)
{
    rx_irq(4);
}
#endif

#endif /* UART_NUMOF */
