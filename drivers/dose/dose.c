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
 *              Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>
#include <string.h>

#include "board.h"
#include "dose.h"
#include "random.h"
#include "irq.h"
#include "periph/timer.h"

#include "checksum/crc16_ccitt.h"
#include "net/eui_provider.h"
#include "net/netdev/eth.h"
#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if !defined(DOSE_TIMER_DEV) && IS_ACTIVE(MODULE_DOSE_WATCHDOG)
#error "DOSE_TIMER_DEV needs to be set by the board"
#endif

static dose_signal_t state_transit_blocked(dose_t *ctx, dose_signal_t signal);
static dose_signal_t state_transit_idle(dose_t *ctx, dose_signal_t signal);
static dose_signal_t state_transit_recv(dose_t *ctx, dose_signal_t signal);
static dose_signal_t state_transit_send(dose_t *ctx, dose_signal_t signal);
static void state(dose_t *ctx, dose_signal_t src);
static void _isr_uart(void *arg, uint8_t c);
static void _isr_gpio(void *arg);
static void _isr_ztimer(void *arg);
static void clear_recv_buf(dose_t *ctx);
static void _isr(netdev_t *netdev);
static int _recv(netdev_t *dev, void *buf, size_t len, void *info);
static uint8_t wait_for_state(dose_t *ctx, uint8_t state);
static int send_octet(dose_t *ctx, uint8_t c);
static int _send(netdev_t *dev, const iolist_t *iolist);
static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len);
static int _set(netdev_t *dev, netopt_t opt, const void *value, size_t len);
static int _init(netdev_t *dev);
static void _poweron(dose_t *dev);
static void _poweroff(dose_t *dev, dose_state_t sleep_state);

static void _crc_cb(void *ctx, uint8_t *data, size_t len)
{
    uint16_t *crc = ctx;
    *crc = crc16_ccitt_false_update(*crc, data, len);
}

static void _init_standby(dose_t *ctx, const dose_params_t *params)
{
    ctx->standby_pin = params->standby_pin;
    if (gpio_is_valid(ctx->standby_pin) &&
        gpio_init(ctx->standby_pin, GPIO_OUT)) {
        gpio_clear(ctx->standby_pin);
    }
}

static void _init_sense(dose_t *ctx, const dose_params_t *params)
{
#ifdef MODULE_PERIPH_UART_RXSTART_IRQ
    (void)params;
    uart_rxstart_irq_configure(ctx->uart, _isr_gpio, ctx);
#else
    ctx->sense_pin = params->sense_pin;
    if (gpio_is_valid(ctx->sense_pin)) {
        gpio_init_int(ctx->sense_pin, GPIO_IN, GPIO_FALLING, _isr_gpio, ctx);
        gpio_irq_disable(ctx->sense_pin);
    }
#endif
}

static inline void _enable_sense(dose_t *ctx)
{
#ifdef MODULE_PERIPH_UART_RXSTART_IRQ
    uart_rxstart_irq_enable(ctx->uart);
#else
    if (gpio_is_valid(ctx->sense_pin)) {
        gpio_irq_enable(ctx->sense_pin);
    }
#endif
}

static inline void _disable_sense(dose_t *ctx)
{
#ifdef MODULE_PERIPH_UART_RXSTART_IRQ
    uart_rxstart_irq_disable(ctx->uart);
#else
    if (gpio_is_valid(ctx->sense_pin)) {
        gpio_irq_disable(ctx->sense_pin);
    }
#endif
}

#ifdef MODULE_DOSE_WATCHDOG
static unsigned _watchdog_users;
static dose_t *_dose_base;
static uint8_t _dose_numof;

static inline void _watchdog_start(void)
{
    if (_watchdog_users) {
        return;
    }

    _watchdog_users++;
    timer_start(DOSE_TIMER_DEV);
}

static inline void _watchdog_stop(void)
{
    if (_watchdog_users == 0 || --_watchdog_users) {
        return;
    }

    timer_stop(DOSE_TIMER_DEV);
}

static void _dose_watchdog_cb(void *arg, int chan)
{
    (void) chan;
    (void) arg;

    for (unsigned i = 0; i < _dose_numof; ++i) {
        dose_t *ctx = &_dose_base[i];

        switch (ctx->state) {
        case DOSE_STATE_RECV:
            if (ctx->recv_buf_ptr_last != ctx->rb.cur) {
                ctx->recv_buf_ptr_last = ctx->rb.cur;
                break;
            }

            if (ctx->flags & DOSE_FLAG_RECV_BUF_DIRTY) {
                break;
            }

            DEBUG_PUTS("timeout");
            state(&_dose_base[i], DOSE_SIGNAL_ZTIMER);
            break;
        default:
            break;
        }
    }
}

static void _watchdog_init(unsigned timeout_us)
{
    timer_init(DOSE_TIMER_DEV, US_PER_SEC, _dose_watchdog_cb, NULL);
    timer_set_periodic(DOSE_TIMER_DEV, 0, timeout_us,
                       TIM_FLAG_RESET_ON_MATCH | TIM_FLAG_SET_STOPPED);
}
#else
static inline void _watchdog_start(void) {}
static inline void _watchdog_stop(void) {}
#endif

static dose_signal_t state_transit_blocked(dose_t *ctx, dose_signal_t signal)
{
    (void) signal;
    uint32_t backoff;

    backoff = random_uint32_range(0, 2 * ctx->timeout_base);
    ztimer_set(ZTIMER_USEC, &ctx->timeout, backoff);

    return DOSE_SIGNAL_NONE;
}

static dose_signal_t state_transit_idle(dose_t *ctx, dose_signal_t signal)
{
    (void) ctx;
    (void) signal;

    if (ctx->state == DOSE_STATE_RECV) {
        bool dirty = ctx->flags & DOSE_FLAG_RECV_BUF_DIRTY;
        bool done  = ctx->flags & DOSE_FLAG_END_RECEIVED;

        _watchdog_stop();

        /* We got here from RECV state. The driver's thread has to look
         * if this frame should be processed. By queuing NETDEV_EVENT_ISR,
         * the netif thread will call _isr at some time. */
        if (crb_end_chunk(&ctx->rb, !dirty && done)) {
            netdev_trigger_event_isr(&ctx->netdev);
        }

        clear_recv_buf(ctx);
    }

    /* Enable interrupt for start bit sensing */
    _enable_sense(ctx);

    /* Execute pending send */
    if (ctx->flags & DOSE_FLAG_SEND_PENDING) {
        return DOSE_SIGNAL_SEND;
    }

    return DOSE_SIGNAL_NONE;
}

static dose_signal_t state_transit_recv(dose_t *ctx, dose_signal_t signal)
{
    dose_signal_t rc = DOSE_SIGNAL_NONE;

    if (ctx->state != DOSE_STATE_RECV) {
        /* We freshly entered this state. Thus, no start bit sensing is required
         * anymore. Disable RX Start IRQs during the transmission. */
        _disable_sense(ctx);
        _watchdog_start();
        crb_start_chunk(&ctx->rb);
    }

    if (signal == DOSE_SIGNAL_UART) {
        /* We received a new octet */
        bool esc   = ctx->flags & DOSE_FLAG_ESC_RECEIVED;
        bool dirty = ctx->flags & DOSE_FLAG_RECV_BUF_DIRTY;
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

            if (!dirty && !crb_add_byte(&ctx->rb, ctx->uart_octet)) {
                SETBIT(ctx->flags, DOSE_FLAG_RECV_BUF_DIRTY);
            }
        }
    }

    if (rc == DOSE_SIGNAL_NONE && !IS_ACTIVE(MODULE_DOSE_WATCHDOG)) {
        /* No signal is returned. We stay in the RECV state. */
        ztimer_set(ZTIMER_USEC, &ctx->timeout, ctx->timeout_base);
    }

    return rc;
}

static dose_signal_t state_transit_send(dose_t *ctx, dose_signal_t signal)
{
    (void) signal;

    if (ctx->state != DOSE_STATE_SEND) {
        /* Disable RX Start IRQs during the transmission. */
        _disable_sense(ctx);
    }

    /* Don't trace any END octets ... the timeout or the END signal
     * will bring us back to the BLOCKED state after _send has emitted
     * its last octet. */
#ifndef MODULE_PERIPH_UART_COLLISION
    ztimer_set(ZTIMER_USEC, &ctx->timeout, ctx->timeout_base);
#endif

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
            case DOSE_STATE_IDLE + DOSE_SIGNAL_SEND:
                signal = state_transit_blocked(ctx, signal);
                ctx->state = DOSE_STATE_BLOCKED;
                break;

            case DOSE_STATE_SEND + DOSE_SIGNAL_END:
            case DOSE_STATE_SEND + DOSE_SIGNAL_ZTIMER:
            case DOSE_STATE_INIT + DOSE_SIGNAL_INIT:
            case DOSE_STATE_RECV + DOSE_SIGNAL_END:
            case DOSE_STATE_RECV + DOSE_SIGNAL_ZTIMER:
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

            case DOSE_STATE_BLOCKED + DOSE_SIGNAL_ZTIMER:
            case DOSE_STATE_SEND + DOSE_SIGNAL_UART:
                signal = state_transit_send(ctx, signal);
                ctx->state = DOSE_STATE_SEND;
                break;
            default:
                DEBUG("dose state(): unexpected state transition (STATE=0x%02x SIGNAL=0x%02x)\n", ctx->state, signal);
                /* fall-through */
            case DOSE_STATE_RECV + DOSE_SIGNAL_SEND:
                signal = DOSE_SIGNAL_NONE;
        }
    } while (signal != DOSE_SIGNAL_NONE);

    /* Indicate state change by unlocking state mutex */
    mutex_unlock(&ctx->state_mtx);
    irq_restore(irq_state);
}

static void _isr_uart(void *arg, uint8_t c)
{
    dose_t *dev = arg;

    dev->uart_octet = c;
    state(dev, DOSE_SIGNAL_UART);
}

static void _isr_gpio(void *arg)
{
    dose_t *dev = arg;

    state(dev, DOSE_SIGNAL_GPIO);
}

static void _isr_ztimer(void *arg)
{
    dose_t *dev = arg;

    switch (dev->state) {
#ifndef MODULE_DOSE_WATCHDOG
    case DOSE_STATE_RECV:
#endif
    case DOSE_STATE_BLOCKED:
    case DOSE_STATE_SEND:
        state(dev, DOSE_SIGNAL_ZTIMER);
        break;
    default:
        ;
    }
}

static void clear_recv_buf(dose_t *ctx)
{
    unsigned irq_state = irq_disable();

#ifdef MODULE_DOSE_WATCHDOG
    ctx->recv_buf_ptr_last = NULL;
#endif
    CLRBIT(ctx->flags, DOSE_FLAG_RECV_BUF_DIRTY);
    CLRBIT(ctx->flags, DOSE_FLAG_END_RECEIVED);
    CLRBIT(ctx->flags, DOSE_FLAG_ESC_RECEIVED);
    irq_restore(irq_state);
}

static void _isr(netdev_t *netdev)
{
    uint8_t dst[ETHERNET_ADDR_LEN];
    dose_t *ctx = container_of(netdev, dose_t, netdev);
    size_t len;

    /* Check for minimum length of an Ethernet packet */
    if (!crb_get_chunk_size(&ctx->rb, &len) ||
        len < sizeof(ethernet_hdr_t) + DOSE_FRAME_CRC_LEN) {
        DEBUG("dose _isr(): frame too short -> drop\n");
        crb_consume_chunk(&ctx->rb, NULL, 0);
        return;
    }

    /* Check the dst mac addr if the iface is not in promiscuous mode */
    if (!(ctx->opts & DOSE_OPT_PROMISCUOUS)) {

        /* get destination address - length of RX frame has ben checked before */
        crb_peek_bytes(&ctx->rb, dst, offsetof(ethernet_hdr_t, dst), sizeof(dst));

        /* destination has to be either broadcast or our address */
        if ((dst[0] & 0x1) == 0 && memcmp(dst, ctx->mac_addr.uint8, ETHERNET_ADDR_LEN) != 0) {
            DEBUG("dose _isr(): dst mac not matching -> drop\n");
            crb_consume_chunk(&ctx->rb, NULL, 0);
            return;
        }
    }

    /* Check the CRC */
    uint16_t crc = 0xffff;
    crb_chunk_foreach(&ctx->rb, _crc_cb, &crc);

    if (crc != 0x0000) {
        DEBUG("dose _isr(): wrong crc 0x%04x -> drop\n", crc);
        crb_consume_chunk(&ctx->rb, NULL, 0);
        return;
    }

    /* Finally schedule a _recv method call */
    DEBUG("dose _isr(): NETDEV_EVENT_RX_COMPLETE\n");
    ctx->netdev.event_callback(&ctx->netdev, NETDEV_EVENT_RX_COMPLETE);
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    int res;
    dose_t *ctx = container_of(dev, dose_t, netdev);

    (void)info;

    if (!buf && !len) {
        size_t pktlen;
        /* Return the amount of received bytes */
        if (crb_get_chunk_size(&ctx->rb, &pktlen)) {
            return pktlen - DOSE_FRAME_CRC_LEN;
        } else {
            return 0;
        }
    }

    if (crb_consume_chunk(&ctx->rb, buf, len)) {
        res = len;
    } else {
        res = -1;
    }

    size_t dummy;
    if (crb_get_chunk_size(&ctx->rb, &dummy)) {
        DEBUG("dose: %" PRIuSIZE " byte pkt in rx queue\n", dummy);
        netdev_trigger_event_isr(&ctx->netdev);
    }

    return res;
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

#ifdef MODULE_PERIPH_UART_COLLISION
    return uart_collision_detected(ctx->uart);
#endif

    /* Wait for a state transition */
    uint8_t new_state = wait_for_state(ctx, DOSE_STATE_ANY);
    if (new_state != DOSE_STATE_SEND) {
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

static inline void _send_start(dose_t *ctx)
{
#ifdef MODULE_PERIPH_UART_TX_ONDEMAND
    uart_enable_tx(ctx->uart);
#endif
#ifdef MODULE_PERIPH_UART_COLLISION
    uart_collision_detect_enable(ctx->uart);
#else
    (void)ctx;
#endif
}

static inline void _send_done(dose_t *ctx, bool collision)
{
#ifdef MODULE_PERIPH_UART_TX_ONDEMAND
    uart_disable_tx(ctx->uart);
#endif
#ifdef MODULE_PERIPH_UART_COLLISION
    uart_collision_detect_disable(ctx->uart);
    if (collision) {
        state(ctx, DOSE_SIGNAL_ZTIMER);
    }
#else
    (void)ctx;
    (void)collision;
#endif
}

static int _confirm_send(netdev_t *dev, void *info)
{
    (void)dev;
    (void)info;
    return -EOPNOTSUPP;
}

static int _send(netdev_t *dev, const iolist_t *iolist)
{
    dose_t *ctx = container_of(dev, dose_t, netdev);
    int8_t retries = 3;
    size_t pktlen;
    uint16_t crc;

    /* discard data when interface is in SLEEP mode */
    if (ctx->state == DOSE_STATE_SLEEP) {
        return -ENETDOWN;
    }

    /* sending data wakes the interface from STANDBY */
    if (ctx->state == DOSE_STATE_STANDBY) {
        _poweron(ctx);
    }

send:
    crc = 0xffff;
    pktlen = 0;

    /* Indicate intention to send */
    SETBIT(ctx->flags, DOSE_FLAG_SEND_PENDING);
    state(ctx, DOSE_SIGNAL_SEND);

    /* Wait for transition to SEND state */
    wait_for_state(ctx, DOSE_STATE_SEND);
    CLRBIT(ctx->flags, DOSE_FLAG_SEND_PENDING);

    _send_start(ctx);

    /* Send packet buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        size_t n = iol->iol_len;
        pktlen += n;
        uint8_t *ptr = iol->iol_base;
        crc = crc16_ccitt_false_update(crc, ptr, n);
        while (n--) {
            /* Send data octet */
            if (send_data_octet(ctx, *ptr)) {
                goto collision;
            }

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

    _send_done(ctx, false);

    /* Get out of the SEND state */
    state(ctx, DOSE_SIGNAL_END);

    return pktlen;

collision:
    _send_done(ctx, true);
    DEBUG("dose _send(): collision!\n");
    if (--retries < 0) {
        return -EBUSY;
    }

    goto send;
}

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    dose_t *ctx = container_of(dev, dose_t, netdev);

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
        case NETOPT_MAX_PDU_SIZE:
            if (CONFIG_DOSE_RX_BUF_LEN < (ETHERNET_FRAME_LEN + DOSE_FRAME_CRC_LEN)) {
                if (max_len < sizeof(uint16_t)) {
                    return -EINVAL;
                }
                *((uint16_t *)value) = CONFIG_DOSE_RX_BUF_LEN - DOSE_FRAME_CRC_LEN;
                return sizeof(uint16_t);
            }
            /* fall-through */
        default:
            return netdev_eth_get(dev, opt, value, max_len);
    }

    return 0;
}

static void _poweron(dose_t *ctx)
{
    /* interface is already powered on - do nothing */
    if (ctx->state != DOSE_STATE_STANDBY &&
        ctx->state != DOSE_STATE_SLEEP) {
        return;
    }

    if (gpio_is_valid(ctx->standby_pin)) {
        gpio_clear(ctx->standby_pin);
    }

    uart_poweron(ctx->uart);
    _enable_sense(ctx);

    ctx->state = DOSE_STATE_IDLE;
}

static void _poweroff(dose_t *ctx, dose_state_t sleep_state)
{
    /* interface is already powered off - do nothing */
    if (ctx->state == DOSE_STATE_STANDBY ||
        ctx->state == DOSE_STATE_SLEEP) {
        return;
    }

    /* allow powering off without a state transition */
    if (ctx->state != DOSE_STATE_IDLE) {
        wait_for_state(ctx, DOSE_STATE_IDLE);
    }

    if (gpio_is_valid(ctx->standby_pin)) {
        gpio_set(ctx->standby_pin);
    }

    _disable_sense(ctx);
    uart_poweroff(ctx->uart);

    ctx->state = sleep_state;
}

static int _set_state(dose_t *ctx, netopt_state_t state)
{
    switch (state) {
    case NETOPT_STATE_STANDBY:
        _poweroff(ctx, DOSE_STATE_STANDBY);
        return sizeof(netopt_state_t);
    case NETOPT_STATE_SLEEP:
        _poweroff(ctx, DOSE_STATE_SLEEP);
        return sizeof(netopt_state_t);
    case NETOPT_STATE_IDLE:
        _poweron(ctx);
        return sizeof(netopt_state_t);
    default:
        break;
    }

    return -ENOTSUP;
}

static int _set(netdev_t *dev, netopt_t opt, const void *value, size_t len)
{
    dose_t *ctx = container_of(dev, dose_t, netdev);

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
        case NETOPT_STATE:
            assert(len <= sizeof(netopt_state_t));
            return _set_state(ctx, *((const netopt_state_t *)value));
        default:
            return netdev_eth_set(dev, opt, value, len);
    }

    return 0;
}

static int _init(netdev_t *dev)
{
    dose_t *ctx = container_of(dev, dose_t, netdev);
    unsigned irq_state;

    /* Set state machine to defaults */
    irq_state = irq_disable();
    ctx->opts = 0;
    ctx->flags = 0;
    ctx->state = DOSE_STATE_INIT;
    crb_init(&ctx->rb, ctx->recv_buf, sizeof(ctx->recv_buf));
    irq_restore(irq_state);

    state(ctx, DOSE_SIGNAL_INIT);

    dev->event_callback(dev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static const netdev_driver_t netdev_driver_dose = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
    .confirm_send = _confirm_send,
};

void dose_setup(dose_t *ctx, const dose_params_t *params, uint8_t index)
{
    ctx->netdev.driver = &netdev_driver_dose;

    mutex_init(&ctx->state_mtx);

    ctx->uart = params->uart;
    uart_init(ctx->uart, params->baudrate, _isr_uart, (void *) ctx);

    _init_sense(ctx, params);
    _init_standby(ctx, params);

    netdev_register(&ctx->netdev, NETDEV_DOSE, index);

    assert(sizeof(ctx->mac_addr.uint8) == ETHERNET_ADDR_LEN);
    netdev_eui48_get(&ctx->netdev, &ctx->mac_addr);
    DEBUG("dose dose_setup(): mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
          ctx->mac_addr.uint8[0], ctx->mac_addr.uint8[1], ctx->mac_addr.uint8[2],
          ctx->mac_addr.uint8[3], ctx->mac_addr.uint8[4], ctx->mac_addr.uint8[5]
          );

    /* The timeout base is the minimal timeout base used for this driver.
     * To calculate how long it takes to transfer one byte we assume
     * 8 data bits + 1 start bit + 1 stop bit per byte.
     */
    ctx->timeout_base = CONFIG_DOSE_TIMEOUT_BYTES * 10UL * US_PER_SEC / params->baudrate;
    DEBUG("dose timeout set to %" PRIu32 " µs\n", ctx->timeout_base);
    ctx->timeout.callback = _isr_ztimer;
    ctx->timeout.arg = ctx;

#ifdef MODULE_DOSE_WATCHDOG
    if (index >= _dose_numof) {
        _dose_numof = index + 1;
    }
    if (index == 0) {
        _dose_base = ctx;
        _watchdog_init(ctx->timeout_base * 2);
    }
#endif /* MODULE_DOSE_WATCHDOG */
}
