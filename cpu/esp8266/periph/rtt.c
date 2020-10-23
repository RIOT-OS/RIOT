/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       Low-level RTT driver implementation for ESP8266
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include "cpu.h"
#include "log.h"
#include "periph/rtt.h"

#include "esp_common.h"
#include "esp/common_macros.h"
#include "esp/dport_regs.h"
#include "esp/rtc_regs.h"
#include "sdk/sdk.h"

#define FRC2_CLK_DIV_256    2   /* divider for the 80 MHz AHB clock */

/**
 * FRC2 is a 32-bit countup timer, triggers interrupt when reaches alarm value.
 */
typedef struct {
    uint32_t load;
    uint32_t count;
    union {
        struct {
            uint32_t intr_hold : 1;
            uint32_t reserved1 : 1;
            uint32_t clk_div   : 2;
            uint32_t reserved2 : 2;
            uint32_t reload    : 1;
            uint32_t enable    : 1;
            uint32_t intr_sta  : 1;
            uint32_t reserved3 : 23;
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t clear    : 1;
            uint32_t reserved1: 31;
        };
        uint32_t val;
    } intr;
    uint32_t alarm;
} frc2_struct_t;

/*
 * linker script esp8266.peripherals.ld will make sure this points to the
 * hardware register address
 */
extern volatile frc2_struct_t frc2;

typedef struct {
    uint32_t alarm;         /**< alarm */
    rtt_cb_t alarm_cb;      /**< alarm callback */
    rtt_cb_t overflow_cb;   /**< overflow callback */
    void *alarm_arg;        /**< argument for alarm callback */
    void *overflow_arg;     /**< argument for overflow callback */
} rtt_config_t;

static rtt_config_t rtt_config;

static uint32_t RTC_BSS_ATTR _rtt_counter_saved;
static uint32_t RTC_BSS_ATTR _rtc_counter_saved;

extern uint32_t pm_rtc_clock_cali_proc(void);
extern uint32_t pm_rtc2usec(uint32_t rtc_cycles, uint32_t period);

void rtt_restore_counter(void);

void IRAM rtt_cb(void *arg)
{
    /* triggered alarm */
    uint32_t alarm = frc2.alarm;

    if (alarm == rtt_config.alarm) {
        rtt_cb_t alarm_cb = rtt_config.alarm_cb;
        void * alarm_arg = rtt_config.alarm_arg;
        /* clear the alarm first (includes setting next alarm to overflow) */
        rtt_clear_alarm();
        /* call the alarm handler afterwards if callback was defined*/
        if (alarm_cb) {
            alarm_cb(alarm_arg);
        }
    }

    if (alarm == 0) {
        /* set next alarm which is either an alarm if configured or overflow */
        frc2.alarm = rtt_config.alarm;
        /* call the overflow handler if configured */
        if (rtt_config.overflow_cb) {
            rtt_config.overflow_cb(rtt_config.overflow_arg);
        }
    }
}

void rtt_init(void)
{
    DEBUG("%s saved rtt=%u rtc=%u\n",
          __func__, _rtt_counter_saved, _rtc_counter_saved);

    frc2.ctrl.clk_div = FRC2_CLK_DIV_256;
    frc2.ctrl.reload = 0;
    frc2.ctrl.intr_hold = 0;
    frc2.ctrl.enable = 1;

    /* initialize rtt_config structure after reboot or deep sleep */
    rtt_clear_alarm();
    rtt_clear_overflow_cb();

    if (_rtt_counter_saved || _rtc_counter_saved) {
        /* if not in init after power on, restore the RTT counter value */
        rtt_restore_counter();
    }
    else {
        frc2.load = 0;
        DEBUG("%s after power on\n", __func__);
    }

    /* emulate overflow interrupt */
    frc2.alarm = 0;

    ets_isr_attach (ETS_FRC2_INUM, rtt_cb, NULL);
    ets_isr_unmask (BIT(ETS_FRC2_INUM));
    DPORT.INT_ENABLE |= DPORT_INT_ENABLE_FRC2;
}

void rtt_poweron(void)
{
    /* power on simply reactivates the FRC2 counter */
    frc2.ctrl.enable = 1;
}

void rtt_poweroff(void)
{
    /* power off simply deactivates the FRC2 counter */
    frc2.ctrl.enable = 0;
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    /* there is no overflow interrupt, we emulate */
    rtt_config.overflow_cb = cb;
    rtt_config.overflow_arg = arg;
}

void rtt_clear_overflow_cb(void)
{
    /* there is no overflow interrupt, we emulate */
    rtt_config.overflow_cb = NULL;
    rtt_config.overflow_arg = NULL;
}

uint32_t rtt_get_counter(void)
{
    return frc2.count;
}

void rtt_set_counter(uint32_t counter)
{
    frc2.load = counter;

    if (counter > frc2.alarm) {
        /* overflow is the next interrupt event */
        frc2.alarm = 0;
    }
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    rtt_config.alarm = alarm;
    rtt_config.alarm_cb = cb;
    rtt_config.alarm_arg = arg;

    if (frc2.count < alarm) {
        frc2.alarm = alarm;
    }
}

uint32_t rtt_get_alarm(void)
{
    return rtt_config.alarm;
}

void rtt_clear_alarm(void)
{
    frc2.alarm = 0;
    rtt_config.alarm = 0;
    rtt_config.alarm_cb = NULL;
    rtt_config.alarm_arg = NULL;
}

void rtt_save_counter(void)
{
    /* save counters before going to sleep or reboot */
    _rtt_counter_saved = frc2.count;
    _rtc_counter_saved = RTC.COUNTER;

    DEBUG("%s saved rtt=%u rtc=%u\n",
          __func__, _rtt_counter_saved, _rtc_counter_saved);
}

void rtt_restore_counter(void)
{
    uint32_t rtc_diff = RTC.COUNTER - _rtc_counter_saved;
    uint32_t rtc_diff_us = pm_rtc2usec(rtc_diff, pm_rtc_clock_cali_proc());
    uint32_t rtt_diff = RTT_US_TO_TICKS(rtc_diff_us);

    frc2.load = _rtt_counter_saved + rtt_diff;

    DEBUG("%s rtc_diff=%u rtt_diff=%u load=%u\n", __func__,
          rtc_diff, rtt_diff, _rtt_counter_saved + rtt_diff);
}
