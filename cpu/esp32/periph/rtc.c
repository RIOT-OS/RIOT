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
 * The RTC low-level driver uses either the RTC hardware timer directly or the
 * PLL-controlled 64-bit microsecond system timer to emulate an RTC timer. The
 * RTC hardware timer can be clocked with either an external 32.768 kHz crystal
 * or an internal adjustable 150 kHz RC oscillator. Which RTC timer is used,
 * is controlled by modules as following:
 *
 * default
 *   Since the RTC hardware timer with the 150 kHz RC oscillator is less
 *   accurate than the emulated RTC timer, the emulated RTC timer is used by
 *   default. In this case, the RTC hardware timer is only used with the
 *   internal 150 kHz oscillator in deep sleep mode and during a reset.
 *
 * module `esp_rtc_timer`
 *   To use the RTC hardware timer with the 150 kHz Oscilator, the module
 *   `esp_rtc_timer` has to be enabled.
 *
 * module `esp_rtc_timer_32k`
 *   To use the RTC hardware timer with the external 32.768 kHz crystal, the
 *   module 'esp_rtc_timer_32k` has to be enabled. If the module
 *   `esp_rtc_timer_32k` is used, but no external 32.768 kHz crystal is
 *   available, the RTC low-level driver uses the RTC hardware driver,
 *   but with the internal 150 kHz RC oscillator.
 */

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
#include "timex.h"
#include "xtensa/xtensa_api.h"

/* TODO move to TIMER_SYSTEM definition in periph_cpu.h */
#define TIMER_SYSTEM_GROUP      TIMERG0
#define TIMER_SYSTEM_INT_MASK   BIT(0)
#define TIMER_SYSTEM_INT_SRC    ETS_TG0_T0_LEVEL_INTR_SOURCE

/* RTC timer interrupt source */
#define TIMER_RTC_INT_SRC       ETS_RTC_CORE_INTR_SOURCE

#define RTC_CLK_CAL_FRACT       19  /* fractional bits of calibration value */

/* we can't include soc/rtc.h because of rtc_init declaration conflicts */
extern uint32_t rtc_clk_slow_freq_get_hz(void);
extern uint32_t esp_clk_slowclk_cal_get(void);

/* static variables */
static rtc_alarm_cb_t _rtc_alarm_cb = NULL;
static void*          _rtc_alarm_arg = NULL;
static time_t         _sys_alarm_time = 0;

#define RTC_BSS_ATTR __attribute__((section(".rtc.bss")))

/* save several time stamps in RTC memory */
static uint64_t RTC_BSS_ATTR _rtc_time_init_us; /* RTC time on init in us */
static uint64_t RTC_BSS_ATTR _rtc_time_init;    /* RTC time on init in cycles */
static uint64_t RTC_BSS_ATTR _rtc_time_set_us;  /* RTC time on set in us */
static uint64_t RTC_BSS_ATTR _rtc_time_set;     /* RTC time on set in cycles */
static uint64_t RTC_BSS_ATTR _sys_time_set_us;  /* system time on set in us */
static time_t   RTC_BSS_ATTR _sys_time_set;     /* system time on set in sec */
static uint64_t RTC_BSS_ATTR _sys_time_off_us;  /* system time offset in us */

/* forward declarations */
static time_t _sys_get_time (void);             /* system time in seconds */
static uint64_t _rtc_get_time_raw(void);        /* RTC time in cycles */
static uint64_t _rtc_time_to_us(uint64_t raw);  /* convert RTC cycles to us */
static void IRAM_ATTR _rtc_timer_handler(void* arg);

void rtc_init(void)
{
    uint64_t _rtc_time_us = _rtc_time_to_us(_rtc_get_time_raw());

    if (_rtc_time_init == 0 && _rtc_time_init_us == 0) {
        /* only set it new, if it was not set before */
        _rtc_time_init = _rtc_get_time_raw();
        _rtc_time_init_us = _rtc_time_us;
        _sys_time_off_us = 0;

        DEBUG("%s saved rtc_init=%lld rtc_init_us=%lld\n",
              __func__, _rtc_time_init, _rtc_time_init_us);

    }
    else {
        _sys_time_off_us = _rtc_time_us - _rtc_time_set_us;
    }
    _sys_time_set_us = 0;
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
    _rtc_time_set = _rtc_get_time_raw();
    _rtc_time_set_us = _rtc_time_to_us(_rtc_time_set);

    _sys_time_set = rtc_mktime(ttime);
    _sys_time_set_us = system_get_time_64();
    _sys_time_off_us = 0;

    DEBUG("%s sys_time=%ld rtc_time=%lld rtc_time_us=%lld\n",
          __func__, _sys_time_set, _rtc_time_set, _rtc_time_set_us);

    return 0;
}

int rtc_get_time(struct tm *ttime)
{
    rtc_localtime(_sys_get_time(), ttime);

    DEBUG("%s sys_time=%ld rtc_time=%lld rtc_time_us=%lld\n", __func__,
          _sys_get_time(), _rtc_get_time_raw(), _rtc_time_to_us(_rtc_get_time_raw()));

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    rtc_localtime(_sys_alarm_time, time);
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    _rtc_alarm_cb = cb;
    _rtc_alarm_arg = arg;

    /* determine the offset of alarm time to the set time in seconds */
    _sys_alarm_time = rtc_mktime(time);
    time_t _sys_time_offset = _sys_alarm_time - _sys_time_set;

#if MODULE_ESP_RTC_TIMER

    /* determine the offset of alarm time to current time in RTC time */
    uint64_t _rtc_time_alarm;
    _rtc_time_alarm = _rtc_time_set + _sys_time_offset * rtc_clk_slow_freq_get_hz();
    DEBUG("%s sys=%ld sys_alarm=%ld rtc=%lld rtc_alarm=%lld\n", __func__,
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
    intr_matrix_set(PRO_CPU_NUM, ETS_RTC_CORE_INTR_SOURCE, CPU_INUM_RTC);

    /* set interrupt handler and enable the CPU interrupt */
    xt_set_interrupt_handler(CPU_INUM_RTC, _rtc_timer_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_RTC));

#else

    /* determine the offset of alarm time to the RTC set time in us */
    uint64_t _sys_alarm_us;
    _sys_alarm_us = _sys_time_set_us - _sys_time_off_us + _sys_time_offset * US_PER_SEC;

    DEBUG("%s sys_time=%ld sys_time_offset=%ld "
          "sys_time_us=%lld sys_time_alarm_us=%lld\n", __func__,
          _sys_get_time(), _sys_time_offset, system_get_time_64(), _sys_alarm_us);

    /* set the timer value */
    TIMER_SYSTEM.alarm_high = (uint32_t)(_sys_alarm_us >> 32);
    TIMER_SYSTEM.alarm_low  = (uint32_t)(_sys_alarm_us & 0xffffffff);

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

#if MODULE_ESP_RTC_TIMER

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
#if MODULE_ESP_RTC_TIMER
    return _sys_time_set +
           (_rtc_time_to_us(_rtc_get_time_raw() - _rtc_time_set) / US_PER_SEC);
#else
    return _sys_time_set +
           ((_sys_time_off_us + system_get_time_64() - _sys_time_set_us) / US_PER_SEC);
#endif
}

static uint64_t _rtc_get_time_raw(void)
{
    /* trigger timer register update */
    RTCCNTL.time_update.update = 1;
    /* wait until values in registers are valid */
    while (!RTCCNTL.time_update.valid) { }
    /* read the time and return */
    uint64_t rtc_time;
    rtc_time  = RTCCNTL.time0;
    rtc_time += ((uint64_t)RTCCNTL.time1.val) << 32;
    return rtc_time;
}

static uint64_t _rtc_time_to_us(uint64_t raw)
{
    const uint32_t cal = esp_clk_slowclk_cal_get();
    return ((((raw >> 32) * cal) << (32 - RTC_CLK_CAL_FRACT)) + /* high part */
            (((raw & 0xffffffff) * cal) >> RTC_CLK_CAL_FRACT)); /* low part */
}

static void IRAM_ATTR _rtc_timer_handler(void* arg)
{
    irq_isr_enter();

#if MODULE_ESP_RTC_TIMER

    /* check for RTC timer interrupt */
    if (RTCCNTL.int_st.rtc_main_timer) {
        /* disable RTC timer alarm */
        RTCCNTL.slp_timer1.main_timer_alarm_en = 0;
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
