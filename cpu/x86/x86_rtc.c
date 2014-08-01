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
 * @brief       Reading and interrupt handling for the Real Time Clock (RTC).
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_pic.h"
#include "x86_rtc.h"
#include "irq.h"

#include <stdio.h>

#include "kernel.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static bool valid;

static int32_t alarm_msg_content, periodic_msg_content, update_msg_content;
static unsigned alarm_pid = KERNEL_PID_NULL, periodic_pid = KERNEL_PID_NULL, update_pid = KERNEL_PID_NULL;

static void alarm_callback_default(uint8_t reg_c)
{
    if (alarm_pid != KERNEL_PID_NULL) {
        msg_t m;
        m.type = reg_c | (RTC_REG_B_INT_ALARM << 8);
        m.content.value = alarm_msg_content;
        msg_send_int(&m, alarm_pid);
    }
}

static void periodic_callback_default(uint8_t reg_c)
{
    if (periodic_pid != KERNEL_PID_NULL) {
        msg_t m;
        m.type = reg_c | (RTC_REG_B_INT_PERIODIC << 8);
        m.content.value = periodic_msg_content;
        msg_send_int(&m, periodic_pid);
    }
}

static void update_callback_default(uint8_t reg_c)
{
    if (update_pid != KERNEL_PID_NULL) {
        msg_t m;
        m.type = reg_c | (RTC_REG_B_INT_UPDATE << 8);
        m.content.value = update_msg_content;
        msg_send_int(&m, update_pid);
    }
}

static x86_rtc_callback_t alarm_callback = alarm_callback_default;
static x86_rtc_callback_t periodic_callback = periodic_callback_default;
static x86_rtc_callback_t update_callback = update_callback_default;

void x86_rtc_set_alarm_callback(x86_rtc_callback_t cb)
{
    alarm_callback = cb ? cb : alarm_callback_default;
}

void x86_rtc_set_periodic_callback(x86_rtc_callback_t cb)
{
    periodic_callback = cb ? cb : periodic_callback_default;
}

void x86_rtc_set_update_callback(x86_rtc_callback_t cb)
{
    update_callback = cb ? cb : update_callback_default;
}

static void rtc_irq_handler(uint8_t irq_num)
{
    (void) irq_num; /* == PIC_NUM_RTC */

    uint8_t c = x86_cmos_read(RTC_REG_C);
    DEBUG("RTC: c = 0x%02x, IRQ=%u, A=%u, P=%u, U=%u\n", c, c & RTC_REG_C_IRQ ? 1 : 0,
                                                            c & RTC_REG_C_IRQ_ALARM ? 1 : 0,
                                                            c & RTC_REG_C_IRQ_PERIODIC ? 1 : 0,
                                                            c & RTC_REG_C_IRQ_UPDATE ? 1 : 0);
    if (!(c & RTC_REG_C_IRQ)) {
        return;
    }
    if (c & RTC_REG_C_IRQ_ALARM) {
        alarm_callback(c);
    }
    if (c & RTC_REG_C_IRQ_PERIODIC) {
        periodic_callback(c);
    }
    if (c & RTC_REG_C_IRQ_UPDATE) {
        update_callback(c);
    }
}

void x86_init_rtc(void)
{
    uint8_t d = x86_cmos_read(RTC_REG_D);
    valid = (d & RTC_REG_D_VALID) != 0;
    if (!valid) {
        puts("Warning: RTC does not work.");
        return;
    }

    x86_cmos_write(RTC_REG_B, x86_cmos_read(RTC_REG_B) & ~RTC_REG_B_INT_MASK);
    rtc_irq_handler(0);
    x86_pic_set_handler(PIC_NUM_RTC, &rtc_irq_handler);
    x86_pic_enable_irq(PIC_NUM_RTC);

    x86_rtc_data_t now;
    x86_rtc_read(&now);
    printf("RTC initialized [%02hhu:%02hhu:%02hhu, %04u-%02hhu-%02hhu]\n",
           now.hour, now.minute, now.second,
           now.century * 100 + now.year, now.month, now.day);

    if (x86_cmos_read(RTC_REG_POST) & (RTC_REG_POST_POWER_LOSS | RTC_REG_POST_TIME_INVALID)) {
        puts("Warning: RTC time is invalid (power loss?)");
    }
}

static inline bool is_update_in_progress(void)
{
    return (x86_cmos_read(RTC_REG_A) & RTC_REG_A_UPDATING) != 0;
}

static uint8_t bcd2binary(uint8_t datum)
{
    return (datum / 16) * 10 + (datum % 16);
}

static uint8_t binary2bcd(uint8_t datum)
{
    return (datum / 10) * 16 + (datum % 10);
}

bool x86_rtc_read(x86_rtc_data_t *dest)
{
    if (!valid) {
        return false;
    }

    unsigned old_status = disableIRQ();

    while (is_update_in_progress()) {
        asm volatile ("pause");
    }

    uint8_t b = x86_cmos_read(RTC_REG_B);
    do {
        dest->second  = x86_cmos_read(RTC_REG_SECOND);
        dest->minute  = x86_cmos_read(RTC_REG_MINUTE);
        dest->hour    = x86_cmos_read(RTC_REG_HOUR);
        dest->day     = x86_cmos_read(RTC_REG_DAY);
        dest->month   = x86_cmos_read(RTC_REG_MONTH);
        dest->year    = x86_cmos_read(RTC_REG_YEAR);
        dest->century = bcd2binary(x86_cmos_read(RTC_REG_CENTURY));
    } while (dest->second != x86_cmos_read(RTC_REG_SECOND));
    if (dest->century == 0) {
        dest->century = 20; // safe guess
    }

    if (!(b & RTC_REG_B_BIN)) {
        dest->second  = bcd2binary(dest->second);
        dest->minute  = bcd2binary(dest->minute);
        dest->hour    = ((dest->hour & 0x0F) + (((dest->hour & 0x70) / 16) * 10)) | (dest->hour & 0x80);
        dest->day     = bcd2binary(dest->day);
        dest->month   = bcd2binary(dest->month);
        dest->year    = bcd2binary(dest->year);
    }
    if (!(b & RTC_REG_B_24H) && (dest->hour & 0x80)) {
        dest->hour = ((dest->hour & 0x7F) + 12) % 24;
    }

    restoreIRQ(old_status);
    return true;
}

bool x86_rtc_set_alarm(const x86_rtc_data_t *when, uint32_t msg_content, kernel_pid_t target_pid, bool allow_replace)
{
    if (!valid) {
        return false;
    }

    unsigned old_status = disableIRQ();
    bool result;
    if (target_pid == KERNEL_PID_NULL) {
        result = true;
        alarm_pid = KERNEL_PID_NULL;

        uint8_t b = x86_cmos_read(RTC_REG_B);
        x86_cmos_write(RTC_REG_B, b & ~RTC_REG_B_INT_ALARM);
    }
    else {
        result = allow_replace || alarm_pid == KERNEL_PID_NULL;
        if (result) {
            alarm_msg_content = msg_content;
            alarm_pid = target_pid;

            uint8_t b = x86_cmos_read(RTC_REG_B);
            if (b & RTC_REG_B_BIN) {
                x86_cmos_write(RTC_REG_ALARM_SECOND, when->second);
                x86_cmos_write(RTC_REG_ALARM_MINUTE, when->minute);
                x86_cmos_write(RTC_REG_ALARM_HOUR, when->hour);
            }
            else {
                x86_cmos_write(RTC_REG_ALARM_SECOND, binary2bcd(when->second));
                x86_cmos_write(RTC_REG_ALARM_MINUTE, binary2bcd(when->minute));
                x86_cmos_write(RTC_REG_ALARM_HOUR, binary2bcd(when->hour));
            }
            x86_cmos_write(RTC_REG_B, b | RTC_REG_B_INT_ALARM);
        }
    }
    rtc_irq_handler(0);
    restoreIRQ(old_status);
    return result;
}

bool x86_rtc_set_periodic(uint8_t hz, uint32_t msg_content, kernel_pid_t target_pid, bool allow_replace)
{
    if (!valid) {
        return false;
    }

    unsigned old_status = disableIRQ();
    bool result;
    if (target_pid == KERNEL_PID_NULL || hz == RTC_REG_A_HZ_OFF) {
        result = true;
        periodic_pid = KERNEL_PID_NULL;

        uint8_t old_divider = x86_cmos_read(RTC_REG_A) & ~RTC_REG_A_HZ_MASK;
        x86_cmos_write(RTC_REG_A, old_divider | RTC_REG_A_HZ_OFF);
        x86_cmos_write(RTC_REG_B, x86_cmos_read(RTC_REG_B) & ~RTC_REG_B_INT_PERIODIC);
    }
    else {
        result = allow_replace || periodic_pid == KERNEL_PID_NULL;
        if (result) {
            periodic_msg_content = msg_content;
            periodic_pid = target_pid;

            uint8_t old_divider = x86_cmos_read(RTC_REG_A) & ~RTC_REG_A_HZ_MASK;
            x86_cmos_write(RTC_REG_A, old_divider | hz);
            x86_cmos_write(RTC_REG_B, x86_cmos_read(RTC_REG_B) | RTC_REG_B_INT_PERIODIC);
        }
    }
    rtc_irq_handler(0);
    restoreIRQ(old_status);
    return result;
}

bool x86_rtc_set_update(uint32_t msg_content, kernel_pid_t target_pid, bool allow_replace)
{
    if (!valid) {
        return false;
    }

    unsigned old_status = disableIRQ();
    bool result;
    if (target_pid == KERNEL_PID_NULL) {
        result = true;
        update_pid = KERNEL_PID_NULL;

        x86_cmos_write(RTC_REG_B, x86_cmos_read(RTC_REG_B) & ~RTC_REG_B_INT_UPDATE);
    }
    else {
        result = allow_replace || update_pid == KERNEL_PID_NULL;
        if (result) {
            update_msg_content = msg_content;
            update_pid = target_pid;

            x86_cmos_write(RTC_REG_B, x86_cmos_read(RTC_REG_B) | RTC_REG_B_INT_UPDATE);
        }
    }
    rtc_irq_handler(0);
    restoreIRQ(old_status);
    return result;
}
