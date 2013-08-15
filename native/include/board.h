/**
 * Native Board interface
 *
 * The native board uses call level hardware simulation.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @defgroup native_board
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

void _native_LED_GREEN_OFF(void);
#define LED_GREEN_OFF (_native_LED_GREEN_OFF())
void _native_LED_GREEN_ON(void);
#define LED_GREEN_ON (_native_LED_GREEN_ON())
void _native_LED_GREEN_TOGGLE(void);
#define LED_GREEN_TOGGLE (_native_LED_GREEN_TOGGLE())
void _native_LED_RED_OFF(void);
#define LED_RED_OFF (_native_LED_RED_OFF())
void _native_LED_RED_ON(void);
#define LED_RED_ON (_native_LED_RED_ON())
void _native_LED_RED_TOGGLE(void);
#define LED_RED_TOGGLE (_native_LED_RED_TOGGLE())
