/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     x86-irq
 * @{
 *
 * @file
 * @brief       Longterm and shortterm timeout handler using callbacks.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "hwtimer_cpu.h"
#include "x86_hwtimer.h"
#include "x86_rtc.h"
#include "x86_threading.h"
#include "arch/hwtimer_arch.h"
#include "irq.h"
#include "thread.h"

#include <stdint.h>
#include <stdio.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define US_PER_SECOND (1000l * 1000l)
#define START_MAX_US_PREMATURELY (US_PER_SECOND / 2048)

#define NNN_TS_ITERATIONS (1024)
#define NNN_TICK_ITERATIONS (16)

#define TICK_HZ_VAL   (256)
#define TICK_HZ_REG_A (RTC_REG_A_HZ_256)

#define ASM_FUN_ATTRIBUTES \
    __attribute__((noinline)) \
    __attribute__((no_instrument_function)) \
    __attribute__((optimize("Os", "omit-frame-pointer")))

static bool set_next_alarm(bool may_call);

static uint64_t ts_base;
static x86_rtc_data_t rtc_base;

static uint64_t ts_per_nop_nop_nop;
static uint64_t nop_nop_nops_per_tick;
static uint64_t nop_nop_nops_per_second;
static uint64_t instructions_per_second;

static void ASM_FUN_ATTRIBUTES nop_nop_nop(void)
{
#   include "nop_nop_nop.inc"
}

static uint64_t ASM_FUN_ATTRIBUTES rdtsc(void)
{
    uint64_t result;
    asm volatile ("cpuid" :: "a"(0) : "ebx", "ecx", "edx");
    asm volatile ("rdtsc" : "=A"(result));
    return result;
}

static volatile bool periodic_interrupt_called;

static void flip_periodic_interrupt_called(uint8_t reg_c)
{
    (void) reg_c;

    periodic_interrupt_called = !periodic_interrupt_called;
}

static void measure_nop_nop_nops_per_tick(void)
{
    x86_rtc_set_periodic_callback(flip_periodic_interrupt_called);
    x86_rtc_set_periodic(TICK_HZ_REG_A, 0, 0, true);
    for (unsigned i = 0; i < NNN_TICK_ITERATIONS; ++i) {
        periodic_interrupt_called = false;

        eINT();
        while (!periodic_interrupt_called) {
            nop_nop_nop();
        }
        while (periodic_interrupt_called) {
            nop_nop_nop();
        }
        uint64_t counting_start = rdtsc();
        while (!periodic_interrupt_called) {
            ++nop_nop_nops_per_tick;
            nop_nop_nop();
        }
        dINT();
        uint64_t counting_end = rdtsc();

        ts_per_nop_nop_nop += counting_end - counting_start;
    }
    x86_rtc_set_periodic_callback(NULL);
    x86_rtc_set_periodic(RTC_REG_A_HZ_OFF, 0, KERNEL_PID_UNDEF, false);

    /* instructions_per_second = nop_nop_nops_per_second * ts_per_nop_nop_nop: */
    instructions_per_second = nop_nop_nops_per_tick * TICK_HZ_VAL * ts_per_nop_nop_nop / nop_nop_nops_per_tick / NNN_TICK_ITERATIONS;
    /* nop_nop_nops_per_second = nop_nop_nops_per_tick / TICK_HZ_VAL: */
    nop_nop_nops_per_second = (nop_nop_nops_per_tick * TICK_HZ_VAL) / NNN_TICK_ITERATIONS;
    ts_per_nop_nop_nop /= nop_nop_nops_per_tick;
    nop_nop_nops_per_tick /= NNN_TICK_ITERATIONS;
}

static void update_cb(uint8_t reg_c)
{
    periodic_interrupt_called = reg_c & RTC_REG_C_IRQ_UPDATE;
    DEBUG("DEBUG update_cb(0x%02hhx): periodic_interrupt_called = %u\n", reg_c, periodic_interrupt_called);
}

static void init_bases(void)
{
    x86_rtc_set_periodic_callback(update_cb);
    x86_rtc_set_periodic(RTC_REG_A_HZ_2, 0, 0, true);

    eINT();
    periodic_interrupt_called = false;
    while (!periodic_interrupt_called) {
        asm volatile ("hlt");
    }
    dINT();

    x86_rtc_read(&rtc_base);
    ts_base = rdtsc();

    printf("  %02hhu:%02hhu:%02hhu, %04u-%02hhu-%02hhu is %llu\n",
           rtc_base.hour, rtc_base.minute, rtc_base.second,
           rtc_base.century * 100 + rtc_base.year, rtc_base.month, rtc_base.day,
           ts_base);

    x86_rtc_set_periodic_callback(NULL);
    x86_rtc_set_periodic(RTC_REG_A_HZ_OFF, 0, KERNEL_PID_UNDEF, false);
}

void x86_init_hwtimer(void)
{
    puts("Measuring CPU speed:");

    measure_nop_nop_nops_per_tick();

    printf("  CPU speed = %.3f MHz\n", instructions_per_second / (1024.f * 1024));

    init_bases();
}

static void (*hwtimer_callback)(int timer);

static char hwtimer_stack[KERNEL_CONF_STACKSIZE_DEFAULT];
static kernel_pid_t hwtimer_pid = KERNEL_PID_UNDEF;

struct alarm_time {
    uint64_t ts_absolute_alarm;
    bool enabled;
    struct alarm_time *next, *prev;
};

static struct alarm_time timers[HWTIMER_MAXTIMERS];
static struct alarm_time *timers_start;
static bool hwtimer_ie;
static unsigned rtc_alarm_ie, rtc_update_ie, rtc_periodic_ie;

static bool timer_unlink(unsigned i)
{
    bool do_yield = false;

    bool was_start = timers_start == &timers[i];

    DEBUG("DEBUG timer_unlink(%u): was_start=%u\n", i, was_start);

    if (timers[i].next) {
        timers[i].next->prev = timers[i].prev;
    }
    if (timers[i].prev) {
        timers[i].prev->next = timers[i].next;
    }
    if (was_start) {
        timers_start = timers[i].next;
    }
    timers[i].next = timers[i].prev = NULL;
    if (was_start && timers[i].enabled) {
        do_yield = set_next_alarm(false);
    }
    timers[i].enabled = false;

    return do_yield;
}

static void *hwtimer_tick_handler(void *arg)
{
    (void) arg;

    msg_t msg_array[2];
    msg_init_queue(msg_array, sizeof (msg_array) / sizeof (*msg_array));
    while (1) {
        msg_t m;
        msg_receive(&m);

        dINT();
        set_next_alarm(true);
        eINT();
    }

    return NULL;
}

static void stop_alarms(void)
{
    DEBUG("DEBUG stop_alarms(): AIE=%u, UIE=%u, PIE=%u\n",
          rtc_alarm_ie, rtc_update_ie, rtc_periodic_ie);

    if (rtc_alarm_ie) {
        rtc_alarm_ie = 0;
        x86_rtc_set_alarm(NULL, 0, KERNEL_PID_UNDEF, false);
    }

    if (rtc_update_ie) {
        rtc_update_ie = 0;
        x86_rtc_set_update(0, KERNEL_PID_UNDEF, false);
    }

    if (rtc_periodic_ie) {
        rtc_periodic_ie = 0;
        x86_rtc_set_periodic(RTC_REG_A_HZ_OFF, 0, KERNEL_PID_UNDEF, false);
    }
}

static bool set_next_alarm(bool may_call)
{
    if (!hwtimer_ie) {
        return false;
    }

    while (timers_start) {
        uint64_t ts_now = rdtsc();
        int64_t ts_future = (int64_t) timers_start->ts_absolute_alarm - (int64_t) ts_now;

        /* prevent overflows */
        int64_t us_future = ts_future;
        us_future *= (int64_t) (US_PER_SECOND / 0x1000);
        us_future /= (int64_t) (instructions_per_second / 0x1000);

        unsigned timer_i = timers_start - timers;

        DEBUG("DEBUG set_next_alarm(): timers_start=%u, us_future=%lli, ts_future=%lli\n",
              timer_i, us_future, ts_future);

        if (us_future <= START_MAX_US_PREMATURELY) {
            DEBUG("      callback(%u) (%lli µs prematurely), may_call=%u\n",
                  timer_i, us_future, may_call);
            if (!may_call) {
                msg_t m;
                msg_send_int(&m, hwtimer_pid);
                return true;
            }
            else {
                bool do_yield = timer_unlink(timer_i);

                eINT();
                hwtimer_callback(timer_i);
                if (do_yield) {
                    thread_yield();
                }
                dINT();

                continue;
            }
        }

        us_future -= START_MAX_US_PREMATURELY / 2;
        if (us_future > 5 * US_PER_SECOND) {
            us_future -= US_PER_SECOND;

            int8_t seconds = (us_future / US_PER_SECOND) % 60;
            int8_t minutes = (us_future / US_PER_SECOND) / 60 % 60;
            int8_t hours = (us_future / US_PER_SECOND) / 60 / 60 % 24;

            DEBUG("     setting AIE %02hhu:%02hhu:%02hhu\n", hours, minutes, seconds);

            x86_rtc_data_t rtc_now;
            x86_rtc_read(&rtc_now);

            rtc_now.second += seconds;
            if (rtc_now.second >= 60) {
                rtc_now.second -= 60;
                ++minutes;
            }
            rtc_now.minute += minutes;
            if (rtc_now.minute >= 60) {
                rtc_now.minute -= 60;
                ++hours;
            }
            rtc_now.hour += hours;
            if (rtc_now.hour > 24) {
                rtc_now.hour -= 24;
            }

            rtc_alarm_ie = false;
            stop_alarms();
            x86_rtc_set_alarm(&rtc_now, 0, hwtimer_pid, true);
            rtc_alarm_ie = true;
        }
        else if (us_future > 1 * US_PER_SECOND) {
            DEBUG("      setting UIE\n");

            rtc_update_ie = false;
            stop_alarms();
            x86_rtc_set_update(0, hwtimer_pid, true);
            rtc_update_ie = true;
        }
        else {
            /* TODO: this has to work without an epic if-else construct */
            unsigned hz;
            if ((unsigned long) us_future >= 1 * US_PER_SECOND / 2) {
                hz = RTC_REG_A_HZ_2;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 4) {
                hz = RTC_REG_A_HZ_4;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 8) {
                hz = RTC_REG_A_HZ_8;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 16) {
                hz = RTC_REG_A_HZ_16;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 32) {
                hz = RTC_REG_A_HZ_32;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 64) {
                hz = RTC_REG_A_HZ_64;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 128) {
                hz = RTC_REG_A_HZ_128;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 256) {
                hz = RTC_REG_A_HZ_256;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 512) {
                hz = RTC_REG_A_HZ_512;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 1024) {
                hz = RTC_REG_A_HZ_1024;
            }
            else if ((unsigned long) us_future >= 1 * US_PER_SECOND / 2048) {
                hz = RTC_REG_A_HZ_2048;
            }
            else if ((unsigned long) us_future < 1 * US_PER_SECOND / 4096) {
                hz = RTC_REG_A_HZ_8192;
            }
            else {
                hz = RTC_REG_A_HZ_4096;
            }

            if (hz != rtc_periodic_ie) {
                DEBUG("      setting PIE reg_c |= 0x%02x\n", hz);

                rtc_periodic_ie = false;
                stop_alarms();
                x86_rtc_set_periodic(hz, 0, hwtimer_pid, true);
                rtc_periodic_ie = hz;
            }
        }

        return false;
    }

    stop_alarms();
    return false;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    (void) fcpu;

    hwtimer_callback = handler;
    hwtimer_pid = thread_create(hwtimer_stack,
                                sizeof (hwtimer_stack),
                                1,
                                CREATE_STACKTEST,
                                hwtimer_tick_handler,
                                NULL,
                                "x86-hwtimer");
    hwtimer_ie = true;
}

/* µs since x86_init_hwtimer() */
unsigned long hwtimer_arch_now(void)
{
    unsigned old_state = disableIRQ();
    uint64_t result = rdtsc();
    restoreIRQ(old_state);
    return (result - ts_base) * US_PER_SECOND / instructions_per_second;
}

void hwtimer_arch_set(unsigned long offset_us, short timer)
{
    unsigned old_state = disableIRQ();
    hwtimer_arch_set_absolute(offset_us + hwtimer_arch_now(), timer);
    restoreIRQ(old_state);
}

void hwtimer_arch_enable_interrupt(void)
{
    bool do_yield = false;

    unsigned old_state = disableIRQ();
    if (!hwtimer_ie) {
        hwtimer_ie = true;
        do_yield = set_next_alarm(false);
    }
    restoreIRQ(old_state);

    if (do_yield) {
        thread_yield();
    }
}

void hwtimer_arch_disable_interrupt(void)
{
    unsigned old_state = disableIRQ();
    if (hwtimer_ie) {
        stop_alarms();
        hwtimer_ie = false;
    }
    restoreIRQ(old_state);
}

void hwtimer_arch_set_absolute(unsigned long value_us_, short timer)
{
    unsigned old_state = disableIRQ();

    DEBUG("DEBUG hwtimer_arch_set_absolute(%lu, %hu)\n", value_us_, timer);

    bool new_top = false;

    if (timers[timer].enabled) {
        DEBUG("      overwriting timers[%u]\n", timer);

        timers[timer].enabled = false;
        timer_unlink(timer);

        if (timers_start == &timers[timer]) {
            new_top = true;
        }
    }

    uint64_t now_ts = rdtsc();
    uint64_t now_us = ((now_ts - ts_base) * US_PER_SECOND) / instructions_per_second;

    uint64_t future_us = value_us_;
    if (value_us_ < now_us) {
        future_us += 0x10000ull * 0x10000ull;
    }

    future_us -= now_us;
    uint64_t future_ts = (future_us * instructions_per_second) / US_PER_SECOND;
    DEBUG("      future_us=%llu, future_ts=%llu, now_ts=%llu\n", future_us, future_ts, now_ts);

    timers[timer].ts_absolute_alarm = future_ts + now_ts;
    timers[timer].enabled = true;

    if (timers_start) {
        struct alarm_time *prev = timers_start;
        while (1) {
            if (prev->ts_absolute_alarm < timers[timer].ts_absolute_alarm) {
                if (prev->next) {
                    prev = prev->next;
                }
                else {
                    prev->next = &timers[timer];
                    timers[timer].prev = prev;
                    break;
                }
            }
            else {
                timers[timer].next = prev;
                timers[timer].prev = prev->prev;
                if (timers[timer].prev) {
                    timers[timer].prev->next = &timers[timer];
                }
                else {
                    timers_start = &timers[timer];
                    new_top = true;
                }
                prev->prev = &timers[timer];
                break;
            }
        }
    }
    else {
        timers_start = &timers[timer];
        new_top = true;
    }

    bool do_yield = false;
    if (new_top) {
        do_yield = set_next_alarm(false);
    }

    restoreIRQ(old_state);

    if (do_yield) {
        thread_yield();
    }
}

void hwtimer_arch_unset(short timer)
{
    bool do_yield = false;

    unsigned old_state = disableIRQ();
    if (timers[timer].enabled) {
        bool new_top = timers[timer].prev == NULL;
        timers[timer].enabled = false;
        timer_unlink(timer);
        if (new_top) {
            do_yield = set_next_alarm(false);
        }
    }
    restoreIRQ(old_state);

    if (do_yield) {
        thread_yield();
    }
}
