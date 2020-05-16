/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mcb2388
 * @{
 *
 * @file
 * @brief       Demo Application for the MCB2388 board
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <stdio.h>

#include "hd44780.h"
#include "hd44780_params.h"

#include "periph/adc.h"
#include "periph/rtc.h"

#include "fmt.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "xtimer.h"

/* update the display at 50 Hz */
#define FRAME_TIME_US (1000000/50)

/* application states */
enum {
    STATE_ENTER_DAY,
    STATE_ENTER_MON,
    STATE_ENTER_YEAR,
    STATE_ENTER_HOUR,
    STATE_ENTER_MIN,
    STATE_ENTER_SEC,
    STATE_RUNNING,
};

/* Print a number on the HD4470 display
 *
 * The display acts like a terminal, so if not specified otherwise
 * new characters are just printed after the previous ones.
 */
static void print_num(hd44780_t *dev, unsigned num, bool print)
{
    char buffer[5];
    int res = snprintf(buffer, sizeof(buffer), "%02u", num);
    if (print) {
        hd44780_print(dev, buffer);
    } else while (res--) {
        hd44780_write(dev, ' ');
    }
}

/* read a sample from the ADC and scale it down to the desired range */
static uint8_t read_adc_knob(uint8_t max_val)
{
    int sample = adc_sample(0, ADC_RES_6BIT);
    return (max_val * sample) >> 16;
}

/* print the 'Enter Time' screen */
static void input_time(hd44780_t *dev, struct tm *now, unsigned state, bool blink)
{
    hd44780_clear(dev);
    hd44780_set_cursor(dev, 0, 0);

    print_num(dev, now->tm_hour, state != STATE_ENTER_HOUR || blink);
    hd44780_write(dev, ':');
    print_num(dev, now->tm_min, state != STATE_ENTER_MIN || blink);
    hd44780_write(dev, ':');
    print_num(dev, now->tm_sec, state != STATE_ENTER_SEC || blink);

    hd44780_set_cursor(dev, 0, 1);
    hd44780_print(dev, "Enter Time");
}

/* print the 'Enter Date' screen */
static void input_date(hd44780_t *dev, struct tm *now, unsigned state, bool blink)
{
    hd44780_clear(dev);
    hd44780_set_cursor(dev, 0, 0);

    print_num(dev, now->tm_mday, state != STATE_ENTER_DAY || blink);
    hd44780_write(dev, '.');
    print_num(dev, now->tm_mon + 1, state != STATE_ENTER_MON || blink);
    hd44780_write(dev, '.');
    print_num(dev, now->tm_year + 1900, state != STATE_ENTER_YEAR || blink);

    hd44780_set_cursor(dev, 0, 1);
    hd44780_print(dev, "Enter Date");
}

/* print the 'Time & Date' screen */
static void print_time_and_date(hd44780_t *dev, struct tm *now)
{
    hd44780_clear(dev);
    hd44780_set_cursor(dev, 0, 0);

    print_num(dev, now->tm_hour, 1);
    hd44780_write(dev, ':');
    print_num(dev, now->tm_min, 1);
    hd44780_write(dev, ':');
    print_num(dev, now->tm_sec, 1);

    hd44780_set_cursor(dev, 0, 1);
    print_num(dev, now->tm_mday, 1);
    hd44780_write(dev, '.');
    print_num(dev, now->tm_mon + 1, 1);
    hd44780_write(dev, '.');
    print_num(dev, now->tm_year + 1900, 1);
}

/* The 8 on-board LEDs are connected to one port from PIN0 - PIN7 in a row.
 * They can used directly to output 8-bit values.
 */
static inline void _led_display(uint8_t val)
{
    FIO_PORTS[2].CLR = 0xFF;
    FIO_PORTS[2].SET = val;
}

/* show blink pattern on the LEDs */
static void blinky(void)
{
    static uint8_t cur;
    static int8_t inc = 1;
    const uint8_t pattern[] = {
        0x81, 0x42, 0x24, 0x18
    };

    _led_display(pattern[cur]);

    if ((uint8_t)(cur + inc) >= sizeof(pattern)) {
        inc = -inc;
    }

    cur += inc;
}

/* main application thread, takes care of updating the display and
 * processing user input.
 */
static void* display_thread(void *ctx)
{
    hd44780_t dev;

    /* init display */
    if (hd44780_init(&dev, &hd44780_params[0]) != 0) {
        puts("init display failed");
        return NULL;
    }

    /* init inputs */
    gpio_init(BTN0_PIN, BTN0_MODE);
    adc_init(0);

    /* get the current time */
    struct tm rtc_now;
    rtc_get_time(&rtc_now);

    xtimer_ticks32_t now = xtimer_now();

    uint8_t cooldown  = 0;
    uint8_t state = STATE_ENTER_DAY;
    for (unsigned i = 0; ; ++i) {

        /* create blink effect - toggle every 16th loop iteration */
        bool blink = i & 0x10;

        switch (state) {
        case STATE_ENTER_HOUR:
            rtc_now.tm_hour = read_adc_knob(24);
            input_time(&dev, &rtc_now, state, blink);
            break;
        case STATE_ENTER_MIN:
            rtc_now.tm_min = read_adc_knob(60);
            input_time(&dev, &rtc_now, state, blink);
            break;
        case STATE_ENTER_SEC:
            rtc_now.tm_sec = read_adc_knob(60);
            input_time(&dev, &rtc_now, state, blink);
            break;
        case STATE_ENTER_DAY:
            rtc_now.tm_mday = read_adc_knob(31) + 1;
            input_date(&dev, &rtc_now, state, blink);
            break;
        case STATE_ENTER_MON:
            rtc_now.tm_mon = read_adc_knob(12);
            input_date(&dev, &rtc_now, state, blink);
            break;
        case STATE_ENTER_YEAR:
            rtc_now.tm_year = read_adc_knob(100) + 100;
            input_date(&dev, &rtc_now, state, blink);
            break;
        default:
            rtc_get_time(&rtc_now);
            print_time_and_date(&dev, &rtc_now);
        }

        /* prevent advancing two components if the user holds
         * the button down for too long.
         */
        if (cooldown) {
            --cooldown;
        }

        if (!gpio_read(BTN0_PIN) && !cooldown) {
            cooldown = 10;

            if (state < STATE_RUNNING) {
                ++state;
            }

            if (state == STATE_RUNNING) {
                rtc_set_time(&rtc_now);
            }
        }

        if (state == STATE_RUNNING) {
            _led_display(rtc_now.tm_sec);
        } else if ((i & 0x7) == 0x7) {
            /* advance the blink pattern */
            blinky();
        }

        xtimer_periodic_wakeup(&now, FRAME_TIME_US);
    }

    return ctx;
}

static char display_stack[THREAD_STACKSIZE_DEFAULT];

int main(void)
{
    /* start the application thread */
    thread_create(display_stack, sizeof(display_stack),
                  THREAD_PRIORITY_MAIN, 0, display_thread,
                  NULL, "display");

    /* this thread is used for the shell now */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
