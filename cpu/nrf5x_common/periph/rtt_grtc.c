/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf54l
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       RTT implementation for the nRF54L family, based on the GRTC
 *              peripheral
 *
 * The nRF54L has no classical RTC peripheral, low power timing is provided
 * by the GRTC (Global Real-Time Counter): a 52-bit, 1 MHz counter running
 * from the LFCLK (interpolated with the 16 MHz clock when active).
 *
 * This driver exposes the lower 32 bits of the SYSCOUNTER. As the hardware
 * has no 32-bit overflow event, the overflow callback is emulated with a
 * compare channel set to the next multiple of 2^32.
 *
 * The secure application core uses the GRTC IRQ group 2 (see GRTC_IRQ_GROUP
 * in the nRF54L15 interim vendor header): IRQ line GRTC_2, the INTENSET2
 * interrupt enable register and the SYSCOUNTER[2] counter view.
 *
 * Compare channel 0 is left unused (it is coupled with the INTERVAL
 * auto-reload feature), channel 1 is used for the user alarm and channel 2
 * for the overflow emulation.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "periph/rtt.h"
#include "periph_conf.h"

/* SYSCOUNTER view and interrupt group of the secure application core */
#define GRTC_DOMAIN_IDX     (2U)

/* compare channel assignment */
#define CHAN_ALARM          (1U)
#define CHAN_OVERFLOW       (2U)

/* Maximum number of SYSCOUNTER ticks the counter may advance between the
 * moment the caller computed the alarm value (from rtt_get_counter()) and the
 * moment the compare channel is actually armed below. An alarm that maps
 * within this many ticks into the past is treated as imminent (fire as soon
 * as possible) instead of being deferred by a full 32-bit period. */
#define SCHED_GUARD         (100U)

/* Minimum number of SYSCOUNTER ticks an alarm is armed ahead of the counter.
 * The 52-bit SYSCOUNTER does not wrap in practice, so a compare set to a value
 * the counter already passed would never match. Keeping the target this many
 * ticks in the future guarantees the alarm fires from a regular (non-reentrant)
 * compare interrupt shortly after, even for an offset of 0. The value must be
 * larger than the worst case arming overhead so the retry loop terminates in a
 * single pass. */
#define RTT_MIN_FUTURE      (10U)

/* LFCLK source used to clock the GRTC, the board can override this (e.g.
 * CLOCK_LFCLK_SRC_SRC_LFRC when no 32.768 kHz crystal is mounted) */
#ifndef CLOCK_LFCLK
#define CLOCK_LFCLK         (CLOCK_LFCLK_SRC_SRC_LFXO)
#endif

typedef struct {
    rtt_cb_t cb;
    void *arg;
} rtt_ctx_t;

static rtt_ctx_t alarm_ctx;
static rtt_ctx_t overflow_ctx;

static uint64_t _counter64(void)
{
    uint32_t low, high;

    /* if the lower part overflowed between the two register reads, the
     * OVERFLOW flag is set in the higher part and the read is repeated */
    do {
        low = NRF_GRTC->SYSCOUNTER[GRTC_DOMAIN_IDX].SYSCOUNTERL;
        high = NRF_GRTC->SYSCOUNTER[GRTC_DOMAIN_IDX].SYSCOUNTERH;
    } while (high & GRTC_SYSCOUNTER_SYSCOUNTERH_OVERFLOW_Msk);

    return ((uint64_t)(high & GRTC_SYSCOUNTER_SYSCOUNTERH_VALUE_Msk) << 32) | low;
}

static void _set_cc(unsigned chan, uint64_t target)
{
    /* writing CCL disables the compare channel, writing CCH re-arms it */
    NRF_GRTC->CC[chan].CCL = (uint32_t)target;
    NRF_GRTC->CC[chan].CCH = (uint32_t)(target >> 32) & GRTC_CC_CCH_CCH_Msk;
    NRF_GRTC->CC[chan].CCEN = GRTC_CC_CCEN_ACTIVE_Enable;
}

/* arm @p chan to fire when the SYSCOUNTER reaches the 52-bit @p target */
static void _arm(unsigned chan, uint64_t target)
{
    NRF_GRTC->EVENTS_COMPARE[chan] = 0;
    _set_cc(chan, target);
    NRF_GRTC->INTENSET2 = (GRTC_INTENSET2_COMPARE0_Msk << chan);
}

/* arm the overflow compare on the next 2^32 boundary, always in the future */
static void _arm_overflow(void)
{
    uint64_t now = _counter64();
    uint64_t target = (now & ~(uint64_t)UINT32_MAX) + (1ULL << 32);

    _arm(CHAN_OVERFLOW, target);
}

void rtt_init(void)
{
    /* start the low frequency clock, the GRTC needs it as time base */
    if (!NRF_CLOCK->LFCLK.RUN) {
        NRF_CLOCK->LFCLK.SRC = CLOCK_LFCLK;
        NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
        NRF_CLOCK->TASKS_LFCLKSTART = 1;
        while (!NRF_CLOCK->EVENTS_LFCLKSTARTED) {}
    }

    /* start the SYSCOUNTER */
    NRF_GRTC->TASKS_CLEAR = 1;
    NRF_GRTC->MODE = GRTC_MODE_SYSCOUNTEREN_Msk;
    NRF_GRTC->TASKS_START = 1;

    /* keep the SYSCOUNTER in active state even when the CPU sleeps, so that
     * reads are always valid and compares are cycle accurate */
    NRF_GRTC->SYSCOUNTER[GRTC_DOMAIN_IDX].ACTIVE = 1;

    NVIC_EnableIRQ(GRTC_2_IRQn);
}

uint32_t rtt_get_counter(void)
{
    return (uint32_t)_counter64();
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    overflow_ctx.cb = cb;
    overflow_ctx.arg = arg;

    /* emulate the 32-bit overflow with a compare on the next multiple
     * of 2^32 */
    _arm_overflow();
}

void rtt_clear_overflow_cb(void)
{
    overflow_ctx.cb = NULL;
    NRF_GRTC->INTENCLR2 = (GRTC_INTENSET2_COMPARE0_Msk << CHAN_OVERFLOW);
    NRF_GRTC->CC[CHAN_OVERFLOW].CCEN = GRTC_CC_CCEN_ACTIVE_Disable;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    alarm_ctx.cb = cb;
    alarm_ctx.arg = arg;

    uint64_t now = _counter64();
    uint64_t target = (now & ~(uint64_t)UINT32_MAX) | alarm;

    /* only defer to the next 32-bit period if the alarm lies clearly (more
     * than the arming race window) in the past, otherwise it was meant to fire
     * (almost) immediately and must not be pushed a full period ahead */
    if ((int64_t)(now - target) > (int64_t)SCHED_GUARD) {
        target += (1ULL << 32);
    }

    /* An alarm that maps into the (recent) past would never match the 52-bit
     * SYSCOUNTER. Keep the target at least RTT_MIN_FUTURE ticks ahead and
     * re-check after arming the channel: should the counter have reached it
     * meanwhile, push it forward and re-arm. This way the callback always
     * fires from a regular compare interrupt shortly after, never reentrantly
     * from within this call. */
    uint64_t cnt;
    do {
        cnt = _counter64();
        if ((int64_t)(target - cnt) < (int64_t)RTT_MIN_FUTURE) {
            target = cnt + RTT_MIN_FUTURE;
        }
        _set_cc(CHAN_ALARM, target);
    } while ((int64_t)(_counter64() - target) >= 0);

    NRF_GRTC->EVENTS_COMPARE[CHAN_ALARM] = 0;
    NRF_GRTC->INTENSET2 = (GRTC_INTENSET2_COMPARE0_Msk << CHAN_ALARM);
}

uint32_t rtt_get_alarm(void)
{
    return NRF_GRTC->CC[CHAN_ALARM].CCL;
}

void rtt_clear_alarm(void)
{
    alarm_ctx.cb = NULL;
    NRF_GRTC->INTENCLR2 = (GRTC_INTENSET2_COMPARE0_Msk << CHAN_ALARM);
    NRF_GRTC->CC[CHAN_ALARM].CCEN = GRTC_CC_CCEN_ACTIVE_Disable;
}

void rtt_poweron(void)
{
    NRF_GRTC->TASKS_START = 1;
}

void rtt_poweroff(void)
{
    NRF_GRTC->TASKS_STOP = 1;
}

void isr_grtc_2(void)
{
    if (NRF_GRTC->EVENTS_COMPARE[CHAN_ALARM]) {
        NRF_GRTC->EVENTS_COMPARE[CHAN_ALARM] = 0;
        NRF_GRTC->INTENCLR2 = (GRTC_INTENSET2_COMPARE0_Msk << CHAN_ALARM);
        NRF_GRTC->CC[CHAN_ALARM].CCEN = GRTC_CC_CCEN_ACTIVE_Disable;
        if (alarm_ctx.cb) {
            alarm_ctx.cb(alarm_ctx.arg);
        }
    }

    if (NRF_GRTC->EVENTS_COMPARE[CHAN_OVERFLOW]) {
        NRF_GRTC->EVENTS_COMPARE[CHAN_OVERFLOW] = 0;
        if (overflow_ctx.cb) {
            overflow_ctx.cb(overflow_ctx.arg);
        }
        /* re-arm for the next overflow */
        if (overflow_ctx.cb) {
            _arm_overflow();
        }
        else {
            NRF_GRTC->INTENCLR2 = (GRTC_INTENSET2_COMPARE0_Msk << CHAN_OVERFLOW);
        }
    }

    cortexm_isr_end();
}
