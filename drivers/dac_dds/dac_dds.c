/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_dac
 * @{
 *
 * @file
 * @brief       Common DAC function fallback implementations
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <assert.h>
#include "board.h"
#include "dac_dds.h"
#include "dac_dds_params.h"
#include "irq.h"
#include "kernel_defines.h"
#include "macros/units.h"
#include "periph/timer.h"

static struct dac_ctx {
    const uint8_t *buffers[2];  /* The two sample buffers                   */
    size_t buffer_len[2];       /* Size of the sample buffers               */
    size_t idx;                 /* Current position in the current buffer   */
    dac_dds_cb_t cb;            /* Called when the current buffer is done   */
    void *cb_arg;               /* Callback argument                        */
    uint16_t sample_ticks;      /* Timer ticks per sample                   */
    uint8_t cur;                /* Active sample buffer                     */
    uint8_t playing;            /* DAC is playing                           */
    uint8_t is_16bit;           /* Sample size is 16 instead of 8 bit       */
} _ctx[DAC_DDS_NUMOF];

static void _timer_cb(void *arg, int chan)
{
    (void)chan;

    struct dac_ctx *ctx = arg;

    dac_dds_t dac_dds  = ctx - _ctx;
    dac_t dac          = dac_dds_params[dac_dds].dac;
    const uint8_t cur  = ctx->cur;
    const uint8_t *buf = ctx->buffers[cur];
    const size_t len   = ctx->buffer_len[cur];

    if (ctx->is_16bit) {
        uint8_t l = buf[ctx->idx++];
        uint8_t h = buf[ctx->idx++];

        dac_set(dac, (h << 8) | l);
    } else {
        dac_set(dac, buf[ctx->idx++] << 8);
    }

    if (ctx->idx >= len) {

        /* invalidate old buffer */
        ctx->buffer_len[cur] = 0;

        ctx->idx = 0;
        ctx->cur = !cur;

        /* stop playing if no more samples are queued */
        if (ctx->buffer_len[!cur] == 0) {
            ctx->playing = 0;
            timer_stop(dac_dds_params[dac_dds].timer);
        /* notify user that next sample buffer can be queued */
        } else if (ctx->cb) {
            ctx->cb(ctx->cb_arg);
        }
    }
}

void dac_dds_init(dac_dds_t dac, uint16_t sample_rate, uint8_t flags,
                   dac_dds_cb_t cb, void *cb_arg)
{
    assert(dac < DAC_DDS_NUMOF);

    _ctx[dac].cb           = cb;
    _ctx[dac].cb_arg       = cb_arg;
    _ctx[dac].sample_ticks = dac_dds_params[dac].timer_hz / sample_rate;
    _ctx[dac].is_16bit     = flags & DAC_FLAG_16BIT;

    timer_init(dac_dds_params[dac].timer, dac_dds_params[dac].timer_hz, _timer_cb, &_ctx[dac]);
}

void dac_dds_set_cb(dac_dds_t dac, dac_dds_cb_t cb, void *cb_arg)
{
    unsigned state = irq_disable();

    /* allow to update cb_arg independent of cb */
    if (cb || cb_arg == NULL) {
        _ctx[dac].cb     = cb;
    }
    _ctx[dac].cb_arg = cb_arg;

    irq_restore(state);
}

bool dac_dds_play(dac_dds_t dac, const void *buf, size_t len)
{
    struct dac_ctx *ctx = &_ctx[dac];

    unsigned state = irq_disable();

    uint8_t next          = !ctx->cur;
    ctx->buffers[next]    = buf;
    ctx->buffer_len[next] = len;

    bool is_playing = ctx->playing;
    irq_restore(state);

    if (is_playing) {
        return true;
    }

    ctx->playing = 1;
    ctx->cur = next;

    timer_set_periodic(dac_dds_params[dac].timer, 0, ctx->sample_ticks,
                       TIM_FLAG_RESET_ON_MATCH | TIM_FLAG_RESET_ON_SET);

    /* We can already queue the next buffer */
    if (ctx->cb) {
        ctx->cb(ctx->cb_arg);
    }

    return false;
}

void dac_dds_stop(dac_dds_t dac)
{
    timer_stop(dac_dds_params[dac].timer);
    _ctx[dac].playing = 0;
}
