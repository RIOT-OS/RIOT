/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_init
 * @{
 *
 * @file
 * @brief       Init on board LEDs
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "led.h"
#include "periph/gpio.h"
#include "kernel_defines.h"

#define LED_INIT(x) do {                                            \
                        if (IS_ACTIVE(MODULE_PERIPH_INIT_LED##x)) {   \
                            gpio_init(LED##x##_PIN, GPIO_OUT);      \
                            LED##x##_OFF;                           \
                        }                                           \
                    } while (0)

__attribute__ ((weak))
void led_init(void)
{
    if (!IS_USED(MODULE_PERIPH_GPIO)) {
        return;
    }

    /* The condition is dual: We don't init if the LED is absent (eg. when a
     * LEDn_PIN is defined, but there is a higher level driver such as SAUL PWM that makes the
     * direct use impossible), but we also don't init if there is no pin (eg.
     * on native where there is a different mechanism for LEDs). */
#if defined(LED0_IS_PRESENT) && defined(LED0_PIN)
    LED_INIT(0);
#endif
#if defined(LED1_IS_PRESENT) && defined(LED1_PIN)
    LED_INIT(1);
#endif
#if defined(LED2_IS_PRESENT) && defined(LED2_PIN)
    LED_INIT(2);
#endif
#if defined(LED3_IS_PRESENT) && defined(LED3_PIN)
    LED_INIT(3);
#endif
#if defined(LED4_IS_PRESENT) && defined(LED4_PIN)
    LED_INIT(4);
#endif
#if defined(LED5_IS_PRESENT) && defined(LED5_PIN)
    LED_INIT(5);
#endif
#if defined(LED6_IS_PRESENT) && defined(LED6_PIN)
    LED_INIT(6);
#endif
#if defined(LED7_IS_PRESENT) && defined(LED7_PIN)
    LED_INIT(7);
#endif
}
