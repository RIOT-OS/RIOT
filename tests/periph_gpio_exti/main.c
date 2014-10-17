/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test external interrupt for GPIO driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "periph_conf.h"
#include "periph/gpio.h"

#define ACTIVE_FLANK        GPIO_RISING
#define PULL                GPIO_NOPULL

uint32_t state = 0;
uint32_t old_state = 0;

#if GPIO_0_EN
void g0_cb(void *arg){
    puts("GPIO_0 triggered");
}
#endif

#if GPIO_1_EN
void g1_cb(void *arg){
    puts("GPIO_1 triggered");
}
#endif

#if GPIO_2_EN
void g2_cb(void *arg){
    puts("GPIO_2 triggered");
}
#endif

#if GPIO_3_EN
void g3_cb(void *arg){
    puts("GPIO_3 triggered");
}
#endif

#if GPIO_4_EN
void g4_cb(void *arg){
    puts("GPIO_4 triggered");
}
#endif

#if GPIO_5_EN
void g5_cb(void *arg){
    puts("GPIO_5 triggered");
}
#endif

#if GPIO_6_EN
void g6_cb(void *arg){
    puts("GPIO_6 triggered");
}
#endif

#if GPIO_7_EN
void g7_cb(void *arg){
    puts("GPIO_7 triggered");
}
#endif

#if GPIO_8_EN
void g8_cb(void *arg){
    puts("GPIO_8 triggered");
}
#endif

#if GPIO_9_EN
void g9_cb(void *arg){
    puts("GPIO_9 triggered");
}
#endif

#if GPIO_10_EN
void g10_cb(void *arg){
    puts("GPIO_10 triggered");
}
#endif

#if GPIO_11_EN
void g11_cb(void *arg){
    puts("GPIO_11 triggered");
}
#endif

#if GPIO_12_EN
void g12_cb(void *arg){
    puts("GPIO_12 triggered");
}
#endif

#if GPIO_13_EN
void g13_cb(void *arg){
    puts("GPIO_13 triggered");
}
#endif

#if GPIO_14_EN
void g14_cb(void *arg){
    puts("GPIO_14 triggered");
}
#endif

#if GPIO_15_EN
void g15_cb(void *arg){
    puts("GPIO_15 triggered");
}
#endif

int main(void)
{
    puts("GPIO driver test - external interrupts");
    puts("Initializing all GPIOs as external interrupt sources");

#if GPIO_0_EN
    gpio_init_int(GPIO_0, PULL, ACTIVE_FLANK, g0_cb, 0);
#endif
#if GPIO_1_EN
    gpio_init_int(GPIO_1, PULL, ACTIVE_FLANK, g1_cb, 0);
#endif
#if GPIO_2_EN
    gpio_init_int(GPIO_2, PULL, ACTIVE_FLANK, g2_cb, 0);
#endif
#if GPIO_3_EN
    gpio_init_int(GPIO_3, PULL, ACTIVE_FLANK, g3_cb, 0);
#endif
#if GPIO_4_EN
    gpio_init_int(GPIO_4, PULL, ACTIVE_FLANK, g4_cb, 0);
#endif
#if GPIO_5_EN
    gpio_init_int(GPIO_5, PULL, ACTIVE_FLANK, g5_cb, 0);
#endif
#if GPIO_6_EN
    gpio_init_int(GPIO_6, PULL, ACTIVE_FLANK, g6_cb, 0);
#endif
#if GPIO_7_EN
    gpio_init_int(GPIO_7, PULL, ACTIVE_FLANK, g7_cb, 0);
#endif
#if GPIO_8_EN
    gpio_init_int(GPIO_8, PULL, ACTIVE_FLANK, g8_cb, 0);
#endif
#if GPIO_9_EN
    gpio_init_int(GPIO_9, PULL, ACTIVE_FLANK, g9_cb, 0);
#endif
#if GPIO_10_EN
    gpio_init_int(GPIO_10, PULL, ACTIVE_FLANK, g10_cb, 0);
#endif
#if GPIO_11_EN
    gpio_init_int(GPIO_11, PULL, ACTIVE_FLANK, g11_cb, 0);
#endif
#if GPIO_12_EN
    gpio_init_int(GPIO_12, PULL, ACTIVE_FLANK, g12_cb, 0);
#endif
#if GPIO_13_EN
    gpio_init_int(GPIO_13, PULL, ACTIVE_FLANK, g13_cb, 0);
#endif
#if GPIO_14_EN
    gpio_init_int(GPIO_14, PULL, ACTIVE_FLANK, g14_cb, 0);
#endif
#if GPIO_15_EN
    gpio_init_int(GPIO_15, PULL, ACTIVE_FLANK, g15_cb, 0);
#endif

    puts("Initialization done, going to sleep now");

    return 0;
}
