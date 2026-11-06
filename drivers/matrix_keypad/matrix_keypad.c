/*
 * SPDX-FileCopyrightText: 2021 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_matrix_keypad
 * @{
 *
 * @file
 * @brief       Device driver implementation for the drivers
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <string.h>

#include "matrix_keypad.h"
#include "matrix_keypad_params.h"

#include "periph/gpio.h"
#include "ztimer.h"

static uint8_t _mask_bits(uint8_t bits)
{
    return bits & MATRIX_KEYPAD_DEBOUNCE_MASK;
}

static void _flip_state(matrix_keypad_t *dev, size_t row, size_t column)
{
    dev->state[row] ^= (1 << column);
}

static void _setup_columns(matrix_keypad_t *dev)
{
    for (size_t i = 0; i < CONFIG_MATRIX_KEYPAD_NUM_COLUMNS; i++) {
        gpio_t column = dev->params->columns[i];
        if (gpio_is_valid(column)) {
            gpio_init(column, GPIO_IN_PU);
        }
    }
}

static void _setup_rows(matrix_keypad_t *dev)
{
    for (size_t i = 0; i < CONFIG_MATRIX_KEYPAD_NUM_ROWS; i++) {
        gpio_t row = dev->params->rows[i];
        if (gpio_is_valid(row)) {
            gpio_init(row, MATRIX_KEYPAD_ROWS_GPIO_MODE); /* Open drain to ensure rows don't conflict */
            gpio_set(row);
        }
    }
}

unsigned _update_key(matrix_keypad_t *dev,
                     size_t row, size_t column, bool status)
{
    /* Pattern based debounce:
     * https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/
     */
    /* get the current stored state */
    bool state = dev->state[row] & (1 << column);
    /* get the history of the pin */
    uint8_t *debounce = &dev->debounce[row][column];

    *debounce = (*debounce << 1) | status; /* Update state */

    /* Pin history masking must match the begin pattern if it is currently
     * pressed and match the end pattern if it is released */
    uint8_t pattern = state ? CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_BEGIN
                            : CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_END;

    if (_mask_bits(*debounce) == pattern) {
        /* Changed */
        *debounce = 0U - (unsigned)state; /* 0x0 if released, 0xFF if pressed */
        _flip_state(dev, row, column);
        dev->callback(dev->arg, row, column, status);
        return 1;
    }
    return 0;
}

int matrix_keypad_init(matrix_keypad_t *dev, const matrix_keypad_params_t *params,
                       matrix_keypad_cb_t callback, void *arg)
{
    static_assert(
        (CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_BEGIN & CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_END) == 0,
        "Debounce patterns must not overlap");
    memset(dev, 0, sizeof(matrix_keypad_t));
    dev->params = params;
    dev->callback = callback;
    dev->arg = arg;
    _setup_columns(dev);
    _setup_rows(dev);
    return 0;
}

size_t matrix_keypad_scan(matrix_keypad_t *dev)
{
    size_t res = 0;

    /* Scan rows */
    for (size_t i = 0; i < CONFIG_MATRIX_KEYPAD_NUM_ROWS; i++) {
        gpio_t row = dev->params->rows[i];

        if (!gpio_is_valid(row)) {
            continue;
        }

        /* Pull the row low */
        gpio_clear(row);

        /* Wait for the row delay */
        if (dev->params->row2col_delay) {
            ztimer_sleep(ZTIMER_USEC, dev->params->row2col_delay);
        }

        /* Scan columns */
        for (size_t j = 0; j < CONFIG_MATRIX_KEYPAD_NUM_COLUMNS; j++) {
            gpio_t column = dev->params->columns[j];
            if (!gpio_is_valid(column)) {
                continue;
            }
            bool status = !gpio_read(column);
            res += _update_key(dev, i, j, status);
        }
        /* Return the row to high-Z */
        gpio_set(row);
    }
    return res;
}
