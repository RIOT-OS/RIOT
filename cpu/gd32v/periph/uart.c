/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           GD32V UART peripheral implementation
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 */
#include <assert.h>
#include <stdint.h>

#include "periph/uart.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "cpu.h"
#include "clic.h"

#define RXENABLE            (USART_CTL0_REN_Msk | USART_CTL0_RBNEIE_Msk)

/**
 * @brief   Allocate memory to store the callback functions
 *
 * Extend standard uart_isr_ctx_t with data_mask field. This is needed
 * in order to mask parity bit.
 */
static struct {
    uart_rx_cb_t rx_cb;     /**< data received interrupt callback */
    void *arg;              /**< argument to both callback routines */
    uint8_t data_mask;      /**< mask applied to the data register */
} isr_ctx[UART_NUMOF];

static inline void _uart_isr(uart_t uart);

static inline USART_Type *dev(uart_t uart)
{
    return uart_config[uart].dev;
}

static inline void uart_init_pins(uart_t uart, uart_rx_cb_t rx_cb)
{
    /* configure TX pin */
    gpio_init_af(uart_config[uart].tx_pin, GPIO_AF_OUT_PP);
    /* configure RX pin */
    if (rx_cb) {
        gpio_init(uart_config[uart].rx_pin, GPIO_IN_PU);
    }
}

static inline void uart_enable_clock(uart_t uart)
{
    /* TODO: add pm blocker */
    periph_clk_en(uart_config[uart].bus, uart_config[uart].rcu_mask);
}

static inline void uart_disable_clock(uart_t uart)
{
    periph_clk_dis(uart_config[uart].bus, uart_config[uart].rcu_mask);
    /* TODO remove pm blocker */
}

static inline void uart_init_usart(uart_t uart, uint32_t baudrate)
{
    uint16_t mantissa;
    uint8_t fraction;
    uint32_t clk;

    /* calculate and apply baudrate */
    clk = periph_apb_clk(uart_config[uart].bus) / baudrate;
    mantissa = (uint16_t)(clk / 16);
    fraction = (uint8_t)(clk - (mantissa * 16));
    dev(uart)->BAUD = ((mantissa & 0x0fff) << 4) | (fraction & 0x0f);
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    /* save ISR context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg = arg;
    isr_ctx[uart].data_mask = 0xFF;

    uart_enable_clock(uart);

    /* reset UART configuration -> defaults to 8N1 mode */
    dev(uart)->CTL0 = 0;
    dev(uart)->CTL1 = 0;
    dev(uart)->CTL2 = 0;

    uart_init_usart(uart, baudrate);

    /* Attach pins to enabled UART periph. Note: It is important that the UART
     * interface is configured prior to attaching the pins, as otherwise the
     * signal level flickers during initialization resulting in garbage being
     * sent. */
    uart_init_pins(uart, rx_cb);

    /* enable RX interrupt if applicable */
    if (rx_cb) {
        clic_set_handler(uart_config[uart].irqn, _uart_isr);
        clic_enable_interrupt(uart_config[uart].irqn, CPU_DEFAULT_IRQ_PRIO);
        dev(uart)->CTL0 = (USART_CTL0_UEN_Msk | USART_CTL0_TEN_Msk | RXENABLE);
    }
    else {
        dev(uart)->CTL0 = (USART_CTL0_UEN_Msk | USART_CTL0_TEN_Msk);
    }

    return UART_OK;
}

#ifdef MODULE_PERIPH_UART_MODECFG
int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    assert(uart < UART_NUMOF);

    isr_ctx[uart].data_mask = 0xFF;

    if (parity) {
        switch (data_bits) {
        case UART_DATA_BITS_6:
            data_bits = UART_DATA_BITS_7;
            isr_ctx[uart].data_mask = 0x3F;
            break;
        case UART_DATA_BITS_7:
            data_bits = UART_DATA_BITS_8;
            isr_ctx[uart].data_mask = 0x7F;
            break;
        case UART_DATA_BITS_8:
#ifdef USART_CTL0_M0
            data_bits = USART_CTL0_M0;
#else
            data_bits = USART_CTL0_M;
#endif
            break;
        default:
            return UART_NOMODE;
        }
    }
    if ((data_bits & UART_INVALID_MODE) || (parity & UART_INVALID_MODE)) {
        return UART_NOMODE;
    }

#ifdef USART_CTL0_M1
    if (!(dev(uart)->ISR & USART_ISR_TC)) {
        return UART_INTERR;
    }
    dev(uart)->CTL0 &= ~(USART_CTL0_UEN_Msk | USART_CTL0_TEN_Msk);
#endif

    dev(uart)->CTL1 &= ~USART_CTL1_STOP;
    dev(uart)->CTL0 &= ~(USART_CTL0_PS | USART_CTL0_PCE | USART_CTL0_M);

    dev(uart)->CTL1 |= stop_bits;
    dev(uart)->CTL0 |= (USART_CTL0_UE | USART_CTL0_TE | data_bits | parity);

    return UART_OK;
}
#endif /* MODULE_PERIPH_UART_MODECFG */

static inline void send_byte(uart_t uart, uint8_t byte)
{
    while (!(dev(uart)->STAT & USART_STAT_TBE_Msk)) {}
    dev(uart)->DATA = byte;
}

static inline void wait_for_tx_complete(uart_t uart)
{
    while (!(dev(uart)->STAT & USART_STAT_TC_Msk)) {}
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);
#if DEVELHELP
    /* If tx is not enabled don't try to send */
    if (!(dev(uart)->CTL0 & USART_CTL0_TEN_Msk)) {
        return;
    }
#endif
    for (size_t i = 0; i < len; i++) {
        send_byte(uart, data[i]);
    }
    /* make sure the function is synchronous by waiting for the transfer to
     * finish */
    wait_for_tx_complete(uart);
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    uart_enable_clock(uart);

    dev(uart)->CTL0 |= (USART_CTL0_UEN_Msk);
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    dev(uart)->CTL0 &= ~(USART_CTL0_UEN_Msk);

    uart_disable_clock(uart);
}

static inline void _irq_handler(uart_t uart)
{
    uint32_t status = dev(uart)->STAT;

    if (status & USART_STAT_RBNE_Msk) {
        isr_ctx[uart].rx_cb(isr_ctx[uart].arg,
                            (uint8_t)dev(uart)->DATA & isr_ctx[uart].data_mask);
    }
    if (status & USART_STAT_ORERR_Msk) {
        dev(uart)->DATA;
    }
}

static void _uart_isr(unsigned irq)
{
    switch (irq) {
#ifdef UART_0_IRQN
    case UART_0_IRQN:
        _irq_handler(UART_DEV(0));
        break;
#endif
#ifdef UART_1_IRQN
    case UART_1_IRQN:
        _irq_handler(UART_DEV(1));
        break;
#endif
#ifdef UART_2_IRQN
    case UART_2_IRQN:
        _irq_handler(UART_DEV(2));
        break;
#endif
#ifdef UART_3_IRQN
    case UART_3_IRQN:
        _irq_handler(UART_DEV(3));
        break;
#endif
#ifdef UART_4_IRQN
    case UART_4_IRQN:
        _irq_handler(UART_DEV(4));
        break;
#endif
#ifdef UART_5_IRQN
    case UART_5_IRQN:
        _irq_handler(UART_DEV(5));
        break;
#endif
    default:
        assert(false);
    }
}
