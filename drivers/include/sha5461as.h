/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sha5461as SHA5461AS
 * @ingroup     drivers_display
 * @brief       Device driver interface for the SHA5461AS 7 segments display
 * @{
 *
 * @file
 * @brief       Device driver interface for the SHA5461AS 7 segments display
 *
 * @author      Pierre Le Meur <pierre1.lemeur@orange.com>
 */

#ifndef SHA5461AS_H
#define SHA5461AS_H

#include <inttypes.h>

#include "periph/gpio.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SHA5461AS_MAX_DIGITS     (4)              /**< Max pin number, can be below or equal */
#ifndef XTIMER_DEV
#define XTIMER_DEV               TIMER_DEV(0)     /**< default timer */
#endif
#ifndef XTIMER_HZ
#define XTIMER_HZ                MHZ(1)           /**< default timer hz */
#endif
#ifndef SHA5461AS_DELAY
#define SHA5461AS_DELAY          (XTIMER_HZ/2600) /**< with 1MHz and a cycle as 2600 we got 2.6ms each shift, so 
                                                   undetectable frequency by human eyes*/
#endif
#define TIMER_RUNNING            (1)               /**< status when timer is running */
#define TIMER_STOPPED            (0)               /**< status when timer is stopped */

/**
 * @brief   Return codes
 */
enum {
    SHA5461AS_OK = 0,        /**< All ok */
    SHA5461AS_ERR_A_GPIO,    /**< Something went wrong with A GPIO */
    SHA5461AS_ERR_B_GPIO,    /**< Something went wrong with B GPIO */
    SHA5461AS_ERR_C_GPIO,    /**< Something went wrong with C GPIO */
    SHA5461AS_ERR_D_GPIO,    /**< Something went wrong with D GPIO */
    SHA5461AS_ERR_E_GPIO,    /**< Something went wrong with E GPIO */
    SHA5461AS_ERR_F_GPIO,    /**< Something went wrong with F GPIO */
    SHA5461AS_ERR_G_GPIO,    /**< Something went wrong with G GPIO */
    SHA5461AS_ERR_DP_GPIO,   /**< Something went wrong with DP GPIO */
    SHA5461AS_ERR_DIG1_GPIO, /**< Something went wrong with DIG1 GPIO */
    SHA5461AS_ERR_DIG2_GPIO, /**< Something went wrong with DIG2 GPIO */
    SHA5461AS_ERR_DIG3_GPIO, /**< Something went wrong with DIG3 GPIO */
    SHA5461AS_ERR_DIG4_GPIO, /**< Something went wrong with DIG4 GPIO */
    SHA5461AS_ERR_DIGITS,    /**< Something went wrong with digits value */
};

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    gpio_t a;      /**< Data input pin */
    gpio_t b;      /**< Data input pin */
    gpio_t c;      /**< Data input pin */
    gpio_t d;      /**< Data input pin */
    gpio_t e;      /**< Data input pin */
    gpio_t f;      /**< Data input pin */
    gpio_t g;      /**< Data input pin */
    gpio_t dp;     /**< Data input pin */
    gpio_t dig1;   /**< Data input pin */
    gpio_t dig2;   /**< Data input pin */
    gpio_t dig3;   /**< Data input pin */
    gpio_t dig4;   /**< Data input pin */
} sha5461as_params_t;

/**
 * @brief   Device descriptor for the SHA5461AS
 */
typedef struct {
    sha5461as_params_t params; /**< Device parameters */
    uint32_t value; /**< Binary value to display */
    uint8_t digits; /**< Number of digits */
    uint8_t current_digit; /**< Displayed digit */
    int status; /**< Status of the timer_periodic for this dev */
} sha5461as_t;

/**
 * @brief   Initialize the given DSP0401
 *
 * @param[out] dev      Initialized device descriptor of SHA5461AS device
 * @param[in]  params   Device parameters to use
 *
 * @return              SHA5461AS_OK if everything is good
 * @return              -SHA5461AS_ERR_A_GPIO if an error occured during A GPIO
 * @return              -SHA5461AS_ERR_B_GPIO if an error occured during B GPIO
 * @return              -SHA5461AS_ERR_C_GPIO if an error occured during C GPIO
 * @return              -SHA5461AS_ERR_D_GPIO if an error occured during D GPIO
 * @return              -SHA5461AS_ERR_E_GPIO if an error occured during E GPIO
 * @return              -SHA5461AS_ERR_F_GPIO if an error occured during F GPIO
 * @return              -SHA5461AS_ERR_G_GPIO if an error occured during G GPIO
 * @return              -SHA5461AS_ERR_DP_GPIO if an error occured during DP GPIO
 * @return              -SHA5461AS_ERR_DIG1_GPIO if an error occured during DIG1 GPIO
 * @return              -SHA5461AS_ERR_DIG2_GPIO if an error occured during DIG2 GPIO
 * @return              -SHA5461AS_ERR_DIG3_GPIO if an error occured during DIG3 GPIO
 * @return              -SHA5461AS_ERR_DIG4_GPIO if an error occured during DIG4 GPIO
 */
int sha5461as_init(sha5461as_t* dev, const sha5461as_params_t* params); 

/**
 * @brief   Shift the current digit off and the next one on  
 * 
 * @param[in] dev       Initialized device descriptor of SHA5461AS device
 * 
 */
int sha5461as_shift(sha5461as_t* dev);

/**
 * @brief   Set the value for every digit
 *
 * @param[in] dev       Initialized device descriptor of SHA5461AS device
 * @param[in] index     the digit number between [0, SHA5461AS_DIGITS[
 * @param[in] value     the value as an @ref uint32_t
 *
 */
int sha5461as_set_all_value(sha5461as_t* dev, uint32_t value);

/**
 * @brief   Set the value for one digit
 *
 * @param[in] dev       Initialized device descriptor of SHA5461AS device
 * @param[in] index     the digit number between [0, SHA5461AS_DIGITS[
 * @param[in] value     the value as an @ref uint8_t, A is equal to the first bit 
 *                        B the second.... DP the 8th bit. 
 *
 */
int sha5461as_set_value(sha5461as_t* dev, int index, uint8_t value);

/**
 * @brief   Set the value for one digit
 *
 * @param[in] dev       Initialized device descriptor of SHA5461AS device
 * @param[in] index     the digit number between [0, SHA5461AS_DIGITS[
 * @param[in] value     the value as an @ref uint8_t, A is equal to the first bit 
 *                        B the second.... DP the 8th bit. 
 *
 */
int sha5461as_set_value(sha5461as_t* dev, int index, uint8_t value);

/**
 * @brief   Start/stop an periodic timer event to shift between each 7seg
 * 
 * @param[in] dev       Initialized device descriptor of SHA5461AS device  
 */
int sha5461as_display(sha5461as_t* dev);

#ifdef __cplusplus
}
#endif

#endif /* SHA5461AS_H */
/** @} */
