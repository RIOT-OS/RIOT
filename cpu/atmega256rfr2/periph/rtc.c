/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt, Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief       Low-Level RTC Implementation using a watch crystal on async. Timer 2
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>

#include "cpu.h"
#include "periph/rtc.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define YEAR_OFFSET (1900)

typedef struct tm tm_t;

static volatile tm_t no_alarm = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static volatile tm_t current_time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static volatile tm_t alarm_time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static volatile rtc_alarm_cb_t alarm_cb;
static void *alarm_arg;

uint8_t __compare_time(volatile tm_t *time1, volatile tm_t *time2)
{
    if (time1->tm_hour == time2->tm_hour && time1->tm_mday == time2->tm_mday
        && time1->tm_min == time2->tm_min && time1->tm_mon == time2->tm_mon
        && time1->tm_sec == time2->tm_sec && time1->tm_year == time2->tm_year) {
        return 1;
    }
    return 0;
}

void rtc_init(void)
{
    /* Configure Port G3/4 as crystal oscillator input for TC2 */
    DDRG &= ~((1 << PG3) | (1 << PG4));
    /* Allready power on so it can swing in*/
    rtc_poweron();
    /* Disable interrupts of TC2 */
    TIMSK2 &= ~((1 << TOIE2) | (1 << OCIE2A) | (1 << OCIE2B));

    /* Set TC to asynchronous mode with external clock (32,768kHz) */
    ASSR |= (1 << AS2);
    /* Reset timer value*/
    TCNT2 = 0;
    /* Normal mode of operation */
    TCCR2A = 0;
    /* TC0 with prescaler 128 resulting in 1 second for an overflow
     * 2^8*128/32768Hz = 1s*/
    TCCR2B = (1 << CS22) | (1 << CS20);

    /* Wait for TC2 to be ready */
    while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB))) {}

    /* Clear interrupt flags */
    TIFR2 = (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);
    /* Overflow Interrupt Enable */
    TIMSK2 |= (1 << TOIE2);
    /* Global interrupts Enable */
    sei();
}

int rtc_set_time(tm_t *time)
{
    /* Disable Overflow interrupts */
    TIMSK2 &= ~(1 << TOIE2);
    /* Set new time */
    current_time = *time;
    /* Overflow Interrupt Enable */
    TIMSK2 |= (1 << TOIE2);
    return 0;
}

int rtc_get_time(tm_t *time)
{
    /* Ensure no overflow happend */
    do {
        *time = current_time;
    } while (!__compare_time(&current_time, time));

    return 0;
}

int rtc_set_alarm(tm_t *time, rtc_alarm_cb_t cb, void *arg)
{
    /* Check if callback and time are given */
    if ((cb == NULL) || (time == NULL)) {
        return -1;
    }

    /* Check if alarm time is in the past */
    tm_t tm_now = current_time;
    int32_t diff = difftime(mk_gmtime(time), mk_gmtime(&tm_now));
    if (diff <= 0) {
        return -1;
    }

    /* Check if time is valid */
    if (time->tm_sec < 60 && time->tm_min < 60 && time->tm_hour < 24
        && time->tm_mday < 32 && time->tm_mon < 12) {

        alarm_time = *time;
        alarm_cb = cb;
        alarm_arg = arg;
        return 0;
    }
    return -2;
}

int rtc_get_alarm(tm_t *time)
{
    /* Ensure alarm did not happen */
    do {
        *time = alarm_time;
    } while (!__compare_time(&alarm_time, time));

    return 0;
}

void rtc_clear_alarm(void)
{
    /* Set alarm time to zero */
    alarm_time = no_alarm;
}

void rtc_poweroff(void)
{
    PRR0 |= (1 << PRTIM2);
}

void rtc_poweron(void)
{
    PRR0 &= ~(1 << PRTIM2);
}

uint8_t __not_leap(void)
{
    if (!((current_time.tm_year + YEAR_OFFSET) % 100)) {
        return (current_time.tm_year + YEAR_OFFSET) % 400;
    }
    else {
        return (current_time.tm_year + YEAR_OFFSET) % 4;
    }
}

ISR(TIMER2_OVF_vect){
    __enter_isr();

    current_time.tm_sec++;
    if (current_time.tm_sec == 60) {
        current_time.tm_sec = 0;
        current_time.tm_min++;
        if (current_time.tm_min == 60) {
            current_time.tm_min = 0;
            current_time.tm_hour++;
            if (current_time.tm_hour == 24) {
                current_time.tm_hour = 0;
                current_time.tm_mday++;
                current_time.tm_yday++;
                if (current_time.tm_mday == 32) {
                    current_time.tm_mon++;
                    current_time.tm_mday = 1;
                }
                else if (current_time.tm_mday == 31) {
                    if (current_time.tm_mon == 4 || current_time.tm_mon == 6
                        || current_time.tm_mon == 9 || current_time.tm_mon == 11) {
                        current_time.tm_mon++;
                        current_time.tm_mday = 1;
                    }
                }
                else if (current_time.tm_mday == 30) {
                    if (current_time.tm_mon == 2) {
                        current_time.tm_mon++;
                        current_time.tm_mday = 1;
                    }
                }
                else if (current_time.tm_mday == 29) {
                    if (current_time.tm_mon == 2 && __not_leap()) {
                        current_time.tm_mon++;
                        current_time.tm_mday = 1;
                    }
                }
                if (current_time.tm_mon == 13) {
                    current_time.tm_year++;
                    current_time.tm_mon = 1;
                }
            }
        }
    }

    if (__compare_time(&current_time, &alarm_time)) {
        /* Set alarm time to zero */
        alarm_time = no_alarm;
        /* Execute callback */
        (*alarm_cb)(alarm_arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
        thread_yield_isr();
    }

    __exit_isr();
}
