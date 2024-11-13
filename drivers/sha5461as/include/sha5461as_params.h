/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sha5461as
 * @{
 *
 * @file
 * @brief       Default configuration for SHA5461AS
 *
 * @author      Pierre Le Meur <pierre1.lemeur@orange.com>
 *
 */

#ifndef SHA5461AS_PARAMS_H
#define SHA5461AS_PARAMS_H

#include "sha5461as.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    Set default configuration parameters for the SHA5461AS (for arduino-nano-33-ble)
 * @{
 */
#ifndef SHA5461AS_PARAM_A_PIN
/**
 * @brief   SHA5461AS the A pin
 */
#define SHA5461AS_PARAM_A_PIN            GPIO_PIN(1, 11)  /* D2 */
#endif
#ifndef SHA5461AS_PARAM_B_PIN
/**
 * @brief   SHA5461AS the B pin
 */
#define SHA5461AS_PARAM_B_PIN            GPIO_PIN(1, 12)  /* D3 */
#endif
#ifndef SHA5461AS_PARAM_C_PIN
/**
 * @brief   SHA5461AS the C pin
 */
#define SHA5461AS_PARAM_C_PIN            GPIO_PIN(1, 15)  /* D4 */
#endif
#ifndef SHA5461AS_PARAM_D_PIN
/**
 * @brief   SHA5461AS the D pin
 */
#define SHA5461AS_PARAM_D_PIN            GPIO_PIN(1, 13)  /* D5 */
#endif
#ifndef SHA5461AS_PARAM_E_PIN
/**
 * @brief   SHA5461AS the E pin
 */
#define SHA5461AS_PARAM_E_PIN            GPIO_PIN(1, 14)  /* D6 */
#endif
#ifndef SHA5461AS_PARAM_F_PIN
/**
 * @brief   SHA5461AS the F pin
 */
#define SHA5461AS_PARAM_F_PIN            GPIO_PIN(0, 23)  /* D7 */
#endif
#ifndef SHA5461AS_PARAM_G_PIN
/**
 * @brief   SHA5461AS the G pin
 */
#define SHA5461AS_PARAM_G_PIN            GPIO_PIN(0, 21)  /* D8 */
#endif
#ifndef SHA5461AS_PARAM_DP_PIN
/**
 * @brief   SHA5461AS the DP pin
 */
#define SHA5461AS_PARAM_DP_PIN           GPIO_PIN(0, 27)  /* D9 */
#endif

#ifndef SHA5461AS_PARAM_DIG1_PIN
/**
 * @brief   SHA5461AS the first digit pin
 */
#define SHA5461AS_PARAM_DIG1_PIN         GPIO_PIN(1, 2)   /* D10 */
#endif
#ifndef SHA5461AS_PARAM_DIG2_PIN
/**
 * @brief   SHA5461AS the second digit pin
 */
#define SHA5461AS_PARAM_DIG2_PIN         GPIO_PIN(1, 1)   /* D11 */
#endif
#ifndef SHA5461AS_PARAM_DIG3_PIN
/**
 * @brief   SHA5461AS the third digit pin
 */
#define SHA5461AS_PARAM_DIG3_PIN         GPIO_PIN(1, 8)   /* D12 */
#endif
#ifndef SHA5461AS_PARAM_DIG4_PIN
/**
 * @brief   SHA5461AS the fourth digit pin
 */
#define SHA5461AS_PARAM_DIG4_PIN         GPIO_PIN(0, 13)   /* D13 */
#endif

#ifndef SHA5461AS_PARAMS
/**
 * @brief   The initialization parameters for the SHA5461AS driver
 */
#define SHA5461AS_PARAMS              { .a = SHA5461AS_PARAM_A_PIN,    \
                                        .b = SHA5461AS_PARAM_B_PIN,    \
                                        .c = SHA5461AS_PARAM_C_PIN,    \
                                        .d = SHA5461AS_PARAM_D_PIN,    \
                                        .e = SHA5461AS_PARAM_E_PIN,    \
                                        .f = SHA5461AS_PARAM_F_PIN,    \
                                        .g = SHA5461AS_PARAM_G_PIN,    \
                                        .dp = SHA5461AS_PARAM_DP_PIN,   \
                                        .dig1 = SHA5461AS_PARAM_DIG1_PIN, \
                                        .dig2 = SHA5461AS_PARAM_DIG2_PIN, \
                                        .dig3 = SHA5461AS_PARAM_DIG3_PIN, \
                                        .dig4 = SHA5461AS_PARAM_DIG4_PIN    }

#endif
/**@}*/

/**
 * @brief   Configure SHA5461AS
 */
static const sha5461as_params_t sha5461as_params[] =
{
    SHA5461AS_PARAMS,
};

#ifdef __cplusplus
}
#endif

#endif /* SHA5461AS_PARAMS_H */
/** @} */
