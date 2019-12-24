/*
 * Copyright (C) 2019 Alexei Bezborodov
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 * @file
 * @brief       Low-level RTC driver implementation
 *
 * @author      Alexei Bezborodov <alexeibv+riotos@narod.ru>
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtc.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

static struct {
    rtc_alarm_cb_t cb;          /**< callback called from RTC interrupt */
    void *arg;                  /**< argument passed to the callback */
} isr_ctx;

static void _rtc_enter_config_mode(void)
{
    /* disable backup domain write protection */
    PWR->CR |= PWR_CR_DBP;

    /* wait until the RTOFF bit is 1 (no RTC register writes ongoing). */
    while ((RTC->CRL & RTC_CRL_RTOFF) == 0) {}

    /* enter configuration mode. */
    RTC->CRL |= RTC_CRL_CNF;
}

static void _rtc_exit_config_mode(void)
{
    /* exit configuration mode. */
    RTC->CRL &= ~RTC_CRL_CNF;

    /* wait until the RTOFF bit is 1 (our RTC register write finished). */
    while ((RTC->CRL & RTC_CRL_RTOFF) == 0) {}

    /* disable backup domain write protection */
    PWR->CR &= ~PWR_CR_DBP;
}

static bool _is_rtc_enable(void)
{
    return ((RCC->BDCR & RCC_BDCR_RTCEN) == RCC_BDCR_RTCEN);
}

static void _rtc_config(void)
{
    DEBUG("[RTC] config\n");

    /* enable APB1 clocks */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;

    /* disable backup domain write protection */
    PWR->CR |= PWR_CR_DBP;

    /* resets the entire backup domain */
    RCC->BDCR |= RCC_BDCR_BDRST;
    /* reset not activated */
    RCC->BDCR &= ~RCC_BDCR_BDRST;

    /* oscillator clock used as RTC clock */
    RCC->BDCR |= RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_LSE;

    /* turn on LSE crystal */
    RCC->BDCR |= RCC_BDCR_LSEON;
    while ((RCC->BDCR & RCC_BDCR_LSEON) != RCC_BDCR_LSEON) {}

    /* calibration clock from 0 to 0x7F */
    BKP->RTCCR |= 0;

    /* second interrupt is disabled. */
    RTC->CRH &= ~RTC_CRH_SECIE;

    _rtc_enter_config_mode();

    /* if the input clock frequency (fRTCCLK) is 32.768 kHz, write 7FFFh in this register to get a signal period of 1 second. */
    RTC->PRLH = 0;
    RTC->PRLL = 0x7FFF;

    _rtc_exit_config_mode();

    /* wait registers synchronize flag */
    RTC->CRL &= (uint16_t)~RTC_CRL_RSF;
    while((RTC->CRL & RTC_CRL_RSF) != RTC_CRL_RSF) {}

    /* disable backup domain write protection */
    PWR->CR &= ~PWR_CR_DBP;
}

static time_t _rtc_get_time(void)
{
    return (RTC->CNTH << 16) | RTC->CNTL;
}

static void _rtc_set_time(time_t counter_val)
{
    _rtc_enter_config_mode();
    RTC->CNTH = (counter_val & 0xffff0000) >> 16;
    RTC->CNTL = counter_val & 0x0000ffff;
    _rtc_exit_config_mode();
}

void rtc_init(void)
{
    /* save current time if RTC already works */
    bool is_rtc_enable = _is_rtc_enable();
    time_t cur_time = 0;
    if (is_rtc_enable) {
        cur_time = _rtc_get_time();
    }

    /* config RTC */
    _rtc_config();

    /* restore current time after config */
    if (is_rtc_enable) {
        _rtc_set_time(cur_time);
    }
}

int rtc_set_time(struct tm *time)
{
    time_t timestamp = mktime(time);

    if (timestamp == -1) {
        return -1;
    }

    _rtc_set_time(timestamp);

    DEBUG("%s timestamp=%"PRIu32"\n", __func__, (uint32_t)timestamp);

    return 0;
}

int rtc_get_time(struct tm *time)
{
    time_t timestamp = _rtc_get_time();
    localtime_r(&timestamp, time);

    DEBUG("%s timestamp=%"PRIu32"\n", __func__, (uint32_t)timestamp);

    return 0;
}

static void _rtc_enable_alarm(void)
{
    /* clear alarm flag */
    RTC->CRL &= ~RTC_CRL_ALRF;

    _rtc_enter_config_mode();
    RTC->CRH |= (RTC_CRH_ALRIE);
    _rtc_exit_config_mode();
}

static void _rtc_disable_alarm(void)
{
    _rtc_enter_config_mode();
    RTC->CRH &= ~(RTC_CRH_ALRIE | RTC_CRH_SECIE);
    _rtc_exit_config_mode();
}

static time_t _rtc_get_alarm_time(void)
{
    return (RTC->ALRH << 16) | RTC->ALRL;
}

static void _rtc_set_alarm_time(time_t alarm_time)
{
    _rtc_enter_config_mode();
    RTC->ALRL = (alarm_time & 0x0000ffff);
    RTC->ALRH = (alarm_time & 0xffff0000) >> 16;
    _rtc_exit_config_mode();
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* enable global RTC interrupt */
    if (cb && !NVIC_GetEnableIRQ(RTC_IRQn)) {
        NVIC_EnableIRQ(RTC_IRQn);
        DEBUG("%s enable RTC IRQ\n", __func__);
    }

    /* save callback and argument */
    isr_ctx.cb = cb;
    isr_ctx.arg = arg;

    /* set wakeup time */
    time_t timestamp = mktime(time);

    if (timestamp == -1) {
        return -2;
    }

    _rtc_disable_alarm();
    _rtc_set_alarm_time(timestamp);

    /* enable Alarm */
    _rtc_enable_alarm();

    DEBUG("%s timestamp=%"PRIu32"\n", __func__, (uint32_t)timestamp);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    time_t timestamp = _rtc_get_alarm_time();
    localtime_r(&timestamp, time);

    DEBUG("%s timestamp=%"PRIu32"\n", __func__, (uint32_t)timestamp);

    return 0;
}

static void _rtc_clear_isr_ctx(void)
{
    isr_ctx.cb = NULL;
    isr_ctx.arg = NULL;
}

void rtc_clear_alarm(void)
{
    _rtc_disable_alarm();
    _rtc_set_alarm_time(0);

    _rtc_clear_isr_ctx();
}

void rtc_poweron(void)
{
    /* RTC is always on */
    return;
}

void rtc_poweroff(void)
{
    /* RTC is always on */
    return;
}

void isr_rtc(void)
{
    _rtc_disable_alarm();

    if (isr_ctx.cb != NULL) {
        isr_ctx.cb(isr_ctx.arg);
    }

    cortexm_isr_end();
}
