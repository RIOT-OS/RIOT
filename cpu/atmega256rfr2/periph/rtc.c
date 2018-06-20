#include <avr/interrupt.h>
#include <stdio.h>
#include "periph/rtc.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define YEAR_OFFSET (1900)

static volatile struct tm no_alarm = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static volatile struct tm current_time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static volatile struct tm alarm_time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static volatile rtc_alarm_cb_t alarm_cb;
static void *alarm_arg;



void rtc_init(void)
{
    DDRG &= ~((1 << PG3) | (1 << PG4));
    rtc_poweron();
    TIMSK2 &= ~((1 << TOIE2) | (1 << OCIE2A) | (1 << OCIE2B));
    ASSR |= (1 << AS2);
    TCNT2 = 0;
    TCCR2A = 0;
    TCCR2B = (1 << CS22) | (1 << CS20);
    while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB))) {}
    TIMSK2 |= (1 << TOIE2);
    sei();
}

int rtc_set_time(struct tm *time)
{
    current_time = *time;
    return 0;
}

int rtc_get_time(struct tm *time)
{
    *(time) = current_time;
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    if (time->tm_sec < 60 && time->tm_min < 60 && time->tm_hour < 24 && time->tm_mday < 32 \
        && time->tm_wday < 7 && time->tm_mon < 12 && time != NULL) {
        //time ok set alarm
        alarm_time = *time;
        alarm_cb = cb;
        alarm_arg = arg;
        return 0;
    }
    return -2;
}

int rtc_get_alarm(struct tm *time)
{
    *time = alarm_time;
    return 0;
}

void rtc_clear_alarm(void)
{
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

uint8_t __compare_time(volatile struct tm *time1, volatile struct tm *time2)
{
    if (time1->tm_hour == time2->tm_hour && time1->tm_mday == time2->tm_mday \
        && time1->tm_min == time2->tm_min && time1->tm_mon == time2->tm_mon \
        && time1->tm_sec == time2->tm_sec && time1->tm_year == time2->tm_year) {
        return 1;
    }
    return 0;
}

ISR(TIMER2_OVF_vect){
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
                else if (current_time.tm_mday  == 31) {
                    if (current_time.tm_mon == 4 || current_time.tm_mon == 6 || current_time.tm_mon == 9 || current_time.tm_mon == 11) {
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
        (*alarm_cb)(alarm_arg);
    }
}
