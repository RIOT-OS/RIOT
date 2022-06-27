/*
 * Copyright (C) 2022 Gunar Schorcht
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

/* RIOT headers have to be included before ESP-IDF headers! */
#include "cpu.h"
#include "esp/common_macros.h"
#include "irq_arch.h"
#include "log.h"
#include "periph/rtt.h"
#include "rtt_arch.h"
#include "syscalls.h"
#include "timex.h"

/* ESP-IDF headers */
#include "esp_attr.h"
#include "esp_sleep.h"
#include "hal/interrupt_controller_types.h"
#include "hal/interrupt_controller_ll.h"
#include "hal/timer_hal.h"
#include "rom/ets_sys.h"
#include "soc/periph_defs.h"
#include "soc/timer_group_struct.h"

#if __xtensa__
#include "xtensa/xtensa_api.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define TIMER_SYSTEM_INT_MASK   BIT(TIMER_SYSTEM_INDEX)

#define SYS_US_TO_TICKS(us)     ((((uint64_t)us) << 15) / US_PER_SEC)
#define SYS_TICKS_TO_US(cnt)    (((uint64_t)cnt * US_PER_SEC) >> 15)

/* system timer is defined and initialized in syscalls.c */
extern timer_hal_context_t sys_timer;

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
    intr_matrix_set(PRO_CPU_NUM, TIMER_SYSTEM_INT_SRC, CPU_INUM_RTT);

    /* set interrupt handler and enable the CPU interrupt */
    intr_cntrl_ll_set_int_handler(CPU_INUM_RTT, _sys_isr, NULL);
    intr_cntrl_ll_enable_interrupts(BIT(CPU_INUM_RTT));
}

static void _sys_poweroff(void)
{
    /* reset interrupt handler and disable the CPU interrupt */
    intr_cntrl_ll_disable_interrupts(BIT(CPU_INUM_RTT));
    intr_cntrl_ll_set_int_handler(CPU_INUM_RTT, NULL, NULL);
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

    DEBUG("%s alarm=%" PRIu32 " rtt_diff=%" PRIu32 " "
          "sys_diff=%" PRIu64 " sys_alarm=%" PRIu64 " @sys_time=%" PRIu64 "\n", __func__,
          alarm, rtt_diff, _sys_diff, _sys_alarm_time, _sys_time);

    /* save the alarm configuration for interrupt handling */
    _sys_alarm.alarm_set = alarm;
    _sys_alarm.alarm_cb = cb;
    _sys_alarm.alarm_arg = arg;

    /* set the timer value */
    timer_hal_set_alarm_value(&sys_timer, _sys_alarm_time);

    /* clear the bit in status and set the bit in interrupt enable */
    timer_hal_clear_intr_status(&sys_timer);
    timer_hal_intr_enable(&sys_timer);

    /* enable the timer alarm */
    timer_hal_set_alarm_enable(&sys_timer, true);
}

static void _sys_clear_alarm(void)
{
    /* disable alarms first */
    timer_hal_intr_disable(&sys_timer);
    timer_hal_set_alarm_enable(&sys_timer, false);

    /* clear the bit in interrupt status register */
    timer_hal_clear_intr_status(&sys_timer);

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

    DEBUG("%s rtc_time_saved=%" PRIu64 " sys_time_saved=%" PRIu64 "\n", __func__,
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

    DEBUG("%s rtc_time_saved=%" PRIu64 " rtc_time_diff=%" PRIu64 " "
          "sys_time_saved=%" PRIu64 " sys_time_offset=%" PRIu64 "\n", __func__,
          _rtc_counter_saved, _rtc_time_diff,
          _sys_counter_saved, _sys_counter_offset);
}

static void IRAM _sys_isr(void *arg)
{
    uint32_t int_status;

    timer_hal_get_intr_status(&sys_timer, &int_status);
    if (!(int_status & TIMER_SYSTEM_INT_MASK)) {
        /* return in case of another timer interrupt */
        return;
    }

    /* disable alarms first */
    timer_hal_intr_disable(&sys_timer);
    timer_hal_set_alarm_enable(&sys_timer, false);

    /* clear the bit in interrupt status register */
    timer_hal_clear_intr_status(&sys_timer);

    /* save the lower 32 bit of the current counter value */
    uint32_t counter = _sys_get_counter();

    DEBUG("%s %" PRIu32 "\n", __func__, counter);

    if (_sys_alarm.alarm_cb) {
        DEBUG("%s alarm %" PRIu32 "\n", __func__, counter);

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
