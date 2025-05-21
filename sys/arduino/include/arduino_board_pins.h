/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_arduino
 * @{
 *
 * @file
 * @brief       Arduino digital pin map
 *
 * @note        The contents of this file are mostly generated using the
 *              python snippets documented. Do not edit these parts by hand,
 *              but rather adjust the python snippets and regenerate.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include "arduino_iomap.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   The digital pin connected to the LED
 *
 * E.g. 13 when LED is connected to D13.
 */
#define ARDUINO_LED             /* board specific number */

/**
 * @brief   Number of the last digital pin
 *
 * E.g. 42 if D42 is the digital pin with the highest number.
 */
#define ARDUINO_PIN_LAST        /* board specific number */
#endif /* DOXYGEN */

#if !defined(ARDUINO_LED) && !defined(DOXYGEN)
#define ARDUINO_LED         (13)
#endif

#ifndef ARDUINO_PIN_LAST
#  error "ARDUINO_PIN_LAST undefined"
#endif

/**
 * @brief   Look-up table for the Arduino's digital pins:
 *
 * Generate using
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.py}
 * format = """#ifdef ARDUINO_PIN_{0:}
 *     ARDUINO_PIN_{0:},
 * #elif ARDUINO_PIN_LAST > {0:}
 *     GPIO_UNDEF,
 * #endif"""
 * for i in range(70):
 *     print(format.format(i))
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The Arduino MEGA2560 has D0 to D69, so 70 digital pins is the largest
 * Arduino form factor there is.
 */
static const gpio_t arduino_pinmap[] = {
#ifndef DOXYGEN
#ifdef ARDUINO_PIN_0
    ARDUINO_PIN_0,
#elif ARDUINO_PIN_LAST > 0
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_1
    ARDUINO_PIN_1,
#elif ARDUINO_PIN_LAST > 1
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_2
    ARDUINO_PIN_2,
#elif ARDUINO_PIN_LAST > 2
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_3
    ARDUINO_PIN_3,
#elif ARDUINO_PIN_LAST > 3
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_4
    ARDUINO_PIN_4,
#elif ARDUINO_PIN_LAST > 4
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_5
    ARDUINO_PIN_5,
#elif ARDUINO_PIN_LAST > 5
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_6
    ARDUINO_PIN_6,
#elif ARDUINO_PIN_LAST > 6
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_7
    ARDUINO_PIN_7,
#elif ARDUINO_PIN_LAST > 7
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_8
    ARDUINO_PIN_8,
#elif ARDUINO_PIN_LAST > 8
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_9
    ARDUINO_PIN_9,
#elif ARDUINO_PIN_LAST > 9
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_10
    ARDUINO_PIN_10,
#elif ARDUINO_PIN_LAST > 10
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_11
    ARDUINO_PIN_11,
#elif ARDUINO_PIN_LAST > 11
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_12
    ARDUINO_PIN_12,
#elif ARDUINO_PIN_LAST > 12
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_13
    ARDUINO_PIN_13,
#elif ARDUINO_PIN_LAST > 13
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_14
    ARDUINO_PIN_14,
#elif ARDUINO_PIN_LAST > 14
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_15
    ARDUINO_PIN_15,
#elif ARDUINO_PIN_LAST > 15
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_16
    ARDUINO_PIN_16,
#elif ARDUINO_PIN_LAST > 16
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_17
    ARDUINO_PIN_17,
#elif ARDUINO_PIN_LAST > 17
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_18
    ARDUINO_PIN_18,
#elif ARDUINO_PIN_LAST > 18
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_19
    ARDUINO_PIN_19,
#elif ARDUINO_PIN_LAST > 19
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_20
    ARDUINO_PIN_20,
#elif ARDUINO_PIN_LAST > 20
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_21
    ARDUINO_PIN_21,
#elif ARDUINO_PIN_LAST > 21
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_22
    ARDUINO_PIN_22,
#elif ARDUINO_PIN_LAST > 22
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_23
    ARDUINO_PIN_23,
#elif ARDUINO_PIN_LAST > 23
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_24
    ARDUINO_PIN_24,
#elif ARDUINO_PIN_LAST > 24
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_25
    ARDUINO_PIN_25,
#elif ARDUINO_PIN_LAST > 25
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_26
    ARDUINO_PIN_26,
#elif ARDUINO_PIN_LAST > 26
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_27
    ARDUINO_PIN_27,
#elif ARDUINO_PIN_LAST > 27
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_28
    ARDUINO_PIN_28,
#elif ARDUINO_PIN_LAST > 28
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_29
    ARDUINO_PIN_29,
#elif ARDUINO_PIN_LAST > 29
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_30
    ARDUINO_PIN_30,
#elif ARDUINO_PIN_LAST > 30
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_31
    ARDUINO_PIN_31,
#elif ARDUINO_PIN_LAST > 31
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_32
    ARDUINO_PIN_32,
#elif ARDUINO_PIN_LAST > 32
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_33
    ARDUINO_PIN_33,
#elif ARDUINO_PIN_LAST > 33
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_34
    ARDUINO_PIN_34,
#elif ARDUINO_PIN_LAST > 34
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_35
    ARDUINO_PIN_35,
#elif ARDUINO_PIN_LAST > 35
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_36
    ARDUINO_PIN_36,
#elif ARDUINO_PIN_LAST > 36
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_37
    ARDUINO_PIN_37,
#elif ARDUINO_PIN_LAST > 37
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_38
    ARDUINO_PIN_38,
#elif ARDUINO_PIN_LAST > 38
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_39
    ARDUINO_PIN_39,
#elif ARDUINO_PIN_LAST > 39
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_40
    ARDUINO_PIN_40,
#elif ARDUINO_PIN_LAST > 40
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_41
    ARDUINO_PIN_41,
#elif ARDUINO_PIN_LAST > 41
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_42
    ARDUINO_PIN_42,
#elif ARDUINO_PIN_LAST > 42
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_43
    ARDUINO_PIN_43,
#elif ARDUINO_PIN_LAST > 43
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_44
    ARDUINO_PIN_44,
#elif ARDUINO_PIN_LAST > 44
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_45
    ARDUINO_PIN_45,
#elif ARDUINO_PIN_LAST > 45
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_46
    ARDUINO_PIN_46,
#elif ARDUINO_PIN_LAST > 46
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_47
    ARDUINO_PIN_47,
#elif ARDUINO_PIN_LAST > 47
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_48
    ARDUINO_PIN_48,
#elif ARDUINO_PIN_LAST > 48
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_49
    ARDUINO_PIN_49,
#elif ARDUINO_PIN_LAST > 49
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_50
    ARDUINO_PIN_50,
#elif ARDUINO_PIN_LAST > 50
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_51
    ARDUINO_PIN_51,
#elif ARDUINO_PIN_LAST > 51
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_52
    ARDUINO_PIN_52,
#elif ARDUINO_PIN_LAST > 52
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_53
    ARDUINO_PIN_53,
#elif ARDUINO_PIN_LAST > 53
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_54
    ARDUINO_PIN_54,
#elif ARDUINO_PIN_LAST > 54
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_55
    ARDUINO_PIN_55,
#elif ARDUINO_PIN_LAST > 55
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_56
    ARDUINO_PIN_56,
#elif ARDUINO_PIN_LAST > 56
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_57
    ARDUINO_PIN_57,
#elif ARDUINO_PIN_LAST > 57
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_58
    ARDUINO_PIN_58,
#elif ARDUINO_PIN_LAST > 58
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_59
    ARDUINO_PIN_59,
#elif ARDUINO_PIN_LAST > 59
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_60
    ARDUINO_PIN_60,
#elif ARDUINO_PIN_LAST > 60
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_61
    ARDUINO_PIN_61,
#elif ARDUINO_PIN_LAST > 61
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_62
    ARDUINO_PIN_62,
#elif ARDUINO_PIN_LAST > 62
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_63
    ARDUINO_PIN_63,
#elif ARDUINO_PIN_LAST > 63
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_64
    ARDUINO_PIN_64,
#elif ARDUINO_PIN_LAST > 64
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_65
    ARDUINO_PIN_65,
#elif ARDUINO_PIN_LAST > 65
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_66
    ARDUINO_PIN_66,
#elif ARDUINO_PIN_LAST > 66
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_67
    ARDUINO_PIN_67,
#elif ARDUINO_PIN_LAST > 67
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_68
    ARDUINO_PIN_68,
#elif ARDUINO_PIN_LAST > 68
    GPIO_UNDEF,
#endif
#ifdef ARDUINO_PIN_69
    ARDUINO_PIN_69,
#elif ARDUINO_PIN_LAST > 69
    GPIO_UNDEF,
#endif
#endif /* DOXYGEN */
};

#ifdef __cplusplus
}
#endif

/** @} */
