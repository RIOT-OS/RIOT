/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the LTC4150 coulomb counter driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>

#include "fmt.h"
#include "fmt_table.h"
#include "led.h"
#include "ltc4150.h"
#include "thread.h"
#include "ztimer.h"
#include "timex.h"

typedef struct {
    uint64_t last_usec;
    uint64_t now_usec;
    ltc4150_dir_t dir;
} test_recorder_data_t;

static void pulse_cb(ltc4150_dev_t *, ltc4150_dir_t, uint64_t, void *);
static void reset_cb(ltc4150_dev_t *, uint64_t, void *);

static ltc4150_last_minute_data_t last_minute_data;
static test_recorder_data_t test_data;
static const ltc4150_recorder_t test_recorder = {
    .pulse = pulse_cb,
    .reset = reset_cb,
};
static kernel_pid_t target_pid;
static char busy_thread_stack[THREAD_STACKSIZE_DEFAULT];
static ltc4150_dev_t ltc4150;
static int change_of_load_level = 0;

static const ltc4150_recorder_t *recorders[] = {
    &ltc4150_last_minute,
    &test_recorder,
    NULL
};
static void *recorder_data[] = {
    &last_minute_data,
    &test_data,
};

#define LTC4150_PARAM_RECS            (recorders)
#define LTC4150_PARAM_RECDATA         (recorder_data)

#include "ltc4150_params.h"

/**
 * @brief Callback function to reset/initialize the recorder data
 */
static void reset_cb(ltc4150_dev_t *dev, uint64_t now_usec, void *_data)
{
    (void)dev;
    test_recorder_data_t *data = _data;
    data->last_usec = data->now_usec = now_usec;
    data->dir = LTC4150_DISCHARGE;
}

/**
 * @brief Callback function to record the current pulse
 */
static void pulse_cb(ltc4150_dev_t *dev, ltc4150_dir_t dir, uint64_t now_usec,
                     void *_data)
{
    (void)dev;
    static msg_t m = { .content = { .value = 0} };

    test_recorder_data_t *data = _data;
    data->last_usec = data->now_usec;
    data->now_usec = now_usec;
    data->dir = dir;

    msg_send(&m, target_pid);
}

/**
 * @brief Busy waits for the given amount of seconds
 * @param seconds    Number of seconds to roast the CPU
 */
static void spin(uint32_t seconds)
{
    ztimer_spin(ZTIMER_USEC, US_PER_SEC * seconds);
}

/**
 * @brief Thread that will put three levels of CPU load on the MCU
 */
static void *busy_thread(void *arg)
{
    (void)arg;
    while (1) {
        /* one minute of ~0% CPU usage */
        LED0_OFF;
        LED1_OFF;
        ztimer_sleep(ZTIMER_USEC, 60 * US_PER_SEC);
        change_of_load_level = 1;

        /* one minute of ~50% CPU usage */
        for (unsigned i = 0; i < 30; i++) {
            LED0_OFF;
            LED1_OFF;
            ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
            LED0_ON;
            LED1_ON;
            spin(1);
        }
        change_of_load_level = 1;

        /* one minute of 100% CPU usage */
        LED0_ON;
        LED1_ON;
        spin(60);
        change_of_load_level = 1;
    }

    /* unreachable */
    return NULL;
}

/**
 * @brief Print a table column with the given current as E-01
 * @param current   Value to print in the column (as E-01)
 * @param width     Width of the column
 */
static void print_current(int32_t current, size_t width)
{
    char sbuf[2];

    print_col_s32_dec(current/10, width - 2);
    sbuf[0] = '.';
    sbuf[1] = '0' + current % 10;
    print(sbuf, 2);
}

int main(void)
{
    target_pid = thread_getpid();
    uint32_t ten_uc_per_pulse;
    msg_t m;
    int retval;

    retval = ltc4150_init(&ltc4150, &ltc4150_params[0]);

    /* Pre-compute the charge corresponding to one pulse */
    ltc4150_pulses2c(&ltc4150, &ten_uc_per_pulse, NULL, 10000, 0);

    if (retval) {
        print_str("Failed to initialize LTC4150 driver:");
        switch (retval) {
            case -EINVAL:
                print_str("Invalid parameter\n");
                break;
            case -EIO:
                print_str("GPIO or interrupt configuration failed\n");
                break;
            default:
                print_str("Unknown (should no happen, file a bug)\n");
                break;
        }
        return -1;
    }

    /* Start the thread that will keep the MCU busy */
    thread_create(busy_thread_stack, sizeof(busy_thread_stack),
                  THREAD_PRIORITY_MAIN + 1, 0,
                  busy_thread, NULL, "busy_thread");

    print_str("This test will put three levels of load on the MCU:\n"
        " 1. One minute of little to no load (LEDs(*) off)\n"
        " 2. One minute of about 50% CPU load (LEDs(*) blinking)\n"
        " 3. One minute of 100% CPU load (LEDs(*) constantly on)\n"
        "\n"
        "    (*) LED0 and LED1, if present on your board\n"
        "\n"
        "During this time the charge drawn is measured and printed on every\n"
        "pulse the LTC4150 generates. A horizontal line in the table\n"
        "separates values of different load levels. On the MSB-A2 the\n"
        "expected result per column is:\n"
        "\n"
        "  Charging:     Should remain zero\n"
        "  Discharging:  Should increase for every pulse\n"
        "  Average:      Should be something between 60mA to 80mA\n"
        "  Last Minute:  Starts with 0 at boot up and is updated every 10s.\n"
        "                Should be higher for higher system load when looking\n"
        "                at the last update for each load level.\n"
        "                (Note: Not synchronized with load levels!)\n"
        "  Currently:    Should be higher for higher system load. Might be\n"
        "                \"jumpy\" on 50% load, as this implemented by having\n"
        "                one second of 100% load and one second of ~0% load\n"
        "                in turns.\n"
        "\n"
        "Hint: You'll want to look mostly at the rightmost column.\n"
        "Note: The test will repeat endlessly.\n");

    LED0_OFF;

    print_str("+-------------------------------+-----------------------------------+\n"
        "| Total Transferred Charge [mC] |   Current from Power Supply [mA]  |\n"
        "| Charging      | Discharging   | Average | Last Minute | Currently |\n"
        "+---------------+---------------+---------+-------------+-----------+\n");

    while (1) {
        /* Wait for the next pulse of the LTC4150 */
        msg_receive(&m);
        uint32_t charged, discharged;
        int16_t avg_current;
        int32_t current;

        if (change_of_load_level) {
            print_str("+---------------+---------------+---------+-------------+-----------+\n");
            change_of_load_level = 0;
        }

        /* Get & print total charge transferred */
        if (ltc4150_charge(&ltc4150, &charged, &discharged)) {
            print_str("ltc4150_charge() failed!\n");
            return -1;
        }
        print_str("| ");
        print_col_u32_dec(charged, 13);
        print_str(" | ");
        print_col_u32_dec(discharged, 13);
        print_str(" | ");

        /* Get & print avg current */
        if (ltc4150_avg_current(&ltc4150, &avg_current)) {
            print_str("ltc4150_avg_current() failed!\n");
            return -1;
        }
        print_current(avg_current, 7);
        print_str(" | ");

        /* Get & print last minute current */
        if (ltc4150_last_minute_charge(&ltc4150, &last_minute_data,
                                       &charged, &discharged)
            ) {
            print_str("ltc4150_last_minute_charge() failed!\n");
            return -1;
        }
        current = (int32_t)discharged - (int32_t)charged;
        current /= 60;
        print_col_s32_dec(current, 11);
        print_str(" | ");

        /* Calculate & print the current between the last two pulses */
        current = (int32_t)((test_data.now_usec - test_data.last_usec) / MS_PER_SEC);
        current = ten_uc_per_pulse / current;
        if (test_data.dir == LTC4150_CHARGE) {
            current = -current;
        }
        print_current(current, 9);
        print_str(" |\n");
    }

    return 0;
}
