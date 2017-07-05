/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup drivers_periph_led
 * @{
 *
 * @file
 * @brief       common LED macros and init function
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "board.h"
#include "periph/led.h"

void led_init(void)
{
#ifdef LED0_PIN
    LED0_INIT;
#endif /* LED0_PIN */

#ifdef LED1_PIN
    LED1_INIT;
#endif /* LED1_PIN */

#ifdef LED2_PIN
    LED2_INIT;
#endif /* LED2_PIN */

#ifdef LED3_PIN
    LED3_INIT;
#endif /* LED3_PIN */

#ifdef LED4_PIN
    LED4_INIT;
#endif /* LED4_PIN */

#ifdef LED5_PIN
    LED5_INIT;
#endif /* LED5_PIN */

#ifdef LED6_PIN
    LED6_INIT;
#endif /* LED6_PIN */

#ifdef LED7_PIN
    LED7_INIT;
#endif /* LED7_PIN */
}
