/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_digit7seg
 * @file
 * @brief       Default params for device driver with less than 5 digits of
 *              7 segments without IC
 * @author      Pierre Le Meur <pierre1.lemeur@orange.com>
 */

#ifndef DIGIT7SEG_PARAMS_H
#define DIGIT7SEG_PARAMS_H

#include "digit7seg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for a digit 7 segments (for arduino-nano-33-ble)
 * @{
 */
#ifndef DIGIT7SEG_PARAM_A_PIN
/**
 * @brief   digit 7 seg a pin
 */
#   define DIGIT7SEG_PARAM_A_PIN            GPIO_PIN(1, 11)  /* D2 */
#endif
#ifndef DIGIT7SEG_PARAM_B_PIN
/**
 * @brief   digit 7 seg b pin
 */
#   define DIGIT7SEG_PARAM_B_PIN            GPIO_PIN(1, 12)  /* D3 */
#endif
#ifndef DIGIT7SEG_PARAM_C_PIN
/**
 * @brief   digit 7 seg c pin
 */
#   define DIGIT7SEG_PARAM_C_PIN            GPIO_PIN(1, 15)  /* D4 */
#endif
#ifndef DIGIT7SEG_PARAM_D_PIN
/**
 * @brief   digit 7 seg d pin
 */
#   define DIGIT7SEG_PARAM_D_PIN            GPIO_PIN(1, 13)  /* D5 */
#endif
#ifndef DIGIT7SEG_PARAM_E_PIN
/**
 * @brief   digit 7 seg e pin
 */
#   define DIGIT7SEG_PARAM_E_PIN            GPIO_PIN(1, 14)  /* D6 */
#endif
#ifndef DIGIT7SEG_PARAM_F_PIN
/**
 * @brief   digit 7 seg f pin
 */
#   define DIGIT7SEG_PARAM_F_PIN            GPIO_PIN(0, 23)  /* D7 */
#endif
#ifndef DIGIT7SEG_PARAM_G_PIN
/**
 * @brief   digit 7 seg g pin
 */
#   define DIGIT7SEG_PARAM_G_PIN            GPIO_PIN(0, 21)  /* D8 */
#endif
#ifndef DIGIT7SEG_PARAM_DP_PIN
/**
 * @brief  digit 7 seg dp pin
 */
#   define DIGIT7SEG_PARAM_DP_PIN           GPIO_PIN(0, 27)  /* D9 */
#endif

#ifndef DIGIT7SEG_PARAM_DIG1_PIN
/**
 * @brief   digit 7 seg first digit pin
 */
#   define DIGIT7SEG_PARAM_DIG1_PIN         GPIO_PIN(1, 2)   /* D10 */
#endif
#ifndef DIGIT7SEG_PARAM_DIG2_PIN
/**
 * @brief   digit 7 seg second digit pin
 */
#   define DIGIT7SEG_PARAM_DIG2_PIN         GPIO_PIN(1, 1)   /* D11 */
#endif
#ifndef DIGIT7SEG_PARAM_DIG3_PIN
/**
 * @brief   digit 7 seg third digit pin
 */
#   define DIGIT7SEG_PARAM_DIG3_PIN         GPIO_PIN(1, 8)   /* D12 */
#endif
#ifndef DIGIT7SEG_PARAM_DIG4_PIN
/**
 * @brief   digit 7 seg fourth digit pin
 */
#   define DIGIT7SEG_PARAM_DIG4_PIN         GPIO_PIN(0, 13)   /* D13 */
#endif
#ifndef DIGIT7SEG_PARAM_TIMER
/**
 * @brief   digit 7 seg timer to use
 */
#   define DIGIT7SEG_PARAM_TIMER            TIMER_DEV(2)
#endif
#ifndef DIGIT7SEG_PARAM_DIGITS
/**
 * @brief   digit 7 seg number of digits
 */
#   define DIGIT7SEG_PARAM_DIGITS           (4)
#endif

#ifndef DIGIT7SEG_PARAMS
/**
 * @brief   The initialization parameters for the digit 7 seg driver
 */
#define DIGIT7SEG_PARAMS              { .data_a = DIGIT7SEG_PARAM_A_PIN,   \
                                        .data_b = DIGIT7SEG_PARAM_B_PIN,   \
                                        .data_c = DIGIT7SEG_PARAM_C_PIN,   \
                                        .data_d = DIGIT7SEG_PARAM_D_PIN,   \
                                        .data_e = DIGIT7SEG_PARAM_E_PIN,   \
                                        .data_f = DIGIT7SEG_PARAM_F_PIN,   \
                                        .data_g = DIGIT7SEG_PARAM_G_PIN,   \
                                        .data_dp = DIGIT7SEG_PARAM_DP_PIN, \
                                        .dig1 = DIGIT7SEG_PARAM_DIG1_PIN,  \
                                        .dig2 = DIGIT7SEG_PARAM_DIG2_PIN,  \
                                        .dig3 = DIGIT7SEG_PARAM_DIG3_PIN,  \
                                        .dig4 = DIGIT7SEG_PARAM_DIG4_PIN,  \
                                        .timer = DIGIT7SEG_PARAM_TIMER,    \
                                        .digits = DIGIT7SEG_PARAM_DIGITS }

#endif
/**@}*/

/**
 * @brief   Configure a digit 7 segments
 */
static const digit7seg_params_t digit7seg_params[] =
{
    DIGIT7SEG_PARAMS,
};

#ifdef __cplusplus
}
#endif

#endif /* DIGIT7SEG_PARAMS_H */
