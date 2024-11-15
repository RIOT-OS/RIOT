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
    DIGIT7SEG_ERR_A_GPIO,       /**< Something went wrong with A GPIO */
    DIGIT7SEG_ERR_B_GPIO,       /**< Something went wrong with B GPIO */
    DIGIT7SEG_ERR_C_GPIO,       /**< Something went wrong with C GPIO */
    DIGIT7SEG_ERR_D_GPIO,       /**< Something went wrong with D GPIO */
    DIGIT7SEG_ERR_E_GPIO,       /**< Something went wrong with E GPIO */
    DIGIT7SEG_ERR_F_GPIO,       /**< Something went wrong with F GPIO */
    DIGIT7SEG_ERR_G_GPIO,       /**< Something went wrong with G GPIO */
    DIGIT7SEG_ERR_DP_GPIO,      /**< Something went wrong with DP GPIO */
    DIGIT7SEG_ERR_DIG1_GPIO,    /**< Something went wrong with DIG1 GPIO */
    DIGIT7SEG_ERR_DIG2_GPIO,    /**< Something went wrong with DIG2 GPIO */
    DIGIT7SEG_ERR_DIG3_GPIO,    /**< Something went wrong with DIG3 GPIO */
    DIGIT7SEG_ERR_DIG4_GPIO,    /**< Something went wrong with DIG4 GPIO */
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

#ifdef __cplusplus
}
#endif

#endif /* DIGIT7SEG_H */
/** @} */
