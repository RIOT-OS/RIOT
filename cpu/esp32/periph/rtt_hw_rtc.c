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

/* RIOT headers have to be included before ESP-IDF headers! */
#include "cpu.h"
#include "esp/common_macros.h"
#include "esp_common.h"
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
#include "rom/ets_sys.h"
#include "soc/periph_defs.h"
#include "soc/rtc_cntl_struct.h"

#if __xtensa__
#include "soc/dport_reg.h"
#include "xtensa/xtensa_api.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define RTC_CLK_CAL_FRACT       19  /* fractional bits of calibration value */

typedef struct {
    uint32_t alarm_set;     /**< alarm set at interface */
    rtt_cb_t alarm_cb;      /**< alarm callback */
    void *alarm_arg;        /**< argument for alarm callback */
} _rtc_alarm_t;

static _rtc_alarm_t _rtc_alarm;

/* we can't include soc/rtc.h because of rtc_init declaration conflicts */
extern uint32_t esp_clk_slowclk_cal_get(void);

/* convert hardware counter to 32-bit RTT counter */
uint32_t _rtt_hw_to_rtt_counter(uint32_t hw_counter);

static void IRAM _rtc_isr(void *arg);

/* converts a 48-bit RTC counter value to microseconds */
uint64_t _rtc_counter_to_us(uint64_t raw)
{
    const uint32_t cal = esp_clk_slowclk_cal_get();
    return ((((raw >> 32) * cal) << (32 - RTC_CLK_CAL_FRACT)) + /* high part */
            (((raw & 0xffffffff) * cal) >> RTC_CLK_CAL_FRACT)); /* low part */
}

static void _rtc_init(void)
{
}

static void _rtc_poweron(void)
{
    /* route all interrupt sources to the same RTT level type interrupt */
    intr_matrix_set(PRO_CPU_NUM, ETS_RTC_CORE_INTR_SOURCE, CPU_INUM_RTT);

    /* set interrupt handler and enable the CPU interrupt */
    intr_cntrl_ll_set_int_handler(CPU_INUM_RTT, _rtc_isr, NULL);
    intr_cntrl_ll_enable_interrupts(BIT(CPU_INUM_RTT));
}

static void _rtc_poweroff(void)
{
    /* reset interrupt handler and disable the CPU interrupt */
    intr_cntrl_ll_disable_interrupts(BIT(CPU_INUM_RTT));
    intr_cntrl_ll_set_int_handler(CPU_INUM_RTT, NULL, NULL);
}

uint64_t _rtc_get_counter(void)
{
    /* trigger timer register update */
    RTCCNTL.time_update.update = 1;
#if defined(CPU_FAM_ESP32)
    /* wait until values in registers are valid */
    while (!RTCCNTL.time_update.valid) {
        ets_delay_us(1);
    }
    /* read the time from 48-bit counter and return */
    return (((uint64_t)RTCCNTL.time1.val) << 32) + RTCCNTL.time0;
#elif defined(CPU_FAM_ESP32C3) || defined(CPU_FAM_ESP32S2) || defined(CPU_FAM_ESP32S3)
    /* read the time from 48-bit counter and return */
    return (((uint64_t)RTCCNTL.time_high0.val) << 32) + RTCCNTL.time_low0;
#else
#error "MCU implementation is missing"
#endif
}

static void _rtc_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* compute the time difference for 32.768 kHz as 32-bit value */
    uint64_t rtc_counter = _rtc_get_counter();
    uint32_t rtt_diff = alarm - rtc_counter;

    /* use computed time difference directly to set the RTC counter alarm */
    uint64_t rtc_alarm = (rtc_counter + rtt_diff) & RTT_HW_COUNTER_MAX;

    DEBUG("%s alarm=%" PRIu32 " rtt_diff=%" PRIu32
          " rtc_alarm=%" PRIu64 " @rtc=%" PRIu64 "\n",
          __func__, alarm, rtt_diff, rtc_alarm, rtc_counter);

    /* save the alarm configuration for interrupt handling */
    _rtc_alarm.alarm_set = alarm;
    _rtc_alarm.alarm_cb = cb;
    _rtc_alarm.alarm_arg = arg;

    /* set the timer value */
    RTCCNTL.slp_timer0 = rtc_alarm & 0xffffffff;
    RTCCNTL.slp_timer1.slp_val_hi = rtc_alarm >> 32;

#if __xtensa__
    DEBUG("%s %08x%08x \n", __func__,
          RTCCNTL.slp_timer1.slp_val_hi, RTCCNTL.slp_timer0);
#else
    DEBUG("%s %08x%08x \n", __func__,
          (unsigned)RTCCNTL.slp_timer1.slp_val_hi, (unsigned)RTCCNTL.slp_timer0);
#endif

    /* enable RTC timer alarm */
    RTCCNTL.slp_timer1.main_timer_alarm_en = 1;

    /* clear and enable RTC timer interrupt */
    RTCCNTL.int_clr.rtc_main_timer = 1;
    RTCCNTL.int_ena.rtc_main_timer = 1;
}

static void _rtc_clear_alarm(void)
{
    /* disable alarms first */
    RTCCNTL.slp_timer1.main_timer_alarm_en = 0;

    /* clear the bit in interrupt enable and status register */
    RTCCNTL.int_clr.rtc_main_timer = 0;
    RTCCNTL.int_ena.rtc_main_timer = 0;

    /* reset the alarm configuration for interrupt handling */
    _rtc_alarm.alarm_set = 0;
    _rtc_alarm.alarm_cb = NULL;
    _rtc_alarm.alarm_arg = NULL;
}

static void _rtc_save_counter(void)
{
}

static void _rtc_restore_counter(bool in_init)
{
    (void)in_init;
}

static void IRAM _rtc_isr(void *arg)
{
    /* disable alarms first */
    RTCCNTL.slp_timer1.main_timer_alarm_en = 0;

    /* clear the bit in interrupt enable and status register */
    RTCCNTL.int_clr.rtc_main_timer = 0;
    RTCCNTL.int_ena.rtc_main_timer = 0;

    /* save the lower 32 bit of the current counter value */
    uint32_t counter = _rtc_get_counter();

    DEBUG("%s %" PRIu32 "\n", __func__, counter);

    if (_rtc_alarm.alarm_cb) {
        DEBUG("%s alarm %" PRIu32 "\n", __func__, counter);

        rtt_cb_t alarm_cb = _rtc_alarm.alarm_cb;
        void *alarm_arg = _rtc_alarm.alarm_arg;

        /* clear the alarm first */
        _rtc_alarm.alarm_cb = NULL;
        _rtc_alarm.alarm_arg = NULL;

        /* call the alarm handler afterwards if callback was defined */
        alarm_cb(alarm_arg);
    }
}

const rtt_hw_driver_t _rtt_hw_rtc_driver = {
        .init = _rtc_init,
        .get_counter = _rtc_get_counter,
        .set_alarm = _rtc_set_alarm,
        .clear_alarm = _rtc_clear_alarm,
        .poweron = _rtc_poweron,
        .poweroff = _rtc_poweroff,
        .save_counter = _rtc_save_counter,
        .restore_counter = _rtc_restore_counter,
};
