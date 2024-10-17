/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_arduino
 * @{
 *
 * @file
 * @brief       Mapping of Arduino digital pin numbers to PWM settings
 *
 * @note        The contents of this file are mostly generated using the
 *              python snippets documented. Do not edit these parts by hand,
 *              but rather adjust the python snippets and regenerate.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ARDUINO_BOARD_PWM_H
#define ARDUINO_BOARD_PWM_H

#include "arduino_iomap.h"
#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   PWM frequency
 */
#define ARDUINO_PWM_FREQU       /* implementation defined */
#else
#  ifndef ARDUINO_PWM_FREQU
#    define ARDUINO_PWM_FREQU   (490U)
#  endif
#endif

/**
 * @brief   List of PWM GPIO mappings
 *
 * Generate using
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.py}
 * format = """#ifdef ARDUINO_PIN_{0:}_PWM_DEV
 *     {{ .dev = ARDUINO_PIN_{0:}_PWM_DEV, .chan = ARDUINO_PIN_{0:}_PWM_CHAN, .pin = {0:} }},
 * #endif"""
 * for i in range(32):
 *     print(format.format(i))
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We assume D0..D31 are the only candidates for PWM pins as a wild guess. Once
 * this no longer holds, the list needs to be extended.
 */
static const arduino_pwm_t arduino_pwm_list[] = {
#ifdef ARDUINO_PIN_0_PWM_DEV
    { .dev = ARDUINO_PIN_0_PWM_DEV, .chan = ARDUINO_PIN_0_PWM_CHAN, .pin = 0 },
#endif
#ifdef ARDUINO_PIN_1_PWM_DEV
    { .dev = ARDUINO_PIN_1_PWM_DEV, .chan = ARDUINO_PIN_1_PWM_CHAN, .pin = 1 },
#endif
#ifdef ARDUINO_PIN_2_PWM_DEV
    { .dev = ARDUINO_PIN_2_PWM_DEV, .chan = ARDUINO_PIN_2_PWM_CHAN, .pin = 2 },
#endif
#ifdef ARDUINO_PIN_3_PWM_DEV
    { .dev = ARDUINO_PIN_3_PWM_DEV, .chan = ARDUINO_PIN_3_PWM_CHAN, .pin = 3 },
#endif
#ifdef ARDUINO_PIN_4_PWM_DEV
    { .dev = ARDUINO_PIN_4_PWM_DEV, .chan = ARDUINO_PIN_4_PWM_CHAN, .pin = 4 },
#endif
#ifdef ARDUINO_PIN_5_PWM_DEV
    { .dev = ARDUINO_PIN_5_PWM_DEV, .chan = ARDUINO_PIN_5_PWM_CHAN, .pin = 5 },
#endif
#ifdef ARDUINO_PIN_6_PWM_DEV
    { .dev = ARDUINO_PIN_6_PWM_DEV, .chan = ARDUINO_PIN_6_PWM_CHAN, .pin = 6 },
#endif
#ifdef ARDUINO_PIN_7_PWM_DEV
    { .dev = ARDUINO_PIN_7_PWM_DEV, .chan = ARDUINO_PIN_7_PWM_CHAN, .pin = 7 },
#endif
#ifdef ARDUINO_PIN_8_PWM_DEV
    { .dev = ARDUINO_PIN_8_PWM_DEV, .chan = ARDUINO_PIN_8_PWM_CHAN, .pin = 8 },
#endif
#ifdef ARDUINO_PIN_9_PWM_DEV
    { .dev = ARDUINO_PIN_9_PWM_DEV, .chan = ARDUINO_PIN_9_PWM_CHAN, .pin = 9 },
#endif
#ifdef ARDUINO_PIN_10_PWM_DEV
    { .dev = ARDUINO_PIN_10_PWM_DEV, .chan = ARDUINO_PIN_10_PWM_CHAN, .pin = 10 },
#endif
#ifdef ARDUINO_PIN_11_PWM_DEV
    { .dev = ARDUINO_PIN_11_PWM_DEV, .chan = ARDUINO_PIN_11_PWM_CHAN, .pin = 11 },
#endif
#ifdef ARDUINO_PIN_12_PWM_DEV
    { .dev = ARDUINO_PIN_12_PWM_DEV, .chan = ARDUINO_PIN_12_PWM_CHAN, .pin = 12 },
#endif
#ifdef ARDUINO_PIN_13_PWM_DEV
    { .dev = ARDUINO_PIN_13_PWM_DEV, .chan = ARDUINO_PIN_13_PWM_CHAN, .pin = 13 },
#endif
#ifdef ARDUINO_PIN_14_PWM_DEV
    { .dev = ARDUINO_PIN_14_PWM_DEV, .chan = ARDUINO_PIN_14_PWM_CHAN, .pin = 14 },
#endif
#ifdef ARDUINO_PIN_15_PWM_DEV
    { .dev = ARDUINO_PIN_15_PWM_DEV, .chan = ARDUINO_PIN_15_PWM_CHAN, .pin = 15 },
#endif
#ifdef ARDUINO_PIN_16_PWM_DEV
    { .dev = ARDUINO_PIN_16_PWM_DEV, .chan = ARDUINO_PIN_16_PWM_CHAN, .pin = 16 },
#endif
#ifdef ARDUINO_PIN_17_PWM_DEV
    { .dev = ARDUINO_PIN_17_PWM_DEV, .chan = ARDUINO_PIN_17_PWM_CHAN, .pin = 17 },
#endif
#ifdef ARDUINO_PIN_18_PWM_DEV
    { .dev = ARDUINO_PIN_18_PWM_DEV, .chan = ARDUINO_PIN_18_PWM_CHAN, .pin = 18 },
#endif
#ifdef ARDUINO_PIN_19_PWM_DEV
    { .dev = ARDUINO_PIN_19_PWM_DEV, .chan = ARDUINO_PIN_19_PWM_CHAN, .pin = 19 },
#endif
#ifdef ARDUINO_PIN_20_PWM_DEV
    { .dev = ARDUINO_PIN_20_PWM_DEV, .chan = ARDUINO_PIN_20_PWM_CHAN, .pin = 20 },
#endif
#ifdef ARDUINO_PIN_21_PWM_DEV
    { .dev = ARDUINO_PIN_21_PWM_DEV, .chan = ARDUINO_PIN_21_PWM_CHAN, .pin = 21 },
#endif
#ifdef ARDUINO_PIN_22_PWM_DEV
    { .dev = ARDUINO_PIN_22_PWM_DEV, .chan = ARDUINO_PIN_22_PWM_CHAN, .pin = 22 },
#endif
#ifdef ARDUINO_PIN_23_PWM_DEV
    { .dev = ARDUINO_PIN_23_PWM_DEV, .chan = ARDUINO_PIN_23_PWM_CHAN, .pin = 23 },
#endif
#ifdef ARDUINO_PIN_24_PWM_DEV
    { .dev = ARDUINO_PIN_24_PWM_DEV, .chan = ARDUINO_PIN_24_PWM_CHAN, .pin = 24 },
#endif
#ifdef ARDUINO_PIN_25_PWM_DEV
    { .dev = ARDUINO_PIN_25_PWM_DEV, .chan = ARDUINO_PIN_25_PWM_CHAN, .pin = 25 },
#endif
#ifdef ARDUINO_PIN_26_PWM_DEV
    { .dev = ARDUINO_PIN_26_PWM_DEV, .chan = ARDUINO_PIN_26_PWM_CHAN, .pin = 26 },
#endif
#ifdef ARDUINO_PIN_27_PWM_DEV
    { .dev = ARDUINO_PIN_27_PWM_DEV, .chan = ARDUINO_PIN_27_PWM_CHAN, .pin = 27 },
#endif
#ifdef ARDUINO_PIN_28_PWM_DEV
    { .dev = ARDUINO_PIN_28_PWM_DEV, .chan = ARDUINO_PIN_28_PWM_CHAN, .pin = 28 },
#endif
#ifdef ARDUINO_PIN_29_PWM_DEV
    { .dev = ARDUINO_PIN_29_PWM_DEV, .chan = ARDUINO_PIN_29_PWM_CHAN, .pin = 29 },
#endif
#ifdef ARDUINO_PIN_30_PWM_DEV
    { .dev = ARDUINO_PIN_30_PWM_DEV, .chan = ARDUINO_PIN_30_PWM_CHAN, .pin = 30 },
#endif
#ifdef ARDUINO_PIN_31_PWM_DEV
    { .dev = ARDUINO_PIN_31_PWM_DEV, .chan = ARDUINO_PIN_31_PWM_CHAN, .pin = 31 },
#endif
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ARDUINO_BOARD_PWM_H */
