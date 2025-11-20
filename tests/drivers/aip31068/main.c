/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @brief       Test application for the AIP31068 I2C LCD controller
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xtimer.h"
#include "shell.h"

#include "aip31068.h"
#include "aip31068_params.h"

#define ROW_COUNT   2
#define COL_COUNT   16

/* font is either 5x8 or 5x10, so always 5 columns per character */
#define PIXEL_COLUMNS_PER_CHAR  5

static aip31068_t aip31068_dev;

static const uint8_t custom_char_heart[] =
        { 0, 0, 10, 31, 31, 14, 4, 0 };

static const uint8_t custom_char_progress_bar_1[] =
        { 16, 16, 16, 16, 16, 16, 16, 16 };

static const uint8_t custom_char_progress_bar_2[] =
        { 24, 24, 24, 24, 24, 24, 24, 24 };

static const uint8_t custom_char_progress_bar_3[] =
        { 28, 28, 28, 28, 28, 28, 28, 28 };

static const uint8_t custom_char_progress_bar_4[] =
        { 30, 30, 30, 30, 30, 30, 30, 30 };

static const uint8_t custom_char_progress_bar_5[] =
        { 31, 31, 31, 31, 31, 31, 31, 31 };

static bool _progress_bar_enabled;
static uint8_t _progress_bar_row;

/**
 * @brief Initialize controller for progress bar feature.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 * @param[in] row   Row where the progress bar is displayed
 */
static void _init_progress_bar(aip31068_t* dev, uint8_t row);

/**
 * @brief Enable or disable the progress bar.
 *
 * When enabled, the last five custom symbols are reserved to display the
 * progress bar (CUSTOM_SYMBOL_4 to CUSTOM_SYMBOL_8) and can't be used.
 * Assignments via setCustomSymbol() to these keys will be ignored. The given
 * line will be reserved completely for the progress bar. Any text written to
 * that line will be overwritten by the progress bar on an update.
 *
 * @note: Auto scroll will be disabled and the display will be scrolled to
 *        its original position. Don't use scrolling when using the
 *        progressbar, otherwise it won't display correctly.
 *
 * @note: Text insertion mode will be set to LEFT_TO_RIGHT.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] enabled   Enable or disable
 */
static void _set_progress_bar_enabled(aip31068_t *dev, bool enabled);

/**
 * @brief Set the row for displaying the progress bar.
 *
 * Defaults to the last row, according to the given row count in the constructor.
 *
 * @param[in] row   Row where the progress bar is displayed
 */
static void _set_progress_bar_row(uint8_t row);

/**
 * @brief Set the progress of the progress bar and draw the update.
 *
 * @note: This function changes the cursor position. You will have to use
 *        setCursorPosition in order to return to your required cursor position.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] progress  Progress in percentage (0 to 100)
 */
static void _set_progress(aip31068_t *dev, uint8_t progress);

static int _turn_on(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    aip31068_turn_on(&aip31068_dev);
    return 0;
}

static int _turn_off(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    aip31068_turn_off(&aip31068_dev);
    return 0;
}

static int _clear(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    aip31068_clear(&aip31068_dev);
    return 0;
}

static int _home(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    aip31068_return_home(&aip31068_dev);
    return 0;
}

static int _autoscroll(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: autoscroll <0 or 1>");
        return 1;
    }
    else {
        uint8_t enabled = atoi(argv[1]);
        aip31068_set_auto_scroll_enabled(&aip31068_dev, enabled);
    }

    return 0;
}

static int _cursor_blinking(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: cursor_blinking <0 or 1>");
        return 1;
    }
    else {
        uint8_t enabled = atoi(argv[1]);
        aip31068_set_cursor_blinking_enabled(&aip31068_dev, enabled);
    }

    return 0;
}

static int _cursor_visible(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: cursor_visible <0 or 1>");
        return 1;
    }
    else {
        uint8_t enabled = atoi(argv[1]);
        aip31068_set_cursor_visible(&aip31068_dev, enabled);
    }

    return 0;
}

static int _cursor_position(int argc, char **argv)
{
    if (argc != 3) {
        puts("usage: cursor_position <row (e.g. 0 for first row)> <column (e.g. 0 for first column)>");
        return 1;
    }
    else {
        uint8_t row = atoi(argv[1]);
        uint8_t column = atoi(argv[2]);

        aip31068_set_cursor_position(&aip31068_dev, row, column);
    }

    return 0;
}

static int _text_insertion(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: text_insertion <mode (0-1)>");
        puts("  mode 0: LEFT_TO_RIGHT");
        puts("  mode 1: RIGHT_TO_LEFT");
        return 1;
    }
    else {
        aip31068_text_insertion_mode_t mode;
        switch (atoi(argv[1])) {
            case 0:
                mode = LEFT_TO_RIGHT;
                break;
            case 1:
                mode = RIGHT_TO_LEFT;
                break;
            default:
                puts("mode needs to be one of [0-1]");
                puts("  mode 0: LEFT_TO_RIGHT");
                puts("  mode 1: RIGHT_TO_LEFT");
                return 1;
        }

        aip31068_set_text_insertion_mode(&aip31068_dev, mode);
    }

    return 0;
}

static int _cursor_left(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    aip31068_move_cursor_left(&aip31068_dev);

    return 0;
}

static int _cursor_right(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    aip31068_move_cursor_right(&aip31068_dev);

    return 0;
}

static int _scroll_left(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    aip31068_scroll_display_left(&aip31068_dev);
    return 0;
}

static int _scroll_right(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    aip31068_scroll_display_right(&aip31068_dev);
    return 0;
}

static int _create_custom_symbol(int argc, char **argv)
{
    if (argc != 10) {
        puts("usage: create_custom_symbol <symbol (0-7)> <row 0 (0-31)> ... <row 7 (0-31)>");
        puts("       row values in decimal (e.g. 31 for a full row)");
        puts("       heart example: 0 0 10 31 31 14 4 0");
        return 1;
    }
    else {
        uint8_t symbol = atoi(argv[1]);

        uint8_t row0 = atoi(argv[2]);
        uint8_t row1 = atoi(argv[3]);
        uint8_t row2 = atoi(argv[4]);
        uint8_t row3 = atoi(argv[5]);
        uint8_t row4 = atoi(argv[6]);
        uint8_t row5 = atoi(argv[7]);
        uint8_t row6 = atoi(argv[8]);
        uint8_t row7 = atoi(argv[9]);

        if (symbol > 7) {
            puts("custom symbol needs to be one of [0-7]");
            return 1;
        }

        uint8_t charmap[] = { row0, row1, row2, row3, row4, row5, row6, row7 };
        aip31068_set_custom_symbol(&aip31068_dev, symbol, charmap);
    }

    return 0;
}

static int _print_custom_symbol(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: print_custom_symbol <symbol (0-7)>");
        return 1;
    }
    else {
        uint8_t symbol = atoi(argv[1]);

        if (symbol > 7) {
            puts("custom symbol needs to be one of [0-7]");
            return 1;
        }

        aip31068_print_custom_symbol(&aip31068_dev, symbol);
    }

    return 0;
}

static int _print(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: print <text>");
        return 1;
    }
    else {
        aip31068_print(&aip31068_dev, argv[1]);
    }

    return 0;
}

static int _progressbar(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: progressbar <0 or 1>");
        return 1;
    }
    else {
        uint8_t enabled = atoi(argv[1]);
        _set_progress_bar_enabled(&aip31068_dev, enabled);
    }

    return 0;
}

static int _progressbar_row(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: progressbar_row <row (e.g. 0 for first row)>");
        return 1;
    }
    else {
        uint8_t row = atoi(argv[1]);
        _set_progress_bar_row(row);
    }

    return 0;
}

static int _progress(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: progress <progress (0-100)>");
        return 1;
    }
    else {
        uint8_t progress = atoi(argv[1]);
        _set_progress(&aip31068_dev, progress);
    }

    return 0;
}

static int _run_demo(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("[DEMO START]");

    /* 1. custom symbol */
    aip31068_print(&aip31068_dev, "Hello world! ");
    aip31068_print_custom_symbol(&aip31068_dev, CUSTOM_SYMBOL_1);

    xtimer_sleep(1);

    /* 2.a scroll right with text insertion following LEFT_TO_RIGHT */
    aip31068_clear(&aip31068_dev);
    aip31068_set_text_insertion_mode(&aip31068_dev, LEFT_TO_RIGHT);
    aip31068_print(&aip31068_dev, "scroll right");
    xtimer_msleep(500);

    for (int i = 0; i < 4; i++) {
        aip31068_scroll_display_right(&aip31068_dev);
        xtimer_msleep(500);
    }

    /* 2.b scroll left with text insertion following RIGHT_TO_LEFT */
    aip31068_clear(&aip31068_dev);
    aip31068_set_text_insertion_mode(&aip31068_dev, RIGHT_TO_LEFT);
    aip31068_set_cursor_position(&aip31068_dev, 0, COL_COUNT - 1);
    aip31068_print(&aip31068_dev, "tfel llorcs");
    xtimer_msleep(500);

    for (int i = 0; i < 5; i++) {
        aip31068_scroll_display_left(&aip31068_dev);
        xtimer_msleep(500);
    }
    aip31068_set_text_insertion_mode(&aip31068_dev, LEFT_TO_RIGHT);

    /* 3. turning on/off display */
    aip31068_clear(&aip31068_dev);
    aip31068_print(&aip31068_dev, "turning off...");
    xtimer_sleep(1);
    aip31068_turn_off(&aip31068_dev);
    xtimer_sleep(1);

    aip31068_clear(&aip31068_dev);
    aip31068_print(&aip31068_dev, "turned on again");
    aip31068_turn_on(&aip31068_dev);
    xtimer_sleep(1);

    /* 4. autoscroll */
    aip31068_clear(&aip31068_dev);

    if (ROW_COUNT > 1) {
        aip31068_set_cursor_position(&aip31068_dev, 1, 0);
        aip31068_print(&aip31068_dev, "This is also a very long line");
    }

    aip31068_set_cursor_position(&aip31068_dev, 0, COL_COUNT);
    aip31068_set_auto_scroll_enabled(&aip31068_dev, true);

    char long_line[] = "This is a very long line";
    for (int i = 0; i < (int) strlen(long_line); i++) {
        aip31068_print_char(&aip31068_dev, long_line[i]);
        xtimer_msleep(250);
    }
    aip31068_set_auto_scroll_enabled(&aip31068_dev, false);
    xtimer_sleep(2);

    /* 5. return home */
    aip31068_return_home(&aip31068_dev);
    xtimer_sleep(1);

    /* 6. cursor blinking */
    aip31068_clear(&aip31068_dev);
    aip31068_set_cursor_blinking_enabled(&aip31068_dev, true);
    xtimer_sleep(2);

    for (int i = 0; i < COL_COUNT; i++) {

        if (i == (int) (COL_COUNT / 2)) {
            aip31068_set_cursor_blinking_enabled(&aip31068_dev, false);
        }

        if (i == (int) (COL_COUNT / 2) + 2) {
            aip31068_set_cursor_blinking_enabled(&aip31068_dev, true);
        }

        aip31068_set_cursor_position(&aip31068_dev, 0, i);
        xtimer_msleep(500);
    }
    xtimer_sleep(1);
    aip31068_return_home(&aip31068_dev);
    xtimer_msleep(500);
    aip31068_set_cursor_blinking_enabled(&aip31068_dev, false);

    /* 7. show and move cursor */
    aip31068_set_cursor_visible(&aip31068_dev, true);
    aip31068_clear(&aip31068_dev);
    xtimer_sleep(1);
    for (int i = 0; i <= 9; i++) {
        aip31068_print_char(&aip31068_dev, '0' + i);
        xtimer_msleep(250);
    }

    aip31068_return_home(&aip31068_dev);
    xtimer_msleep(100);
    for (int i = 0; i < 50; i++) {
        aip31068_move_cursor_right(&aip31068_dev);
        xtimer_msleep(100);
    }
    for (int i = 50; i > 0 ; i--) {
        aip31068_move_cursor_left(&aip31068_dev);
        xtimer_msleep(100);
    }

    aip31068_set_cursor_visible(&aip31068_dev, false);
    aip31068_clear(&aip31068_dev);

    /* 8. write 10 characters on the next line (if available), because 40
     * characters per line is the maximum */
    if (ROW_COUNT > 1) {
        aip31068_clear(&aip31068_dev);
        aip31068_set_text_insertion_mode(&aip31068_dev, LEFT_TO_RIGHT);
        for (int i = 0; i < 50; i++) {
            if (i % 2 == 0) {
                aip31068_print(&aip31068_dev, "A");
            }
            else {
                aip31068_print(&aip31068_dev, "B");
            }

            xtimer_msleep(100);
        }

        aip31068_clear(&aip31068_dev);
        aip31068_set_text_insertion_mode(&aip31068_dev, RIGHT_TO_LEFT);
        aip31068_set_cursor_position(&aip31068_dev, 0, COL_COUNT - 1);
        for (int i = 0; i < 50; i++) {
            if (i % 2 == 0) {
                aip31068_print(&aip31068_dev, "X");
            }
            else {
                aip31068_print(&aip31068_dev, "Y");
            }

            xtimer_msleep(100);
        }
    }

    aip31068_clear(&aip31068_dev);

    /* 9. progress bar */
    _set_progress_bar_enabled(&aip31068_dev, true);
    aip31068_set_cursor_position(&aip31068_dev, 0, 0);
    aip31068_print(&aip31068_dev, "Progress: ");

    for (int j = 0; j <= 100; j++) {

        aip31068_set_cursor_position(&aip31068_dev, 0, 10);

        char string_rep[6];
        sprintf(string_rep, "%d %%", j);

        aip31068_print(&aip31068_dev, string_rep);

        _set_progress(&aip31068_dev, j);
        xtimer_msleep(100);
    }
    _set_progress_bar_enabled(&aip31068_dev, false);
    aip31068_clear(&aip31068_dev);

    puts("[DEMO END]");

    return 0;
}

static void _set_progress_bar_enabled(aip31068_t *dev, bool enabled)
{
    _progress_bar_enabled = enabled;

    if (enabled) {
        _init_progress_bar(dev, dev->params.row_count - 1);
    }
}

static void _set_progress_bar_row(uint8_t row)
{
    _progress_bar_row = row;
}

static void _set_progress(aip31068_t *dev, uint8_t progress)
{
    if (!_progress_bar_enabled) {
        return;
    }

    /* calculate the number of pixel-columns on a single line */
    int bar_count = dev->params.col_count * PIXEL_COLUMNS_PER_CHAR;

    if (progress > 100) {
        progress = 100;
    }

    /* How many bars to display for given progress? */
    int progress_bar_count = bar_count * progress / 100;

    /* number of completely filled sections / characters */
    int full_bar_count = progress_bar_count / PIXEL_COLUMNS_PER_CHAR;

    /* number of bars in the last section / character remaining */
    int remainder_bar_count = progress_bar_count % PIXEL_COLUMNS_PER_CHAR;

    aip31068_set_cursor_position(dev, _progress_bar_row, 0);

    for (int i = 0; i < full_bar_count; i++) {
        aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_8);
    }

    uint8_t blank_count = dev->params.col_count - full_bar_count;

    switch (remainder_bar_count) {

        case 1:
            aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_4);
            blank_count--;
            break;

        case 2:
            aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_5);
            blank_count--;
            break;

        case 3:
            aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_6);
            blank_count--;
            break;

        case 4:
            aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_7);
            blank_count--;
            break;
    }

    /* clear the rest of the line, so it appears as empty part of the progressbar */
    for (int i = 0; i < blank_count; i++) {
        aip31068_print(dev, " ");
    }
}

static void _init_progress_bar(aip31068_t* dev, uint8_t row)
{
    _progress_bar_row = row;

    /* if autoscroll was used, the progress bar would be displayed incorrectly */
    aip31068_set_auto_scroll_enabled(dev, false);

    /* undo any scrolling */
    aip31068_return_home(dev);

    /* progress bar should increase from left to right */
    aip31068_set_text_insertion_mode(dev, LEFT_TO_RIGHT);

    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_4, custom_char_progress_bar_1);
    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_5, custom_char_progress_bar_2);
    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_6, custom_char_progress_bar_3);
    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_7, custom_char_progress_bar_4);
    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_8, custom_char_progress_bar_5);
}

static const shell_command_t shell_commands[] = {
    { "turn_on", "Turn on LCD.", _turn_on },
    { "turn_off", "Turn off LCD.", _turn_off },
    { "clear", "Clear display content.", _clear },
    { "home", "Return cursor to home position (0, 0).", _home },
    { "autoscroll", "Enable or disable autoscroll.", _autoscroll },
    { "cursor_blinking", "Enable or disable cursor blinking.", _cursor_blinking },
    { "cursor_visible", "Show or hide the cursor.", _cursor_visible },
    { "cursor_position", "Set cursor position.", _cursor_position },
    { "text_insertion", "Set text insertion mode.", _text_insertion },
    { "cursor_left", "Move cursor one to the left.", _cursor_left },
    { "cursor_right", "Move cursor one to the right.", _cursor_right },
    { "scroll_left", "Scroll display content one to the left.", _scroll_left },
    { "scroll_right", "Scroll display content one to the right.", _scroll_right },
    { "create_custom_symbol", "Create a custom symbol.", _create_custom_symbol },
    { "print_custom_symbol", "Print a custom symbol.", _print_custom_symbol },
    { "print", "Print a string.", _print },
    { "progressbar", "Enable/ disable progressbar feature.", _progressbar },
    { "progressbar_row", "Set row for progressbar.", _progressbar_row },
    { "progress", "Set progress for progressbar.", _progress },
    { "run_demo", "Demonstration of all functions.", _run_demo },
    { NULL, NULL, NULL }
};

int main(void)
{
    int rc = 0;
    if ((rc = aip31068_init(&aip31068_dev, &aip31068_params[0])) != 0) {
        printf("Initialization failed! rc = %d", rc);
        return 1;
    }

    aip31068_turn_on(&aip31068_dev);
    aip31068_set_custom_symbol(&aip31068_dev, CUSTOM_SYMBOL_1, custom_char_heart);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
