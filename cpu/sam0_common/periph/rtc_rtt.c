/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_rtc
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file        rtc_rtt.c
 * @brief       Low-level RTC/RTT driver implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      FWX <FWX@dialine.fr>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdint.h>
#include "periph/rtc.h"
#include "periph/rtt.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* SAML21 rev B needs an extra bit, which in rev A defaults to 1, but isn't
 * visible. Thus define it here. */
#ifndef RTC_MODE0_CTRLA_COUNTSYNC
#define RTC_MODE0_CTRLA_COUNTSYNC_Pos   15
#define RTC_MODE0_CTRLA_COUNTSYNC       (0x1ul << RTC_MODE0_CTRLA_COUNTSYNC_Pos)
#endif
#ifndef RTC_MODE2_CTRLA_CLOCKSYNC
#define RTC_MODE2_CTRLA_CLOCKSYNC_Pos   15
#define RTC_MODE2_CTRLA_CLOCKSYNC       (0x1ul << RTC_MODE2_CTRLA_CLOCKSYNC_Pos)
#endif

#ifdef REG_RTC_MODE0_CTRLA
#define RTC_MODE0_PRESCALER       \
    (__builtin_ctz(2 * RTT_CLOCK_FREQUENCY / RTT_FREQUENCY) << \
    RTC_MODE0_CTRLA_PRESCALER_Pos)
#else
#define RTC_MODE0_PRESCALER       \
    (__builtin_ctz(RTT_CLOCK_FREQUENCY / RTT_FREQUENCY) << \
    RTC_MODE0_CTRL_PRESCALER_Pos)
#endif

typedef struct {
    rtc_alarm_cb_t cb;        /**< callback called from RTC interrupt */
    void *arg;                /**< argument passed to the callback */
} rtc_state_t;

static rtc_state_t alarm_cb;
static rtc_state_t overflow_cb;

/* At 1Hz, RTC goes till 63 years (2^5, see 17.8.22 in datasheet)
 * struct tm younts the year since 1900, use the difference to RIOT_EPOCH
 * as an offset so the user can set years in RIOT_EPOCH + 63
 */
static uint16_t reference_year = RIOT_EPOCH - 1900;

static void _wait_syncbusy(void)
{
    if (IS_ACTIVE(MODULE_PERIPH_RTT)) {
#ifdef REG_RTC_MODE0_SYNCBUSY
    while (RTC->MODE0.SYNCBUSY.reg) {}
#else
    while (RTC->MODE0.STATUS.bit.SYNCBUSY) {}
#endif
    } else {
#ifdef REG_RTC_MODE2_SYNCBUSY
    while (RTC->MODE2.SYNCBUSY.reg) {}
#else
    while (RTC->MODE2.STATUS.bit.SYNCBUSY) {}
#endif
    }
}

#if defined(MODULE_PERIPH_RTC) || defined(MODULE_PERIPH_RTT)
static void _read_req(void)
{
#ifdef RTC_READREQ_RREQ
    RTC->MODE0.READREQ.reg = RTC_READREQ_RREQ;
    _wait_syncbusy();
#endif
}
#endif

static void _poweron(void)
{
#ifdef MCLK
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
#else
    PM->APBAMASK.reg |= PM_APBAMASK_RTC;
#endif
}

__attribute__((unused))
static bool _power_is_on(void)
{
#ifdef MCLK
    return MCLK->APBAMASK.reg & MCLK_APBAMASK_RTC;
#else
    return PM->APBAMASK.reg & PM_APBAMASK_RTC;
#endif
}

__attribute__((unused))
static void _poweroff(void)
{
#ifdef MCLK
    MCLK->APBAMASK.reg &= ~MCLK_APBAMASK_RTC;
#else
    PM->APBAMASK.reg &= ~PM_APBAMASK_RTC;
#endif
}

static inline void _rtc_set_enabled(bool on)
{
#ifdef REG_RTC_MODE2_CTRLA
    RTC->MODE2.CTRLA.bit.ENABLE = on;
#else
    RTC->MODE2.CTRL.bit.ENABLE = on;
#endif
    _wait_syncbusy();
}

static inline void _rtt_reset(void)
{
#ifdef RTC_MODE0_CTRL_SWRST
    RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_SWRST;
    while (RTC->MODE0.CTRL.bit.SWRST) {}
#else
    RTC->MODE0.CTRLA.reg = RTC_MODE2_CTRLA_SWRST;
    while (RTC->MODE0.CTRLA.bit.SWRST) {}
#endif
}

#ifdef CPU_COMMON_SAMD21
#ifdef MODULE_PERIPH_RTC
static void _rtc_clock_setup(void)
{
    /* Use 1024 Hz GCLK */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_GEN(SAM0_GCLK_1KHZ)
                      | GCLK_CLKCTRL_ID_RTC;
    while (GCLK->STATUS.bit.SYNCBUSY) {}
}
#endif /* MODULE_PERIPH_RTC */

#ifdef MODULE_PERIPH_RTT
static void _rtt_clock_setup(void)
{
    /* Use 32 kHz GCLK */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_GEN(SAM0_GCLK_32KHZ)
                      | GCLK_CLKCTRL_ID_RTC;
    while (GCLK->STATUS.bit.SYNCBUSY) {}
}
#endif /* MODULE_PERIPH_RTT */

#else /* CPU_COMMON_SAMD21 - Clock Setup */

#ifdef MODULE_PERIPH_RTC
static void _rtc_clock_setup(void)
{
    /* RTC source clock is external oscillator at 1kHz */
#if EXTERNAL_OSC32_SOURCE
    OSC32KCTRL->XOSC32K.bit.EN1K = 1;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K;

    /* RTC uses internal 32,768KHz Oscillator */
#elif INTERNAL_OSC32_SOURCE
    OSC32KCTRL->OSC32K.bit.EN1K = 1;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K;

    /* RTC uses Ultra Low Power internal 32,768KHz Oscillator */
#elif ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K;

#else
#error "No clock source for RTC selected. "
#endif
}
#endif /* MODULE_PERIPH_RTC */

#if defined(MODULE_PERIPH_RTT) || RTC_NUM_OF_TAMPERS
static void _rtt_clock_setup(void)
{
    /* RTC source clock is external oscillator at 32kHz */
#if EXTERNAL_OSC32_SOURCE
    OSC32KCTRL->XOSC32K.bit.EN32K = 1;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K;

    /* RTC uses internal 32,768KHz Oscillator */
#elif INTERNAL_OSC32_SOURCE
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_OSC32K;

    /* RTC uses Ultra Low Power internal 32,768KHz Oscillator */
#elif ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K;

#else
#error "No clock source for RTT selected. "
#endif
}
#endif /* MODULE_PERIPH_RTT */
#endif /* !CPU_COMMON_SAMD21 - Clock Setup */

#ifdef MODULE_PERIPH_RTC
static void _rtc_init(void)
{
#ifdef REG_RTC_MODE2_CTRLA
    if (RTC->MODE2.CTRLA.bit.MODE == RTC_MODE2_CTRLA_MODE_CLOCK_Val) {
        return;
    }

    _rtt_reset();

    /* RTC config with RTC_MODE2_CTRL_CLKREP = 0 (24h) */
    RTC->MODE2.CTRLA.reg = RTC_MODE2_CTRLA_PRESCALER_DIV1024   /* CLK_RTC_CNT = 1KHz / 1024 -> 1Hz */
                         | RTC_MODE2_CTRLA_CLOCKSYNC           /* Clock Read Synchronization Enable */
                         | RTC_MODE2_CTRLA_MODE_CLOCK;
#else
    if (RTC->MODE2.CTRL.bit.MODE == RTC_MODE2_CTRL_MODE_CLOCK_Val) {
        return;
    }

    _rtt_reset();

    RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_PRESCALER_DIV1024
                        | RTC_MODE2_CTRL_MODE_CLOCK;
#endif
}

void rtc_init(void)
{
    _poweroff();
    _rtc_clock_setup();
    _poweron();

    _rtc_init();

    /* disable all interrupt sources */
    RTC->MODE2.INTENCLR.reg = RTC_MODE2_INTENCLR_MASK;

    /* enable overflow interrupt */
    RTC->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_OVF;

    /* Clear interrupt flags */
    RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_OVF
                           | RTC_MODE2_INTFLAG_ALARM0;

    _rtc_set_enabled(1);

    NVIC_EnableIRQ(RTC_IRQn);
}
#endif /* MODULE_PERIPH_RTC */

#ifdef MODULE_PERIPH_RTT
void rtt_init(void)
{
    _rtt_clock_setup();
    _poweron();
    _rtt_reset();

    /* set 32bit counting mode & enable the RTC */
#ifdef REG_RTC_MODE0_CTRLA
    RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_MODE(0)
                         | RTC_MODE0_CTRLA_ENABLE
                         | RTC_MODE0_CTRLA_COUNTSYNC
                         | RTC_MODE0_PRESCALER;
#else
    RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE(0)
                        | RTC_MODE0_CTRL_ENABLE
                        | RTC_MODE0_PRESCALER;
#endif
    _wait_syncbusy();

    /* initially clear flag */
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0
                           | RTC_MODE0_INTFLAG_OVF;

    NVIC_EnableIRQ(RTC_IRQn);
}
#endif /* MODULE_PERIPH_RTT */

#if RTC_NUM_OF_TAMPERS

static rtc_state_t tamper_cb;
static uint32_t tampctr;

/* check if pin is a RTC tamper pin */
static int _rtc_pin(gpio_t pin)
{
    for (unsigned i = 0; i < ARRAY_SIZE(rtc_tamper_pins); ++i) {
        if (rtc_tamper_pins[i] == pin) {
            return i;
        }
    }

    return -1;
}

static void _set_tampctrl(uint32_t reg)
{
    _rtc_set_enabled(0);
    RTC->MODE0.TAMPCTRL.reg = reg;
    _rtc_set_enabled(1);
}

void rtc_tamper_init(void)
{
    DEBUG("tamper init\n");

    /* configure RTC clock only if it is not already configured */
    if (!IS_ACTIVE(MODULE_PERIPH_RTC) &&
        !IS_ACTIVE(MODULE_PERIPH_RTT)) {

        if (!_power_is_on()) {
            _rtt_clock_setup();
            _poweron();
        }

        /* disable all interrupt sources */
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_MASK;
    }

    /* disable old tamper events */
    _set_tampctrl(0);
    NVIC_EnableIRQ(RTC_IRQn);
}

int rtc_tamper_register(gpio_t pin, gpio_flank_t flank)
{
    int in = _rtc_pin(pin);

    if (in < 0) {
        return -1;
    }

    tampctr |= RTC_TAMPCTRL_IN0ACT_WAKE << (2 * in);

    if (flank == GPIO_RISING) {
        tampctr |= RTC_TAMPCTRL_TAMLVL0 << in;
    } else if (flank == GPIO_FALLING) {
        tampctr &= ~(RTC_TAMPCTRL_TAMLVL0 << in);
    }

    return 0;
}

void rtc_tamper_enable(void)
{
    DEBUG("enable tamper\n");

    /* clear tamper id */
    RTC->MODE0.TAMPID.reg = 0x1F;

    /* write TAMPCTRL register */
    _set_tampctrl(tampctr);

    /* work around errata 2.17.4:
     * ignore the first tamper event on the rising edge */
    if (RTC->MODE0.TAMPCTRL.reg & RTC_TAMPCTRL_TAMLVL_Msk) {

        /* If an RTC alarm happened before, the spurious tamper
         * event is sometimes not generated.
         * Tamper event must happen within one RTC clock period. */
        unsigned timeout = CLOCK_CORECLOCK / 32768;

        /* prevent RTC interrupt from triggering */
        NVIC_DisableIRQ(RTC_IRQn);

        /* enable tamper detect as wake-up source */
        RTC->MODE0.INTENSET.bit.TAMPER = 1;

        /* wait for first tamper event */
        while (!RTC->MODE0.INTFLAG.bit.TAMPER && --timeout) {}

        /* clear tamper flag flag */
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_TAMPER;

        /* restore RTC IRQ */
        NVIC_EnableIRQ(RTC_IRQn);
    } else {
        /* no spurious event on falling edge */
        RTC->MODE0.INTENSET.bit.TAMPER = 1;
    }

    DEBUG("tamper enabled\n");
}

uint8_t rtc_get_tamper_event(void)
{
    uint32_t ret = RTC->MODE0.TAMPID.reg;

    /* clear tamper event */
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_TAMPER;
    RTC->MODE0.TAMPID.reg  = ret;

    return ret & RTC_TAMPID_TAMPID_Msk;
}

uint8_t rtc_tamper_pin_mask(gpio_t pin)
{
    int idx = _rtc_pin(pin);
    if (idx < 0) {
        return 0;
    }

    return 1 << idx;
}
#endif /* RTC_NUM_OF_TAMPERS */

#ifdef MODULE_PERIPH_RTC
int rtc_get_alarm(struct tm *time)
{
    RTC_MODE2_ALARM_Type alarm;

    /* Read alarm register in one time */
    alarm.reg = RTC->MODE2.Mode2Alarm[0].ALARM.reg;

    time->tm_year = alarm.bit.YEAR + reference_year;
    if ((time->tm_year < reference_year) ||
        (time->tm_year > (reference_year + 63))) {
        return -1;
    }

    time->tm_mon = alarm.bit.MONTH - 1;
    time->tm_mday = alarm.bit.DAY;
    time->tm_hour = alarm.bit.HOUR;
    time->tm_min = alarm.bit.MINUTE;
    time->tm_sec = alarm.bit.SECOND;

    return 0;
}

int rtc_get_time(struct tm *time)
{
    RTC_MODE2_CLOCK_Type clock;

    /* Read register in one time */
    _read_req();
    clock.reg = RTC->MODE2.CLOCK.reg;

    time->tm_year = clock.bit.YEAR + reference_year;

    if ((time->tm_year < reference_year) ||
        (time->tm_year > (reference_year + 63))) {
        return -1;
    }

    time->tm_mon = clock.bit.MONTH - 1;
    time->tm_mday = clock.bit.DAY;
    time->tm_hour = clock.bit.HOUR;
    time->tm_min = clock.bit.MINUTE;
    time->tm_sec = clock.bit.SECOND;
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* prevent old alarm from ringing */
    rtc_clear_alarm();

    /* normalize input */
    rtc_tm_normalize(time);

    if ((time->tm_year < reference_year) ||
        (time->tm_year > (reference_year + 63))) {
        return -2;
    }
    else {
        RTC->MODE2.Mode2Alarm[0].ALARM.reg = RTC_MODE2_ALARM_YEAR(time->tm_year - reference_year)
                                           | RTC_MODE2_ALARM_MONTH(time->tm_mon + 1)
                                           | RTC_MODE2_ALARM_DAY(time->tm_mday)
                                           | RTC_MODE2_ALARM_HOUR(time->tm_hour)
                                           | RTC_MODE2_ALARM_MINUTE(time->tm_min)
                                           | RTC_MODE2_ALARM_SECOND(time->tm_sec);
        RTC->MODE2.Mode2Alarm[0].MASK.reg = RTC_MODE2_MASK_SEL(6);
    }

    _wait_syncbusy();

    /* Enable IRQ */
    alarm_cb.cb  = cb;
    alarm_cb.arg = arg;

    /* enable alarm interrupt and clear flag */
    RTC->MODE2.INTFLAG.reg  = RTC_MODE2_INTFLAG_ALARM0;
    RTC->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_ALARM0;

    return 0;
}

int rtc_set_time(struct tm *time)
{
    /* normalize input */
    rtc_tm_normalize(time);

    if ((time->tm_year < reference_year) ||
        (time->tm_year > reference_year + 63)) {
        return -1;
    }
    else {
        RTC->MODE2.CLOCK.reg = RTC_MODE2_CLOCK_YEAR(time->tm_year - reference_year)
                             | RTC_MODE2_CLOCK_MONTH(time->tm_mon + 1)
                             | RTC_MODE2_CLOCK_DAY(time->tm_mday)
                             | RTC_MODE2_CLOCK_HOUR(time->tm_hour)
                             | RTC_MODE2_CLOCK_MINUTE(time->tm_min)
                             | RTC_MODE2_CLOCK_SECOND(time->tm_sec);
    }

    _wait_syncbusy();
    return 0;
}

void rtc_clear_alarm(void)
{
    /* disable alarm interrupt */
    RTC->MODE2.INTENCLR.reg = RTC_MODE2_INTENCLR_ALARM0;
}

void rtc_poweron(void)
{
    _poweron();
}

void rtc_poweroff(void)
{
    _poweroff();
}
#endif /* MODULE_PERIPH_RTC */

#ifdef MODULE_PERIPH_RTT
void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    /* clear overflow cb to avoid race while assigning */
    rtt_clear_overflow_cb();

    /* set callback variables */
    overflow_cb.cb  = cb;
    overflow_cb.arg = arg;

    /* enable overflow interrupt */
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_OVF;
}
void rtt_clear_overflow_cb(void)
{
    /* disable overflow interrupt */
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_OVF;
}

uint32_t rtt_get_counter(void)
{
    _wait_syncbusy();
    _read_req();
    return RTC->MODE0.COUNT.reg;
}

void rtt_set_counter(uint32_t count)
{
    RTC->MODE0.COUNT.reg = count;
    _wait_syncbusy();
}

uint32_t rtt_get_alarm(void)
{
    _wait_syncbusy();
    return RTC->MODE0.COMP[0].reg;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* disable interrupt to avoid race */
    rtt_clear_alarm();

    /* setup callback */
    alarm_cb.cb  = cb;
    alarm_cb.arg = arg;

    /* set COMP register */
    RTC->MODE0.COMP[0].reg = alarm;
    _wait_syncbusy();

    /* enable compare interrupt and clear flag */
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
}

void rtt_clear_alarm(void)
{
    /* disable compare interrupt */
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;
}

void rtt_poweron(void)
{
    _poweron();
}

void rtt_poweroff(void)
{
    _poweroff();
}
#endif /* MODULE_PERIPH_RTT */

static void _isr_rtc(void)
{
    if (!IS_ACTIVE(MODULE_PERIPH_RTC)) {
        return;
    }

    if (RTC->MODE2.INTFLAG.bit.ALARM0) {
        /* clear flag */
        RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0;

        if (alarm_cb.cb) {
            alarm_cb.cb(alarm_cb.arg);
        }
    }
    if (RTC->MODE2.INTFLAG.bit.OVF) {
        /* clear flag */
        RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_OVF;
        /* At 1Hz, RTC goes till 63 years (2^5, see 17.8.22 in datasheet)
        * Start RTC again with reference_year 64 years more (Be careful with alarm set) */
        reference_year += 64;
    }
}

static void _isr_rtt(void)
{
    if (!IS_ACTIVE(MODULE_PERIPH_RTT)) {
        return;
    }

    if (RTC->MODE0.INTFLAG.bit.OVF) {
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF;
        if (overflow_cb.cb) {
            overflow_cb.cb(overflow_cb.arg);
        }
    }
    if (RTC->MODE0.INTFLAG.bit.CMP0) {
        /* clear flag */
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
        /* disable interrupt */
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;
        if (alarm_cb.cb) {
            alarm_cb.cb(alarm_cb.arg);
        }
    }
}

static void _isr_tamper(void)
{
#ifdef RTC_MODE0_INTFLAG_TAMPER
    if (RTC->MODE0.INTFLAG.bit.TAMPER) {
        RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_TAMPER;
        if (tamper_cb.cb) {
            tamper_cb.cb(tamper_cb.arg);
        }
    }
#endif
}

void isr_rtc(void)
{
    _isr_rtc();
    _isr_rtt();
    _isr_tamper();

    cortexm_isr_end();
}
