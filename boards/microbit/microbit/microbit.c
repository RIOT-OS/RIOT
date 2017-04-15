/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_microbit
 * @{
 *
 * @file
 * @brief       BBC micro:bit specific LED matrix handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include "xtimer.h"

#include "board.h"
#include "microbit.h"
#include "mineplex.h"
#include "periph/gpio.h"
#include "periph/timer.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   The visible number of rows and columns of the LED matrix
 */
#define ROWS                MICROBIT_MATRIX_ROWS
#define COLS                MICROBIT_MATRIX_COLS

/**
 * @brief   The electrical number of rows and columns
 */
#define ROWS_HW             (3)
#define COLS_HW             (9)

/**
 * @brief   The refresh rate used for drawing the contents
 *
 * We want a refresh rate of at least 50Hz (->20ms), so the LEDs do not flicker.
 */
#define REFRESH             (6000)      /* 6ms * 3 rows -> ~55Hz */

/**
 * @brief   GPIO pins driving the rows
 */
static const gpio_t rows[ROWS_HW] = {
    MICROBIT_LED_ROW1,
    MICROBIT_LED_ROW2,
    MICROBIT_LED_ROW3
};

/**
 * @brief   GPIO pins driving the columns
 */
static const gpio_t cols[COLS_HW] = {
    MICROBIT_LED_COL1,
    MICROBIT_LED_COL2,
    MICROBIT_LED_COL3,
    MICROBIT_LED_COL4,
    MICROBIT_LED_COL5,
    MICROBIT_LED_COL6,
    MICROBIT_LED_COL7,
    MICROBIT_LED_COL8,
    MICROBIT_LED_COL9,
};

/**
 * @brief   Map electrical layout to visible layout
 *
 * The electrical layout of the matrix is different than the visible layout
 * (3x9 -> 5x5). This array maps from the visible 5 by 5 layout to the actual
 * 3 by 9 layout used by the hardware.
 */
static const uint8_t pixmap[5][5] = {
    {  0, 12,  1, 13,  2 },
    { 21, 22, 23, 24, 25 },
    { 10,  8, 11, 26,  9 },
    {  7,  6,  5,  4,  3 },
    { 20, 15, 18, 14, 19 }
};

/**
 * @brief   Buffer holding the current 'image' that is displayed
 */
static uint8_t framebuf[ROWS_HW * COLS_HW] = { 0 };

/**
 * @brief   Internal counter to keep track of which row needs to be refreshed
 *          next
 */
static unsigned cur_row = 0;

/**
 * @brief   Write a Mineplex encoded character into the given buffer
 *
 * @param[in]  c    character to write
 * @param[out] buf  buffer to write the encoded character into, MUST be able to
 *                  hold 25 byte
 */
static void char2buf(char c, uint8_t *buf)
{
    const uint8_t *raw = mineplex_char(c);

    /* set each row */
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            buf[(row * COLS) + col] = (raw[row] & (1 << col));
        }
    }
}

/**
 * @brief   Shift out and replace an image with the next, column by column
 *
 * @param[in|out] cur   current 'image', will be overwritten
 * @param[in] next      image to shift in
 * @param[in] delay     delay between each column
 */
static void shift_next(uint8_t *cur, const uint8_t *next, uint32_t delay)
{
    for (int i = 0; i < COLS; i++) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < (COLS - 1); c++) {
                cur[(r * COLS) + c] = cur[(r * COLS) + c + 1];
            }
            cur[(r * COLS) + COLS - 1] = next[(r * COLS) + i];
        }
        microbit_matrix_set_raw((uint8_t *)cur);
        xtimer_usleep(delay);
    }
}

static void refresh(void *arg, int channel)
{
    (void)arg;
    (void)channel;

    /* set next refresh */
    timer_set(TIMER_DEV(2), 0, REFRESH);

    /* disable current row */
    gpio_clear(rows[cur_row]);
    /* goto next row */
    cur_row = ((++cur_row) < ROWS_HW) ? cur_row : 0;
    /* setup columns */
    int base = (COLS_HW * cur_row);
    for (int i = 0; i < COLS_HW; i++) {
        gpio_write(cols[i], !(framebuf[base + i]));
    }
    /* and finally enable the new row */
    gpio_set(rows[cur_row]);
}

void microbit_matrix_init(void)
{
    /* initialize rows */
    for (int i = 0; i < ROWS_HW; i++) {
        gpio_init(rows[i], GPIO_OUT);
        gpio_clear(rows[i]);
    }
    /* initialize columns */
    for (int i = 0; i < COLS_HW; i++) {
        gpio_init(cols[i], GPIO_OUT);
        gpio_set(cols[i]);
    }
    /* and finally initialize and start the refresh timer */
    timer_init(TIMER_DEV(2), 1000000, refresh, NULL);
    timer_set(TIMER_DEV(2), 0, REFRESH);
}

void microbit_matrix_on(uint8_t row, uint8_t col)
{
    if ((row >= 5) || (col >= 5)) {
        return;
    }

    framebuf[pixmap[row][col]] = 0x01;
}

void microbit_matrix_off(uint8_t row, uint8_t col)
{
    if ((row >= 5) || (col >= 5)) {
        return;
    }

    framebuf[pixmap[row][col]] = 0x00;
}

void microbit_matrix_set_raw(const uint8_t *buf) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            framebuf[pixmap[row][col]] = buf[(row * COLS) + col];
        }
    }
}

void microbit_matrix_set_char(char c)
{
    uint8_t buf[ROWS * COLS];

    char2buf(c, buf);
    microbit_matrix_set_raw(buf);
}

void microbit_matrix_shift_str(const char *str, uint32_t delay)
{
    uint8_t curbuf[ROWS][COLS];
    uint8_t newbuf[ROWS][COLS];

    char2buf(' ', (uint8_t *)curbuf);
    microbit_matrix_set_raw((uint8_t *)curbuf);
    while (*str) {
        char2buf(*str++, (uint8_t *)newbuf);
        shift_next((uint8_t *)curbuf, (uint8_t *)newbuf, delay);
    }
    char2buf(' ', (uint8_t *)newbuf);
    shift_next((uint8_t *)curbuf, (uint8_t *)newbuf, delay);
}
