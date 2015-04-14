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

#if GPIO_16_EN
void g16_cb(void *arg){
    puts("GPIO_16 triggered");
}
#endif

#if GPIO_17_EN
void g17_cb(void *arg){
    puts("GPIO_17 triggered");
}
#endif

#if GPIO_18_EN
void g18_cb(void *arg){
    puts("GPIO_18 triggered");
}
#endif

#if GPIO_19_EN
void g19_cb(void *arg){
    puts("GPIO_19 triggered");
}
#endif

#if GPIO_20_EN
void g20_cb(void *arg){
    puts("GPIO_20 triggered");
}
#endif

#if GPIO_21_EN
void g21_cb(void *arg){
    puts("GPIO_21 triggered");
}
#endif

#if GPIO_22_EN
void g22_cb(void *arg){
    puts("GPIO_22 triggered");
}
#endif

#if GPIO_23_EN
void g23_cb(void *arg){
    puts("GPIO_23 triggered");
}
#endif

#if GPIO_24_EN
void g24_cb(void *arg){
    puts("GPIO_24 triggered");
}
#endif

#if GPIO_25_EN
void g25_cb(void *arg){
    puts("GPIO_25 triggered");
}
#endif

#if GPIO_26_EN
void g26_cb(void *arg){
    puts("GPIO_26 triggered");
}
#endif

#if GPIO_27_EN
void g27_cb(void *arg){
    puts("GPIO_27 triggered");
}
#endif

#if GPIO_28_EN
void g28_cb(void *arg){
    puts("GPIO_28 triggered");
}
#endif

#if GPIO_29_EN
void g29_cb(void *arg){
    puts("GPIO_29 triggered");
}
#endif

#if GPIO_30_EN
void g30_cb(void *arg){
    puts("GPIO_30 triggered");
}
#endif

#if GPIO_31_EN
void g31_cb(void *arg){
    puts("GPIO_31 triggered");
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
#if GPIO_16_EN
    gpio_init_int(GPIO_16, PULL, ACTIVE_FLANK, g16_cb, 0);
#endif
#if GPIO_17_EN
    gpio_init_int(GPIO_17, PULL, ACTIVE_FLANK, g17_cb, 0);
#endif
#if GPIO_18_EN
    gpio_init_int(GPIO_18, PULL, ACTIVE_FLANK, g18_cb, 0);
#endif
#if GPIO_19_EN
    gpio_init_int(GPIO_19, PULL, ACTIVE_FLANK, g19_cb, 0);
#endif
#if GPIO_20_EN
    gpio_init_int(GPIO_20, PULL, ACTIVE_FLANK, g20_cb, 0);
#endif
#if GPIO_21_EN
    gpio_init_int(GPIO_21, PULL, ACTIVE_FLANK, g21_cb, 0);
#endif
#if GPIO_22_EN
    gpio_init_int(GPIO_22, PULL, ACTIVE_FLANK, g22_cb, 0);
#endif
#if GPIO_23_EN
    gpio_init_int(GPIO_23, PULL, ACTIVE_FLANK, g23_cb, 0);
#endif
#if GPIO_24_EN
    gpio_init_int(GPIO_24, PULL, ACTIVE_FLANK, g24_cb, 0);
#endif
#if GPIO_25_EN
    gpio_init_int(GPIO_25, PULL, ACTIVE_FLANK, g25_cb, 0);
#endif
#if GPIO_26_EN
    gpio_init_int(GPIO_26, PULL, ACTIVE_FLANK, g26_cb, 0);
#endif
#if GPIO_27_EN
    gpio_init_int(GPIO_27, PULL, ACTIVE_FLANK, g27_cb, 0);
#endif
#if GPIO_28_EN
    gpio_init_int(GPIO_28, PULL, ACTIVE_FLANK, g28_cb, 0);
#endif
#if GPIO_29_EN
    gpio_init_int(GPIO_29, PULL, ACTIVE_FLANK, g29_cb, 0);
#endif
#if GPIO_30_EN
    gpio_init_int(GPIO_30, PULL, ACTIVE_FLANK, g30_cb, 0);
#endif
#if GPIO_31_EN
    gpio_init_int(GPIO_31, PULL, ACTIVE_FLANK, g31_cb, 0);
#endif

    puts("Initialization done, going to sleep now");

    return 0;
}
