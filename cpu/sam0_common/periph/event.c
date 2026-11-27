/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam0_common
 * @{
 *
 * @file
 * @brief       Low-level EVSYS driver implementation
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "bitarithm.h"
#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph_event.h"

#ifndef EVSYS_CHANNELS
#error "EVSYS_CHANNELS must be defined by CPU"
#endif

#ifndef EVSYS_GCLK_SOURCE
#define EVSYS_GCLK_SOURCE   SAM0_GCLK_PERIPH
#endif

static uint32_t _evsys_channel_free;

static void _poweron(void)
{
#ifdef MCLK
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_EVSYS;
#else
    PM->APBCMASK.reg |= PM_APBCMASK_EVSYS;
#endif
}

MAYBE_UNUSED
static void _poweroff(void)
{
#ifdef MCLK
    MCLK->APBBMASK.reg &= ~MCLK_APBBMASK_EVSYS;
#else
    PM->APBCMASK.reg &= ~PM_APBCMASK_EVSYS;
#endif
}

static void _channel_clock_disable(event_channel_t channel)
{
    uint32_t pchctrl = GCLK->PCHCTRL[EVSYS_GCLK_ID_0 + channel].reg;

    GCLK->PCHCTRL[EVSYS_GCLK_ID_0 + channel].reg = pchctrl & (~GCLK_PCHCTRL_CHEN);
    do {
        pchctrl = GCLK->PCHCTRL[EVSYS_GCLK_ID_0 + channel].reg;
    } while (pchctrl & GCLK_PCHCTRL_CHEN);
}

static void _channel_clock_enable(event_channel_t channel, uint8_t gclk_src)
{
    _channel_clock_disable(channel);

    uint32_t pchctrl = GCLK_PCHCTRL_GEN(gclk_src);
    GCLK->PCHCTRL[EVSYS_GCLK_ID_0 + channel].reg = pchctrl;

    pchctrl |= GCLK_PCHCTRL_CHEN;
    GCLK->PCHCTRL[EVSYS_GCLK_ID_0 + channel].reg = pchctrl;
    do {
        pchctrl = GCLK->PCHCTRL[EVSYS_GCLK_ID_0 + channel].reg;
    } while (!(pchctrl & GCLK_PCHCTRL_CHEN));
}

void periph_event_init(void)
{
    /* Enable EVSYS clock */
    _poweron();

    /* Software reset */
#ifdef EVSYS_CTRLA_SWRST
    EVSYS->CTRLA.reg = EVSYS_CTRLA_SWRST;
    while (EVSYS->CTRLA.reg & EVSYS_CTRLA_SWRST) {}
#else
    EVSYS->CTRL.reg = EVSYS_CTRL_SWRST;
    while (EVSYS->CTRL.reg & EVSYS_CTRL_SWRST) {}
#endif
    /* All channels free */
    _evsys_channel_free = (((1u << (EVSYS_CHANNELS - 1)) - 1) << 1) | 1;
}

event_channel_t periph_event_channel_alloc(void)
{
    event_channel_t ch;
    int status = irq_disable();

    if (_evsys_channel_free) {
        ch = bitarithm_lsb(_evsys_channel_free);
        CLRBIT(_evsys_channel_free, 1u << ch);
    }
    else {
        ch = -1;
    }

    irq_restore(status);
    return ch;
}

void periph_event_channel_free(event_channel_t channel)
{
    assert(channel < EVSYS_CHANNELS);

    int status = irq_disable();
    SETBIT(_evsys_channel_free, 1u << channel);
#ifdef EVSYS_CHANNEL_CHANNEL_Msk
    EVSYS->CHANNEL.reg &= ~EVSYS_CHANNEL_CHANNEL_Msk;
#else
    EVSYS->Channel[channel].CHANNEL.reg = 0;
#endif
    _channel_clock_disable(channel);
    irq_restore(status);
}

void periph_event_channel_config(event_channel_t channel,
                                 event_generator_t generator,
                                 event_path_t path,
                                 event_edge_t edge)
{
    assert(channel < EVSYS_CHANNELS);

    /* Configure event generator */
#ifdef EVSYS_CHANNEL_CHANNEL_Pos
    /* handle older hardware */
    EVSYS->CHANNEL.reg =
        EVSYS_CHANNEL_CHANNEL(channel) |
        EVSYS_CHANNEL_EVGEN(generator) |
        EVSYS_CHANNEL_PATH(path) |
        EVSYS_CHANNEL_EDGSEL(edge);
#else
    EVSYS->Channel[channel].CHANNEL.reg =
        EVSYS_CHANNEL_EVGEN(generator) |
        EVSYS_CHANNEL_PATH(path) |
        EVSYS_CHANNEL_EDGSEL(edge);
#endif

    if (path != EVENT_PATH_ASYNCHRONOUS) {
        _channel_clock_enable(channel, EVSYS_GCLK_SOURCE);
    }
}

void periph_event_attach(event_channel_t channel, event_user_t user)
{
    assert(channel < EVSYS_CHANNELS);

    /* Attach user to channel  */
#ifdef EVSYS_USER_USER_Pos
    EVSYS->USER.reg = (EVSYS_USER_CHANNEL(channel + 1) | EVSYS_USER_USER(user));
#else
    EVSYS->USER[user].reg = EVSYS_USER_CHANNEL(channel + 1);
#endif
}

void periph_event_detach(event_channel_t channel, event_user_t user)
{
    (void)channel;
    assert(channel < EVSYS_CHANNELS);

    /* Detach user from channel */
#ifdef EVSYS_USER_USER_Msk
    EVSYS->USER.reg &= ~EVSYS_USER_USER_Msk;
#else
    EVSYS->USER[user].reg = 0;
#endif
}

