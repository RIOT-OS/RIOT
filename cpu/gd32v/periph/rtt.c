/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       Low-level RTT driver implementation for STM32F1
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph/rtt.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define EXTI_RTC_BIT       (1UL << 17)

#define RTT_PRESCALER   ((RTT_CLOCK_FREQUENCY / RTT_FREQUENCY) - 1 )

static inline void _rtt_enter_config_mode(void);
static inline void _rtt_leave_config_mode(void);
static void isr_rtc(unsigned irqn);

static rtt_cb_t alarm_cb;
static void *alarm_arg;

static rtt_cb_t overflow_cb;
static void *overflow_arg;

void rtt_init(void)
{
    /* enable backup domain clock */
    periph_clk_en(APB1, RCU_APB1EN_BKPIEN_Msk);

    /* enable write access to backup domain registers */
    PMU->CTL |= PMU_CTL_BKPWEN_Msk;

    /* reset the entire backup domain */
    RCU->BDCTL |= RCU_BDCTL_BKPRST_Msk;
    RCU->BDCTL &= ~RCU_BDCTL_BKPRST_Msk;

    rtt_poweron();

    /* clear RSYN flag */
    RTT_DEV->CTL &= ~(RTC_CTL_RSYNF_Msk);
    while ((RTC->CTL & RTC_CTL_RSYNF_Msk) != RTC_CTL_RSYNF_Msk) { }

    _rtt_enter_config_mode();

    /* reset RTC counter */
    RTT_DEV->CNTH = 0x0000;
    RTT_DEV->CNTL = 0x0000;

    /* set prescaler */
    RTT_DEV->PSCH = (RTT_PRESCALER >> 16) & 0x000f;
    RTT_DEV->PSCL = RTT_PRESCALER & 0xffff;

    _rtt_leave_config_mode();

    /* configure the EXTI channel, as RTC interrupts are routed through it.
     * Needs to be configured to trigger on rising edges. */
    EXTI->FTEN &= ~(EXTI_RTC_BIT);
    EXTI->RTEN |= EXTI_RTC_BIT;
    EXTI->INTEN |= EXTI_RTC_BIT;
    EXTI->PD |= EXTI_RTC_BIT;

    /* enable global RTC interrupt */
    clic_set_handler(RTT_IRQ, isr_rtc);
    clic_enable_interrupt(RTT_IRQ, RTT_IRQ_PRIORITY);
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    overflow_cb = cb;
    overflow_arg = arg;

    _rtt_enter_config_mode();

    /* Enable overflow interrupt */
    RTT_DEV->INTEN |= RTC_INTEN_OVIE_Msk;

    _rtt_leave_config_mode();
}

void rtt_clear_overflow_cb(void)
{
    _rtt_enter_config_mode();

    /* Clear overflow interrupt */
    RTT_DEV->INTEN &= ~(RTC_INTEN_OVIE_Msk);

    _rtt_leave_config_mode();

    overflow_cb = NULL;
    overflow_arg = NULL;
}

uint32_t rtt_get_counter(void)
{
    /* wait for synchronization */
    while (!(RTT_DEV->CTL & RTC_CTL_RSYNF_Msk)) { }

    return ((uint32_t)RTT_DEV->CNTH << 16 ) | (uint32_t)(RTT_DEV->CNTL);
}

void rtt_set_counter(uint32_t counter)
{
    _rtt_enter_config_mode();

    /* Set RTC counter MSB word */
    RTT_DEV->CNTH = counter >> 16;
    /* Set RTC counter LSB word */
    RTT_DEV->CNTL = counter & 0xffff;

    _rtt_leave_config_mode();
}

/* RTC->ALRMH and RTC->ALRML are writable only. Therefore the current alarm
 * time must be stored separately in a variable for rtt_get_alarm. */
static uint32_t _rtt_alarm = 0;

uint32_t rtt_get_alarm(void)
{
    /* wait for synchronization */
    while (!(RTT_DEV->CTL & RTC_CTL_RSYNF_Msk)) { }

    return _rtt_alarm;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* save the current alarm time */
    _rtt_alarm = alarm;

    _rtt_enter_config_mode();

    /* Disable alarm*/
    RTT_DEV->INTEN &= ~RTC_INTEN_ALRMIE_Msk;
    /* Save new cb and argument */
    alarm_cb = cb;
    alarm_arg = arg;

    /* Set the alarm MSB word */
    RTT_DEV->ALRMH = alarm >> 16;
    /* Set the alarm LSB word */
    RTT_DEV->ALRML = (alarm & 0xffff);

    /* Enable alarm interrupt */
    RTT_DEV->INTEN |= RTC_INTEN_ALRMIE_Msk;

    _rtt_leave_config_mode();
}

void rtt_clear_alarm(void)
{
    _rtt_enter_config_mode();

    /* Disable alarm interrupt */
    RTT_DEV->INTEN &= ~RTC_INTEN_ALRMIE_Msk;
    /* Set the ALARM MSB word to reset value */
    RTT_DEV->ALRMH = 0xffff;
    /* Set the ALARM LSB word to reset value */
    RTT_DEV->ALRML = 0xffff;

    _rtt_leave_config_mode();
}

#define RCU_BDCTL_RTCSRC_CK_LXTAL   1
#define RCU_BDCTL_RTCSRC_CK_IRC40K  2

void rtt_poweron(void)
{
    /* enable backup domain clock */
    periph_clk_en(APB1, RCU_APB1EN_BKPIEN_Msk);

    /* enable write access to backup domain registers */
    PMU->CTL |= PMU_CTL_BKPWEN_Msk;

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

    /* disable write access to backup domain registers */
    PMU->CTL &= ~PMU_CTL_BKPWEN_Msk;
}

void rtt_poweroff(void)
{
    /* enable write access to backup domain registers */
    PMU->CTL |= PMU_CTL_BKPWEN_Msk;

    /* enable RTC clock */
    RCU->BDCTL &= ~RCU_BDCTL_RTCEN_Msk;

    /* disable write access to backup domain registers */
    PMU->CTL &= ~PMU_CTL_BKPWEN_Msk;

    /* disable backup domain clock */
    periph_clk_dis(APB1, RCU_APB1EN_BKPIEN_Msk);
}

static inline void _rtt_enter_config_mode(void)
{
    /* enable write access to backup domain registers */
    PMU->CTL |= PMU_CTL_BKPWEN_Msk;

    /* wait until the LWOFF bit is 1 (Last write operation finished). */
    while ((RTC->CTL & RTC_CTL_LWOFF_Msk) == 0) { }

    /* enter configuration mode. */
    RTC->CTL |= RTC_CTL_CMF_Msk;
}

static inline void _rtt_leave_config_mode(void)
{
    /* exit configuration mode. */
    RTC->CTL &= ~RTC_CTL_CMF_Msk;

    /* wait until the LWOFF bit is 1 (Last write operation finished). */
    while ((RTC->CTL & RTC_CTL_LWOFF_Msk) == 0) { }

    /* disable write access to backup domain registers */
    PMU->CTL &= ~PMU_CTL_BKPWEN_Msk;
}

static void isr_rtc(unsigned irqn)
{
    (void)irqn;

    if (RTT_DEV->CTL & RTC_CTL_ALRMIF_Msk) {
        RTT_DEV->CTL &= ~(RTC_CTL_ALRMIF_Msk);
        if (alarm_cb) {
            alarm_cb(alarm_arg);
        }
    }
    if (RTT_DEV->CTL & RTC_CTL_OVIF_Msk) {
        RTT_DEV->CTL &= ~(RTC_CTL_OVIF_Msk);
        if (overflow_cb) {
            overflow_cb(overflow_arg);
        }
    }

    EXTI->PD |= EXTI_RTC_BIT;
}
