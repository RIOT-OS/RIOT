/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_microbit
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

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief   The visible number of rows and columns of the LED matrix
 */
#define ROWS                MICROBIT_MATRIX_ROWS
#define COLS                MICROBIT_MATRIX_COLS

/**
 * @brief   The refresh rate used for drawing the contents
 *
 * We want a refresh rate of at least 50Hz (->20ms), so the LEDs do not flicker.
 */
#define REFRESH             (6000)      /* 6ms * 3 rows -> ~55Hz */

#if defined(BOARD_MICROBIT)
/**
 * @brief   The electrical number of rows and columns
 */
#define ROWS_HW             (3U)
#define COLS_HW             (9U)

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
static const uint8_t pixmap[ROWS][COLS] = {
    {  0, 12,  1, 13,  2 },
    { 21, 22, 23, 24, 25 },
    { 10,  8, 11, 26,  9 },
    {  7,  6,  5,  4,  3 },
    { 20, 15, 18, 14, 19 }
};

/**
 * @brief   Timer dev
 */
#define TIMER_DEV_NUM       (2)

#elif defined(BOARD_MICROBIT_V2)
/**
 * @brief   The electrical number of rows and columns
 */
#define ROWS_HW             (5U)
#define COLS_HW             (5U)

/**
 * @brief   GPIO pins driving the rows
 */
static const gpio_t rows[ROWS_HW] = {
    MICROBIT_LED_ROW1,
    MICROBIT_LED_ROW2,
    MICROBIT_LED_ROW3,
    MICROBIT_LED_ROW4,
    MICROBIT_LED_ROW5,
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
};

/**
 * @brief   Map electrical layout to visible layout
 */
static const uint8_t pixmap[ROWS][COLS] = {
    {  0,  1,  2,  3,  4 },
    {  5,  6,  7,  8,  9 },
    { 10, 11, 12, 13, 14 },
    { 15, 16, 17, 18, 19 },
    { 20, 21, 22, 23, 24 }
};

/**
 * @brief   Timer dev
 */
#define TIMER_DEV_NUM       (1)
#else
#error "Module only compatible with microbit and microbit-v2 boards."
#endif

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
    for (unsigned row = 0; row < ROWS; row++) {
        for (unsigned col = 0; col < COLS; col++) {
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
    for (unsigned i = 0; i < COLS; i++) {
        for (unsigned r = 0; r < ROWS; r++) {
            for (unsigned c = 0; c < (COLS - 1); c++) {
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
    timer_set(TIMER_DEV(TIMER_DEV_NUM), 0, REFRESH);

    /* disable current row */
    gpio_clear(rows[cur_row]);
    /* goto next row */
    cur_row = ((++cur_row) < ROWS_HW) ? cur_row : 0;
    /* setup columns */
    unsigned base = (COLS_HW * cur_row);
    for (unsigned i = 0; i < COLS_HW; i++) {
        gpio_write(cols[i], !(framebuf[base + i]));
    }
    /* and finally enable the new row */
    gpio_set(rows[cur_row]);
}

void microbit_matrix_init(void)
{
    /* initialize rows */
    for (unsigned i = 0; i < ROWS_HW; i++) {
        gpio_init(rows[i], GPIO_OUT);
        gpio_clear(rows[i]);
    }
    /* initialize columns */
    for (unsigned i = 0; i < COLS_HW; i++) {
        gpio_init(cols[i], GPIO_OUT);
        gpio_set(cols[i]);
    }
    /* and finally initialize and start the refresh timer */
    timer_init(TIMER_DEV(TIMER_DEV_NUM), 1000000, refresh, NULL);
    timer_set(TIMER_DEV(TIMER_DEV_NUM), 0, REFRESH);
}

void microbit_matrix_on(uint8_t row, uint8_t col)
{
    if ((row >= ROWS) || (col >= COLS)) {
        return;
    }

    framebuf[pixmap[row][col]] = 0x01;
}

void microbit_matrix_off(uint8_t row, uint8_t col)
{
    if ((row >= ROWS) || (col >= COLS)) {
        return;
    }

    framebuf[pixmap[row][col]] = 0x00;
}

void microbit_matrix_set_raw(const uint8_t *buf)
{
    for (unsigned row = 0; row < ROWS; row++) {
        for (unsigned col = 0; col < COLS; col++) {
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
