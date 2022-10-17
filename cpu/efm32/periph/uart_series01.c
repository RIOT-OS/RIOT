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

/**
 * @brief   Defines whether LEUART is enabled and supported
 */
#if EFM32_LEUART_ENABLED && defined(LEUART_COUNT) && LEUART_COUNT > 0
#define USE_LEUART
#endif

#ifdef USE_LEUART
#include "em_leuart.h"
#include "em_leuart_utils.h"
#endif

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

#ifdef USE_LEUART
/**
 * @brief   Check if device is a U(S)ART device.
 */
static inline bool _is_usart(uart_t dev)
{
    return ((uint32_t) uart_config[dev].dev) < LEUART0_BASE;
}
#endif

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
    gpio_init(uart_config[dev].rx_pin, GPIO_IN_PU);
    gpio_init(uart_config[dev].tx_pin, GPIO_OUT | 1); /* 1 for high */

    /* initialize the UART/USART/LEUART device */
#ifdef USE_LEUART
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

        USART_InitAsync(uart, &init);

        /* configure pin functions */
#if defined(_SILICON_LABS_32B_SERIES_0)
        uart->ROUTE = (uart_config[dev].loc |
                       USART_ROUTE_RXPEN |
                       USART_ROUTE_TXPEN);
#elif defined(_SILICON_LABS_32B_SERIES_1)
        uart->ROUTELOC0 = uart_config[dev].loc;
        uart->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
#endif

        /* enable receive interrupt */
        USART_IntEnable(uart, USART_IEN_RXDATAV);

#ifdef USE_LEUART
    } else {
        LEUART_TypeDef *leuart = (LEUART_TypeDef *) uart_config[dev].dev;

        /* enable clocks */
        CMU_ClockEnable(cmuClock_CORELE, true);
        CMU_ClockEnable(uart_config[dev].cmu, true);

        /* reset and initialize peripheral */
        LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;

        init.enable = leuartDisable;
        init.baudrate = baudrate;

        LEUART_Init(leuart, &init);

        /* configure pin functions */
#if defined(_SILICON_LABS_32B_SERIES_0)
        leuart->ROUTE = (uart_config[dev].loc |
                         LEUART_ROUTE_RXPEN |
                         LEUART_ROUTE_TXPEN);
#elif defined(_SILICON_LABS_32B_SERIES_1)
        leuart->ROUTELOC0 = uart_config[dev].loc;
        leuart->ROUTEPEN = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
#endif

        /* enable receive interrupt */
        LEUART_IntEnable(leuart, LEUART_IEN_RXDATAV);
    }
#endif

    /* enable the interrupt */
    if (rx_cb) {
        NVIC_ClearPendingIRQ(uart_config[dev].irq);
        NVIC_EnableIRQ(uart_config[dev].irq);
    }

    uart_poweron(dev);

    return 0;
}

#ifdef MODULE_PERIPH_UART_MODECFG
int uart_mode(uart_t dev, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    if (parity == UART_PARITY_MARK || parity == UART_PARITY_SPACE) {
        return UART_NOMODE;
    }

#ifdef USE_LEUART
    if (_is_usart(dev)) {
#endif
        USART_TypeDef *uart = (USART_TypeDef *) uart_config[dev].dev;

        USART_FrameSet(uart,
                       USART_DataBits2Def(data_bits),
                       USART_StopBits2Def(stop_bits),
                       USART_Parity2Def(parity));
#ifdef USE_LEUART
    } else {
        if (data_bits != UART_DATA_BITS_8) {
            return UART_NOMODE;
        }

        LEUART_TypeDef *leuart = (LEUART_TypeDef *) uart_config[dev].dev;

        LEUART_FrameSet(leuart,
                        LEUART_DataBits2Def(data_bits),
                        LEUART_StopBits2Def(stop_bits),
                        LEUART_Parity2Def(parity));
    }
#endif

    return UART_OK;
}
#endif

void uart_write(uart_t dev, const uint8_t *data, size_t len)
{
#ifdef USE_LEUART
    if (_is_usart(dev)) {
#endif
        USART_TypeDef *usart = uart_config[dev].dev;

        while (len--) {
            USART_Tx(usart, *(data++));
        }

        /* spin until transmission is complete */
        while (!(usart->STATUS & USART_STATUS_TXC)) {}

#ifdef USE_LEUART
    } else {
        LEUART_TypeDef *leuart = uart_config[dev].dev;

        while (len--) {
            LEUART_Tx(leuart, *(data++));
        }

        /* spin until transmission is complete */
        while (!(leuart->STATUS & LEUART_STATUS_TXC)) {}
    }
#endif
}

void uart_poweron(uart_t dev)
{
    CMU_ClockEnable(uart_config[dev].cmu, true);

#ifdef USE_LEUART
    if (_is_usart(dev)) {
#endif
        USART_TypeDef *usart = uart_config[dev].dev;

        /* enable tx */
        USART_Enable_TypeDef enable = usartEnableTx;

        /* enable rx if needed */
        if (isr_ctx[dev].rx_cb) {
            enable |= usartEnableRx;
        }

        USART_Enable(usart, enable);
#ifdef USE_LEUART
    }
    else {
        LEUART_TypeDef *leuart = uart_config[dev].dev;

        /* enable tx */
        LEUART_Enable_TypeDef enable = leuartEnableTx;

        /* enable rx if needed */
        if (isr_ctx[dev].rx_cb) {
            enable |= leuartEnableRx;
        }

        LEUART_Enable(leuart, enable);
    }
#endif
}

void uart_poweroff(uart_t dev)
{
#ifdef USE_LEUART
    if (_is_usart(dev)) {
#endif
        USART_TypeDef *usart = uart_config[dev].dev;

        /* disable tx and rx */
        USART_Enable(usart, usartDisable);
#ifdef USE_LEUART
    }
    else {
        LEUART_TypeDef *leuart = uart_config[dev].dev;

        /* disable tx and rx */
        LEUART_Enable(leuart, leuartDisable);
    }
#endif

    CMU_ClockEnable(uart_config[dev].cmu, false);
}

static void rx_irq(uart_t dev)
{
#ifdef USE_LEUART
    if (_is_usart(dev)) {
#endif
        if (USART_IntGet(uart_config[dev].dev) & USART_IF_RXDATAV) {
            uint8_t c = USART_RxDataGet(uart_config[dev].dev);
            if (isr_ctx[dev].rx_cb) {
                isr_ctx[dev].rx_cb(isr_ctx[dev].arg, c);
            }
        }
#ifdef USE_LEUART
    } else {
        if (LEUART_IntGet(uart_config[dev].dev) & LEUART_IF_RXDATAV) {
            uint8_t c = LEUART_RxDataGet(uart_config[dev].dev);
            if (isr_ctx[dev].rx_cb) {
                isr_ctx[dev].rx_cb(isr_ctx[dev].arg, c);
            }
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
