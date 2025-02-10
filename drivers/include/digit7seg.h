/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_digit7seg
 * @brief       Device driver for less than 5 digits of 7 segments without IC
 * @file
 * @author      Pierre Le Meur <pierre1.lemeur@orange.com>
 * @{
 *
 */

#ifndef DIGIT7SEG_H
#define DIGIT7SEG_H

#include <inttypes.h>

#include "macros/units.h"
#include "periph/gpio.h"
#include "periph/timer.h"

#ifdef MODULE_DIGIT7SEG_FLOAT
#  include <math.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DIGIT7SEG_MAX_DIGITS  (4)     /**< Max pin number, can be below or equal */
#ifndef DIGIT7SEG_TIMER_HZ
#   define DIGIT7SEG_TIMER_HZ    MHZ(1)  /**< default timer hz */
#endif
#ifndef DIGIT7SEG_DELAY
/**
 * With 1MHz and a cycle as 2600 we got 2.6ms each shift,
 * so undetectable frequency by human eyes
 * */
#   define DIGIT7SEG_DELAY       (DIGIT7SEG_TIMER_HZ / 2600)
#endif

/**
 * @brief   Return codes for @ref digit7seg_init
 */
typedef enum {
    DIGIT7SEG_OK = 0,           /**< All ok */
    DIGIT7SEG_ERR_DIGITS,       /**< Something went wrong with digits value */
} digit7seg_error_codes;

/**
 * @brief   Device initialization parameters, each data_* pin representing
 *          a segment and dig* representing a digit pin to turn on.
 * ```
 *        --a--
 *       |     |
 *       f     b
 *       |     |
 *        --g--
 *       |     |
 *       e     c
 *       |     |
 *        --d--    .dp
 *```
 */
typedef struct {
    gpio_t data_a;  /**< Data input pin */
    gpio_t data_b;  /**< Data input pin */
    gpio_t data_c;  /**< Data input pin */
    gpio_t data_d;  /**< Data input pin */
    gpio_t data_e;  /**< Data input pin */
    gpio_t data_f;  /**< Data input pin */
    gpio_t data_g;  /**< Data input pin */
    gpio_t data_dp; /**< Data input pin */
    gpio_t dig1;    /**< Data input pin */
    gpio_t dig2;    /**< Data input pin */
    gpio_t dig3;    /**< Data input pin */
    gpio_t dig4;    /**< Data input pin */
    tim_t timer;    /**< Timer to use in digit7seg_power* */
    uint8_t digits; /**< Number of digits */
} digit7seg_params_t;

/**
 * @brief   Device descriptor for the DIGIT7SEG
 */
typedef struct {
    digit7seg_params_t params; /**< Device parameters */
    uint32_t value;            /**< Binary value to display */
    uint8_t current_digit;     /**< Displayed digit */
} digit7seg_t;

/**
 * @brief   Initialize the given DIGIT7SEG
 *
 * @param[out] dev      DIGIT7SEG device descriptor to initialize
 * @param[in]  params   Device parameters to use
 *
 * @return  0 on success or negative error code, see #digit7seg_error_codes
 */
int digit7seg_init(digit7seg_t *dev, const digit7seg_params_t *params);

/**
 * @brief   Set the value for every digit
 *
 * @param[in] dev       Initialized device descriptor of DIGIT7SEG device
 * @param[in] value     the value as an uint32_t
 *
 */
void digit7seg_set_all_value(digit7seg_t *dev, uint32_t value);

/**
 * @brief   Set the value for one digit
 *
 * @param[in] dev       Initialized device descriptor of DIGIT7SEG device
 * @param[in] index     the digit number between [0, DIGIT7SEG_MAX_DIGITS[
 * @param[in] value     the value as an uint8_t, a is equal to the first bit
 *                       b the second.... dp the 8th bit.
 *
 * @return              0 on success
 * @return              -1 when an incorrect digit is passed
 */
int digit7seg_set_value(digit7seg_t *dev, int index, uint8_t value);

#if MODULE_DIGIT7SEG_INT || DOXYGEN
/**
 * @brief   Set the value for every digit from an int
 * 
 * @param[in] dev       Initialized device descriptor of DIGIT7SEG device
 * @param[in] value     the value between [0, 9999]
 *
 */
int digit7seg_set_int_all_value(digit7seg_t *dev, uint16_t value);

/**
 * @brief   Set the value for one digit from an int
 * 
 * @param[in] dev       Initialized device descriptor of DIGIT7SEG device
 * @param[in] index     the digit number between [0, DIGIT7SEG_MAX_DIGITS[
 * @param[in] value     the value between [0, 9]
 *
 */
int digit7seg_set_int_value(digit7seg_t *dev, int index, uint8_t value);
#endif

#if MODULE_DIGIT7SEG_FLOAT || DOXYGEN
/**
 * @brief   Set a float value dispatched between every display
 * 
 * @param[in] dev       Initialized device descriptor of DIGIT7SEG device
 * @param[in] value     A float value between [0, 999,9]
 * @param[in] precision The multiplier factor ex: value = 234,15, precision = 10
 *                       will be displayed as 234,1 but with
 *                       precision = 100 it will be 34,15
 */
int digit7seg_set_float_value(digit7seg_t *dev, float value, int precision);
#endif


/**
 * @brief   Start an periodic timer event to shift between each 7seg
 *
 * @param[in] dev       Initialized device descriptor of DIGIT7SEG device
 *
 * @return              0 on success
 * @return              -1 if the timer was impossible to start
 */
int digit7seg_poweron(digit7seg_t *dev);

/**
 * @brief   Stop an periodic timer event to shift between each 7seg
 *
 * @param[in] dev       Initialized device descriptor of DIGIT7SEG device
 */
void digit7seg_poweroff(digit7seg_t *dev);

#if MODULE_DIGIT7SEG_INT || MODULE_DIGIT7SEG_FLOAT || DOXYGEN
/**
 * @brief   Display bitfield for numbers between [0, 9]
 */
static const uint8_t digit7seg_bitfield[] = {
    0b00111111, /**< 0 */
    0b00000110, /**< 1 */
    0b01011011, /**< 2 */
    0b01001111, /**< 3 */
    0b01100110, /**< 4 */
    0b01101101, /**< 5 */
    0b01111101, /**< 6 */
    0b00000111, /**< 7 */
    0b01111111, /**< 8 */
    0b01101111  /**< 9 */
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* DIGIT7SEG_H */
/** @} */
