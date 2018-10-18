/*
 * Copyright (C) 2017-2018 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       ContikiMAC netdev implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "net/contikimac.h"
#include "net/ieee802154.h"
#include "net/netdev/layer.h"
#include "net/gnrc/netif.h"
#include "thread.h" /* for thread_getpid */

#include "log.h"
#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Set ENABLE_TRACE to 1 to print single character status bytes over stderr to
 * see algorithm events, useful for debugging */
#define ENABLE_TRACE    (0)
#define TRACE(msg)      if (ENABLE_TRACE) { LOG_ERROR(msg); }

/* Set to 1 to enable debug prints of the time spent in radio ON modes */
#define ENABLE_TIMING_INFO (0)

/* Set to 1 to use LED0_ON/LED0_OFF for a visual feedback of the radio power state */
#ifndef CONTIKIMAC_DEBUG_LEDS
#define CONTIKIMAC_DEBUG_LEDS 0
#endif

#if ENABLE_TIMING_INFO
#define TIMING_PRINTF LOG_ERROR
#else
#define TIMING_PRINTF(...)
#endif

#if CONTIKIMAC_DEBUG_LEDS
#define CONTIKIMAC_LED_ON LED0_ON
#define CONTIKIMAC_LED_OFF LED0_OFF
#else
#define CONTIKIMAC_LED_ON
#define CONTIKIMAC_LED_OFF
#endif

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

/**
 * @brief   TX status definitions
 */
enum {
    CONTIKIMAC_TX_IDLE = 0,
    CONTIKIMAC_TX_STARTED,
    CONTIKIMAC_TX_COMPLETE,
    CONTIKIMAC_TX_MEDIUM_BUSY,
    CONTIKIMAC_TX_NOACK,
};

/**
 * @brief   netdev option flag definitions
 */
enum {
    CONTIKIMAC_OPT_TELL_TX_START   = (1 <<  8),
    CONTIKIMAC_OPT_TELL_TX_END     = (1 <<  9),
    CONTIKIMAC_OPT_TELL_RX_START   = (1 << 10),
    CONTIKIMAC_OPT_TELL_RX_END     = (1 << 11),
};

/**
 * @brief Default timing settings for O-QPSK 250 kbit/s
 */
extern const contikimac_params_t contikimac_params_OQPSK250;

/**
 * @brief Default timing settings for O-QPSK 100 kbit/s
 */
extern const contikimac_params_t contikimac_params_OQPSK100;

/**
 * @brief Default timing settings for BPSK 40 kbit/s
 */
extern const contikimac_params_t contikimac_params_BPSK40;

/**
 * @brief Default timing settings for BPSK 20 kbit/s
 */
extern const contikimac_params_t contikimac_params_BPSK20;

/* Internal constants used for the netdev set NETOPT_STATE calls, as it requires
 * a pointer to the value as argument */
static const netopt_state_t state_standby = NETOPT_STATE_STANDBY;
static const netopt_state_t state_listen  = NETOPT_STATE_IDLE;
static const netopt_state_t state_tx      = NETOPT_STATE_TX;

static uint32_t time_begin = 0;

/**
 * @brief   Check the timeout flag and report timeouts
 *
 * @return  0 if no timeout has occurred
 * @return  non-zero if a timeout has occurred
 */
static int contikimac_check_timeouts(contikimac_t *ctx);

/**
 * @brief   Begin performing a channel check
 *
 * @param[in]   ctx         ContikiMAC context
 */
static void contikimac_channel_check(contikimac_t *ctx);

/**
 * @brief   Check the channel to detect strobes
 *
 * This will perform up to @c cca_count_max CCA checks spaced @c cca_cycle_period
 * usec apart. If any check detects energy on the channel the function will
 * return 1.
 * The function will return early if energy is detected before @c cca_count_max
 * checks have been performed.
 *
 * @param[in]   ctx         ContikiMAC context
 *
 * @return 1 if a transmission was detected
 * @return 0 if none of the CCA checks detected any energy
 */
static int contikimac_channel_energy_detect(contikimac_t *ctx);

/**
 * @brief   Put the radio device to sleep immediately
 *
 * @param[in]   dev     device to put to sleep
 */
static void contikimac_radio_sleep(netdev_t *dev);

/**
 * @brief  Transmit a packet until timeout or an Ack has been received
 *
 * @pre Packet data has been preloaded
 *
 * @param[in]   ctx         ContikiMAC context
 * @param[in]   broadcast   set to 1 if the frame should use broadcast transmission rules
 */
static void contikimac_send(contikimac_t *ctx, int broadcast);

/**
 * @brief   Periodic handler during wake times to determine when to go back to sleep
 *
 * @param[in]   ctx         ContikiMAC context
 */
static void contikimac_periodic_handler(contikimac_t *ctx);

/**
 * @brief   Set all required network interface options on the lower netdev
 *
 * @param[in]   dev         netdev device to configure
 */
static void contikimac_configure_lower(netdev_t *dev);

/**
 * @brief   Select timing parameters based on current configuration of the lower interface
 *
 * @param[in]   ctx         ContikiMAC context
 */
static void contikimac_select_params(contikimac_t *ctx);

/**
 * @brief   Cancel all timeouts and periodic timers
 *
 * Cancels @c ctx->timers.timeout, @c ctx->timers.periodic, and @c ctx->events.periodic.
 * @c ctx->timers.channel_check is not affected.
 *
 * @param[in]   ctx         ContikiMAC context
 */
static void contikimac_cancel_timers(contikimac_t *ctx);

/**
 * @brief   Set a new timeout
 *
 * Used to put a time limit on certain operations in both the send path and in
 * channel check/receive path.
 *
 * @param[in]   ctx         ContikiMAC context
 * @param[in]   timeout_us  Timeout value, in microseconds
 */
static void contikimac_set_timeout(contikimac_t *ctx, uint32_t timeout_us);

/**
 * @brief   Read a IEEE802.15.4 frame header to see if it is a broadcast frame
 *
 * @param[in]   iolist  IEEE802.15.4 frame in iolist_t format
 *
 * @return 0 if iolist contains a unicast frame
 * @return non-zero if iolist contains a broadcast frame
 */
static int contikimac_is_bcast(const iolist_t *iolist);

/**
 * @brief   Code deduplication for handling on/off options which correspond to
 *          flags in @ref netdev_ieee802154_t
 *
 * @param[in]   ctx         ContikiMAC context
 * @param[in]   flags       flags mask
 * @param[in]   value       pointer to value (NETOPT_ENABLE, NETOPT_DISABLE)
 * @param[in]   len         value length
 *
 * @return  @c -EOVERFLOW if @p len != @c sizeof(netopt_enable_t)
 * @return  sizeof(netopt_enable_t) otherwise
 */
static int contikimac_netopt_flag_change(contikimac_t *ctx, uint16_t flags,
    const netopt_enable_t *value, size_t len);

/**
 * @brief   xtimer callback for posting events to the event loop
 */
static void cb_post_event(void *arg);

/**
 * @brief   xtimer callback for timeouts during fast sleep
 */
static void cb_timeout(void *arg);

/**
 * @brief   Netdev event callback for the lower device
 *
 * @param[in]   lower       pointer to lower device
 * @param[in]   ev          netdev event
 */
static void cb_netdev(netdev_t *lower, netdev_event_t ev);

/**
 * @brief   Event trampoline for @ref contikimac_channel_check
 *
 * This will also reschedule another channel check event
 */
static void ev_channel_check(event_t *evp);
/**
 * @brief   Event trampoline for @ref contikimac_channel_check
 *
 * This will not reschedule another channel check event
 */
static void ev_channel_check_no_resched(event_t *evp);
/**
 * @brief   Event trampoline for @ref contikimac_periodic
 */
static void ev_periodic(event_t *evp);
/**
 * @brief   Event trampoline for @ref contikimac_netdev_isr
 */
static void ev_isr(event_t *evp);

static void cb_netdev(netdev_t *lower, netdev_event_t ev)
{
    contikimac_t *ctx = lower->context;
    switch (ev) {
        case NETDEV_EVENT_ISR:
            event_post(ctx->evq, &ctx->events.isr.super);
            break;
        case NETDEV_EVENT_RX_STARTED:
            ctx->rx_in_progress = 1;
            if (!ctx->no_sleep) {
                contikimac_cancel_timers(ctx);
                /* Set a timeout for the currently in progress RX frame */
                contikimac_set_timeout(ctx, ctx->params->rx_timeout);
            }
            TRACE("r");
            if (ctx->dev.flags & CONTIKIMAC_OPT_TELL_RX_START) {
                ctx->dev.netdev.event_callback(&ctx->dev.netdev, NETDEV_EVENT_RX_STARTED);
            }
            break;
        case NETDEV_EVENT_RX_COMPLETE:
            contikimac_cancel_timers(ctx);
            TIMING_PRINTF("u: %lu\n", (unsigned long)xtimer_now_usec() - time_begin);
            TRACE("R\n");
            if (ctx->dev.flags & CONTIKIMAC_OPT_TELL_RX_END) {
                /* Defer sleep until netdev_recv is called */
                /* some transceivers, e.g. at86rf2xx, lose their RX buffer
                 * contents when the device enters deep sleep */
                int res = lower->driver->set(lower, NETOPT_STATE, &state_standby, sizeof(state_standby));
                if (res < 0) {
                    DEBUG("contikimac(%d): Failed setting NETOPT_STATE_STANDBY: %d\n",
                          thread_getpid(), res);
                }
                ctx->dev.netdev.event_callback(&ctx->dev.netdev, NETDEV_EVENT_RX_COMPLETE);
            }
            else {
                contikimac_radio_sleep(lower);
            }

            break;
        case NETDEV_EVENT_TX_MEDIUM_BUSY:
            TRACE("m");
            ctx->tx_status = CONTIKIMAC_TX_MEDIUM_BUSY;
#ifdef MODULE_NETSTATS_L2
            lower->stats.tx_failed++;
#endif /* MODULE_NETSTATS_L2 */
            break;
        case NETDEV_EVENT_TX_NOACK:
            TRACE("n");
            ctx->tx_status = CONTIKIMAC_TX_NOACK;
#ifdef MODULE_NETSTATS_L2
            lower->stats.tx_failed++;
#endif /* MODULE_NETSTATS_L2 */
            break;
        case NETDEV_EVENT_TX_COMPLETE:
            TRACE("c");
            ctx->tx_status = CONTIKIMAC_TX_COMPLETE;
#ifdef MODULE_NETSTATS_L2
            lower->stats.tx_success++;
#endif /* MODULE_NETSTATS_L2 */
            break;
        default:
            /* pass through event to upper layer */
            ctx->dev.netdev.event_callback(&ctx->dev.netdev, ev);
            break;
    }
}

void contikimac_setup(contikimac_t *ctx, netdev_t *lower)
{
    ctx->channel_check_period = CONTIKIMAC_DEFAULT_CHANNEL_CHECK_PERIOD;
    ctx->dev.netdev.driver = &contikimac_driver;
    ctx->dev.netdev.lower = lower;
    lower->context = &ctx->dev;
    lower->event_callback = cb_netdev;
}

static int contikimac_netdev_init(netdev_t *dev)
{
    contikimac_t *ctx = (contikimac_t *)dev;
    ctx->seen_silence = 0;
    ctx->rx_in_progress = 0;
    ctx->timeout_flag = 0;
    ctx->no_sleep = 0;
    ctx->tx_status = CONTIKIMAC_TX_IDLE;

    static const netopt_enable_t enable = NETOPT_ENABLE;
    /* ContikiMAC requires link layer ACK requests on outgoing unicast frames */
    netdev_ieee802154_set((netdev_ieee802154_t *)dev, NETOPT_ACK_REQ, &enable, sizeof(enable));

    ctx->evq = &((gnrc_netif_t *)(dev->context))->evq;
    ctx->channel_check_period = CONTIKIMAC_DEFAULT_CHANNEL_CHECK_PERIOD;
    ctx->burst_timeout = CONTIKIMAC_DEFAULT_BURST_TIMEOUT;
    ctx->reply_delay = CONTIKIMAC_DEFAULT_REPLY_DELAY;
    ctx->timers.channel_check = (const xtimer_t) {
        .target = 0, .long_target = 0,
        .callback = cb_post_event,
        .arg = &ctx->events.channel_check,
    };
    ctx->timers.extra_channel_check = (const xtimer_t) {
        .target = 0, .long_target = 0,
        .callback = cb_post_event,
        .arg = &ctx->events.extra_channel_check,
    };
    ctx->timers.periodic = (const xtimer_t) {
        .target = 0, .long_target = 0,
        .callback = cb_post_event,
        .arg = &ctx->events.periodic,
    };
    ctx->timers.timeout = (const xtimer_t) {
        .target = 0, .long_target = 0,
        .callback = cb_timeout,
        /* The periodic handler will check the timeout flag and put the device to sleep */
        .arg = &ctx->events.periodic,
    };
    ctx->events.channel_check = (const event_contikimac_t) {
        .super = { .list_node.next = NULL, .handler = ev_channel_check, },
        .ctx = ctx,
    };
    ctx->events.extra_channel_check = (const event_contikimac_t) {
        .super = { .list_node.next = NULL, .handler = ev_channel_check_no_resched, },
        .ctx = ctx,
    };
    ctx->events.periodic = (const event_contikimac_t) {
        .super = { .list_node.next = NULL, .handler = ev_periodic, },
        .ctx = ctx,
    };
    ctx->events.isr = (const event_contikimac_t) {
        .super = { .list_node.next = NULL, .handler = ev_isr, },
        .ctx = ctx,
    };
    /* initialize lower driver */
    netdev_t *lower_netdev = dev->lower;
    netdev_ieee802154_t *lower_802154 = (netdev_ieee802154_t*)lower_netdev;
    lower_netdev->driver->init(lower_netdev);

    /* configure netopt settings on the lower interface */
    contikimac_configure_lower(lower_netdev);
    contikimac_select_params(ctx);
    /* Copy default protocol and PAN ID from lower device */
    ctx->dev.proto = lower_802154->proto;
    ctx->dev.pan = lower_802154->pan;
    /* start the event loop by posting a channel check event */
    cb_post_event(&ctx->events.channel_check);

    return 0;
}

static int contikimac_is_bcast(const iolist_t *iolist)
{
    size_t len = 0;
    uint8_t buf[IEEE802154_MAX_HDR_LEN] = {0};
    while (iolist) {
        if ((len + iolist->iol_len) >= sizeof(buf)) {
            memcpy(&buf[len], iolist->iol_base, sizeof(buf) - len);
            len = sizeof(buf);
            break;
        }
        memcpy(&buf[len], iolist->iol_base, iolist->iol_len);
        len += iolist->iol_len;
    }
    if (len >= IEEE802154_MIN_FRAME_LEN) {
        uint8_t dst[IEEE802154_LONG_ADDRESS_LEN];
        le_uint16_t dst_pan;
        int res = ieee802154_get_dst(buf, dst, &dst_pan);
        if ((res == IEEE802154_ADDR_BCAST_LEN) &&
            (memcmp(ieee802154_addr_bcast, dst, IEEE802154_ADDR_BCAST_LEN) == 0)) {
            return 1;
        }
    }
    return 0;
}

static int contikimac_netdev_send(netdev_t *dev, const iolist_t *iolist)
{
    contikimac_t *ctx = (contikimac_t *)dev;
    netdev_t *lower = ctx->dev.netdev.lower;
    if (ctx->tx_status) {
        /* TX already in progress, something must be wrong */
        LOG_ERROR("contikimac(%d): double TX!\n", thread_getpid());
        return -1;
    }
    if (ctx->rx_in_progress) {
        /* TODO let the in progress RX frame finish before starting TX */
    }
    /* Go to standby before transmitting to avoid having incoming
     * packets corrupt the frame buffer on single buffered
     * transceivers (e.g. at86rf2xx). Also works around an issue
     * on at86rf2xx where the frame buffer is lost after the
     * first transmission because the driver puts the transceiver
     * in sleep mode. */
    int res = lower->driver->set(lower, NETOPT_STATE, &state_standby, sizeof(state_standby));
    if (res < 0) {
        DEBUG("contikimac(%d): Failed setting NETOPT_STATE_STANDBY: %d\n",
              thread_getpid(), res);
    }
    uint8_t broadcast = contikimac_is_bcast(iolist);
    /* Preload the frame */
    res = lower->driver->send(lower, iolist);
    if (res < 0) {
        LOG_ERROR("contikimac(%d): Preload failed: %d\n",
              thread_getpid(), res);
        return res;
    }
    if (ctx->dev.flags & CONTIKIMAC_OPT_TELL_TX_START) {
        ctx->dev.netdev.event_callback(&ctx->dev.netdev, NETDEV_EVENT_TX_STARTED);
    }
    contikimac_send(ctx, broadcast);
    if (ctx->dev.flags & CONTIKIMAC_OPT_TELL_TX_END) {
        switch (ctx->tx_status) {
            case CONTIKIMAC_TX_IDLE:
            case CONTIKIMAC_TX_COMPLETE:
                ctx->dev.netdev.event_callback(&ctx->dev.netdev, NETDEV_EVENT_TX_COMPLETE);
                break;
            case CONTIKIMAC_TX_MEDIUM_BUSY:
                ctx->dev.netdev.event_callback(&ctx->dev.netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);
                break;
            case CONTIKIMAC_TX_NOACK:
            default:
                ctx->dev.netdev.event_callback(&ctx->dev.netdev, NETDEV_EVENT_TX_NOACK);
                break;
        }
    }
    ctx->tx_status = CONTIKIMAC_TX_IDLE;
    if (ctx->no_sleep) {
        res = lower->driver->set(lower, NETOPT_STATE, &state_listen, sizeof(state_listen));
        if (res < 0) {
            DEBUG("contikimac(%d): Failed setting NETOPT_STATE_IDLE: %d\n",
                  thread_getpid(), res);
        }
    }
    else {
        /* The current RX cycle will have been disrupted by the TX,
         * switch back to sleep state */
        if (!broadcast) {
            /* If we sent a unicast message we trigger a channel check after a short
             * delay to catch immediate replies from the other node. This should improve
             * latency and reduce the energy usage of the other node by reducing the
             * number of retries needed. */
            /* TODO make this configurable */
            xtimer_set(&ctx->timers.extra_channel_check, ctx->reply_delay);
        }
        contikimac_radio_sleep(lower);
        ctx->rx_in_progress = 0;
    }
    return 0;
}

static int contikimac_netdev_recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    contikimac_t *ctx = (contikimac_t *)dev;
    netdev_t *lower = ctx->dev.netdev.lower;
    ctx->rx_in_progress = 0;
    int res = lower->driver->recv(lower, buf, len, info);
    if (buf || len) {
        /* buf == NULL && len == 0 is used to query the frame length without
         * discarding the RX buffer, only allow state transitions if we are
         * finished with the buffer */
        TRACE("G\n");
        unsigned expect_burst = 0;
        if (res > 0) {
            /* Assume IEEE 802.15.4 frame header */
            if (buf && (((uint8_t *)buf)[0] & IEEE802154_FCF_FRAME_PEND)) {
                expect_burst = 1;
            }
        }
        if (ctx->tx_status != CONTIKIMAC_TX_IDLE) {
            /* TX in progress, don't touch the state */
            return res;
        }
        if (ctx->no_sleep || expect_burst) {
            res = lower->driver->set(lower, NETOPT_STATE, &state_listen, sizeof(state_listen));
            if (res < 0) {
                DEBUG("contikimac(%d): Failed setting NETOPT_STATE_IDLE: %d\n",
                          thread_getpid(), res);
                return res;
            }
            if (!ctx->no_sleep) {
                contikimac_set_timeout(ctx, ctx->burst_timeout);
            }
        }
        else {
            /* We are done with this transaction, go back to sleep */
            contikimac_radio_sleep(lower);
        }
    }
    return res;
}

static int contikimac_netdev_get(netdev_t *dev, netopt_t opt, void *value, size_t len)
{
    contikimac_t *ctx = (contikimac_t *)dev;
    netdev_t *lower = ctx->dev.netdev.lower;
    switch (opt) {
        case NETOPT_MAC_NO_SLEEP:
            if (len != sizeof(netopt_enable_t)) {
                return -EOVERFLOW;
            }
            *((netopt_enable_t *)value) = (ctx->no_sleep ? NETOPT_ENABLE : NETOPT_DISABLE);
            return sizeof(netopt_enable_t);

        default:
            break;
    }
    int res = lower->driver->get(lower, opt, value, len);
    return res;
}

static int contikimac_netopt_flag_change(contikimac_t *ctx, uint16_t flags,
    const netopt_enable_t *value, size_t len)
{
    if (len != sizeof(const netopt_enable_t)) {
        return -EOVERFLOW;
    }
    const netopt_enable_t *enable = value;
    if (*enable == NETOPT_DISABLE) {
        ctx->dev.flags &= ~(flags);
    }
    else {
        ctx->dev.flags |= flags;
    }
    return sizeof(const netopt_enable_t);
}

static int contikimac_netdev_set(netdev_t *dev, netopt_t opt, const void *value, size_t len)
{
    contikimac_t *ctx = (contikimac_t *)dev;
    DEBUG("opt: %u\n", (unsigned) opt);
    switch (opt) {
        /* ContikiMAC provides its own TX/RX feedback events.
         * The corresponding events on the lower device are used internally by
         * the implementation to provide feedback for timeouts */
        case NETOPT_TX_START_IRQ:
            return contikimac_netopt_flag_change(ctx, CONTIKIMAC_OPT_TELL_TX_START, value, len);
        case NETOPT_TX_END_IRQ:
            return contikimac_netopt_flag_change(ctx, CONTIKIMAC_OPT_TELL_TX_END, value, len);
        case NETOPT_RX_START_IRQ:
            return contikimac_netopt_flag_change(ctx, CONTIKIMAC_OPT_TELL_RX_START, value, len);
        case NETOPT_RX_END_IRQ:
            return contikimac_netopt_flag_change(ctx, CONTIKIMAC_OPT_TELL_RX_END, value, len);

        /* Disallow changing these setting from higher layers */
        case NETOPT_PRELOADING:
        case NETOPT_CSMA:
        case NETOPT_RETRANS:
            return -EINVAL;

        case NETOPT_MAC_NO_SLEEP:
            if (len != sizeof(const netopt_enable_t)) {
                return -EOVERFLOW;
            }
            ctx->no_sleep = (*((const netopt_enable_t *)value) != NETOPT_DISABLE);
            /* Reset the radio duty cycling state */
            contikimac_cancel_timers(ctx);
            ctx->rx_in_progress = 0;
            ctx->seen_silence = 0;
            if (ctx->no_sleep) {
                /* switch the radio to listen state */
                int res = dev->lower->driver->set(dev->lower, NETOPT_STATE, &state_listen, sizeof(state_listen));
                if (res < 0) {
                    DEBUG("contikimac(%d): Failed setting NETOPT_STATE_IDLE: %d\n",
                          thread_getpid(), res);
                }
            }
            else {
                /* start the event loop by posting a channel check event */
                cb_post_event(&ctx->events.channel_check);
            }
            return sizeof(const netopt_enable_t);

        case NETOPT_STATE:
            DEBUG("State: %u\n", *((unsigned *)value));
            break;

        default:
            break;
    }
    int res = dev->lower->driver->set(dev->lower, opt, value, len);
    if (res > 0) {
        /*  XXX refactor this when the IEEE 802.15.4 state is refactored */
        netdev_ieee802154_set((netdev_ieee802154_t *)dev, opt, value, len);
        switch (opt) {
            case NETOPT_CHANNEL:
            case NETOPT_CHANNEL_PAGE:
                contikimac_select_params(ctx);
                break;
            default:
                break;
        }
    }
    return res;
}

static void contikimac_netdev_isr(netdev_t *dev)
{
    contikimac_t *ctx = (contikimac_t *)dev;
    /* To get the wait timing right we will save the timestamp here.
     * The time of the last IRQ before the TX_OK or TX_NOACK flag was
     * set is used as an approximation of when the TX operation finished */
    ctx->last_irq = xtimer_now();

    dev->lower->driver->isr(dev->lower);
}

const netdev_driver_t contikimac_driver = {
    .init = contikimac_netdev_init,
    .send = contikimac_netdev_send,
    .recv = contikimac_netdev_recv,
    .get  = contikimac_netdev_get,
    .set  = contikimac_netdev_set,
    .isr  = contikimac_netdev_isr,
};

/**
 * @brief   Shared asserts for the xtimer callbacks below
 */
static inline void event_asserts(void *arg)
{
    assert(arg);
    event_contikimac_t *evp = arg;
    contikimac_t *ctx = evp->ctx;
    assert(ctx);
    assert(ctx->evq);
}

static void cb_post_event(void *arg)
{
    event_asserts(arg);
    event_contikimac_t *evp = arg;
    contikimac_t *ctx = evp->ctx;
    event_post(ctx->evq, &evp->super);
}

static void cb_timeout(void *arg)
{
    event_asserts(arg);
    event_contikimac_t *evp = arg;
    contikimac_t *ctx = evp->ctx;
    ctx->timeout_flag = 1;
    event_post(ctx->evq, &evp->super);
}

static int contikimac_check_timeouts(contikimac_t *ctx)
{
    if (ctx->timeout_flag) {
        contikimac_cancel_timers(ctx);
        TIMING_PRINTF("t: %lu\n", (unsigned long)xtimer_now_usec() - time_begin);
        if (ctx->tx_status) {
            DEBUG("contikimac(%d): TX timeout\n",
                      thread_getpid());
        }
        else if (ctx->rx_in_progress) {
            DEBUG("contikimac(%d): RX timeout\n",
                      thread_getpid());
        }
        else if (ctx->seen_silence) {
            DEBUG("contikimac(%d): Fast sleep (long silence)\n",
                      thread_getpid());
        }
        else {
            DEBUG("contikimac(%d): Fast sleep (noise)\n",
                      thread_getpid());
        }
        return 1;
    }
    return 0;
}

static void contikimac_periodic_handler(contikimac_t *ctx)
{
    if (ctx->no_sleep) {
        return;
    }
    if (ctx->tx_status) {
        /* Transmission in progress, do not do anything to disrupt it */
        return;
    }
    netdev_t *lower = ctx->dev.netdev.lower;
    if (contikimac_check_timeouts(ctx)) {
        /* Timeout has occurred */
        TRACE("T\n");
        contikimac_radio_sleep(lower);
        ctx->rx_in_progress = 0;
        return;
    }
    /* Periodically perform CCA checks to evaluate channel usage */
    if (ctx->seen_silence) {
        /* Performing a CCA check while a packet is being received may cause the
         * driver to abort the reception, we will only do CCAs while waiting for
         * the first silence */
        TRACE("s");
        return;
    }
    /* We have detected some energy on the channel, we will keep checking
     * the channel periodically until it is idle, then switch to listen state */
    netopt_enable_t channel_clear;
    int res = lower->driver->get(lower, NETOPT_IS_CHANNEL_CLR, &channel_clear, sizeof(channel_clear));
    if (res < 0) {
        DEBUG("contikimac(%d): Failed getting NETOPT_IS_CHANNEL_CLR: %d\n",
                  thread_getpid(), res);
        return;
    }
    if (!channel_clear) {
        TRACE("B");
        /* Do next CCA */
        xtimer_set(&ctx->timers.periodic, ctx->params->after_ed_scan_interval);
        event_post(ctx->evq, &ctx->events.periodic.super);
        return;
    }
    /* Silence detected */
    ctx->seen_silence = 1;
    /* We have detected an idle channel, expect incoming traffic very soon */
    res = lower->driver->set(lower, NETOPT_STATE, &state_listen, sizeof(state_listen));
    if (res < 0) {
        DEBUG("contikimac(%d): Failed setting NETOPT_STATE_IDLE: %d\n",
                  thread_getpid(), res);
        return;
    }
    /* Set timeout in case we only detected noise */
    TRACE("L");
    contikimac_set_timeout(ctx, ctx->params->listen_timeout);
}

static void contikimac_channel_check(contikimac_t *ctx)
{
    if (ctx->no_sleep) {
        return;
    }
    if (ctx->tx_status) {
        /* Transmission in progress, do not do anything to disrupt it */
        return;
    }
    if (contikimac_check_timeouts(ctx)) {
        /* Timeout has occurred */
    }
    else if (ctx->rx_in_progress) {
        /* Currently sending/receiving a frame, skip this channel check */
        TRACE("_");
        DEBUG("contikimac(%d): Skip channel check, RX in progress\n",
            thread_getpid());
        return;
    }
    netdev_t *lower = ctx->dev.netdev.lower;

    if (ENABLE_TIMING_INFO) {
        time_begin = xtimer_now_usec();
    }
    contikimac_cancel_timers(ctx);
    DEBUG("contikimac(%d): Checking channel\n", thread_getpid());
    /* Perform multiple CCA and check the results */
    /* Bring the radio out of sleep mode */
    int res = lower->driver->set(lower, NETOPT_STATE, &state_standby, sizeof(state_standby));
    if (res < 0) {
        DEBUG("contikimac(%d): Failed setting NETOPT_STATE_STANDBY: %d\n",
            thread_getpid(), res);
        return;
    }
    CONTIKIMAC_LED_ON;
    if (contikimac_channel_energy_detect(ctx)) {
        /* Set the radio to listen for incoming packets */
        DEBUG("contikimac(%d): Detected, looking for silence\n", thread_getpid());
        /* Reset the periodic handler state */
        ctx->seen_silence = 0;
        event_post(ctx->evq, &ctx->events.periodic.super);
        TRACE("D\n");
        /* Set timeout in case we only detected noise */
        contikimac_set_timeout(ctx, ctx->params->after_ed_scan_timeout);
    }
    else {
        /* Nothing detected, immediately return to sleep */
        DEBUG("contikimac(%d): Nothing seen\n", thread_getpid());
        TRACE("d");
        contikimac_radio_sleep(lower);
    }
}

static int contikimac_channel_energy_detect(contikimac_t *ctx)
{
    netdev_t *dev = &ctx->dev.netdev;
    xtimer_ticks32_t last_wakeup = xtimer_now();
    for (uint8_t cca = ctx->params->cca_count_max; cca > 0; --cca) {
        netopt_enable_t channel_clear;
        int res = dev->driver->get(dev, NETOPT_IS_CHANNEL_CLR, &channel_clear, sizeof(channel_clear));
        if (res < 0) {
            DEBUG("contikimac(%d): Failed getting NETOPT_IS_CHANNEL_CLR: %d\n",
                thread_getpid(), res);
            break;
        }
        if (!channel_clear) {
            /* Detected some radio energy on the channel */
            return 1;
        }
        xtimer_periodic_wakeup(&last_wakeup, ctx->params->cca_cycle_period);
    }
    return 0;
}

static void contikimac_radio_sleep(netdev_t *dev)
{
    static const netopt_state_t state_sleep = NETOPT_STATE_SLEEP;
    DEBUG("contikimac(%d): Going to sleep\n", thread_getpid());
    int res = dev->driver->set(dev, NETOPT_STATE, &state_sleep, sizeof(state_sleep));
    if (res < 0) {
        DEBUG("contikimac(%d): Failed setting NETOPT_STATE_SLEEP: %d\n",
              thread_getpid(), res);
    }
    CONTIKIMAC_LED_OFF;
    TIMING_PRINTF("r: %lu\n", (unsigned long)xtimer_now_usec() - time_begin);
}

static void contikimac_send(contikimac_t *ctx, int broadcast)
{
    ctx->tx_status = CONTIKIMAC_TX_STARTED;
    int do_transmit = 1;
    uint32_t time_before = 0;
    (void) time_before;
    if (ENABLE_TIMING_INFO) {
        time_before = xtimer_now_usec();
    }
    /* For reliable communications, we need to strobe the frame transmission for
     * at least the full wake-up interval, plus the length of two CCA checks,
     * plus one more frame to allow the receiver to detect the packet on the
     * wake-up interval boundary, plus yet one more frame for the actual
     * transmission in the worst case scenario. */
    uint32_t tx_strobe_timeout = ctx->channel_check_period + 2 * ctx->params->cca_cycle_period;

    /* TX aborts listening mode */
    contikimac_cancel_timers(ctx);

#ifdef MODULE_NETSTATS_L2
    if (broadcast) {
        ++lower->stats.tx_mcast_count;
    }
    else {
        ++lower->stats.tx_unicast_count;
    }
#endif

    /* Check that the channel is clear before starting strobe */
    /* This check avoids problems where a reply sent in response to a broadcast
     * transmission interrupts the broadcasting node and prevents other nodes
     * from detecting the broadcast. The broadcasting node will also sometimes
     * fail to receive the reply because it is still in strobe mode */
    /* We avoid using CSMA on the actual TX operations below because it will add
     * variable delays to the transmission and mess up the protocol timing */
    contikimac_set_timeout(ctx, tx_strobe_timeout + ctx->params->rx_timeout);
    while (contikimac_channel_energy_detect(ctx)) {
        DEBUG("contikimac(%d): wait for TX opportunity\n", thread_getpid());
        if (ctx->timeout_flag) {
            DEBUG("contikimac(%d): Channel busy\n", thread_getpid());
            ctx->tx_status = CONTIKIMAC_TX_MEDIUM_BUSY;
            return;
        }
    }
    contikimac_cancel_timers(ctx);
    /* Set timeout for TX strobe */
    contikimac_set_timeout(ctx, tx_strobe_timeout);
    netdev_t *lower = ctx->dev.netdev.lower;
    unsigned tx_sequence = 2; /* counts down after the strobe timeout has happened */
    while(tx_sequence) {
        if (do_transmit) {
            /* For extra verbose TX timing info: */
            /*
            TIMING_PRINTF("S: %lu\n", (unsigned long)xtimer_now_usec() - time_before);
            */
            do_transmit = 0;
            ctx->tx_status = CONTIKIMAC_TX_STARTED;
            if (ENABLE_TIMING_INFO) {
                time_before = xtimer_now_usec();
            }
            lower->driver->set(lower, NETOPT_STATE, &state_tx, sizeof(state_tx));
        }
        thread_flags_t tf = thread_flags_wait_any(THREAD_FLAG_EVENT);
        if (tf & THREAD_FLAG_EVENT) {
            event_t *evp;
            /* Perform event queue drain */
            while ((evp = event_get(ctx->evq))) {
                DEBUG("contikimac(%d): event %p\n", thread_getpid(), (void *)evp);
                if (evp->handler) {
                    evp->handler(evp);
                }
            }
        }
        /* Check the TX progress */
        switch (ctx->tx_status) {
            case CONTIKIMAC_TX_COMPLETE:
                /* For unicast, stop after receiving the first Ack */
                TRACE("C");
                if (!broadcast) {
                    TRACE("U\n");
                    TIMING_PRINTF("O: %lu\n", (unsigned long)xtimer_now_usec() - time_before);
                    /* Cancel timeout */
                    contikimac_cancel_timers(ctx);
                    ctx->tx_status = CONTIKIMAC_TX_IDLE;
                    return;
                }
                /* For broadcast and multicast, always transmit for the full strobe
                 * duration, but wait for a short while before retransmitting */
                xtimer_periodic_wakeup(&ctx->last_irq, ctx->params->inter_packet_interval);
                /* fall through */
            case CONTIKIMAC_TX_NOACK:
            case CONTIKIMAC_TX_MEDIUM_BUSY:
                /* Skip wait on TX errors */
                /* Consider the inter_packet_interval already passed without calling
                 * xtimer to verify. Modify this part if inter_packet_interval is much
                 * longer than the Ack timeout */
                TRACE("x");
                /* retransmit */
                do_transmit = 1;
                if (ctx->timeout_flag) {
                    /* Strobe time has passed, try to transmit one more frame
                     * before stopping.
                     * When tx_sequence reaches 0 we will stop retrying */
                     TRACE("X");
                    --tx_sequence;
                }
                break;
            default:
                /* Still waiting to hear back from the TX operation */
                break;
        }
        /* Keep retransmitting until the strobe time has passed, or until we
         * receive an Ack for unicast. */
    }
    /* Timeout flag was set */
    TRACE("t\n");
    contikimac_cancel_timers(ctx);
    ctx->tx_status = CONTIKIMAC_TX_IDLE;
    return;
}

static void contikimac_configure_lower(netdev_t *dev)
{
    /* Enable RX- and TX-started interrupts */
    static const netopt_enable_t enable = NETOPT_ENABLE;
    static const netopt_enable_t disable = NETOPT_DISABLE;
    static const uint8_t zero = 0;

    int res;
    res = dev->driver->set(dev, NETOPT_CSMA, &disable, sizeof(disable));
    if (res < 0) {
        DEBUG("contikimac(%d): disable NETOPT_CSMA failed: %d\n",
                  thread_getpid(), res);
    }
    res = dev->driver->set(dev, NETOPT_RETRANS, &zero, sizeof(zero));
    if (res < 0) {
        DEBUG("contikimac(%d): disable NETOPT_RETRANS failed: %d\n",
                  thread_getpid(), res);
    }
    res = dev->driver->set(dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
    if (res < 0) {
        DEBUG("contikimac(%d): enable NETOPT_RX_START_IRQ failed: %d\n",
                  thread_getpid(), res);
    }
    res = dev->driver->set(dev, NETOPT_RX_END_IRQ, &enable, sizeof(enable));
    if (res < 0) {
        DEBUG("contikimac(%d): enable NETOPT_RX_END_IRQ failed: %d\n",
                  thread_getpid(), res);
    }
    res = dev->driver->set(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));
    if (res < 0) {
        DEBUG("contikimac(%d): enable NETOPT_TX_END_IRQ failed: %d\n",
                  thread_getpid(), res);
    }
    res = dev->driver->set(dev, NETOPT_PRELOADING, &enable, sizeof(enable));
    if (res < 0) {
        DEBUG("contikimac(%d): enable NETOPT_PRELOADING failed: %d\n",
                  thread_getpid(), res);
        LOG_ERROR("contikimac requires NETOPT_PRELOADING, this node will "
                  "likely not be able to communicate with other nodes!\n");
    }
}

static void contikimac_select_params(contikimac_t *ctx)
{
    netdev_t *lower = ctx->dev.netdev.lower;
    /* 802.15.4 channel and page determine which set of parameters we use */
    uint16_t channel;
    uint16_t page;
    int res = lower->driver->get(lower, NETOPT_CHANNEL, &channel, sizeof(channel));
    if (res < 0) {
        LOG_ERROR("contikimac(%d): Error %d reading device channel. "
            "Fall back to timings for O-QPSK 250 kbit/s\n",
            thread_getpid(), res);
        ctx->params = &contikimac_params_OQPSK250;
        return;
    }
    res = lower->driver->get(lower, NETOPT_CHANNEL_PAGE, &page, sizeof(page));
    if (res < 0) {
        /* Assume page = 0 if the device driver does not support NETOPT_CHANNEL_PAGE */
        page = 0;
    }
    switch (page) {
        case 0:
            if (channel == IEEE802154_CHANNEL_MIN_SUBGHZ) {
                /* 868 MHz band, BPSK 20 kbit/s */
                DEBUG("contikimac(%d): using timings for BPSK 20 kbit/s\n",
                    thread_getpid());
                ctx->params = &contikimac_params_BPSK20;
            }
            else if ((channel > IEEE802154_CHANNEL_MIN_SUBGHZ) &&
                     (channel <= IEEE802154_CHANNEL_MAX_SUBGHZ)) {
                /* 915 MHz band, BPSK 40 kbit/s */
                DEBUG("contikimac(%d): using timings for BPSK 40 kbit/s\n",
                    thread_getpid());
                ctx->params = &contikimac_params_BPSK40;
            }
            else if ((channel >= IEEE802154_CHANNEL_MIN) &&
                (channel <= IEEE802154_CHANNEL_MAX)) {
                /* 2.4 GHz band, O-QPSK 250 kbit/s */
                DEBUG("contikimac(%d): using timings for O-QPSK 250 kbit/s\n",
                    thread_getpid());
                ctx->params = &contikimac_params_OQPSK250;
            }
            break;
        case 2:
            if (channel == IEEE802154_CHANNEL_MIN_SUBGHZ) {
                /* 868 MHz band, O-QPSK 100 kbit/s */
                DEBUG("contikimac(%d): using timings for O-QPSK 100 kbit/s\n",
                    thread_getpid());
                ctx->params = &contikimac_params_OQPSK100;
            }
            else if ((channel > IEEE802154_CHANNEL_MIN_SUBGHZ) &&
                     (channel <= IEEE802154_CHANNEL_MAX_SUBGHZ)) {
                /* 915 MHz band, O-QPSK 250 kbit/s */
                DEBUG("contikimac(%d): using timings for O-QPSK 250 kbit/s\n",
                    thread_getpid());
                ctx->params = &contikimac_params_OQPSK250;
            }
            break;
            /* TODO handle more pages and bands */
        default:
            break;
    }
    if (ctx->params == NULL) {
        LOG_ERROR("contikimac(%d): No timing configuration exists "
            "for page %u, channel %u. Fall back to timings for O-QPSK 250 kbit/s\n",
            thread_getpid(), (unsigned int)page, (unsigned int)channel);
        ctx->params = &contikimac_params_OQPSK250;
    }
}

static void contikimac_cancel_timers(contikimac_t *ctx)
{
    xtimer_remove(&ctx->timers.periodic);
    xtimer_remove(&ctx->timers.timeout);
    event_cancel(ctx->evq, &ctx->events.periodic.super);
    ctx->timeout_flag = 0;
}

static void contikimac_set_timeout(contikimac_t *ctx, uint32_t timeout_us)
{
    xtimer_remove(&ctx->timers.timeout);
    ctx->timeout_flag = 0;
    xtimer_set(&ctx->timers.timeout, timeout_us);
}

static void ev_channel_check(event_t *evp)
{
    contikimac_t *ctx = ((event_contikimac_t *)evp)->ctx;
    /* immediately schedule the next channel check */
    xtimer_set(&ctx->timers.channel_check, ctx->channel_check_period);
    contikimac_channel_check(ctx);
}

static void ev_channel_check_no_resched(event_t *evp)
{
    contikimac_t *ctx = ((event_contikimac_t *)evp)->ctx;
    /* skip scheduling a new channel check */
    contikimac_channel_check(ctx);
}

static void ev_periodic(event_t *evp)
{
    contikimac_t *ctx = ((event_contikimac_t *)evp)->ctx;
    contikimac_periodic_handler(ctx);
}

static void ev_isr(event_t *evp)
{
    contikimac_t *ctx = ((event_contikimac_t *)evp)->ctx;
    contikimac_netdev_isr(&ctx->dev.netdev);
}
