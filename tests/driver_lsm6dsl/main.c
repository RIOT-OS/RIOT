/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the LSM6DSL accelerometer/gyroscope driver
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Sylvain Fabre <sylvain.fabre-ext@continental-its.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "shell_commands.h"
#include "xtimer.h"
#include "lsm6dsl.h"
#include "lsm6dsl_params.h"

#define SLEEP_USEC  (500UL * US_PER_MS)

static lsm6dsl_t dev;

static void alert_cb(void *arg)
{
    printf("\n[ Alert : %s]\n", (char*) arg);
}

static int _lsm6dsl_cfg_wkup_int_handler(int argc, char** argv)
{
    if (argc < 5 ) {
        printf("Usage: %s <slope_fds> <wk_ths> <wake_dur> <sleep_dur>\n",
                 argv[0]);
        return 1;
    }

    lsm6dsl_wkup_params_t wkup_param = { .slope_fds = atoi(argv[1]),
                                         .wk_ths = atoi(argv[2]),
                                         .wake_dur = atoi(argv[3]),
                                         .sleep_dur = atoi(argv[4])};
    if (lsm6dsl_acc_configure_wkup_alert(&dev, &wkup_param) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");
    return 0;
}

static int _lsm6dsl_cfg_ff_int_handler(int argc, char** argv)
{
    if (argc < 3 ) {
        printf("Usage: %s <ff_dur> <ff_ths>\n", argv[0]);
        return 1;
    }

    lsm6dsl_wkup_params_t wkup_param = { .ff_dur = atoi(argv[1]),
                                         .ff_ths = atoi(argv[2])};
    if (lsm6dsl_acc_configure_wkup_alert(&dev, &wkup_param) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");
    return 0;
}

static int _lsm6dsl_cfg_6d4d_int_handler(int argc, char** argv)
{
    if (argc < 3 ) {
        printf("Usage: %s <d4d_en> <sixd_ths> <low_pass_on_6d>\n", argv[0]);
        return 1;
    }

    lsm6dsl_6d4d_params_t param_6d4d = { .d4d_en = atoi(argv[1]),
                                         .sixd_ths = atoi(argv[2]),
                                         .low_pass_on_6d = atoi(argv[3])};
    if (lsm6dsl_acc_configure_6D4D_alert(&dev, &param_6d4d) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");
    return 0;
}

static int _lsm6dsl_cfg_tap_int_handler(int argc, char** argv)
{
    if (argc < 6 ) {
        printf("Usage: %s <tap_ths> <shock> <quiet> <duration> <dbl_tap>\n",
                argv[0]);
        return 1;
    }

    lsm6dsl_tap_params_t tap_param = { .tap_ths = atoi(argv[1]),
                                         .shock = atoi(argv[2]),
                                         .quiet = atoi(argv[3]),
                                         .duration = atoi(argv[4]),
                                         .dbl_tap = atoi(argv[5])};
    if (lsm6dsl_acc_configure_tap_alert(&dev, &tap_param) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");
    return 0;
}

static int _lsm6dsl_cfg_ofs_xyz_handler(int argc, char** argv)
{
    if (argc < 4 ) {
        printf("Usage: %s <X-offset> <Y-offset> <Z-offset> \n", argv[0]);
        return 1;
    }

    if (lsm6dsl_acc_set_x_offset(&dev, atoi(argv[1])) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }

    if (lsm6dsl_acc_set_y_offset(&dev, atoi(argv[1])) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }

    if (lsm6dsl_acc_set_z_offset(&dev, atoi(argv[1])) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    return 0;
}

static int _lsm6dsl_cfg_ofs_x_handler(int argc, char** argv)
{
    if (argc < 2 ) {
        printf("Usage: %s <X-offset>\n", argv[0]);
        return 1;
    }

    if (lsm6dsl_acc_set_x_offset(&dev, atoi(argv[1])) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    return 0;
}

static int _lsm6dsl_cfg_ofs_y_handler(int argc, char** argv)
{
    if (argc < 2 ) {
        printf("Usage: %s <Y-offset>\n", argv[0]);
        return 1;
    }

    if (lsm6dsl_acc_set_y_offset(&dev, atoi(argv[1])) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    return 0;
}

static int _lsm6dsl_cfg_ofs_z_handler(int argc, char** argv)
{
    if (argc < 2 ) {
        printf("Usage: %s <Z-offset>\n", argv[0]);
        return 1;
    }

    if (lsm6dsl_acc_set_z_offset(&dev, atoi(argv[1])) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    return 0;
}

static void _usage_configure(void)
{
    printf("Usage: configure <wkup_int | sleep_int | 6d4d_int | ff_int | " \
                "tap_int | ofs_xyz | ofs_x | ofs_y | ofs_z>\n");
    puts(" * wkup_int & sleep_int configuration layout:");
    puts("     uint8_t slope_fds;  /**< 0 slope filter or 1 high-pass " \
                                    "digital filter */");
    puts("     uint8_t wk_ths;     /**< [5:0] threshold for wake up " \
                                            "(1 LSB = FS_XL/(2^6)) */");
    puts("     uint8_t wake_dur;   /**< [1:0] Wake up duration event " \
                                        "(1LSB = 1 ODR_time) */");
    puts("     uint8_t sleep_dur;  /**< [3:0] sleep duration before event " \
                                        "(1LSB = 512/ ODR_XL) */");
    puts("");
    puts(" * wkup_int configuration layout:");
    puts("     uint8_t ff_dur;     /**< Free fall duration event */");
    puts("     uint8_t ff_ths;     /**< Free fall threshold */");
    puts("");
    puts(" * 6d4d configuration layout:");
    puts("     uint8_t d4d_en;         /**< 1 to enable 4D orientation */");
    puts("     uint8_t sixd_ths;       /**< Threshold for 4D/6D detection */");
    puts("     uint8_t low_pass_on_6d; /**< use low pass filter */");
    puts("");
    puts(" * tap configuration layout:");
    puts("     uint8_t tap_ths;        /**< [4:0] threshold for tap detection" \
                                            "(1 LSB = FS_XL/(2^5)) */");
    puts("     uint8_t shock;          /**< [1:0] shock time window " \
                                            "(1LSB = 8/ODR_XL) */");
    puts("     uint8_t quiet;          /**< [1:0] quiet time " \
                                            "(1LSB = 4/ODR_XL) */");
    puts("     uint8_t duration        /**< [3:0] max duration between two " \
                                            "taps (1LSB = 32/ODR_XL) */");
    puts("     uint8_t dbl_tap;        /**< 1: double tap, 0: single tap */");
    puts("    Offsets : max value is 127, min value is -127, 1 LSB = 16 " \
                        "(1/ 2^6  rounded)");
    puts("      X-axis  : Value added, */");
    puts("      Y-axis  : Value added */");
    puts("      Z-axis  : Value subtracted */");
    puts("");
}

static int _lsm6dsl_configure_handler(int argc, char **argv)
{
    if (argc < 2) {
        _usage_configure();
        return 1;
    }
    if (strcmp(argv[1], "wkup_int") == 0 || strcmp(argv[1], "sleep_int")
         == 0) {
        return _lsm6dsl_cfg_wkup_int_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "ff_int") == 0) {
        return _lsm6dsl_cfg_ff_int_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "6d4d_int") == 0) {
        return _lsm6dsl_cfg_6d4d_int_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "tap_int") == 0) {
        return _lsm6dsl_cfg_tap_int_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "ofs_xyz") == 0) {
        return _lsm6dsl_cfg_ofs_xyz_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "ofs_x") == 0) {
        return _lsm6dsl_cfg_ofs_x_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "ofs_y") == 0) {
        return _lsm6dsl_cfg_ofs_y_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "ofs_z") == 0) {
        return _lsm6dsl_cfg_ofs_z_handler(argc -1, &argv[1]);
    }
    else {
        _usage_configure();
    }
    return 0;
}

static void _usage_enable(void)
{
    printf("Usage: enable <wkup_int | sleep_int | 6d4d_int | ff_int | " \
            "tap1_int | tap2_int> <pin> \n");
    puts("");
}

static int _lsm6dsl_enable_handler(int argc, char **argv)
{
    if (argc < 3) {
        _usage_enable();
        return 1;
    }
    if (strcmp(argv[1], "wkup_int") == 0) {
        if (lsm6dsl_acc_enable_alert(&dev, LSM6DSL_INT_WAKE_UP, atoi(argv[2]),
            alert_cb, "WakeUp") != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
        puts(" To trig the alert, move the device ...");
    }
    else if (strcmp(argv[1], "sleep_int") == 0) {
        if (lsm6dsl_acc_enable_alert(&dev, LSM6DSL_INT_SLEEP, atoi(argv[2]),
            alert_cb, "Sleep") != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
        puts(" To trig the alert, move the device and stop, wait configured " \
                "sleep duration");
    }
    else if (strcmp(argv[1], "ff_int") == 0) {
        if (lsm6dsl_acc_enable_alert(&dev, LSM6DSL_INT_FREE_FALL,
            atoi(argv[2]), alert_cb, "FreeFall") != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
        puts(" To trig the alert, release your device at the right height " \
                "to ger a free-fall...");
    }
    else if (strcmp(argv[1], "6d4d_int") == 0) {
        if (lsm6dsl_acc_enable_alert(&dev, LSM6DSL_INT_6D_4D, atoi(argv[2]),
            alert_cb, "6D4D") != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
        puts(" To trig the alert, change the device orientation/inclination");
    }
    else if (strcmp(argv[1], "tap1_int") == 0) {
        if (lsm6dsl_acc_enable_alert(&dev, LSM6DSL_INT_SINGLE_TAP,
            atoi(argv[2]), alert_cb, "SingleTAP") != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
        puts(" To trig the alert, single/double tap on your device");
    }
    else if (strcmp(argv[1], "tap2_int") == 0) {
        if (lsm6dsl_acc_enable_alert(&dev, LSM6DSL_INT_SINGLE_TAP,
            atoi(argv[2]), alert_cb, "DoubleTAP") != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
        puts(" To trig the alert, single/double tap on your device");
    }
    else {
        _usage_enable();
        return 1;
    }

    return 0;
}

static void _usage_disable(void)
{
    printf("Usage: disable <wkup_int | sleep_int | 6d4d_int | ff_int | " \
            "tap1_int | tap2_int> <pin> \n");
    puts("");
}

static int _lsm6dsl_disable_handler(int argc, char **argv)
{
    if (argc < 3) {
        _usage_disable();
        return 1;
    }
    if (strcmp(argv[1], "wkup_int") == 0) {
        if (lsm6dsl_acc_disable_alert(&dev, LSM6DSL_INT_WAKE_UP, atoi(argv[2]))
            != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
    }
    else if (strcmp(argv[1], "sleep_int") == 0) {
        if (lsm6dsl_acc_disable_alert(&dev, LSM6DSL_INT_SLEEP, atoi(argv[2]))
            != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
    }
    else if (strcmp(argv[1], "ff_int") == 0) {
        if (lsm6dsl_acc_disable_alert(&dev, LSM6DSL_INT_FREE_FALL,
            atoi(argv[2]))
            != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
    }
    else if (strcmp(argv[1], "6d4d_int") == 0) {
        if (lsm6dsl_acc_disable_alert(&dev, LSM6DSL_INT_6D_4D, atoi(argv[2]))
                != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
    }
    else if (strcmp(argv[1], "tap1_int") == 0) {
        if (lsm6dsl_acc_disable_alert(&dev, LSM6DSL_INT_SINGLE_TAP,
            atoi(argv[2]))
            != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
    }
    else if (strcmp(argv[1], "tap2_int") == 0) {
        if (lsm6dsl_acc_disable_alert(&dev, LSM6DSL_INT_SINGLE_TAP,
            atoi(argv[2]))
            != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        puts("[SUCCESS]\n");
    }
    else {
        _usage_enable();
        return 1;
    }

    return 0;
}

static void _usage_get_reason(void)
{
    printf("Usage: get_reason <wkup | sleep | 6d4d | ff | tap>");
    puts("");
}

static int _lsm6dsl_get_reason_handler(int argc, char **argv)
{
    if (argc < 2) {
        _usage_get_reason();
        return 1;
    }
    if (strcmp(argv[1], "wkup") == 0 || strcmp(argv[1], "sleep") == 0) {
        lsm6dsl_wkup_src_t wkup;
        if (lsm6dsl_acc_get_wkup_src_alert(&dev, &wkup, false) != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }

        printf(" Wake up reason :\n");
        printf(" ACTIVE :   %d\n", wkup.active);
        printf(" INACTIVE : %d\n", wkup.inactive);
        printf(" X AXIS :   %d\n", wkup.x);
        printf(" Y AXIS :   %d\n", wkup.y);
        printf(" Z AXIS :   %d\n", wkup.z);
    }
    else if (strcmp(argv[1], "ff") == 0) {
        lsm6dsl_wkup_src_t ff;
        if (lsm6dsl_acc_get_wkup_src_alert(&dev, &ff, false) != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        printf(" FREE FALL :   %d", ff.free_fall);
    }
    else if (strcmp(argv[1], "6d4d") == 0) {
        lsm6dsl_6d4d_src_t d6d4;
        if (lsm6dsl_acc_get_6D4D_src_alert(&dev, &d6d4, false) != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        printf(" 6d4d reason :\n");
        printf(" X POS:   %d\n", d6d4.xh);
        printf(" X NEG:   %d\n", d6d4.xl);
        printf(" Y POS:   %d\n", d6d4.yh);
        printf(" Y NEG:   %d\n", d6d4.yl);
        printf(" Z POS:   %d\n", d6d4.zh);
        printf(" Z NEG:   %d\n", d6d4.zl);
    }
    else if (strcmp(argv[1], "tap") == 0) {
        lsm6dsl_tap_src_t tap;
        if (lsm6dsl_acc_get_tap_src_alert(&dev, &tap, false) != LSM6DSL_OK) {
            puts("[ERROR]");
            return 1;
        }
        printf(" Tap reason :\n");
        printf(" DOUBLE TAP:   %d\n", tap.double_tap);
        printf(" SINGLE TAP:   %d\n", tap.single_tap);
        printf(" TAP ACTIVITY: %d\n", tap.tap_ia);
        printf(" TAP SIGN:     %d\n", tap.tap_sign);
        printf(" X TAP:        %d\n", tap.x_tap);
        printf(" Y_TAP:        %d\n", tap.y_tap);
        printf(" Z_TAP:        %d\n", tap.z_tap);
    }
    else {
        _usage_get_reason();
        return 1;
    }

    return 0;
}

static void _usage_read_reason(void)
{
    printf("Usage: read <acc | gyro | temp>");
    puts("");
}

static void _read_gyro(void) {

    lsm6dsl_3d_data_t mag_value;
    if (lsm6dsl_read_gyro(&dev, &mag_value) == LSM6DSL_OK) {
        printf("Gyroscope x: %i y: %i z: %i\n", mag_value.x,
                                                mag_value.y,
                                                mag_value.z);
    }
    else {
        puts("[ERROR] reading gyroscope!\n");
    }
}

static void _read_temp(void)
{
    int16_t temp_value;
    if (lsm6dsl_read_temp(&dev, &temp_value) == LSM6DSL_OK) {
        printf("Temperature [in °C x 100]: %i \n", temp_value);
    }
    else {
        puts("[ERROR] reading temperature!\n");
    }
}

static void _read_acc(void)
{
    lsm6dsl_3d_data_t acc_value;
    if (lsm6dsl_read_acc(&dev, &acc_value) == LSM6DSL_OK) {
        printf("Accelerometer x: %i y: %i z: %i\n", acc_value.x,
                                                    acc_value.y,
                                                    acc_value.z);
    }
    else {
        puts("[ERROR] reading accelerometer!\n");
    }
}

static int _lsm6dsl_get_read_handler(int argc, char **argv)
{
    if (argc < 2) {
        _usage_read_reason();
        return 1;
    }
    if (strcmp(argv[1], "acc") == 0) {
        _read_acc();
        return 0;
    }
    else if (strcmp(argv[1], "gyro") == 0) {
        _read_gyro();
        return 0;
    }
    else if (strcmp(argv[1], "temp") == 0) {
        _read_temp();
        return 0;
    }
    else {
        _usage_read_reason();
    }
    return 0;
}

static void _usage(void)
{
    puts("Usage:");
    puts("  lsm6dsl help");
    puts("  lsm6dsl configure");
    puts("  lsm6dsl enable");
    puts("  lsm6dsl disable");
    puts("  lsm6dsl get_reason");
    puts("  lsm6dsl read");

}
static int lsm6dsl_shell_handler(int argc, char **argv)
{
    if (argc < 2) {
        _usage();
        return 1;
    }

    if (strcmp(argv[1], "help") == 0) {
        _usage();
        return 0;
    }
    else if (strcmp(argv[1], "configure") == 0) {
        return _lsm6dsl_configure_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "enable") == 0) {
        return _lsm6dsl_enable_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "disable") == 0) {
        return _lsm6dsl_disable_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "get_reason") == 0) {
        return _lsm6dsl_get_reason_handler(argc -1, &argv[1]);
    }
    else if (strcmp(argv[1], "read") == 0) {
        return _lsm6dsl_get_read_handler(argc -1, &argv[1]);
    }
    else {
        _usage();
    }
    return 1;
}
static const shell_command_t shell_commands[] = {
    { "lsm6dsl", "lsm6dsl commands ", lsm6dsl_shell_handler},
    { NULL, NULL, NULL }
};

int main(void)
{

    puts("LSM6DSL test application");
    printf("Initializing LSM6DSL sensor at I2C_%i... ", lsm6dsl_params->i2c);

    if (lsm6dsl_init(&dev, lsm6dsl_params) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    puts("Powering down LSM6DSL sensor...");
    if (lsm6dsl_acc_power_down(&dev) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    if (lsm6dsl_gyro_power_down(&dev) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    xtimer_sleep(1);

    puts("Powering up LSM6DSL sensor...");
    if (lsm6dsl_acc_power_up(&dev) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    if (lsm6dsl_gyro_power_up(&dev) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    puts("Latch LSM6DSL interrupt... ");
    if (lsm6dsl_enable_latched_interrupt(&dev) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    puts("Unlatch LSM6DSL interrupt... ");
    if (lsm6dsl_disable_latched_interrupt(&dev) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    _read_acc();
    _read_gyro();
    _read_temp();

    /* start the shell */
    puts("First tests successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
