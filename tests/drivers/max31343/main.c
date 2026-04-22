/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the MAX31343 RTC driver
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "max31343.h"
#include "max31343_params.h"
#include "shell.h"
#include "ztimer.h"

#define ISOSTR_LEN  (20U)   /* "YYYY-MM-DDTHH:mm:ss" + NUL */
#define TEST_DELAY  (2U)    /* seconds used in the self-test */

static max31343_t _dev;

static const struct tm _ref_time = {
    .tm_sec   = 0,
    .tm_min   = 0,
    .tm_hour  = 0,
    .tm_wday  = 1,   /* Thursday */
    .tm_mday  = 9,
    .tm_mon   = 10,   /* January */
    .tm_year  = 126, /* 2026 */
    .tm_isdst = -1,
};

static int _tm_from_str(const char *str, struct tm *t)
{
    char tmp[5];

    if (strlen(str) != ISOSTR_LEN - 1) {
        return -1;
    }
    if (str[4] != '-' || str[7] != '-' || str[10] != 'T' ||
        str[13] != ':' || str[16] != ':') {
        return -1;
    }

    memset(t, 0, sizeof(*t));

    memcpy(tmp, str, 4); tmp[4] = '\0'; str += 5;
    t->tm_year = atoi(tmp) - 1900;

    memcpy(tmp, str, 2); tmp[2] = '\0'; str += 3;
    t->tm_mon = atoi(tmp) - 1;

    memcpy(tmp, str, 2); str += 3;
    t->tm_mday = atoi(tmp);

    memcpy(tmp, str, 2); str += 3;
    t->tm_hour = atoi(tmp);

    memcpy(tmp, str, 2); str += 3;
    t->tm_min = atoi(tmp);

    memcpy(tmp, str, 2);
    t->tm_sec = atoi(tmp);

    t->tm_isdst = -1;
    return 0;
}

static void _print_time(const struct tm *t)
{
    char buf[ISOSTR_LEN];
    size_t n = strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", t);
    buf[n] = '\0';
    printf("%s\n", buf);
}

static int _cmd_get(int argc, char **argv)
{
    (void)argc; (void)argv;

    struct tm t;
    int res = max31343_get_time(&_dev, &t);
    if (res != 0) {
        printf("error: get_time failed (%d)\n", res);
        return 1;
    }
    printf("current time: ");
    _print_time(&t);
    return 0;
}

static int _cmd_set(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <YYYY-MM-DDTHH:mm:ss>\n", argv[0]);
        return 1;
    }

    struct tm t;
    if (_tm_from_str(argv[1], &t) != 0) {
        puts("error: invalid date string");
        return 1;
    }

    int res = max31343_set_time(&_dev, &t);
    if (res != 0) {
        printf("error: set_time failed (%d)\n", res);
        return 1;
    }
    printf("time set to %s\n", argv[1]);
    return 0;
}

static int _cmd_alarm_get(int argc, char **argv)
{
    (void)argc; (void)argv;

    struct tm t;
    int res = max31343_get_alarm(&_dev, &t);
    if (res == -ENOENT) {
        puts("no alarm set");
        return 0;
    }
    if (res != 0) {
        printf("error: get_alarm failed (%d)\n", res);
        return 1;
    }
    printf("alarm time: ");
    _print_time(&t);
    return 0;
}

static int _cmd_alarm_set(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <YYYY-MM-DDTHH:mm:ss>\n", argv[0]);
        return 1;
    }

    struct tm t;
    if (_tm_from_str(argv[1], &t) != 0) {
        puts("error: invalid date string");
        return 1;
    }

    int res = max31343_set_alarm(&_dev, &t);
    if (res != 0) {
        printf("error: set_alarm failed (%d)\n", res);
        return 1;
    }
    printf("alarm set to %s\n", argv[1]);
    return 0;
}

static int _cmd_alarm_enable(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <0|1>\n", argv[0]);
        return 1;
    }

    bool en = (argv[1][0] == '1');
    int res = max31343_alarm_enable(&_dev, en);
    if (res != 0) {
        printf("error: alarm_enable failed (%d)\n", res);
        return 1;
    }
    printf("alarm interrupt %s\n", en ? "enabled" : "disabled");
    return 0;
}

static int _cmd_alarm_clear(int argc, char **argv)
{
    (void)argc; (void)argv;

    int res = max31343_clear_alarm(&_dev);
    if (res != 0) {
        printf("error: clear_alarm failed (%d)\n", res);
        return 1;
    }
    puts("alarm cleared");
    return 0;
}

static int _cmd_temp(int argc, char **argv)
{
    (void)argc; (void)argv;

    int16_t centi;
    int res = max31343_get_temp(&_dev, &centi);
    if (res != 0) {
        printf("error: get_temp failed (%d)\n", res);
        return 1;
    }
    int t_int  = centi / 100;
    int t_frac = centi - t_int * 100;
    if (t_frac < 0) { t_frac = -t_frac; }
    printf("temperature: %d.%02d°C\n", t_int, t_frac);
    return 0;
}

static int _cmd_sqw(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <0-5>  (0=1Hz 1=2Hz 2=4Hz 3=8Hz 4=16Hz 5=32Hz)\n",
               argv[0]);
        return 1;
    }

    int freq = atoi(argv[1]);
    if (freq < 0 || freq > 5) {
        puts("error: frequency out of range (0-5)");
        return 1;
    }

    int res = max31343_set_sqw(&_dev, (max31343_sqw_freq_t)freq);
    if (res != 0) {
        printf("error: set_sqw failed (%d)\n", res);
        return 1;
    }
    printf("SQW frequency set to %d\n", freq);
    return 0;
}

static int _cmd_trickle(int argc, char **argv)
{
    if (argc == 2 && argv[1][0] == '0') {
        int res = max31343_trickle_charge_disable(&_dev);
        if (res != 0) {
            printf("error: set_trickle failed (%d)\n", res);
            return 1;
        }
        puts("trickle charger disabled");
        return 0;
    }

    if (argc != 3) {
        printf("usage: %s <diode 0|1> <res 0|2|3>   or   %s 0  (disable)\n",
               argv[0], argv[0]);
        puts("  diode: 0=Schottky only, 1=Diode+Schottky");
        puts("  res:   0=3kΩ  2=6kΩ  3=11kΩ");
        return 1;
    }

    int diode = atoi(argv[1]);
    int res_sel = atoi(argv[2]);
    if (diode < 0 || diode > 1) {
        puts("error: invalid diode (0 or 1)");
        return 1;
    }

    int res = max31343_trickle_charge_enable(&_dev,
                                           (max31343_trickle_diode_t)diode,
                                           (max31343_trickle_res_t)res_sel);
    if (res != 0) {
        printf("error: set_trickle failed (%d)\n", res);
        return 1;
    }
    puts("trickle charger enabled");
    return 0;
}

static int _cmd_automode(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: %s <0|1> <ttsint 0-7>\n", argv[0]);
        puts("  ttsint: 0=1s 1=2s 2=4s 3=8s 4=16s 5=32s 6=64s 7=128s");
        return 1;
    }

    bool en = (argv[1][0] == '1');
    int ttsint = atoi(argv[2]);
    if (ttsint < 0 || ttsint > 7) {
        puts("error: ttsint out of range (0-7)");
        return 1;
    }

    int res = max31343_temp_set_automode(&_dev, en, (max31343_ttsint_t)ttsint);
    if (res != 0) {
        printf("error: temp_set_automode failed (%d)\n", res);
        return 1;
    }
    printf("temp automode %s, ttsint=%d\n", en ? "enabled" : "disabled", ttsint);
    return 0;
}

static int _cmd_power(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <0|1>  (0=off, 1=on)\n", argv[0]);
        return 1;
    }

    int res;
    if (argv[1][0] == '1') {
        res = max31343_poweron(&_dev);
        if (res == 0) { puts("oscillator powered on"); }
    }
    else {
        res = max31343_poweroff(&_dev);
        if (res == 0) { puts("oscillator powered off"); }
    }

    if (res != 0) {
        printf("error: power command failed (%d)\n", res);
        return 1;
    }
    return 0;
}

static int _cmd_test(int argc, char **argv)
{
    (void)argc; (void)argv;

    struct tm t;
    int res;

    puts("[test] setting reference time...");

    res = max31343_set_time(&_dev, &_ref_time);
    if (res != 0) {
        printf("[test] FAIL: set_time returned %d\n", res);
        return 1;
    }

    /*
     * The MAX31343 only latches a written time at the next internal 1 Hz
     * tick boundary. Waiting two full second guarantees the new value is
     * committed before we read it back.
     */
    ztimer_sleep(ZTIMER_SEC, 2);

    puts("[test] reading back time...");
    res = max31343_get_time(&_dev, &t);
    if (res != 0) {
        printf("[test] FAIL: get_time returned %d\n", res);
        return 1;
    }

    /*
     * Use a local, mutable copy for mktime – _ref_time is const and mktime
     * writes back tm_wday / tm_yday / tm_isdst, which is undefined behaviour
     * on a const object (and a BusFault if it lives in flash).
     *
     * Expected window: [ref, ref + 3s]. The lower bound guards against the
     * chip returning a time before what we wrote; the upper bound gives slack
     * for the 1s latch delay plus two I2C round-trips.
     */
    struct tm ref_copy = _ref_time;
    ref_copy.tm_isdst  = 0;
    t.tm_isdst         = 0;
    time_t ref_t = mktime(&ref_copy);
    time_t got_t = mktime(&t);
    if (got_t < ref_t || got_t > ref_t + 50) {
        printf("[test] FAIL: time readback out of window "
               "(expected [%ld, %ld], got %ld)\n",
               (long)ref_t, (long)(ref_t + 3), (long)got_t);
        return 1;
    }
    puts("[test] time readback OK");

    puts("[test] waiting for clock to tick...");
    ztimer_sleep(ZTIMER_SEC, TEST_DELAY);

    res = max31343_get_time(&_dev, &t);
    if (res != 0) {
        printf("[test] FAIL: get_time returned %d\n", res);
        return 1;
    }
    if (!(mktime(&t) > mktime((struct tm *)&_ref_time))) {
        puts("[test] FAIL: time did not progress");
        return 1;
    }
    puts("[test] time progression OK");

    puts("[test] testing alarm set/get...");
    struct tm alarm_time;
    res = max31343_get_time(&_dev, &alarm_time);
    if (res != 0) {
        printf("[test] FAIL: get_time returned %d\n", res);
        return 1;
    }
    alarm_time.tm_sec += TEST_DELAY;
    mktime(&alarm_time);

    res = max31343_set_alarm(&_dev, &alarm_time);
    if (res != 0) {
        printf("[test] FAIL: set_alarm returned %d\n", res);
        return 1;
    }

    struct tm alarm_readback;
    res = max31343_get_alarm(&_dev, &alarm_readback);
    if (res != 0) {
        printf("[test] FAIL: get_alarm returned %d\n", res);
        return 1;
    }

    time_t t_readback = mktime(&alarm_readback);
    time_t t_alarm    = mktime(&alarm_time);
    time_t alarm_diff = (t_readback > t_alarm)
                        ? (t_readback - t_alarm)
                        : (t_alarm - t_readback);
    if (alarm_diff > 1) {
        printf("[test] FAIL: alarm readback mismatch (diff=%ld s)\n",
               (long)alarm_diff);
        return 1;
    }
    puts("[test] alarm set/get OK");

    puts("[test] testing alarm clear...");
    res = max31343_clear_alarm(&_dev);
    if (res != 0) {
        printf("[test] FAIL: clear_alarm returned %d\n", res);
        return 1;
    }
    puts("[test] alarm clear OK");

    puts("[test] testing temperature read...");
    int16_t centi;
    res = max31343_get_temp(&_dev, &centi);
    if (res != 0) {
        printf("[test] FAIL: get_temp_centi_c returned %d\n", res);
        return 1;
    }
    printf("[test] temperature: %d.%02d°C  OK\n",
           centi / 100, (centi < 0 ? -(centi % 100) : centi % 100));

    puts("[test] all tests PASSED");
    return 0;
}

static const shell_command_t _cmds[] = {
    { "time_get",     "read current time",                        _cmd_get         },
    { "time_set",     "set time (YYYY-MM-DDTHH:mm:ss)",           _cmd_set         },
    { "alarm_get",    "read configured alarm time",               _cmd_alarm_get   },
    { "alarm_set",    "set alarm time (YYYY-MM-DDTHH:mm:ss)",     _cmd_alarm_set   },
    { "alarm_enable", "enable/disable alarm interrupt (0|1)",     _cmd_alarm_enable},
    { "alarm_clear",  "disable alarm and clear flag",             _cmd_alarm_clear },
    { "temp",         "read temperature",                         _cmd_temp        },
    { "sqw",          "set SQW output frequency (0-5)",           _cmd_sqw         },
    { "trickle",      "configure trickle charger",                _cmd_trickle     },
    { "automode",     "configure temp automode and interval",     _cmd_automode    },
    { "power",        "oscillator power on/off (1|0)",            _cmd_power       },
    { "test",         "run built-in self-test",                   _cmd_test        },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MAX31343 RTC driver test\n");

    int res = max31343_init(&_dev, &max31343_params[0]);
    if (res == -ENODATA) {
        puts("warning: oscillator was stopped; time is invalid – please set it");
    }
    else if (res != 0) {
        printf("error: init failed (%d)\n", res);
        return 1;
    }
    else {
        puts("MAX31343 initialized OK");
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(_cmds, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}