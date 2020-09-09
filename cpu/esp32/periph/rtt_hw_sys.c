/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       Low-level RTT driver implementation for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "cpu.h"
#include "esp_attr.h"
#include "esp/common_macros.h"
#include "esp_sleep.h"
#include "irq_arch.h"
#include "log.h"
#include "periph/rtt.h"
#include "rtt_arch.h"
#include "soc/timer_group_struct.h"
#include "syscalls.h"
#include "timex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define TIMER_SYSTEM_GROUP      TIMERG0
#define TIMER_SYSTEM_INT_MASK   BIT(0)
#define TIMER_SYSTEM_INT_SRC    ETS_TG0_T0_LEVEL_INTR_SOURCE

#define SYS_US_TO_TICKS(us)     ((((uint64_t)us) << 15) / US_PER_SEC)
#define SYS_TICKS_TO_US(cnt)    (((uint64_t)cnt * US_PER_SEC) >> 15)

typedef struct {
    uint32_t alarm_set;     /**< alarm set at interface */
    rtt_cb_t alarm_cb;      /**< alarm callback */
    void *alarm_arg;        /**< argument for alarm callback */
} _sys_alarm_t;

static _sys_alarm_t _sys_alarm;

/* variables used to save counters during sleep or reboot */
static uint64_t RTC_BSS_ATTR _rtc_counter_saved;
static uint64_t RTC_BSS_ATTR _sys_counter_saved;

/* the offset of the system time to the RTC time in microseconds */
static uint64_t _sys_counter_offset;

/* forward declarations of functions required from RTC counter */
extern uint64_t _rtc_get_counter(void);
extern uint64_t _rtc_counter_to_us(uint64_t raw);

static void IRAM _sys_isr(void *arg);

static void _sys_init(void)
{
}

static void _sys_poweron(void)
{
    /* route all interrupt sources to the same RTT level type interrupt */
    intr_matrix_set(PRO_CPU_NUM, TIMER_SYSTEM_INT_SRC, CPU_INUM_RTC);

    /* set interrupt handler and enable the CPU interrupt */
    xt_set_interrupt_handler(CPU_INUM_RTC, _sys_isr, NULL);
    xt_ints_on(BIT(CPU_INUM_RTC));
}

static void _sys_poweroff(void)
{
    /* reset interrupt handler and disable the CPU interrupt */
    xt_ints_off(BIT(CPU_INUM_RTC));
    xt_set_interrupt_handler(CPU_INUM_RTC, NULL, NULL);
}

static uint64_t _sys_get_counter(void)
{
    /* convert the 64-bit microsecond system time to 48-bit 32.768 kHz time */
    return SYS_US_TO_TICKS(system_get_time_64() + _sys_counter_offset) & RTT_HW_COUNTER_MAX;
}

static void _sys_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* compute the time difference for 32.768 kHz as 32-bit value */
    uint32_t rtt_diff = alarm - _sys_get_counter();

    /*
     * convert the computed time difference for 32.768 kHz to a 64-bit
     * microsecond value and determine the alarm time for the 64-bit
     * microsecond system timer
     */
    uint64_t _sys_diff = SYS_TICKS_TO_US(rtt_diff);
    uint64_t _sys_time = system_get_time_64();
    uint64_t _sys_alarm_time = _sys_time + _sys_diff;

    DEBUG("%s alarm=%u rtt_diff=%u "
          "sys_diff=%llu sys_alarm=%llu @sys_time=%llu\n", __func__,
          alarm, rtt_diff, _sys_diff, _sys_alarm_time, _sys_time);

    /* save the alarm configuration for interrupt handling */
    _sys_alarm.alarm_set = alarm;
    _sys_alarm.alarm_cb = cb;
    _sys_alarm.alarm_arg = arg;

    /* set the timer value */
    TIMER_SYSTEM.alarm_high = (uint32_t)(_sys_alarm_time >> 32);
    TIMER_SYSTEM.alarm_low  = (uint32_t)(_sys_alarm_time & 0xffffffff);

    /* clear the bit in status and set the bit in interrupt enable */
    TIMER_SYSTEM_GROUP.int_clr_timers.val |= TIMER_SYSTEM_INT_MASK;
    TIMER_SYSTEM_GROUP.int_ena.val |= TIMER_SYSTEM_INT_MASK;

    /* enable the timer alarm */
    TIMER_SYSTEM.config.level_int_en = 1;
    TIMER_SYSTEM.config.alarm_en = 1;
}

static void _sys_clear_alarm(void)
{
    /* disable alarms first */
    TIMER_SYSTEM.config.level_int_en = 0;
    TIMER_SYSTEM.config.alarm_en = 0;

    /* clear the bit in interrupt enable and status register */
    TIMER_SYSTEM_GROUP.int_ena.val &= ~TIMER_SYSTEM_INT_MASK;
    TIMER_SYSTEM_GROUP.int_clr_timers.val |= TIMER_SYSTEM_INT_MASK;

    /* reset the alarm configuration for interrupt handling */
    _sys_alarm.alarm_set = 0;
    _sys_alarm.alarm_cb = NULL;
    _sys_alarm.alarm_arg = NULL;
}

static void _sys_save_counter(void)
{
    critical_enter();

    /* save counters for synchronization after wakeup or reboot */
    _rtc_counter_saved = _rtc_get_counter();
    _sys_counter_saved = system_get_time_64() + _sys_counter_offset;

    critical_exit();

    DEBUG("%s rtc_time_saved=%llu sys_time_saved=%llu\n", __func__,
          _rtc_counter_saved, _sys_counter_saved);
}

static void _sys_restore_counter(bool in_init)
{
    critical_enter();

    /* synchronize RTC counter and the 64-bit microsecond system timer */
    uint64_t _rtc_time_diff = _rtc_get_counter() - _rtc_counter_saved;
    _sys_counter_offset += _rtc_counter_to_us(_rtc_time_diff & RTT_HW_COUNTER_MAX);
    _sys_counter_offset += (in_init) ? _sys_counter_saved : 0;

    critical_exit();

    DEBUG("%s rtc_time_saved=%llu rtc_time_diff=%llu "
          "sys_time_saved=%llu sys_time_offset=%llu\n", __func__,
          _rtc_counter_saved, _rtc_time_diff,
          _sys_counter_saved, _sys_counter_offset);
}

static void IRAM _sys_isr(void *arg)
{
    if (!(TIMER_SYSTEM_GROUP.int_st_timers.val & TIMER_SYSTEM_INT_MASK)) {
        /* return in case of another timer interrupt */
        return;
    }

    /* disable alarms first */
    TIMER_SYSTEM.config.level_int_en = 0;
    TIMER_SYSTEM.config.alarm_en = 0;

    /* clear the bit in interrupt enable and status register */
    TIMER_SYSTEM_GROUP.int_ena.val &= ~TIMER_SYSTEM_INT_MASK;
    TIMER_SYSTEM_GROUP.int_clr_timers.val |= TIMER_SYSTEM_INT_MASK;

    /* save the lower 32 bit of the current counter value */
    uint32_t counter = _sys_get_counter();

    DEBUG("%s %u\n", __func__, counter);

    if (_sys_alarm.alarm_cb) {
        DEBUG("%s alarm %u\n", __func__, counter);

        rtt_cb_t alarm_cb = _sys_alarm.alarm_cb;
        void *alarm_arg = _sys_alarm.alarm_arg;

        /* clear the alarm first */
        _sys_alarm.alarm_cb = NULL;
        _sys_alarm.alarm_arg = NULL;

        /* call the alarm handler afterwards if callback was defined */
        alarm_cb(alarm_arg);
    }
}

const rtt_hw_driver_t _rtt_hw_sys_driver = {
        .init = _sys_init,
        .get_counter = _sys_get_counter,
        .set_alarm = _sys_set_alarm,
        .clear_alarm = _sys_clear_alarm,
        .poweron = _sys_poweron,
        .poweroff = _sys_poweroff,
        .save_counter = _sys_save_counter,
        .restore_counter = _sys_restore_counter,
};
