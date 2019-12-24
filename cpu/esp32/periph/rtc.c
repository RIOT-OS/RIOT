/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_rtc
 * @{
 *
 * @file
 * @brief       Low-level RTC driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

/*
 * If RTC_TIMER_USED is 0, the microsecond system timer is used to emulate an
 * RTC, otherwise the RTC timer is used. Advantage of using RTC over system
 * timer is that it also continues in deep sleep and after software reset.
 */
#define RTC_TIMER_USED 1

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "esp_common.h"

#include <string.h>

#include "cpu.h"
#include "esp_attr.h"
#include "log.h"
#include "irq_arch.h"
#include "periph/rtc.h"
#include "rom/ets_sys.h"
#include "soc/dport_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/timer_group_struct.h"
#include "syscalls.h"
#include "xtensa/xtensa_api.h"

/* TODO move to TIMER_SYSTEM definition in periph_cpu.h */
#define TIMER_SYSTEM_GROUP      TIMERG0
#define TIMER_SYSTEM_INT_MASK   BIT(0)
#define TIMER_SYSTEM_INT_SRC    ETS_TG0_T0_LEVEL_INTR_SOURCE
#define TIMER_SYSTEM_CLK_HZ     (1000000UL)

/* we can't include soc/rtc.h because of rtc_init declaration conflicts */
extern uint32_t rtc_clk_slow_freq_get_hz(void);

#if RTC_TIMER_USED

#define RTC_TIMER_CLK_HZ    rtc_clk_slow_freq_get_hz()

#else /* RTC_TIMER_USED */

#define RTC_TIMER_CLK_HZ    TIMER_SYSTEM_CLK_HZ

#endif /* RTC_TIMER_USED */

/* static variables */
static rtc_alarm_cb_t _rtc_alarm_cb = NULL;
static void*          _rtc_alarm_arg = NULL;
static time_t         _sys_alarm_time = 0;

#define RTC_BSS_ATTR __attribute__((section(".rtc.bss")))

/* save several time stamps */
static uint64_t RTC_BSS_ATTR _rtc_time_init_us;
static uint64_t RTC_BSS_ATTR _rtc_time_init;
static uint64_t RTC_BSS_ATTR _rtc_time_set_us;
static uint64_t RTC_BSS_ATTR _rtc_time_set;
static time_t   RTC_BSS_ATTR _sys_time_set;

/* forward declarations */
static time_t _sys_get_time (void);
static uint64_t _rtc_get_time_raw(void);
static void IRAM_ATTR _rtc_timer_handler(void* arg);

void rtc_init(void)
{
    if (_rtc_time_init == 0 && _rtc_time_init_us == 0) {
        /* only set it new, if it was not set before */
        _rtc_time_init = _rtc_get_time_raw();
        _rtc_time_init_us = _rtc_get_time_raw();

        DEBUG("%s saved rtc_init=%lld rtc_init_us=%lld\n",
              __func__, _rtc_time_init, _rtc_time_init_us);
    }

    #if RTC_TIMER_USED
    /* restore microsecond system timer from RTC timer */
    uint64_t _rtc_time_now = _rtc_get_time_raw();
    uint64_t _sys_time_now = (_rtc_time_now > UINT32_MAX) ?
                              _rtc_time_now / RTC_TIMER_CLK_HZ * TIMER_SYSTEM_CLK_HZ :
                              _rtc_time_now * TIMER_SYSTEM_CLK_HZ / RTC_TIMER_CLK_HZ;

    /* restore system timer */
    TIMER_SYSTEM.load_high = (uint32_t)(_sys_time_now >> 32);
    TIMER_SYSTEM.load_low  = (uint32_t)(_sys_time_now & 0xffffffff);
    TIMER_SYSTEM.reload = 0;

    DEBUG("%s restored rtc_init=%lld rtc_init_us=%lld\n",
          __func__, _rtc_time_init, _rtc_time_init_us);
    #endif
}

void rtc_poweron(void)
{
    /* RTC is always on, also in deep sleep mode */
    return;
}

void rtc_poweroff(void)
{
    /* RTC is always on, also in deep sleep mode */
    return;
}

int rtc_set_time(struct tm *ttime)
{
    _rtc_time_set_us = system_get_time_64();
    _rtc_time_set = _rtc_get_time_raw();
    _sys_time_set = mktime (ttime);

    DEBUG("%s sys_time_set=%ld sys_time_us=%lld rtc_time_set=%lld\n",
          __func__, _sys_time_set, system_get_time_64(), _rtc_time_set);

    return 0;
}

int rtc_get_time(struct tm *ttime)
{
    time_t _sys_time = _sys_get_time();

    DEBUG("%s sys_time=%ld rtc_time=%lld\n", __func__,
          _sys_time, _rtc_get_time_raw());

    struct tm* _time = localtime(&_sys_time);
    if (_time) {
        memcpy(ttime, _time, sizeof(struct tm));
        return 0;
    }
    else {
        return -1;
    }
}

int rtc_get_alarm(struct tm *time)
{
    struct tm* _time = localtime(&_sys_alarm_time);
    if (_time) {
        memcpy(time, _time, sizeof(struct tm));
        return 0;
    }
    else {
        return -1;
    }
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    _rtc_alarm_cb = cb;
    _rtc_alarm_arg = arg;

    /* determine the offset of alarm time to the set time in seconds */
    _sys_alarm_time = mktime(time);
    time_t _sys_time_offset = _sys_alarm_time - _sys_time_set;

    /*
     * RTC doesn't provide alarm functionality in active mode. At least
     * the RTC main timer seems not to work. Therefore we always use the
     * system timer for alarms. The Advantage of using RTC over system timer
     * is that it also continues in deep sleep and after software reset.
     */
    #if 0 /* TODO should be RTC_TIMER_USED */

    /* determine the offset of alarm time to current time in RTC time */
    uint64_t _rtc_time_alarm;
    _rtc_time_alarm = _rtc_time_set + _sys_time_offset * RTC_TIMER_CLK_HZ;

    DEBUG("%s sys=%d sys_alarm=%d rtc=%lld rtc_alarm=%lld\n", __func__,
          _sys_get_time(), _sys_time_offset, _rtc_get_time_raw(), _rtc_time_alarm);

    /* set the timer value */
    RTCCNTL.slp_timer0 = _rtc_time_alarm & 0xffffffff;
    RTCCNTL.slp_timer1.slp_val_hi = _rtc_time_alarm >> 32;

    DEBUG("%s %08x%08x \n", __func__, RTCCNTL.slp_timer1.slp_val_hi, RTCCNTL.slp_timer0);

    /* enable RTC timer alarm */
    RTCCNTL.slp_timer1.main_timer_alarm_en = 1;

    /* clear and enable RTC timer interrupt */
    RTCCNTL.int_clr.rtc_main_timer = 1;
    RTCCNTL.int_ena.rtc_main_timer = 1;

    /* route all RTC interrupt sources to the same level type interrupt */
    intr_matrix_set(PRO_CPU_NUM, DPORT_PRO_RTC_CORE_INTR_MAP_REG, CPU_INUM_RTC);

    /* set interrupt handler and enable the CPU interrupt */
    xt_set_interrupt_handler(CPU_INUM_RTC, _rtc_timer_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_RTC));

    #else

    /* determine the offset of alarm time to the RTC set time */
    uint64_t _rtc_time_alarm;

    #if RTC_TIMER_USED
    /* convert rtc_time_set to time in us taking care with big numbers */
    _rtc_time_alarm = _rtc_time_set_us + _sys_time_offset * TIMER_SYSTEM_CLK_HZ;

    DEBUG("%s sys=%ld sys_alarm=%ld rtc_set_us=%lld rtc_us=%lld rtc_alarm_us=%lld\n", __func__,
          _sys_get_time(), _sys_time_offset,
          _rtc_time_set_us, system_get_time_64(), _rtc_time_alarm);
    #else
    _rtc_time_alarm = _rtc_time_set + _sys_time_offset * TIMER_SYSTEM_CLK_HZ;

    DEBUG("%s sys=%ld sys_alarm=%ld rtc=%lld rtc_alarm=%lld\n", __func__,
          _sys_get_time(), _sys_time_offset, _rtc_get_time_raw(), _rtc_time_alarm);

    #endif

    /* set the timer value */
    TIMER_SYSTEM.alarm_high = (uint32_t)(_rtc_time_alarm >> 32);
    TIMER_SYSTEM.alarm_low  = (uint32_t)(_rtc_time_alarm & 0xffffffff);

    /* clear the bit in status and set the bit in interrupt enable */
    TIMER_SYSTEM_GROUP.int_clr_timers.val |= TIMER_SYSTEM_INT_MASK;
    TIMER_SYSTEM_GROUP.int_ena.val |= TIMER_SYSTEM_INT_MASK;

    /* route all timer interrupt sources to the same level type interrupt */
    intr_matrix_set(PRO_CPU_NUM, TIMER_SYSTEM_INT_SRC, CPU_INUM_RTC);

    /* set interrupt handler and enable the CPU interrupt */
    xt_set_interrupt_handler(CPU_INUM_RTC, _rtc_timer_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_RTC));

    /* enable the timer alarm */
    TIMER_SYSTEM.config.level_int_en = 1;
    TIMER_SYSTEM.config.alarm_en = 1;

    #endif

    return 0;
}

void rtc_clear_alarm(void)
{
    _rtc_alarm_cb = NULL;
    _rtc_alarm_arg = NULL;

    #if 0 /* TODO should be RTC_TIMER_USED, see rtc_set_alarm */

    /* disable RTC timer alarm and disable the RTC timer interrupt */
    RTCCNTL.slp_timer1.main_timer_alarm_en = 0;
    RTCCNTL.int_ena.rtc_main_timer = 0;

    /* route all RTC interrupt sources to the same level type interrupt */
    intr_matrix_set(PRO_CPU_NUM, DPORT_PRO_RTC_CORE_INTR_MAP_REG, CPU_INUM_RTC);

    /* disable the the CPU interrupt */

    #else

    /* reset the bit in interrupt enable */
    TIMER_SYSTEM_GROUP.int_ena.val |= TIMER_SYSTEM_INT_MASK;

    /* disable the CPU interrupt */
    xt_ints_on(BIT(CPU_INUM_RTC));

    /* disable the timer alarm */
    TIMER_SYSTEM.config.level_int_en = 0;
    TIMER_SYSTEM.config.alarm_en = 0;

    xt_ints_on(BIT(CPU_INUM_RTC));

    #endif
}

static time_t _sys_get_time (void)
{
    return _sys_time_set + (_rtc_get_time_raw() - _rtc_time_set) / RTC_TIMER_CLK_HZ;
}

static uint64_t _rtc_get_time_raw(void)
{
    #if RTC_TIMER_USED

    /* trigger timer register update */
    RTCCNTL.time_update.update = 1;
    /* wait until values in registers are valid */
    while (!RTCCNTL.time_update.valid) { }
    /* read the time and return */
    uint64_t rtc_time;
    rtc_time  = RTCCNTL.time0;
    rtc_time += ((uint64_t)RTCCNTL.time1.val) << 32;
    return rtc_time;

    #else

    return system_get_time_64();

    #endif
}

static void IRAM_ATTR _rtc_timer_handler(void* arg)
{
    irq_isr_enter();

    #if 0 /* TODO should be RTC_TIMER_USED */

     /* check for RTC timer interrupt */
    if (RTCCNTL.int_st.rtc_main_timer) {
        /* clear the interrupt */
        RTCCNTL.int_clr.rtc_main_timer = 1;
        /* call back registered function */
        if (_rtc_alarm_cb) {
            _rtc_alarm_cb(_rtc_alarm_arg);
        }
    }
    /* clear all interrupts */
    RTCCNTL.int_clr.val = 0x1ff;

    #else

    /* check for RTC timer interrupt */
    if (TIMER_SYSTEM_GROUP.int_st_timers.val & TIMER_SYSTEM_INT_MASK) {

        DEBUG("%s\n", __func__);

        /* disable alarms */
        TIMER_SYSTEM.config.level_int_en = 0;
        TIMER_SYSTEM.config.alarm_en = 0;

        /* clear the bit in interrupt enable and status register */
        TIMER_SYSTEM_GROUP.int_ena.val &= ~TIMER_SYSTEM_INT_MASK;
        TIMER_SYSTEM_GROUP.int_clr_timers.val |= TIMER_SYSTEM_INT_MASK;

        /* call back registered function */
        if (_rtc_alarm_cb) {
            _rtc_alarm_cb(_rtc_alarm_arg);
        }
    }

   #endif

    irq_isr_exit();
}
