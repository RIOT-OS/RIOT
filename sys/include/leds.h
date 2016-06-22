/*
 * Copyright (C) 2016 IoT-Lab
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef LEDS_H
#define LEDS_H

#ifdef __cplusplus
extern "C" {
#endif


enum leds_mode {
    LED_OFF,
    LED_ON,
    LED_TOGGLE,
};


void leds_set(int leds_flag, enum leds_mode mode);
void leds_set_num(int led, enum leds_mode mode);

#ifdef __cplusplus
}
#endif

#endif //LEDS_H
