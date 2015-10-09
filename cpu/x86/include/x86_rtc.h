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
 * @brief   Interfacing the realtime clock on x86 boards.
 *
 * Only use this module to read the current time.
 * Using the other functions in applications would break the hwtimer.
 *
 * @ingroup x86-irq
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__RTC__H__
#define CPU__X86__RTC__H__

#include "msg.h"
#include "x86_cmos.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A timestamp.
 *
 * The value of the century is unreliable.
 */
typedef union x86_rtc_data {
    __extension__ struct {
        uint8_t second, minute, hour;
        uint8_t day, month, year, century;
    };
    uint64_t timestamp;
} x86_rtc_data_t;

#define RTC_REG_SECOND         (0x00)
#define RTC_REG_ALARM_SECOND   (0x01)
#define RTC_REG_MINUTE         (0x02)
#define RTC_REG_ALARM_MINUTE   (0x03)
#define RTC_REG_HOUR           (0x04)
#define RTC_REG_ALARM_HOUR     (0x05)
#define RTC_REG_DOW            (0x06)
#define RTC_REG_DAY            (0x07)
#define RTC_REG_MONTH          (0x08)
#define RTC_REG_YEAR           (0x09)
#define RTC_REG_A              (0x0a)
#define RTC_REG_B              (0x0b)
#define RTC_REG_C              (0x0c)
#define RTC_REG_D              (0x0d)
#define RTC_REG_POST           (0x0e)
#define RTC_REG_CENTURY        (0x32)

#define RTC_REG_A_HZ_OFF       ( 0 << 0)
#define RTC_REG_A_HZ_8192      ( 3 << 0)
#define RTC_REG_A_HZ_4096      ( 4 << 0)
#define RTC_REG_A_HZ_2048      ( 5 << 0)
#define RTC_REG_A_HZ_1024      ( 6 << 0)
#define RTC_REG_A_HZ_512       ( 7 << 0)
#define RTC_REG_A_HZ_256       ( 8 << 0)
#define RTC_REG_A_HZ_128       ( 9 << 0)
#define RTC_REG_A_HZ_64        (10 << 0)
#define RTC_REG_A_HZ_32        (11 << 0)
#define RTC_REG_A_HZ_16        (12 << 0)
#define RTC_REG_A_HZ_8         (13 << 0)
#define RTC_REG_A_HZ_4         (14 << 0)
#define RTC_REG_A_HZ_2         (15 << 0)
#define RTC_REG_A_HZ_MASK      (15 << 0)
#define RTC_REG_A_DIVIDER_MASK ( 3 << 4)
#define RTC_REG_A_UPDATING     ( 1 << 7)

#define RTC_REG_B_DST          (1 << 0)
#define RTC_REG_B_24H          (1 << 1)
#define RTC_REG_B_BIN          (1 << 2)
#define RTC_REG_B_WAVE         (1 << 3)
#define RTC_REG_B_INT_UPDATE   (1 << 4)
#define RTC_REG_B_INT_ALARM    (1 << 5)
#define RTC_REG_B_INT_PERIODIC (1 << 6)
#define RTC_REG_B_INT_MASK     (7 << 4)
#define RTC_REG_B_UPDATE       (1 << 7)

#define RTC_REG_C_IRQ_UPDATE   (1 << 4)
#define RTC_REG_C_IRQ_ALARM    (1 << 5)
#define RTC_REG_C_IRQ_PERIODIC (1 << 6)
#define RTC_REG_C_IRQ          (1 << 7)

#define RTC_REG_D_VALID        (1 << 7)

#define RTC_REG_POST_TIME_INVALID (1 << 2)
#define RTC_REG_POST_POWER_LOSS   (1 << 7)

/**
 * @brief   Initialize the Real Time Clock.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 *
 * The RTC subsystem will refuse to work if the CMOS says that
 * @li there was a power loss,
 * @li the stored time is invalid (i.e. February 30), or
 * @li the CMOS had a checksum failure.
 */
void x86_init_rtc(void);

/**
 * @brief     Read the current time.
 * @returns   false iff the RTC is unreliable, the value of dest is random is this case.
 *
 * This reads the CMOS value
 * The standard does not tell the timezone of this value.
 */
bool x86_rtc_read(x86_rtc_data_t *dest);

/**
 * @brief   A custom callback handler for RTC interrupts.
 * @param   reg_c   The value of CMOS register C.
 */
typedef void (*x86_rtc_callback_t)(uint8_t reg_c);

/**
 * @brief       Set an RTC alarm.
 * @param[in]   when            Time when the RTC you raise an interrupt. The date part is ignored.
 * @param       msg_content     The value for msg_t::content.value.
 * @param       target_pid      The process which shall receive the message, `KERNEL_PID_UNDEF` to disable.
 * @param       allow_replace   Whether it is allowed to overwrite an existing alarm.
 *
 * The value of msg_t::type will be `reg_c | (RTC_REG_B_INT_UPDATE << 8)`,
 * where `reg_c` is the value of CMOS register C.
 *
 * You should not call this function directly.
 * You should use xtimer instead.
 */
bool x86_rtc_set_alarm(const x86_rtc_data_t *when, uint32_t msg_content, kernel_pid_t target_pid, bool allow_replace);

/**
 * @brief   Set up periodic interrupts
 * @param   hz              How often a second the interrupt should fire, e.g. RTC_REG_A_HZ_8192.
 * @param   msg_content     The value for msg_t::content.value.
 * @param   target_pid      The process which shall receive the message, `KERNEL_PID_UNDEF` to disable.
 * @param   allow_replace   Whether it is allowed to overwrite an existing alarm.
 *
 * The value of msg_t::type will be `reg_c | (RTC_REG_B_INT_PERIODIC << 8)`,
 * where `reg_c` is the value of CMOS register C.
 *
 * You should not call this function directly.
 * You should use xtimer instead.
 */
bool x86_rtc_set_periodic(uint8_t hz, uint32_t msg_content, kernel_pid_t target_pid, bool allow_replace);

/**
 * @brief   Set up secondly interrupts.
 * @param   msg_content     The value for msg_t::content.value.
 * @param   target_pid      The process which shall receive the message, `KERNEL_PID_UNDEF` to disable.
 * @param   allow_replace   Whether it is allowed to overwrite an existing alarm.
 *
 * The value of msg_t::type will be `reg_c | (RTC_REG_B_INT_UPDATE << 8)`,
 * where `reg_c` is the value of CMOS register C.
 *
 * You should not call this function directly.
 * You should use xtimer instead.
 */
bool x86_rtc_set_update(uint32_t msg_content, kernel_pid_t target_pid, bool allow_replace);

/**
 * @brief   Set custom alarm interrupt handler.
 * @param   cb   Your custom handler, or NULL to use default.
 *
 * You must never use this function.
 * It is only there for x86_hwtimer.c,
 * because the hwtimer subsystem gets set up before the message system works.
 */
void x86_rtc_set_alarm_callback(x86_rtc_callback_t cb);

/**
 * @brief   Set custom periodic interrupt handler.
 * @param   cb   Your custom handler, or NULL to use default.
 *
 * You must never use this function.
 * It is only there for x86_hwtimer.c,
 * because the hwtimer subsystem gets set up before the message system works.
 */
void x86_rtc_set_periodic_callback(x86_rtc_callback_t cb);

/**
 * @brief   Set custom update interrupt handler.
 * @param   cb   Your custom handler, or NULL to use default.
 *
 * You must never use this function.
 * It is only there for x86_hwtimer.c,
 * because the hwtimer subsystem gets set up before the message system works.
 */
void x86_rtc_set_update_callback(x86_rtc_callback_t cb);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
