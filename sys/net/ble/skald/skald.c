/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     ble_skald
 * @{
 *
 * @file
 * @brief       Skald's link layer implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "random.h"
#include "luid.h"

#include "net/netdev/ble.h"
#include "net/skald.h"

/* include fitting radio driver */
#if defined(MODULE_NRFBLE)
#include "nrfble.h"
/* add other BLE radio drivers once implemented - and potentially move to
 * auto-init at some point */
#else
#error "[skald] error: unable to find any netdev-ble capable radio"
#endif

#define ENABLE_DEBUG            0
#include "debug.h"

#define JITTER_MIN              (0U)            /* 0ms */
#define JITTER_MAX              (10U)           /* 10ms */

#define ADV_CHAN_NUMOF          sizeof(_adv_chan)
#define ADV_AA                  (0x8e89bed6)    /* access address */
#define ADV_CRC                 (0x00555555)    /* CRC initializer */

#if IS_USED(MODULE_SKALD_UPDATE_PKT_CB)
#  define _on_adv_evt_isr       _on_adv_evt_event
#else
#  define _on_adv_evt_isr       _on_adv_evt
#endif

static const uint8_t _adv_chan[] = SKALD_ADV_CHAN;

static netdev_ble_ctx_t _ble_ctx = {
    .aa.u32 = ADV_AA,
    .crc = ADV_CRC,
};

static netdev_t *_radio;

static void _stop_radio(void)
{
    netdev_ble_stop(_radio);
    _radio->context = NULL;
}

static void _sched_next(skald_ctx_t *ctx)
{
    ctx->last += ctx->adv_itvl_ms;
    /* schedule next advertising event, adding a random jitter between
     * 0ms and 10ms (see spec v5.0-vol6-b-4.4.2.2.1) */
    ctx->last += random_uint32_range(JITTER_MIN, JITTER_MAX);
    /* compensate the time passed since the timer triggered last by using the
     * current value of the timer */
    ztimer_now_t next = (ctx->last > ztimer_now(ZTIMER_MSEC))
                      ? (ctx->last - ztimer_now(ZTIMER_MSEC))
                      : 0;
    ztimer_set(ZTIMER_MSEC, &ctx->timer, next);
}

static void _on_adv_evt(void *arg)
{
    skald_ctx_t *ctx = arg;

    /* advertise on the next adv channel - or skip this event if the radio is
     * busy */
    if ((ctx->cur_chan < ADV_CHAN_NUMOF) && (_radio->context == NULL)) {
        _radio->context = ctx;
        _ble_ctx.chan = _adv_chan[ctx->cur_chan];
#if IS_USED(MODULE_SKALD_UPDATE_PKT_CB)
        if (ctx->update_pkt) {
            ctx->update_pkt(ctx);
        }
#endif
        netdev_ble_set_ctx(_radio, &_ble_ctx);
        netdev_ble_send(_radio, &ctx->pkt);
        ++ctx->cur_chan;
    }
    else {
        ctx->cur_chan = 0;
        _sched_next(ctx);
    }
}

#if IS_USED(MODULE_SKALD_UPDATE_PKT_CB)
static void _event_handler(event_t *event)
{
    skald_ctx_t *ctx = container_of(event, skald_ctx_t, event);

    _on_adv_evt(ctx);
}

static void _on_adv_evt_event(void *arg)
{
    skald_ctx_t *ctx = arg;

    ctx->event.handler = _event_handler;
    event_post(&ctx->queue, &ctx->event);
}
#endif

static void _on_radio_evt(netdev_t *netdev, netdev_event_t event)
{
    (void)netdev;

    if (event == NETDEV_EVENT_TX_COMPLETE) {
        skald_ctx_t *ctx = _radio->context;
        _stop_radio();
        _on_adv_evt_isr(ctx);
    }
}

void skald_init(void)
{
    /* setup and a fitting radio driver - potentially move to auto-init at some
     * point */
#if defined(MODULE_NRFBLE)
    _radio = nrfble_setup();
#endif

    _radio->event_callback = _on_radio_evt;
    _radio->driver->init(_radio);
}

void skald_adv_start(skald_ctx_t *ctx)
{
    assert(ctx);

    /* make sure the given context is not advertising at the moment */
    skald_adv_stop(ctx);

    /* initialize advertising context */
    ctx->timer.callback = _on_adv_evt_isr;
    ctx->timer.arg = ctx;
    ctx->last = ztimer_now(ZTIMER_MSEC);
    ctx->cur_chan = 0;
    ctx->pkt.flags = (BLE_ADV_NONCON_IND | BLE_LL_FLAG_TXADD);
#if IS_USED(MODULE_SKALD_UPDATE_PKT_CB)
    event_queue_init(&ctx->queue);
#endif

    /* start advertising */
    _sched_next(ctx);
#if IS_USED(MODULE_SKALD_UPDATE_PKT_CB)
    event_loop(&ctx->queue);
#endif
}

void skald_adv_stop(skald_ctx_t *ctx)
{
    assert(ctx);

    ztimer_remove(ZTIMER_MSEC, &ctx->timer);
    if (_radio->context == (void *)ctx) {
        _stop_radio();
    }
}

void skald_generate_random_addr(uint8_t *buf)
{
    assert(buf);

    luid_get(buf, BLE_ADDR_LEN);
    /* swap byte 0 and 5, so that the unique byte given by luid does not clash
     * with universal/local and individual/group bits of address */
    uint8_t tmp = buf[5];
    buf[5] = buf[0];
    /* make address individual and local */
    buf[0] = ((tmp & 0xfc) | 0x02);
}
