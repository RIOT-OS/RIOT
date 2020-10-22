/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_soft_uart
 * @{
 *
 * @file
 * @brief       Software UART implementation
 *
 * @author      Benjamin Valentin <benjjamin.valentin@ml-pa.com>
 */

#include <stdio.h>

#include "mutex.h"
#include "soft_uart.h"
#include "soft_uart_params.h"

enum {
    STATE_RX_IDLE,
    STATE_RX_HIGH,
    STATE_RX_LOW
};

enum {
    PARITY_NONE,
    PARITY_EVEN,
    PARITY_ODD,
    PARITY_MARK,
    PARITY_SPACE,
};

struct uart_ctx {
    mutex_t lock;       /**< UART mutex          */
    mutex_t sync;       /**< TX byte done signal */
    uart_rx_cb_t rx_cb; /**< RX callback         */
    void* rx_cb_arg;    /**< RX callback arg     */
    uint32_t bit_time;  /**< timer ticks per bit */
    uint16_t byte_tx;   /**< current TX byte     */
    uint16_t byte_rx;   /**< curretn RX byte     */
    uint8_t bits_tx;    /**< TX bit pos          */
    uint8_t state_rx;   /**< RX state            */
#ifdef MODULE_SOFT_UART_MODECFG
    uint8_t data_bits;  /**< number of data bits */
    uint8_t stop_bits;  /**< number of stop bits */
    uint8_t parity;     /**< parity mode         */
#endif
} soft_uart_ctx[SOFT_UART_NUMOF];

#ifdef MODULE_SOFT_UART_MODECFG
#define BITS_DATA(ctx)      (ctx)->data_bits
#define BITS_STOP(ctx)      (ctx)->stop_bits
#define BITS_PARITY(ctx)    ((ctx)->parity != PARITY_NONE)
#else
#define BITS_DATA(ctx)      8
#define BITS_STOP(ctx)      1
#define BITS_PARITY(ctx)    0
#endif

static void _tx_timer_cb(void *arg, int chan)
{
    soft_uart_t uart = (soft_uart_t)arg;

    const soft_uart_conf_t *cfg = &soft_uart_config[uart];
    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    gpio_write(cfg->tx_pin, ctx->byte_tx & 1);
    ctx->byte_tx >>= 1;

    if (--ctx->bits_tx == 0) {
        timer_clear(cfg->tx_timer, chan);
        mutex_unlock(&ctx->sync);
    }
}

static void _rx_timer_cb(void *arg, int chan)
{
    soft_uart_t uart = (soft_uart_t)arg;

    const soft_uart_conf_t *cfg = &soft_uart_config[uart];
    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    (void)chan;

    timer_stop(cfg->rx_timer);

    /* ignore spurious interrupts */
    if (ctx->state_rx == STATE_RX_IDLE) {
        return;
    }

    ctx->state_rx = STATE_RX_IDLE;
    ctx->rx_cb(ctx->rx_cb_arg, ctx->byte_rx);
}

static void _rx_gpio_cb(void *arg)
{
    soft_uart_t uart = (soft_uart_t)arg;

    const soft_uart_conf_t *cfg = &soft_uart_config[uart];
    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    /* TODO: use Timer Capture feature */
    const uint32_t now = timer_read(cfg->rx_timer);

    if (ctx->state_rx == STATE_RX_IDLE) {
        timer_start(cfg->rx_timer);
        ctx->state_rx = STATE_RX_LOW;
        ctx->byte_rx = 0;
        return;
    }

    /* we only get interrupts on flanks, so all bits
     * till the next interrupt will have the same level. */
    uint8_t bit = now / ctx->bit_time;
    uint8_t mask = 0xff << bit;

    if (ctx->state_rx == STATE_RX_HIGH) {
        ctx->byte_rx &= ~mask;
        ctx->state_rx = STATE_RX_LOW;
    } else {
        ctx->byte_rx |= mask;
        ctx->state_rx = STATE_RX_HIGH;
    }
}

int soft_uart_init(soft_uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= SOFT_UART_NUMOF) {
        return UART_NODEV;
    }

    const soft_uart_conf_t *cfg = &soft_uart_config[uart];
    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    mutex_init(&ctx->lock);
    static const mutex_t init_locked = MUTEX_INIT_LOCKED;
    ctx->sync = init_locked;

    ctx->bit_time = (cfg->timer_freq + baudrate / 2) / baudrate;

    unsigned accuracy = (100 * cfg->timer_freq / ctx->bit_time) / baudrate;
    if (accuracy > 110 || accuracy < 90) {
        return UART_NOBAUD;
    }

    if (!gpio_is_valid(cfg->rx_pin)) {
        rx_cb = NULL;
    }

#ifdef MODULE_SOFT_UART_MODECFG
    ctx->data_bits = 8;
    ctx->stop_bits = 1;
    ctx->parity    = PARITY_NONE;
#endif

    ctx->rx_cb = rx_cb;
    ctx->rx_cb_arg = arg;

    ctx->state_rx = STATE_RX_IDLE;

    if (gpio_is_valid(cfg->tx_pin)) {
        timer_init(cfg->tx_timer, cfg->timer_freq, _tx_timer_cb, (void *)uart);
        gpio_write(cfg->tx_pin, !(cfg->flags & SOFT_UART_FLAG_INVERT_TX));
        gpio_init(cfg->tx_pin, GPIO_OUT);
    }

    if (rx_cb) {
        timer_init(cfg->rx_timer, cfg->timer_freq, _rx_timer_cb, (void *)uart);
        timer_stop(cfg->rx_timer);
        /* timer should fire at the end of the byte */
        timer_set_periodic(cfg->rx_timer, 0, ctx->bit_time * (BITS_DATA(ctx) + BITS_PARITY(ctx) + 1),
                           TIM_FLAG_RESET_ON_MATCH | TIM_FLAG_RESET_ON_SET);
        gpio_init_int(cfg->rx_pin, GPIO_IN, GPIO_BOTH, _rx_gpio_cb, (void*) uart);
    }

    return 0;
}

#ifdef MODULE_SOFT_UART_MODECFG
int soft_uart_mode(soft_uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
                   uart_stop_bits_t stop_bits)
{
    if (uart >= SOFT_UART_NUMOF) {
        return UART_NODEV;
    }

    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    switch (data_bits) {
    case UART_DATA_BITS_5:
        ctx->data_bits = 5;
        break;
    case UART_DATA_BITS_6:
        ctx->data_bits = 6;
        break;
    case UART_DATA_BITS_7:
        ctx->data_bits = 7;
        break;
    default:
    case UART_DATA_BITS_8:
        ctx->data_bits = 8;
        break;
    }

    switch (parity) {
    case UART_PARITY_EVEN:
        ctx->parity = PARITY_EVEN;
        break;
    case UART_PARITY_ODD:
        ctx->parity = PARITY_ODD;
        break;
    case UART_PARITY_MARK:
        ctx->parity = PARITY_MARK;
        break;
    case UART_PARITY_SPACE:
        ctx->parity = PARITY_SPACE;
        break;
    default:
    case UART_PARITY_NONE:
        ctx->parity = PARITY_NONE;
        break;
    }

    switch (stop_bits) {
    case UART_STOP_BITS_2:
        ctx->stop_bits = 2;
        break;
    default:
    case UART_STOP_BITS_1:
        ctx->stop_bits = 1;
        break;
    }

    return 0;
}
#endif /* MODULE_SOFT_UART_MODECF */

static void soft_uart_write_byte(soft_uart_t uart, uint8_t data)
{
    const soft_uart_conf_t *cfg = &soft_uart_config[uart];
    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    /* start bit (LOW) + data bits */
    ctx->bits_tx = 1 + BITS_DATA(ctx);
    ctx->byte_tx = data << 1;

#ifdef MODULE_SOFT_UART_MODECFG
    if (ctx->parity != PARITY_NONE) {
        uint8_t parity = 0;

        switch (ctx->parity) {
        case PARITY_EVEN:
            parity = __builtin_parity(data);
            break;
        case PARITY_ODD:
            parity = !__builtin_parity(data);
            break;
        case PARITY_MARK:
            parity = 1;
            break;
        case PARITY_SPACE:
            parity = 0;
            break;
        }

        ctx->byte_tx |= parity << ctx->bits_tx++;
    }
#endif

    for (int i = 0; i < BITS_STOP(ctx); ++i) {
        ctx->byte_tx |= 1 << ctx->bits_tx++;
    }

    if (cfg->flags & SOFT_UART_FLAG_INVERT_TX) {
        ctx->byte_tx = ~ctx->byte_tx;
    }

    timer_set_periodic(cfg->tx_timer, 0, ctx->bit_time,
                       TIM_FLAG_RESET_ON_MATCH | TIM_FLAG_RESET_ON_SET);
    mutex_lock(&ctx->sync);
}

void soft_uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    const soft_uart_conf_t *cfg = &soft_uart_config[uart];
    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    const uint8_t *end = data + len;

    mutex_lock(&ctx->lock);
    timer_start(cfg->tx_timer);

    while (data != end) {
        soft_uart_write_byte(uart, *data++);
    }

    timer_stop(cfg->tx_timer);
    mutex_unlock(&soft_uart_ctx[uart].lock);
}

void soft_uart_poweron(soft_uart_t uart)
{
    const soft_uart_conf_t *cfg = &soft_uart_config[uart];
    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    if (ctx->rx_cb) {
        gpio_irq_enable(cfg->rx_pin);
    }
}

void soft_uart_poweroff(soft_uart_t uart)
{
    const soft_uart_conf_t *cfg = &soft_uart_config[uart];
    struct uart_ctx *ctx = &soft_uart_ctx[uart];

    if (ctx->rx_cb) {
        gpio_irq_disable(cfg->rx_pin);
    }

    /* timers are already stopped after RX/TX */
}
