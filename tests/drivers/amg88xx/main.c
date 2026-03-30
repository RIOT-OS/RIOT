/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       AMG88xx driver test
 *
 * Interactive test application for the AMG88xx infrared array sensor.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitfield.h"
#include "shell.h"
#include "thread.h"
#include "ztimer.h"

#include "amg88xx.h"
#include "amg88xx_params.h"

/**
 * @brief   Default polling interval in milliseconds
 */
#define AMG88XX_DEFAULT_POLL_MS     (1000)

/**
 * @brief   Device descriptor for the AMG88xx driver
 */
static amg88xx_t _dev;

/**
 * @brief   Flag to enable/disable grid display of pixel values
 */
static bool _grid_enabled = true;

/**
 * @brief   Flag to display grid in raw values (true) or temperature (false)
 */
static bool _grid_raw = true;

#if IS_USED(MODULE_PERIPH_GPIO_IRQ)
static kernel_pid_t _main_pid;

static void _int_cb(void *arg)
{
    (void)arg;

    msg_t msg;
    msg_try_send(&msg, _main_pid);
}
#endif

static void _print_grid(const int16_t *pixels, const uint8_t *int_table)
{
    const unsigned rows = AMG88XX_PIXELS_VERTICAL;
    const unsigned cols = AMG88XX_PIXELS_HORIZONTAL;

    puts("+--------+--------+--------+--------+--------+--------+--------+--------+");

    for (unsigned row = 0; row < rows; row++) {
        for (unsigned col = 0; col < cols; col++) {
            unsigned index = (row * cols) + col;
            bool flagged = (int_table != NULL && bf_isset(int_table, index));

            if (_grid_raw) {
                if (flagged) {
                    printf("|(%6" PRId16 ")", pixels[index]);
                }
                else {
                    printf("| %6" PRId16 " ", pixels[index]);
                }
            }
            else {
                int16_t temp = amg88xx_raw_to_temperature(pixels[index]);
                int16_t whole = (int16_t)(temp / 100);
                int16_t frac = (int16_t)(abs(temp % 100));

                if (flagged) {
                    printf("|(%3" PRId16 ".%02" PRId16 ")", whole, frac);
                }
                else {
                    printf("| %3" PRId16 ".%02" PRId16 " ", whole, frac);
                }
            }
        }

        puts("|");
        puts("+--------+--------+--------+--------+--------+--------+--------+--------+");
    }
}

static int _cmd_read(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int16_t pixels[AMG88XX_PIXELS_COUNT];

    if (amg88xx_get_frame(&_dev, pixels) != 0) {
        puts("error: could not get a frame");
        return 1;
    }

    if (IS_USED(MODULE_DISP_DEV)) {
        extern void display_update(const int16_t *pixels);
        display_update(pixels);
    }

    if (_grid_enabled) {
        _print_grid(pixels, NULL);
    }

    return 0;
}

static int _cmd_temp(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int16_t temp;

    if (amg88xx_get_temperature(&_dev, &temp) != 0) {
        puts("error: could not get temperature");
        return 1;
    }

    printf("Thermistor: %" PRId16 ".%02" PRId16 " deg C\n",
           (int16_t)(temp / 100), (int16_t)abs(temp % 100));

    return 0;
}

static int _cmd_poll(int argc, char **argv)
{
    unsigned interval = AMG88XX_DEFAULT_POLL_MS;

    if (argc >= 2) {
        interval = atoi(argv[1]);
        if (interval == 0) {
            printf("usage: %s <interval>\n", argv[0]);
            return 1;
        }
    }

    int16_t pixels[AMG88XX_PIXELS_COUNT];

    printf("Polling every %u ms...\n", interval);

    while (1) {
        if (amg88xx_get_frame(&_dev, pixels) != 0) {
            puts("error: could not get a frame");
            return 1;
        }

        if (IS_USED(MODULE_DISP_DEV)) {
            extern void display_update(const int16_t *pixels);
            display_update(pixels);
        }

        if (_grid_enabled) {
            _print_grid(pixels, NULL);
        }

        ztimer_sleep(ZTIMER_MSEC, interval);
    }

    return 0;
}

static int _cmd_fps(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <1|10>\n", argv[0]);
        return 1;
    }

    int rate = atoi(argv[1]);
    amg88xx_fps_t fps;

    switch (rate) {
    case 1:
        fps = AMG88XX_FPS_1;
        break;
    case 10:
        fps = AMG88XX_FPS_10;
        break;
    default:
        puts("error: FPS must be 1 or 10");
        return 1;
    }

    if (amg88xx_set_fps(&_dev, fps) != 0) {
        puts("error: could not set frame rate");
        return 1;
    }

    printf("Frame rate set to %d FPS\n", rate);

    return 0;
}

#if IS_USED(MODULE_PERIPH_GPIO_IRQ)
static int _cmd_interrupt(int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: %s <upper> <lower> <hysteresis>\n", argv[0]);
        puts("  values in raw units (0.25 deg C per LSB)");
        puts("  e.g.: interrupt 100 40 8 (25.0 / 10.0 / 2.0 deg C)");
        return 1;
    }

    if (!gpio_is_valid(_dev.params.int_pin)) {
        puts("error: no interrupt pin configured");
        return 1;
    }

    int16_t upper = atoi(argv[1]);
    int16_t lower = atoi(argv[2]);
    int16_t hyst = atoi(argv[3]);

    _main_pid = thread_getpid();

    if (amg88xx_init_int(&_dev, _int_cb, NULL) != 0) {
        puts("error: could not initialize interrupt pin");
        return 1;
    }

    if (amg88xx_set_interrupt_levels(&_dev, upper, lower, hyst) != 0) {
        puts("error: could not set interrupt levels");
        return 1;
    }

    if (amg88xx_set_interrupt(&_dev, AMG88XX_INT_ABSOLUTE, true) != 0) {
        puts("error: could not enable interrupt");
        return 1;
    }

    int16_t upper_t = amg88xx_raw_to_temperature(upper);
    int16_t lower_t = amg88xx_raw_to_temperature(lower);

    printf("Interrupt enabled:"
           " upper=%" PRId16 " (%" PRId16 ".%02" PRId16 " deg C)"
           " lower=%" PRId16 " (%" PRId16 ".%02" PRId16 " deg C)"
           " hyst=%" PRId16 " (raw)\n",
           upper, (int16_t)(upper_t / 100), (int16_t)(abs(upper_t % 100)),
           lower, (int16_t)(lower_t / 100), (int16_t)(abs(lower_t % 100)),
           hyst);

    puts("Waiting for interrupts...\n");

    int16_t pixels[AMG88XX_PIXELS_COUNT];
    uint8_t int_table[AMG88XX_PIXELS_VERTICAL];

    while (1) {
        msg_t msg;
        msg_receive(&msg);

        puts("*** Interrupt triggered! ***\n");

        if (amg88xx_get_interrupt_table(&_dev, int_table) != 0) {
            puts("error: could not read interrupt table");
            return 1;
        }

        if (amg88xx_get_frame(&_dev, pixels) != 0) {
            puts("error: could not get a frame");
            return 1;
        }

        if (IS_USED(MODULE_DISP_DEV)) {
            extern void display_update(const int16_t *pixels);
            display_update(pixels);
        }

        if (_grid_enabled) {
            _print_grid(pixels, int_table);
        }

        if (amg88xx_clear_status(&_dev) != 0) {
            puts("error: could not clear interrupt status");
            return 1;
        }
    }

    return 0;
}
#endif

static int _cmd_mode(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <normal|sleep|standby60|standby10>\n", argv[0]);
        return 1;
    }

    amg88xx_mode_t mode;

    if (strcmp(argv[1], "normal") == 0) {
        mode = AMG88XX_MODE_NORMAL;
    }
    else if (strcmp(argv[1], "sleep") == 0) {
        mode = AMG88XX_MODE_SLEEP;
    }
    else if (strcmp(argv[1], "standby60") == 0) {
        mode = AMG88XX_MODE_STANDBY_60S;
    }
    else if (strcmp(argv[1], "standby10") == 0) {
        mode = AMG88XX_MODE_STANDBY_10S;
    }
    else {
        printf("usage: %s <normal|sleep|standby60|standby10>\n", argv[0]);
        return 1;
    }

    if (amg88xx_set_mode(&_dev, mode) != 0) {
        puts("error: could not set mode");
        return 1;
    }

    printf("Mode set to %s\n", argv[1]);

    return 0;
}

static int _cmd_grid(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off|raw|temp>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "on") == 0) {
        _grid_enabled = true;
        puts("Grid output enabled");
    }
    else if (strcmp(argv[1], "off") == 0) {
        _grid_enabled = false;
        puts("Grid output disabled");
    }
    else if (strcmp(argv[1], "raw") == 0) {
        _grid_raw = true;
        puts("Grid output set to raw");
    }
    else if (strcmp(argv[1], "temp") == 0) {
        _grid_raw = false;
        puts("Grid output set to temperature");
    }
    else {
        printf("usage: %s <on|off|raw|temp>\n", argv[0]);
        return 1;
    }

    return 0;
}

static int _cmd_averaging(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off>\n", argv[0]);
        return 1;
    }

    bool enabled;

    if (strcmp(argv[1], "on") == 0) {
        enabled = true;
    }
    else if (strcmp(argv[1], "off") == 0) {
        enabled = false;
    }
    else {
        printf("usage: %s <on|off>\n", argv[0]);
        return 1;
    }

    if (amg88xx_set_averaging(&_dev, enabled) != 0) {
        puts("error: could not set averaging mode");
        return 1;
    }

    printf("Averaging %s\n", enabled ? "enabled" : "disabled");

    return 0;
}

static int _cmd_dump(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    amg88xx_mode_t mode;
    amg88xx_fps_t fps;
    amg88xx_interrupt_mode_t int_mode;
    bool int_enabled;
    int16_t upper, lower, hyst;
    uint8_t stat;
    bool averaging;

    if (amg88xx_get_mode(&_dev, &mode) != 0) {
        puts("error: could not read mode");
        return 1;
    }

    if (amg88xx_get_fps(&_dev, &fps) != 0) {
        puts("error: could not read FPS");
        return 1;
    }

    if (amg88xx_get_interrupt(&_dev, &int_mode, &int_enabled) != 0) {
        puts("error: could not read interrupt config");
        return 1;
    }

    if (amg88xx_get_interrupt_levels(&_dev, &upper, &lower, &hyst) != 0) {
        puts("error: could not read interrupt levels");
        return 1;
    }

    if (amg88xx_get_status(&_dev, &stat) != 0) {
        puts("error: could not read status register");
        return 1;
    }

    if (amg88xx_get_averaging(&_dev, &averaging) != 0) {
        puts("error: could not read averaging mode");
        return 1;
    }

    const char *mode_str;

    switch (mode) {
    case AMG88XX_MODE_NORMAL:
        mode_str = "normal";
        break;
    case AMG88XX_MODE_SLEEP:
        mode_str = "sleep";
        break;
    case AMG88XX_MODE_STANDBY_60S:
        mode_str = "standby60";
        break;
    case AMG88XX_MODE_STANDBY_10S:
        mode_str = "standby10";
        break;
    default:
        mode_str = "unknown";
        break;
    }

    printf("Mode:      %s\n", mode_str);
    printf("FPS:       %s\n", (fps == AMG88XX_FPS_1) ? "1" : "10");
    printf("Averaging: %s\n", averaging ? "on" : "off");

    printf("Interrupt: %s", int_enabled ? "enabled" : "disabled");

    if (int_enabled) {
        printf(" (%s mode)",
               (int_mode == AMG88XX_INT_ABSOLUTE) ? "absolute" : "difference");
    }

    puts("");

    printf("Levels:    upper=%" PRId16 " lower=%" PRId16
           " hyst=%" PRId16 "\n", upper, lower, hyst);

    printf("Flags:     INT=%s OVF_THS=%s OVF_IRS=%s\n",
           (stat & AMG88XX_STAT_INTF) ? "yes" : "no",
           (stat & AMG88XX_STAT_OVF_THS) ? "yes" : "no",
           (stat & AMG88XX_STAT_OVF_IRS) ? "yes" : "no");

    return 0;
}

SHELL_COMMAND(averaging, "toggle averaging mode (on|off)", _cmd_averaging);
SHELL_COMMAND(dump, "dump sensor registers", _cmd_dump);
SHELL_COMMAND(fps, "set frame rate (1|10)", _cmd_fps);
SHELL_COMMAND(grid, "set grid output (on|off|raw|temp)", _cmd_grid);
SHELL_COMMAND(mode, "set power mode (normal|sleep|standby60|standby10)", _cmd_mode);
SHELL_COMMAND(poll, "continuously poll frames (interval)", _cmd_poll);
SHELL_COMMAND(read, "read a single frame", _cmd_read);
SHELL_COMMAND(temp, "read the thermistor temperature", _cmd_temp);

#if IS_USED(MODULE_PERIPH_GPIO_IRQ)
SHELL_COMMAND(interrupt, "wait for threshold interrupts", _cmd_interrupt);
#endif

int main(void)
{
    puts("Initializing AMG88xx sensor...");

    if (amg88xx_init(&_dev, &amg88xx_params[0]) != 0) {
        puts("[FAILED]");
        return 1;
    }

    puts("[OK]");

    /* spawn a shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
