/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ryan Kurte <ryankurte@gmail.com>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include "cpu.h"

#include "periph/uart.h"
#include "periph/gpio.h"

#include "em_usart.h"
#include "em_usart_utils.h"
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
#include "em_leuart.h"
#include "em_leuart_utils.h"
#endif

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
    gpio_init(uart_config[dev].rx_pin, GPIO_IN);
    gpio_init(uart_config[dev].tx_pin, GPIO_OUT);

    /* initialize the UART/USART/LEUART device */
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
    if (_is_usart(dev)) {
#endif
        USART_TypeDef *uart = (USART_TypeDef *) uart_config[dev].dev;

        /* enable clocks */
        CMU_ClockEnable(cmuClock_HFPER, true);
        CMU_ClockEnable(uart_config[dev].cmu, true);

        /* reset and initialize peripheral */
        USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

        init.enable = usartDisable;
        init.baudrate = baudrate;
#if EFM32_UART_MODES
        init.databits = USART_DataBits2Def((uart_config[dev].mode >> 0) & 0xf);
        init.stopbits = USART_StopBits2Def((uart_config[dev].mode >> 4) & 0xf);
        init.parity = USART_Parity2Def((uart_config[dev].mode >> 8) & 0xf);
#endif

        USART_InitAsync(uart, &init);

        /* configure pin functions */
#ifdef _SILICON_LABS_32B_SERIES_0
        uart->ROUTE = (uart_config[dev].loc |
                       USART_ROUTE_RXPEN |
                       USART_ROUTE_TXPEN);
#else
        uart->ROUTELOC0 = uart_config[dev].loc;
        uart->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
#endif

        /* enable receive interrupt */
        USART_IntEnable(uart, USART_IEN_RXDATAV);

        /* enable peripheral */
        USART_Enable(uart, usartEnable);
#if LOW_POWER_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
    } else {
        LEUART_TypeDef *leuart = (LEUART_TypeDef *) uart_config[dev].dev;

        /* enable clocks */
        CMU_ClockEnable(cmuClock_CORELE, true);
        CMU_ClockEnable(uart_config[dev].cmu, true);

        /* reset and initialize peripheral */
        LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;

        init.enable = leuartDisable;
        init.baudrate = baudrate;
#if EFM32_UART_MODES
        init.databits = LEUART_DataBits2Def((uart_config[dev].mode >> 0) & 0xf);
        init.stopbits = LEUART_StopBits2Def((uart_config[dev].mode >> 4) & 0xf);
        init.parity = LEUART_Parity2Def((uart_config[dev].mode >> 8) & 0xf);
#endif

        LEUART_Init(leuart, &init);

        /* configure pin functions */
#ifdef _SILICON_LABS_32B_SERIES_0
        leuart->ROUTE = (uart_config[dev].loc |
                         LEUART_ROUTE_RXPEN |
                         LEUART_ROUTE_TXPEN);
#else
        leuart->ROUTELOC0 = uart_config[dev].loc;
        leuart->ROUTEPEN = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
#endif

        /* enable receive interrupt */
        LEUART_IntEnable(leuart, LEUART_IEN_RXDATAV);

        /* enable peripheral */
        LEUART_Enable(leuart, leuartEnable);
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

static void rx_irq(uart_t dev)
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
    cortexm_isr_end();
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
