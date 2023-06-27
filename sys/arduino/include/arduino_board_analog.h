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
 * @brief       Arduino analog pin map
 *
 * @note        The contents of this file are mostly generated using the
 *              python snippets documented. Do not edit these parts by hand,
 *              but rather adjust the python snippets and regenerate.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ARDUINO_BOARD_ANALOG_H
#define ARDUINO_BOARD_ANALOG_H

#include "periph/adc.h"
#include "arduino_iomap.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   Number of the last analog pin
 *
 * E.g. 5 if A5 is the analog pin with the highest number.
 */
#define ARDUINO_ANALOG_PIN_LAST /* board specific number */
#endif /* DOXYGEN */

/* A board may not have any analog pins. But if it has any, it MUST define
 * ARDUINO_ANALOG_PIN_LAST. */
#if !defined(ARDUINO_ANALOG_PIN_LAST) && (defined(ARDUINO_A0) || defined(ARDUINO_A1))
#  error "ARDUINO_PIN_LAST undefined despite analog pins available"
#endif

/* A board not having A0 and A1, but having e.g. A3 would also trigger this.
 * Extend as needed when porting new boards. */
#if defined(ARDUINO_ANALOG_PIN_LAST) && !defined(ARDUINO_A0) && !defined(ARDUINO_A1)
#  error "ARDUINO_PIN_LAST defined but no analog pins available"
#endif

#if defined(ARDUINO_ANALOG_PIN_LAST) || defined(DOXYGEN)
/**
 * @brief   Look-up table for the Arduino's analog pins
 *
 * Generate using
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.py}
 * format = """#ifdef ARDUINO_A{0:}
 *     ARDUINO_A{0:},
 * #elif ARDUINO_ANALOG_PIN_LAST > {0:}
 *     ADC_UNDEF,
 * #endif"""
 * for i in range(16):
 *     print(format.format(i))
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The Arduino MEGA 2560 has A0 to A15, so 16 Analog inputs is the largest
 * as of now.
 */
static const adc_t arduino_analog_map[] = {
#ifndef DOXYGEN
#ifdef ARDUINO_A0
    ARDUINO_A0,
#elif ARDUINO_ANALOG_PIN_LAST > 0
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A1
    ARDUINO_A1,
#elif ARDUINO_ANALOG_PIN_LAST > 1
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A2
    ARDUINO_A2,
#elif ARDUINO_ANALOG_PIN_LAST > 2
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A3
    ARDUINO_A3,
#elif ARDUINO_ANALOG_PIN_LAST > 3
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A4
    ARDUINO_A4,
#elif ARDUINO_ANALOG_PIN_LAST > 4
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A5
    ARDUINO_A5,
#elif ARDUINO_ANALOG_PIN_LAST > 5
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A6
    ARDUINO_A6,
#elif ARDUINO_ANALOG_PIN_LAST > 6
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A7
    ARDUINO_A7,
#elif ARDUINO_ANALOG_PIN_LAST > 7
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A8
    ARDUINO_A8,
#elif ARDUINO_ANALOG_PIN_LAST > 8
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A9
    ARDUINO_A9,
#elif ARDUINO_ANALOG_PIN_LAST > 9
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A10
    ARDUINO_A10,
#elif ARDUINO_ANALOG_PIN_LAST > 10
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A11
    ARDUINO_A11,
#elif ARDUINO_ANALOG_PIN_LAST > 11
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A12
    ARDUINO_A12,
#elif ARDUINO_ANALOG_PIN_LAST > 12
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A13
    ARDUINO_A13,
#elif ARDUINO_ANALOG_PIN_LAST > 13
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A14
    ARDUINO_A14,
#elif ARDUINO_ANALOG_PIN_LAST > 14
    ADC_UNDEF,
#endif
#ifdef ARDUINO_A15
    ARDUINO_A15,
#elif ARDUINO_ANALOG_PIN_LAST > 15
    ADC_UNDEF,
#endif
#endif /* DOXYGEN */
};
#endif /* defined(ARDUINO_ANALOG_PIN_LAST) */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_ANALOG_H */
/** @} */
