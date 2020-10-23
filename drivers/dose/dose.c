/*
 * Copyright (C) 2019 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dose
 * @{
 *
 * @file
 * @brief       Implementation of the Differentially Operated Serial Ethernet driver
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "dose.h"
#include "random.h"
#include "irq.h"

#include "net/eui_provider.h"
#include "net/netdev/eth.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static uint16_t crc16_update(uint16_t crc, uint8_t octet);
static dose_signal_t state_transit_blocked(dose_t *ctx, dose_signal_t signal);
static dose_signal_t state_transit_idle(dose_t *ctx, dose_signal_t signal);
static dose_signal_t state_transit_recv(dose_t *ctx, dose_signal_t signal);
static dose_signal_t state_transit_send(dose_t *ctx, dose_signal_t signal);
static void state(dose_t *ctx, dose_signal_t src);
static void _isr_uart(void *arg, uint8_t c);
static void _isr_gpio(void *arg);
static void _isr_xtimer(void *arg);
static void clear_recv_buf(dose_t *ctx);
static void _isr(netdev_t *netdev);
static int _recv(netdev_t *dev, void *buf, size_t len, void *info);
static uint8_t wait_for_state(dose_t *ctx, uint8_t state);
static int send_octet(dose_t *ctx, uint8_t c);
static int _send(netdev_t *dev, const iolist_t *iolist);
static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len);
static int _set(netdev_t *dev, netopt_t opt, const void *value, size_t len);
static int _init(netdev_t *dev);

static uint16_t crc16_update(uint16_t crc, uint8_t octet)
{
    crc = (uint8_t)(crc >> 8) | (crc << 8);
    crc ^= octet;
    crc ^= (uint8_t)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;
    return crc;
}

static dose_signal_t state_transit_blocked(dose_t *ctx, dose_signal_t signal)
{
    uint32_t backoff;
    (void) signal;

    if (ctx->state == DOSE_STATE_RECV) {
        /* We got here from RECV state. The driver's thread has to look
         * if this frame should be processed. By queuing NETDEV_EVENT_ISR,
         * the netif thread will call _isr at some time. */
        SETBIT(ctx->flags, DOSE_FLAG_RECV_BUF_DIRTY);
        netdev_trigger_event_isr((netdev_t *) ctx);
    }

    if (gpio_is_valid(ctx->sense_pin)) {
        /* Enable GPIO interrupt for start bit sensing */
        gpio_irq_enable(ctx->sense_pin);
    }

    /* The timeout will bring us back into IDLE state by a random time.
     * If we entered this state from RECV state, the random time lays
     * in the interval [1 * timeout, 2 * timeout]. If we came from
     * SEND state, a time in the interval [2 * timeout, 3 * timeout]
     * will be picked. This ensures that responding nodes get preferred
     * bus access and sending nodes do not overwhelm listening nodes. */
    if (ctx->state == DOSE_STATE_SEND) {
        backoff = random_uint32_range(2 * ctx->timeout_base, 3 * ctx->timeout_base);
    }
    else {
        backoff = random_uint32_range(1 * ctx->timeout_base, 2 * ctx->timeout_base);
    }
    xtimer_set(&ctx->timeout, backoff);

    return DOSE_SIGNAL_NONE;
}

static dose_signal_t state_transit_idle(dose_t *ctx, dose_signal_t signal)
{
    (void) ctx;
    (void) signal;

    return DOSE_SIGNAL_NONE;
}

static dose_signal_t state_transit_recv(dose_t *ctx, dose_signal_t signal)
{
    dose_signal_t rc = DOSE_SIGNAL_NONE;

    if (ctx->state != DOSE_STATE_RECV && gpio_is_valid(ctx->sense_pin)) {
        /* We freshly entered this state. Thus, no start bit sensing is required
         * anymore. Disable GPIO IRQs during the transmission. */
        gpio_irq_disable(ctx->sense_pin);
    }

    if (signal == DOSE_SIGNAL_UART) {
        /* We received a new octet */
        int esc = (ctx->flags & DOSE_FLAG_ESC_RECEIVED);
        if (!esc && ctx->uart_octet == DOSE_OCTET_ESC) {
            SETBIT(ctx->flags, DOSE_FLAG_ESC_RECEIVED);
        }
        else if (!esc && ctx->uart_octet == DOSE_OCTET_END) {
            SETBIT(ctx->flags, DOSE_FLAG_END_RECEIVED);
            rc = DOSE_SIGNAL_END;
        }
        else {
            if (esc) {
                CLRBIT(ctx->flags, DOSE_FLAG_ESC_RECEIVED);
            }
            /* Since the dirty flag is set after the RECV state is left,
             * it indicates that the receive buffer contains unprocessed data
             * from a previously received frame. Thus, we just ignore new data. */
            if (!(ctx->flags & DOSE_FLAG_RECV_BUF_DIRTY)
                && ctx->recv_buf_ptr < DOSE_FRAME_LEN) {
                ctx->recv_buf[ctx->recv_buf_ptr++] = ctx->uart_octet;
            }
        }
    }

    if (rc == DOSE_SIGNAL_NONE) {
        /* No signal is returned. We stay in the RECV state. */
        xtimer_set(&ctx->timeout, ctx->timeout_base);
    }

    return rc;
}

static dose_signal_t state_transit_send(dose_t *ctx, dose_signal_t signal)
{
    (void) signal;

    if (ctx->state != DOSE_STATE_SEND && gpio_is_valid(ctx->sense_pin)) {
        /* Disable GPIO IRQs during the transmission. */
        gpio_irq_disable(ctx->sense_pin);
    }

    /* Don't trace any END octets ... the timeout or the END signal
     * will bring us back to the BLOCKED state after _send has emitted
     * its last octet. */

    xtimer_set(&ctx->timeout, ctx->timeout_base);

    return DOSE_SIGNAL_NONE;
}

static void state(dose_t *ctx, dose_signal_t signal)
{
    /* Make sure no other thread or ISR interrupts state transitions */
    unsigned irq_state = irq_disable();

    do {
        /* The edges of the finite state machine can be identified by
         * the current state and the signal that caused a state transition.
         * Since the state only occupies the first 4 bits and the signal the
         * last 4 bits of a uint8_t, they can be added together and hence
         * be checked together. */
        switch (ctx->state + signal) {
            case DOSE_STATE_INIT + DOSE_SIGNAL_INIT:
            case DOSE_STATE_RECV + DOSE_SIGNAL_END:
            case DOSE_STATE_RECV + DOSE_SIGNAL_XTIMER:
            case DOSE_STATE_SEND + DOSE_SIGNAL_END:
            case DOSE_STATE_SEND + DOSE_SIGNAL_XTIMER:
                signal = state_transit_blocked(ctx, signal);
                ctx->state = DOSE_STATE_BLOCKED;
                break;

            case DOSE_STATE_BLOCKED + DOSE_SIGNAL_XTIMER:
                signal = state_transit_idle(ctx, signal);
                ctx->state = DOSE_STATE_IDLE;
                break;

            case DOSE_STATE_IDLE + DOSE_SIGNAL_GPIO:
            case DOSE_STATE_IDLE + DOSE_SIGNAL_UART:
            case DOSE_STATE_BLOCKED + DOSE_SIGNAL_GPIO:
            case DOSE_STATE_BLOCKED + DOSE_SIGNAL_UART:
            case DOSE_STATE_RECV + DOSE_SIGNAL_UART:
                signal = state_transit_recv(ctx, signal);
                ctx->state = DOSE_STATE_RECV;
                break;

            case DOSE_STATE_IDLE + DOSE_SIGNAL_SEND:
            case DOSE_STATE_SEND + DOSE_SIGNAL_UART:
                signal = state_transit_send(ctx, signal);
                ctx->state = DOSE_STATE_SEND;
                break;

            default:
                DEBUG("dose state(): unexpected state transition (STATE=0x%02d SIGNAL=0x%02d)\n", ctx->state, signal);
                signal = DOSE_SIGNAL_NONE;
        }
    } while (signal != DOSE_SIGNAL_NONE);

    /* Indicate state change by unlocking state mutex */
    mutex_unlock(&ctx->state_mtx);
    irq_restore(irq_state);
}

static void _isr_uart(void *arg, uint8_t c)
{
    dose_t *dev = (dose_t *) arg;

    dev->uart_octet = c;
    state(dev, DOSE_SIGNAL_UART);
}

static void _isr_gpio(void *arg)
{
    dose_t *dev = (dose_t *) arg;

    state(dev, DOSE_SIGNAL_GPIO);
}

static void _isr_xtimer(void *arg)
{
    dose_t *dev = (dose_t *) arg;

    state(dev, DOSE_SIGNAL_XTIMER);
}

static void clear_recv_buf(dose_t *ctx)
{
    unsigned irq_state = irq_disable();

    ctx->recv_buf_ptr = 0;
    CLRBIT(ctx->flags, DOSE_FLAG_RECV_BUF_DIRTY);
    CLRBIT(ctx->flags, DOSE_FLAG_END_RECEIVED);
    CLRBIT(ctx->flags, DOSE_FLAG_ESC_RECEIVED);
    irq_restore(irq_state);
}

static void _isr(netdev_t *netdev)
{
    dose_t *ctx = (dose_t *) netdev;
    unsigned irq_state;
    int dirty, end;

    /* Get current flags atomically */
    irq_state = irq_disable();
    dirty = (ctx->flags & DOSE_FLAG_RECV_BUF_DIRTY);
    end = (ctx->flags & DOSE_FLAG_END_RECEIVED);
    irq_restore(irq_state);

    /* If the receive buffer does not contain any data just abort ... */
    if (!dirty) {
        DEBUG("dose _isr(): no frame -> drop\n");
        return;
    }

    /* If we haven't received a valid END octet just drop the incomplete frame. */
    if (!end) {
        DEBUG("dose _isr(): incomplete frame -> drop\n");
        clear_recv_buf(ctx);
        return;
    }

    /* The set dirty flag prevents recv_buf or recv_buf_ptr from being
     * touched in ISR context. Thus, it is safe to work with them without
     * IRQs being disabled or mutexes being locked. */

    /* Check for minimum length of an Ethernet packet */
    if (ctx->recv_buf_ptr < sizeof(ethernet_hdr_t) + DOSE_FRAME_CRC_LEN) {
        DEBUG("dose _isr(): frame too short -> drop\n");
        clear_recv_buf(ctx);
        return;
    }

    /* Check the dst mac addr if the iface is not in promiscuous mode */
    if (!(ctx->opts & DOSE_OPT_PROMISCUOUS)) {
        ethernet_hdr_t *hdr = (ethernet_hdr_t *) ctx->recv_buf;
        if ((hdr->dst[0] & 0x1) == 0 && memcmp(hdr->dst, ctx->mac_addr.uint8, ETHERNET_ADDR_LEN) != 0) {
            DEBUG("dose _isr(): dst mac not matching -> drop\n");
            clear_recv_buf(ctx);
            return;
        }
    }

    /* Check the CRC */
    uint16_t crc = 0xffff;
    for (size_t i = 0; i < ctx->recv_buf_ptr; i++) {
        crc = crc16_update(crc, ctx->recv_buf[i]);
    }
    if (crc != 0x0000) {
        DEBUG("dose _isr(): wrong crc 0x%04x -> drop\n", crc);
        clear_recv_buf(ctx);
        return;
    }

    /* Finally schedule a _recv method call */
    DEBUG("dose _isr(): NETDEV_EVENT_RX_COMPLETE\n");
    ctx->netdev.event_callback((netdev_t *) ctx, NETDEV_EVENT_RX_COMPLETE);
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    dose_t *ctx = (dose_t *) dev;

    (void)info;

    size_t pktlen = ctx->recv_buf_ptr - DOSE_FRAME_CRC_LEN;
    if (!buf && !len) {
        /* Return the amount of received bytes */
        return pktlen;
    }
    else if (!buf && len) {
        /* The user drops the packet */
        clear_recv_buf(ctx);
        return pktlen;
    }
    else if (len < pktlen) {
        /* The provided buffer is too small! */
        DEBUG("dose _recv(): receive buffer too small\n");
        clear_recv_buf(ctx);
        return -1;
    }
    else {
        /* Copy the packet to the provided buffer. */
        memcpy(buf, ctx->recv_buf, pktlen);
        clear_recv_buf(ctx);
        return pktlen;
    }
}

static uint8_t wait_for_state(dose_t *ctx, uint8_t state)
{
    do {
        /* This mutex is unlocked by the state machine
         * after every state transition */
        mutex_lock(&ctx->state_mtx);
    } while (state != DOSE_STATE_ANY && ctx->state != state);
    return ctx->state;
}

static int send_octet(dose_t *ctx, uint8_t c)
{
    uart_write(ctx->uart, (uint8_t *) &c, sizeof(c));

    /* Wait for a state transition */
    uint8_t state = wait_for_state(ctx, DOSE_STATE_ANY);
    if (state != DOSE_STATE_SEND) {
        /* Timeout */
        DEBUG("dose send_octet(): timeout\n");
        return -2;
    }
    else if (ctx->uart_octet != c) {
        /* Mismatch */
        DEBUG("dose send_octet(): mismatch\n");
        return -1;
    }

    return 0;
}

static int send_data_octet(dose_t *ctx, uint8_t c)
{
    int rc;

    /* Escape special octets */
    if (c == DOSE_OCTET_ESC || c == DOSE_OCTET_END) {
        rc = send_octet(ctx, DOSE_OCTET_ESC);
        if (rc) {
            return rc;
        }
    }

    /* Send data octet */
    rc = send_octet(ctx, c);

    return rc;
}

static int _send(netdev_t *dev, const iolist_t *iolist)
{
    dose_t *ctx = (dose_t *) dev;
    int8_t retries = 3;
    size_t pktlen;
    uint16_t crc;

send:
    crc = 0xffff;
    pktlen = 0;

    /* Switch to state SEND */
    do {
        wait_for_state(ctx, DOSE_STATE_IDLE);
        state(ctx, DOSE_SIGNAL_SEND);
    } while (wait_for_state(ctx, DOSE_STATE_ANY) != DOSE_STATE_SEND);

    /* Send packet buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        size_t n = iol->iol_len;
        pktlen += n;
        uint8_t *ptr = iol->iol_base;
        while (n--) {
            /* Send data octet */
            if (send_data_octet(ctx, *ptr)) {
                goto collision;
            }

            /* Update CRC */
            crc = crc16_update(crc, *ptr);

            ptr++;
        }
    }

    /* Send CRC */
    network_uint16_t crc_nw = byteorder_htons(crc);
    if (send_data_octet(ctx, crc_nw.u8[0])) {
        goto collision;
    }
    if (send_data_octet(ctx, crc_nw.u8[1])) {
        goto collision;
    }

    /* Send END octet */
    if (send_octet(ctx, DOSE_OCTET_END)) {
        goto collision;
    }

    /* We probably sent the whole packet?! */
    ctx->netdev.event_callback((netdev_t *) ctx, NETDEV_EVENT_TX_COMPLETE);

    /* Get out of the SEND state */
    state(ctx, DOSE_SIGNAL_END);

    return pktlen;

collision:
    DEBUG("dose _send(): collision!\n");
    if (--retries < 0) {
        ctx->netdev.event_callback((netdev_t *) ctx, NETDEV_EVENT_TX_MEDIUM_BUSY);
        return 0;
    }
    goto send;
}

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    dose_t *ctx = (dose_t *) dev;

    switch (opt) {
        case NETOPT_ADDRESS:
            if (max_len < ETHERNET_ADDR_LEN) {
                return -EINVAL;
            }
            memcpy(value, ctx->mac_addr.uint8, ETHERNET_ADDR_LEN);
            return ETHERNET_ADDR_LEN;
        case NETOPT_PROMISCUOUSMODE:
            if (max_len < sizeof(netopt_enable_t)) {
                return -EINVAL;
            }
            if (ctx->opts & DOSE_OPT_PROMISCUOUS) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);
        default:
            return netdev_eth_get(dev, opt, value, max_len);
    }

    return 0;
}

static int _set(netdev_t *dev, netopt_t opt, const void *value, size_t len)
{
    dose_t *ctx = (dose_t *) dev;

    switch (opt) {
        case NETOPT_ADDRESS:
            if (len < ETHERNET_ADDR_LEN) {
                return -EINVAL;
            }
            memcpy(ctx->mac_addr.uint8, value, ETHERNET_ADDR_LEN);
            return ETHERNET_ADDR_LEN;
        case NETOPT_PROMISCUOUSMODE:
            if (len < sizeof(netopt_enable_t)) {
                return -EINVAL;
            }
            if (((const bool *)value)[0]) {
                SETBIT(ctx->opts, DOSE_OPT_PROMISCUOUS);
            }
            else {
                CLRBIT(ctx->opts, DOSE_OPT_PROMISCUOUS);
            }
            return sizeof(netopt_enable_t);
        default:
            return netdev_eth_set(dev, opt, value, len);
    }

    return 0;
}

static int _init(netdev_t *dev)
{
    dose_t *ctx = (dose_t *) dev;
    unsigned irq_state;

    /* Set state machine to defaults */
    irq_state = irq_disable();
    ctx->opts = 0;
    ctx->recv_buf_ptr = 0;
    ctx->flags = 0;
    ctx->state = DOSE_STATE_INIT;
    irq_restore(irq_state);

    state(ctx, DOSE_SIGNAL_INIT);

    return 0;
}

static const netdev_driver_t netdev_driver_dose = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set
};

void dose_setup(dose_t *ctx, const dose_params_t *params, uint8_t index)
{
    static const xtimer_ticks32_t min_timeout = {.ticks32 = XTIMER_BACKOFF};

    ctx->netdev.driver = &netdev_driver_dose;

    mutex_init(&ctx->state_mtx);

    ctx->uart = params->uart;
    uart_init(ctx->uart, params->baudrate, _isr_uart, (void *) ctx);

    ctx->sense_pin = params->sense_pin;
    if (gpio_is_valid(ctx->sense_pin)) {
        gpio_init_int(ctx->sense_pin, GPIO_IN, GPIO_FALLING, _isr_gpio, (void *) ctx);
        gpio_irq_disable(ctx->sense_pin);
    }

    netdev_register(&ctx->netdev, NETDEV_DOSE, index);

    assert(sizeof(ctx->mac_addr.uint8) == ETHERNET_ADDR_LEN);
    netdev_eui48_get(&ctx->netdev, &ctx->mac_addr);
    DEBUG("dose dose_setup(): mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
          ctx->mac_addr.uint8[0], ctx->mac_addr.uint8[1], ctx->mac_addr.uint8[2],
          ctx->mac_addr.uint8[3], ctx->mac_addr.uint8[4], ctx->mac_addr.uint8[5]
          );

    /* The timeout base is the minimal timeout base used for this driver.
     * We have to ensure it is above the XTIMER_BACKOFF. Otherwise state
     * transitions are triggered from another state transition setting up the
     * timeout. */
    ctx->timeout_base = CONFIG_DOSE_TIMEOUT_USEC;
    if (ctx->timeout_base < xtimer_usec_from_ticks(min_timeout)) {
        ctx->timeout_base = xtimer_usec_from_ticks(min_timeout);
    }
    ctx->timeout.callback = _isr_xtimer;
    ctx->timeout.arg = ctx;
}
