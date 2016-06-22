/*
 * Copyright (C) 2016 IoT-Lab
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3. See the file LICENSE in the top level
 * directory for more details.
 */
#include "led.h"
#include "leds.h"

// LED7_ON defined but not LED8_ON
#ifdef LED8_ON
#warning "Only implemented for 8 leds at max"
#endif
#define MAX_NUM_LEDS 8


#define LEDS_MASK ((1 << MAX_NUM_LEDS) - 1)
#define LED_FLAG(i) (1 << (i))
#define HAS_LED_NUM(i, leds_flag) ((LED_FLAG((i)) & (leds_flag)))


/*
 * There is one macro per led number
 * So each led configuration must be done with a macro
 */
#define set_led_num(i, mode)         \
    do {                             \
        switch ((mode)) {            \
            case LED_OFF:            \
                LED ## i ## _OFF;    \
                break;               \
            case LED_ON:             \
                LED ## i ## _ON;     \
                break;               \
            case LED_TOGGLE:         \
                LED ## i ## _TOGGLE; \
                break;               \
        }                            \
    } while (0)


// There should be no parenthesis around 'i' as it will be expanded
#define case_led_and_break(i, mode) \
    case i:                         \
        set_led_num(i, (mode));     \
        break


void leds_set(int leds_flag, enum leds_mode mode)
{
    leds_flag &= LEDS_MASK;

    for (int i = 0; LED_FLAG(i) <= leds_flag; ++i) {
        if (HAS_LED_NUM(i, leds_flag)) {
            leds_set_num(i, mode);
        }
    }
}


void leds_set_num(int led, enum leds_mode mode)
{
    (void)mode;  // no error when no leds
    switch (led) {
#ifdef LED0_PIN
        case_led_and_break(0, mode);
#endif
#ifdef LED1_PIN
        case_led_and_break(1, mode);
#endif
#ifdef LED2_PIN
        case_led_and_break(2, mode);
#endif
#ifdef LED3_PIN
        case_led_and_break(3, mode);
#endif
#ifdef LED4_PIN
        case_led_and_break(4, mode);
#endif
#ifdef LED5_PIN
        case_led_and_break(5, mode);
#endif
#ifdef LED6_PIN
        case_led_and_break(6, mode);
#endif
#ifdef LED7_PIN
        case_led_and_break(7, mode);
#endif
    }
}
