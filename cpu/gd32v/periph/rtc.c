/*
 * SPDX-FileCopyrightText: 2019 Alexei Bezborodov
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_gd32v
 * @{
 * @file
 * @brief       Low-level RTC driver implementation for GD32VF103
 *
 * This driver is a modified copy of the RTC driver for the STM32F1 family.
 *
 * @author      Alexei Bezborodov <alexeibv+riotos@narod.ru>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtc.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define EXTI_RTC_BIT       (1UL << 17)

static struct {
    rtc_alarm_cb_t cb;      /**< callback called from RTC interrupt */
    void *arg;              /**< argument passed to the callback */
} isr_ctx;

/* forward declaration of ISR */
static void isr_rtc_alarm(unsigned irqn);

static void _rtc_enter_config_mode(void)
{
    /* enable write access to backup domain registers */
    PMU->CTL |= PMU_CTL_BKPWEN_Msk;

    /* wait until the LWOFF bit is 1 (Last write operation finished). */
    while ((RTC->CTL & RTC_CTL_LWOFF_Msk) == 0) { }

    /* enter configuration mode. */
    RTC->CTL |= RTC_CTL_CMF_Msk;
}

static void _rtc_exit_config_mode(void)
{
    /* exit configuration mode. */
    RTC->CTL &= ~RTC_CTL_CMF_Msk;

    /* wait until the LWOFF bit is 1 (Last write operation finished). */
    while ((RTC->CTL & RTC_CTL_LWOFF_Msk) == 0) { }

    /* disable write access to backup domain registers */
    PMU->CTL &= ~PMU_CTL_BKPWEN_Msk;
}

static bool _is_rtc_enable(void)
{
    return ((RCU->BDCTL & RCU_BDCTL_RTCEN_Msk) == RCU_BDCTL_RTCEN_Msk);
}

#define RCU_BDCTL_RTCSRC_CK_LXTAL   1
#define RCU_BDCTL_RTCSRC_CK_IRC40K  2

static void _rtc_config(void)
{
    DEBUG("[RTC] config\n");

    /* enable APB1 clocks */
    RCU->APB1EN |= RCU_APB1EN_PMUEN_Msk | RCU_APB1EN_BKPIEN_Msk;

    /* enable write access to backup domain registers */
    PMU->CTL |= PMU_CTL_BKPWEN_Msk;

    /* resets the entire backup domain */
    RCU->BDCTL |= RCU_BDCTL_BKPRST_Msk;
    /* reset not activated */
    RCU->BDCTL &= ~RCU_BDCTL_BKPRST_Msk;

#if CONFIG_BOARD_HAS_LXTAL
    /* oscillator clock used as RTC clock */
    RCU->BDCTL |= RCU_BDCTL_RTCSRC_CK_LXTAL << RCU_BDCTL_RTCSRC_Pos;
    RCU->BDCTL |= RCU_BDCTL_LXTALEN_Msk;
    while ((RCU->BDCTL & RCU_BDCTL_LXTALSTB_Msk) != RCU_BDCTL_LXTALSTB_Msk) { }
#else
    RCU->BDCTL |= RCU_BDCTL_RTCSRC_CK_IRC40K << RCU_BDCTL_RTCSRC_Pos;
#endif

    /* enable RTC clock */
    RCU->BDCTL |= RCU_BDCTL_RTCEN_Msk;

    /* calibration clock from 0 to 0x7F */
    BKP->OCTL |= 0;
    BKP->OCTL |= BKP_OCTL_ASOEN_Msk;

    /* second interrupt is disabled. */
    RTC->INTEN &= ~RTC_INTEN_SCIE_Msk;

    _rtc_enter_config_mode();

#if CONFIG_BOARD_HAS_LXTAL
    /* if the input clock frequency (fRTCCLK) is 32.768 kHz, write 7FFFh in
     * this register to get a signal period of 1 second. */
    RTC->PSCH = 0;
    RTC->PSCL = 0x7FFF;
#else
    /* if the input clock frequency (fRTCCLK) is 40 kHz, write 39999 in
     * this register to get a signal period of 1 second. */
    RTC->PSCH = 0;
    RTC->PSCL = 39999;
#endif

    _rtc_exit_config_mode();

    /* wait registers synchronize flag */
    RTC->CTL &= (uint16_t)~RTC_CTL_RSYNF_Msk;
    while ((RTC->CTL & RTC_CTL_RSYNF_Msk) != RTC_CTL_RSYNF_Msk) { }

    /* disable write access to backup domain registers */
    PMU->CTL &= ~PMU_CTL_BKPWEN_Msk;

    /* configure the EXTI channel, as RTC interrupts are routed through it.
     * Needs to be configured to trigger on rising edges. */
    EXTI->FTEN &= ~(EXTI_RTC_BIT);
    EXTI->RTEN |= EXTI_RTC_BIT;
    EXTI->INTEN |= EXTI_RTC_BIT;
    EXTI->PD |= EXTI_RTC_BIT;

    /* enable global RTC interrupt */
    clic_set_handler(RTC_ALARM_IRQn, isr_rtc_alarm);
    clic_enable_interrupt(RTC_ALARM_IRQn, CPU_DEFAULT_IRQ_PRIO);
}

static uint32_t _rtc_get_time(void)
{
    return (RTC->CNTH << 16) | RTC->CNTL;
}

static void _rtc_set_time(uint32_t counter_val)
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
    uint32_t cur_time = 0;
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
    rtc_tm_normalize(time);

    uint32_t timestamp = rtc_mktime(time);

    _rtc_set_time(timestamp);

    DEBUG("%s timestamp=%"PRIu32"\n", __func__, timestamp);

    return 0;
}

int rtc_get_time(struct tm *time)
{
    uint32_t timestamp = _rtc_get_time();
    rtc_localtime(timestamp, time);

    DEBUG("%s timestamp=%"PRIu32"\n", __func__, timestamp);
    return 0;
}

static void _rtc_enable_alarm(void)
{
    /* clear alarm flag */
    RTC->CTL &= ~RTC_CTL_ALRMIF_Msk;

    _rtc_enter_config_mode();
    RTC->INTEN |= (RTC_INTEN_ALRMIE_Msk);
    _rtc_exit_config_mode();
}

static void _rtc_disable_alarm(void)
{
    _rtc_enter_config_mode();
    RTC->INTEN &= ~RTC_INTEN_ALRMIE_Msk;
    _rtc_exit_config_mode();
}

/* RTC->ALRMH and RTC->ALRML are writable only. Therefore the current alarm
 * time must be stored separately in a variable for _rtc_get_alarm_time. */
static uint32_t _rtc_alarm_time = 0;

static uint32_t _rtc_get_alarm_time(void)
{
    return _rtc_alarm_time;
}

static void _rtc_set_alarm_time(uint32_t alarm_time)
{
    /* save the current alarm time */
    _rtc_alarm_time = alarm_time;
    /* set RTC alarm registers */
    _rtc_enter_config_mode();
    RTC->ALRML = (alarm_time & 0x0000ffff);
    RTC->ALRMH = (alarm_time & 0xffff0000) >> 16;
    _rtc_exit_config_mode();
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    rtc_tm_normalize(time);

    uint32_t timestamp = rtc_mktime(time);

    /* disable existing alarm (if enabled) */
    rtc_clear_alarm();

    /* save callback and argument */
    isr_ctx.cb = cb;
    isr_ctx.arg = arg;

    /* set wakeup time */
    _rtc_set_alarm_time(timestamp);

    /* enable Alarm */
    _rtc_enable_alarm();

    DEBUG("%s timestamp=%"PRIu32"\n", __func__, timestamp);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    uint32_t timestamp = _rtc_get_alarm_time();
    rtc_localtime(timestamp, time);

    DEBUG("%s timestamp=%"PRIu32"\n", __func__, timestamp);

    return 0;
}

void rtc_clear_alarm(void)
{
    _rtc_disable_alarm();

    isr_ctx.cb = NULL;
    isr_ctx.arg = NULL;
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

static void isr_rtc_alarm(unsigned irqn)
{
    (void)irqn;

    if (RTC->CTL & RTC_CTL_ALRMIF_Msk) {
        if (isr_ctx.cb != NULL) {
            isr_ctx.cb(isr_ctx.arg);
        }
        RTC->CTL &= ~RTC_CTL_ALRMIF_Msk;
    }
    EXTI->PD |= EXTI_RTC_BIT;
}
