/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  NOTE: The code in this file comes from https://github.com/arduino/ArduinoCore-avr
  (see ArduinoCore-avr/variants/.../pins_arduino.h)
*/

/**
 * @ingroup    sys_arduino
 * @{
 *
 * @file
 * @brief      Macros for compatibility with Arduino
 */

#ifndef PINS_ARDUINO_H
#define PINS_ARDUINO_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BOARD_ARDUINO_LEONARDO)

/**
 * @brief   Return the PCICR register for the given digital pin
 */
#define digitalPinToPCICR(p)    ((((p) >= 8 && (p) <= 11) || ((p) >= 14 && (p) <= 17) || ((p) >= ARDUINO_PIN_A8 && (p) <= ARDUINO_PIN_A10)) ? (&PCICR) : ((uint8_t *)0))

/**
 * @brief   Return the bit in the PCICR register for the given digital pin
 */
#define digitalPinToPCICRbit(p) 0

/**
 * @brief   Return the PCMSK register for the given digital pin
 */
#define digitalPinToPCMSK(p)    ((((p) >= 8 && (p) <= 11) || ((p) >= 14 && (p) <= 17) || ((p) >= ARDUINO_PIN_A8 && (p) <= ARDUINO_PIN_A10)) ? (&PCMSK0) : ((uint8_t *)0))

/**
 * @brief   Return the bit in the PCMSK register for the given digital pin
 */
#define digitalPinToPCMSKbit(p) ( ((p) >= 8 && (p) <= 11) ? (p) - 4 : ((p) == 14 ? 3 : ((p) == 15 ? 1 : ((p) == 16 ? 2 : ((p) == 17 ? 0 : (p - ARDUINO_PIN_A8 + 4))))))

/**
 * @brief   Return the interrupt number for the given digital pin
 */
#define digitalPinToInterrupt(p) ((p) == 0 ? 2 : ((p) == 1 ? 3 : ((p) == 2 ? 1 : ((p) == 3 ? 0 : ((p) == 7 ? 4 : NOT_AN_INTERRUPT)))))

#elif defined(BOARD_ARDUINO_MEGA2560)

/**
 * @brief   Return the PCICR register for the given digital pin
 */
#define digitalPinToPCICR(p)    ( (((p) >= 10) && ((p) <= 13)) || \
                                  (((p) >= 50) && ((p) <= 53)) || \
                                  (((p) >= 62) && ((p) <= 69)) ? (&PCICR) : ((uint8_t *)0) )

/**
 * @brief   Return the bit in the PCICR register for the given digital pin
 */
#define digitalPinToPCICRbit(p) ( (((p) >= 10) && ((p) <= 13)) || (((p) >= 50) && ((p) <= 53)) ? 0 : \
                                ( (((p) >= 62) && ((p) <= 69)) ? 2 : \
                                0 ) )

/**
 * @brief   Return the PCMSK register for the given digital pin
 */
#define digitalPinToPCMSK(p)    ( (((p) >= 10) && ((p) <= 13)) || (((p) >= 50) && ((p) <= 53)) ? (&PCMSK0) : \
                                ( (((p) >= 62) && ((p) <= 69)) ? (&PCMSK2) : \
                                ((uint8_t *)0) ) )

/**
 * @brief   Return the bit in the PCMSK register for the given digital pin
 */
#define digitalPinToPCMSKbit(p) ( (((p) >= 10) && ((p) <= 13)) ? ((p) - 6) : \
                                ( ((p) == 50) ? 3 : \
                                ( ((p) == 51) ? 2 : \
                                ( ((p) == 52) ? 1 : \
                                ( ((p) == 53) ? 0 : \
                                ( (((p) >= 62) && ((p) <= 69)) ? ((p) - 62) : \
                                0 ) ) ) ) ) )

/**
 * @brief   Return the interrupt number for the given digital pin
 */
#define digitalPinToInterrupt(p) ((p) == 2 ? 0 : ((p) == 3 ? 1 : ((p) >= 18 && (p) <= 21 ? 23 - (p) : NOT_AN_INTERRUPT)))

#elif defined(MODULE_ATMEGA_COMMON)

/**
 * @brief   Return the PCICR register for the given digital pin
 */
#define digitalPinToPCICR(p)    (((p) >= 0 && (p) <= 21) ? (&PCICR) : ((uint8_t *)0))

/**
 * @brief   Return the bit in the PCICR register for the given digital pin
 */
#define digitalPinToPCICRbit(p) (((p) <= 7) ? 2 : (((p) <= 13) ? 0 : 1))

/**
 * @brief   Return the PCMSK register for the given digital pin
 */
#define digitalPinToPCMSK(p)    (((p) <= 7) ? (&PCMSK2) : (((p) <= 13) ? (&PCMSK0) : (((p) <= 21) ? (&PCMSK1) : ((uint8_t *)0))))

/**
 * @brief   Return the bit in the PCMSK register for the given digital pin
 */
#define digitalPinToPCMSKbit(p) (((p) <= 7) ? (p) : (((p) <= 13) ? ((p) - 8) : ((p) - 14)))

/**
 * @brief   Return the interrupt number for the given digital pin
 */
#define digitalPinToInterrupt(p)  ((p) == 2 ? 0 : ((p) == 3 ? 1 : NOT_AN_INTERRUPT))


#endif

#ifdef __cplusplus
}
#endif

#endif /* PINS_ARDUINO_H */
/** @} */
