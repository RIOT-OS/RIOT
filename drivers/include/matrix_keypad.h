/*
 * Copyright (C) 2021 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MATRIX_KEYPAD_H
#define MATRIX_KEYPAD_H

/**
 * @defgroup    drivers_matrix_keypad Matrix Keypad
 * @ingroup     drivers_sensors
 * @brief       Matrix keypad driver for row/column keypads
 *
 * This module implements a simple matrix keypad driver where keys are connected
 * between GPIO columns and rows. It works best with diodes in series with the
 * switches to prevent key ghosting, but it can be used without these diodes.
 * @ref CONFIG_MATRIX_KEYPAD_ROWS_USE_OPEN_DRAIN can be enabled when the keypad
 * doesn't use diodes in the switches.
 *
 * The keypad works by configuring the column GPIOs as input with pull-ups. Each
 * row is configured as open drain with pull-up. One by one the rows are set to
 * pull their output low. For each row the column GPIOs are read and the state
 * is checked. When a key is pressed the column GPIO of that switch will read
 * low as soon as the row it is on is pulled low.
 *
 * The debouncing algorithm is a pattern style debounce where the switch must be
 * in one position for a number of samples, then a set of "don't care" samples
 * and then in the other position for a number of samples. The samples in the
 * middle allow for a period where the switch can be either low or high without
 * affecting the transition. The exact pattern is determined by
 * @ref CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_BEGIN and
 * @ref CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_END. These are used as mask where
 * the switch must be in a determined state. The bits where neither pattern is
 * set is used as the "don't care" set of samples.
 *
 * A full scan of the matrix keypad is done via the @ref matrix_keypad_scan
 * function. This function iterates over all rows and columns to update the
 * stored history of every pin.
 *
 * When a state change is detected on a switch, the @ref matrix_keypad_cb_t
 * callback is called with the row and column number together with the new state
 * of the switch (pressed or not pressed).
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the matrix keypad
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <stdint.h>
#include <stdbool.h>
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of rows
 */
#ifndef CONFIG_MATRIX_KEYPAD_NUM_ROWS
#define CONFIG_MATRIX_KEYPAD_NUM_ROWS   2
#endif

/**
 * @brief Maximum number of columns
 */
#ifndef CONFIG_MATRIX_KEYPAD_NUM_COLUMNS
#define CONFIG_MATRIX_KEYPAD_NUM_COLUMNS   2
#endif

/**
 * @brief Debounce pattern high to low bits
 *
 * This pattern should consist of bits set on the most significant bits of the
 * value. The number of bits set determines the number of scans the key must be
 * in the current state before a change is processed.
 */
#ifndef CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_BEGIN
#define CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_BEGIN  0xC0
#endif

/**
 * @brief Debounce pattern low to high bits
 *
 * This pattern should consist of bits set on the least significant bits of the
 * value. The number of bits set determines the number of scans the key must be
 * in the next state before a change is processed.
 */
#ifndef CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_END
#define CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_END  0x7
#endif

/**
 * @brief Use open drain GPIO mode
 */
#ifndef CONFIG_MATRIX_KEYPAD_ROWS_USE_OPEN_DRAIN
#define CONFIG_MATRIX_KEYPAD_ROWS_USE_OPEN_DRAIN  0
#endif

/**
 * @brief GPIO mode used for the row GPIOs
 */
#if CONFIG_MATRIX_KEYPAD_ROWS_USE_OPEN_DRAIN
#define MATRIX_KEYPAD_ROWS_GPIO_MODE GPIO_OD_PU
#else
#define MATRIX_KEYPAD_ROWS_GPIO_MODE GPIO_OUT
#endif

/**
 * @brief Debounce pattern mask
 */
#define MATRIX_KEYPAD_DEBOUNCE_MASK  \
    (CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_BEGIN | CONFIG_MATRIX_KEYPAD_DEBOUNCE_PATTERN_END)

/**
 * @brief Type definition for a full row (all columns) state, variable width
 * depending on the number of columns.
 */
#if CONFIG_MATRIX_KEYPAD_NUM_COLUMNS <= 8
typedef uint8_t matrix_keypad_state_row_t;
#elif CONFIG_MATRIX_KEYPAD_NUM_COLUMNS <= 16
typedef uint16_t matrix_keypad_state_row_t;
#elif CONFIG_MATRIX_KEYPAD_NUM_COLUMNS <= 32
typedef uint32_t matrix_keypad_state_row_t;
#elif CONFIG_MATRIX_KEYPAD_NUM_COLUMNS <= 64
typedef uint64_t matrix_keypad_state_row_t;
#else
#error Too many columns on matrix keypad.
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    /**
     * @brief GPIO pin array for the rows. GPIO_UNDEF is allowed and scanning
     * is skipped for these.
     */
    gpio_t rows[CONFIG_MATRIX_KEYPAD_NUM_ROWS];
    /**
     * @brief GPIO pin array for the columns. GPIO_UNDEF is allowed and scanning
     * is skipped for these.
     */
    gpio_t columns[CONFIG_MATRIX_KEYPAD_NUM_COLUMNS];
    /**
     * @brief Delay in microseconds between configuring the row gpio and reading
     * out the column. Can be zero to skip the delay.
     */
    uint32_t row2col_delay;
} matrix_keypad_params_t;

/**
 * @brief   Callback for key state changes
 *
 * @param   arg     callback context
 * @param   row     Row that changed
 * @param   column  Column that changed
 * @param   state   New state of the key, 1 = pressed, 0 = released
 */
typedef void (*matrix_keypad_cb_t)(void *arg, size_t row, size_t column, bool state);

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    /**
     * @brief Device initialization parameters
     */
    const matrix_keypad_params_t *params;

    /**
     * @brief Debounce history
     */
    uint8_t debounce[CONFIG_MATRIX_KEYPAD_NUM_ROWS][CONFIG_MATRIX_KEYPAD_NUM_COLUMNS];

    /**
     * @brief Current button state
     */
    matrix_keypad_state_row_t state[CONFIG_MATRIX_KEYPAD_NUM_ROWS];

    /**
     * @brief callback context
     */
    void *arg;

    /**
     * @brief Callback called when a key changes state
     */
    matrix_keypad_cb_t callback;
} matrix_keypad_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 * @param[in]    callback   Callback to call on state changes
 * @param[in]    arg        Context argument for the callback
 *
 * @return                  0 on success
 */
int matrix_keypad_init(matrix_keypad_t *dev,
                       const matrix_keypad_params_t *params,
                       matrix_keypad_cb_t callback,
                       void *arg);

/**
 * @brief   Scan through the keypad matrix
 *
 * This updates the state of the device descriptor, calling
 * @ref matrix_keypad_cb_t when a key press or release has been detected
 *
 * @param[inout] dev        Device descriptor of the driver
 *
 * @return                  Number of keys that changed state
 */
size_t matrix_keypad_scan(matrix_keypad_t *dev);
#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MATRIX_KEYPAD_H */
